#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_io.h"

/* Since xtime_l.h is not available in the BSP, we implement it here */

/* Global Timer Registers for Zynq-7000 */
#define GLOBAL_TMR_BASEADDR 0xF8F00200U
#define GTIMER_COUNTER_LOWER_OFFSET 0x00U
#define GTIMER_COUNTER_UPPER_OFFSET 0x04U

/* CPU Frequency - define this to match your actual system */
#define XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ 666666687

/* XTime type - 64-bit counter value */
typedef unsigned long long XTime;

/**
 * XTime_GetTime - Read the 64-bit Global Timer counter
 * This is the standard Xilinx function, implemented here since xtime_l.h isn't available
 */
void XTime_GetTime(XTime *Xtime_Global)
{
	u32 low;
	u32 high;

	/* Read with rollover protection */
	do {
		high = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET);
		low = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_LOWER_OFFSET);
	} while(Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET) != high);

	*Xtime_Global = (((XTime) high) << 32U) | (XTime) low;
}

int main()
{
	XTime startg, endg;

	init_platform();

	XTime_GetTime(&startg);
	xil_printf("Profile\n\r");
	XTime_GetTime(&endg);

	///////AXI Timer = 2.46 usec

	printf("Number of Clock Ticks :  %0llu\n\r", endg - startg);  ///u64 --> long long unsigned --> llu
	printf("Time Elapsed in nSec :  %0f\n\r", (endg - startg) * (2000000000.0 / XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ));
	printf("Time Elapsed in uSec :  %0f\n\r", (endg - startg) * (2000000.0 / XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ));

	cleanup_platform();
	return 0;
}
