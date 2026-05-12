#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xttcps.h"
#include "xparameters.h"
 
typedef struct {
	u32 outputFreq;
	XInterval interval;
	u8 prescaler;
	u32 option;
 
} ttc_setup;
 
 
XTtcPs_Config *ttc_config;
XTtcPs ttc;
 
void ttc_init(){
	int status;
 
 
	ttc_config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_BASEADDR);
	status = XTtcPs_CfgInitialize(&ttc,ttc_config, ttc_config->BaseAddress);
 
	if(status == XST_SUCCESS)
		xil_printf("TTC Init Successful\n");
	else
		xil_printf("TTC Init Failed\n");
 
	status = XTtcPs_SelfTest(&ttc);
 
	if(status == XST_SUCCESS)
		xil_printf("Self Test Successful\n");
	else
		xil_printf("Self Test Failed\n");
 
 
}
 
 
int main()
{
 
	ttc_setup data = {1,0,0,0};
 
    init_platform();

    xil_printf("\r\n========================================\r\n");
    xil_printf("   TTC (Triple Timer Counter) Test\r\n");
    xil_printf("========================================\r\n\r\n");

    ttc_init();

    xil_printf("\r\n=== Configuring TTC ===\r\n");
    xil_printf("Target frequency: %lu Hz\r\n", data.outputFreq);

    XTtcPs_Stop(&ttc);
    data.option = data.option | XTTCPS_OPTION_DECREMENT | XTTCPS_OPTION_INTERVAL_MODE;
    XTtcPs_SetOptions(&ttc, data.option);
    XTtcPs_CalcIntervalFromFreq(&ttc, data.outputFreq, &(data.interval), &(data.prescaler));

    xil_printf("Calculated interval: %lu\r\n", data.interval);
    xil_printf("Calculated prescaler: %d\r\n", data.prescaler);

    XTtcPs_SetPrescaler(&ttc, data.prescaler);
    XTtcPs_SetInterval(&ttc, data.interval);

    xil_printf("\r\n=== Starting 1-second timer loop ===\r\n");
/*
    XTtcPs_Start(&ttc);
    usleep(500000);
    u16 count = XTtcPs_GetCounterValue(&ttc);
    xil_printf("Count Value : %0d\n", count);
*/
 
    while(1){
 
    	 XTtcPs_Start(&ttc);
    	 while(XTtcPs_GetCounterValue(&ttc));
    	 XTtcPs_Stop(&ttc);
    	 xil_printf("Delay of 1 sec achieved\r\n");
    	 XTtcPs_ResetCounterValue(&ttc);
 
    }
 
 
 
    cleanup_platform();
    return 0;
}