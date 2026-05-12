#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xtmrctr.h"

XTmrCtr_Config *tmr_config;
XTmrCtr tmr;

void tmr_init(){

int status = XTmrCtr_Initialize(&tmr, XPAR_XTMRCTR_0_BASEADDR);

if (status == XST_SUCCESS)
    xil_printf("AXI TMR INIT SUCCESSFUL.\r\n");    
else
    xil_printf("AXI TMR INIT FAILED.\r\n");

status = XTmrCtr_SelfTest(&tmr, 0);

if (status == XST_SUCCESS)
    xil_printf("AXI TMR SELF TEST SUCCESSFUL.\r\n");    
else
    xil_printf("AXI TMR SELF TEST FAILED.\r\n");

}


int main()
{
    u32 count0 = 100000000*5; //counter 0
    u32 count1 = 100000000*8; //counter 1
    
    init_platform();
    tmr_init();
    
    XTmrCtr_Stop(&tmr, 0);
    XTmrCtr_Stop(&tmr, 1);

    XTmrCtr_SetResetValue(&tmr, 0, count0);
    XTmrCtr_SetResetValue(&tmr, 1, count1);

    XTmrCtr_Reset(&tmr, 0);
    XTmrCtr_Reset(&tmr, 1);

    u32 option0 = XTmrCtr_GetOptions(&tmr, 0);
    XTmrCtr_SetOptions(&tmr, 0, option0 | XTC_DOWN_COUNT_OPTION);

    u32 option1 = XTmrCtr_GetOptions(&tmr, 1);
    XTmrCtr_SetOptions(&tmr, 1, option1 | XTC_DOWN_COUNT_OPTION);

    XTmrCtr_Start(&tmr, 0);
    XTmrCtr_Start(&tmr, 1);

    while(!XTmrCtr_IsExpired(&tmr, 0));
    xil_printf("TIMER 0 EXPIRED.\n");

    while(!XTmrCtr_IsExpired(&tmr, 1));
    xil_printf("TIMER 1 EXPIRED.\n");

    cleanup_platform();
    return 0;
}
