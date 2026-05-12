#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
 
#include "xttcps.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
 
XTtcPs_Config *tC;
XTtcPs tI;
 
XScuGic_Config *iC;
XScuGic iI;
  
void eventHdr(){
 
	XTtcPs_DisableInterrupts(&tI,XTTCPS_IXR_INTERVAL_MASK);
 
	while(XTtcPs_GetCounterValue(&tI));
	xil_printf("Interrupt\r\n");
 
	XTtcPs_ClearInterruptStatus(&tI,XTTCPS_IXR_INTERVAL_MASK);
	XTtcPs_EnableInterrupts(&tI,XTTCPS_IXR_INTERVAL_MASK);
}
  
void ttcInit(){
 
int status = 0;
tC = XTtcPs_LookupConfig(XPAR_XTTCPS_0_BASEADDR);
status = XTtcPs_CfgInitialize(&tI,tC,tC->BaseAddress);
 
if(status == XST_SUCCESS)
	xil_printf("TTC Init Successful\r\n");
else
	xil_printf("TTC Init Failed\r\n");
 
 
    u32 freq = 1;
	XInterval tmI;
	u8 tmP;
 
	    XTtcPs_Stop(&tI);
 
	    u32 opt = XTtcPs_GetOptions(&tI);
	    opt = opt | XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_DECREMENT;
 
	    XTtcPs_SetOptions(&tI,opt);
 
	    XTtcPs_CalcIntervalFromFreq(&tI,freq,&tmI,&tmP);
 
	    XTtcPs_SetPrescaler(&tI, tmP);
	    XTtcPs_SetInterval(&tI,tmI);
  
}
 
 
void intrInit(){
	int status = 0;
	iC =XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
	status = XScuGic_CfgInitialize(&iI, iC, iC->CpuBaseAddress);
 
	if(status == XST_SUCCESS)
		xil_printf("GIC Init Successful\r\n");
	else
		xil_printf("GIC Init Failed\r\n");
  
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &iI);
	Xil_ExceptionEnable();
 
 
	XScuGic_Connect(&iI, XPAR_XTTCPS_0_INTR, (Xil_InterruptHandler)eventHdr, &tI);
	XScuGic_Enable(&iI, XPAR_XTTCPS_0_INTR);
  
	XTtcPs_EnableInterrupts(&tI,XTTCPS_IXR_INTERVAL_MASK);
 }
  
int main()
{
 
    init_platform();
    ttcInit();
    intrInit();
  
    XTtcPs_Start(&tI);
 
    while(1);
 
    cleanup_platform();
    return 0;
}
 