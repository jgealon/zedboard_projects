#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include "xil_exception.h"
#include "xscugic.h"
 
XTmrCtr tmr;
 
XScuGic_Config *gic_config;
XScuGic gic;
 
void tmrHandler(){
 
	if (XTmrCtr_IsExpired(&tmr,0)){
		xil_printf("Interrupt\r\n");
	}
 
	XTmrCtr_Reset(&tmr, 0);
 
}
 
void timerInit(){
	int status = 0;
	status = XTmrCtr_Initialize(&tmr, XPAR_XTMRCTR_0_BASEADDR);
	if(status == XST_SUCCESS)
		xil_printf("TMR Init Successful\r\n");
	else
		xil_printf("TMR Init Failed\r\n");
 
	u32 Count = 50000000*5;
	XTmrCtr_SetResetValue(&tmr, 0, Count);
	XTmrCtr_SetOptions(&tmr, 0, XTC_INT_MODE_OPTION | XTC_DOWN_COUNT_OPTION );
 
 
 
}
 
 
void gicInit(){
	int status = 0;
gic_config = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
 
status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);
if(status == XST_SUCCESS)
	xil_printf("GIC Init Successful\n");
else
	xil_printf("GIC Init Failed\n");
 
 
 
Xil_ExceptionInit();
Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler,	&gic);
Xil_ExceptionEnable();
 
XScuGic_Connect(&gic, 61, (Xil_InterruptHandler)tmrHandler, &tmr);
XScuGic_Enable(&gic, 61);
 
 
 
 
}
 
int main()
{
 
    init_platform();
    timerInit();
    gicInit();
	XTmrCtr_Start(&tmr, 0);
 
	while(1)
  {
 
 
  }
    cleanup_platform();
    return 0;
}