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
    u32 count = 100000000*5;
    init_platform();
    tmr_init();
    
    XTmrCtr_Stop(&tmr, 0);
    XTmrCtr_SetResetValue(&tmr, 0, count);
    XTmrCtr_Reset(&tmr, 0);
    u32 option = XTmrCtr_GetOptions(&tmr, 0);
    ////Non-AUTO RELOAD MODE
    XTmrCtr_SetOptions(&tmr, 0, option | XTC_DOWN_COUNT_OPTION);
    
    while(1){
    XTmrCtr_Start(&tmr, 0);

    while(!XTmrCtr_IsExpired(&tmr, 0));
    XTmrCtr_Stop(&tmr, 0);
    xil_printf("Delay of 5 Seconds Achieved.\r\n");
    XTmrCtr_Reset(&tmr, 0);
    }
    cleanup_platform();
    return 0;
}
