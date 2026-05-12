#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xscutimer.h"
#include "xil_exception.h"
 
XScuTimer_Config *tC;
XScuTimer tI;
 
XScuGic_Config *iC;
XScuGic iI;
 
 
 
void tmrHandler(){
	XScuTimer_DisableInterrupt(&tI);
 
	if(XScuTimer_IsExpired(&tI))
	{
		xil_printf("We have an Interrupt at 1 Sec\r\n");
	}
 
	XScuTimer_ClearInterruptStatus(&tI);
	XScuTimer_EnableInterrupt(&tI);
	XScuTimer_RestartTimer(&tI);
 
}
 
 
void tmrInit(){
	int status = 0;
	tC = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_BASEADDR);
	status = XScuTimer_CfgInitialize(&tI,tC,tC->BaseAddr);
	if(status == XST_SUCCESS)
		xil_printf("Timer Init Successful\r\n");
	else
		xil_printf("Timer Init Failed\r\n");
 
	XScuTimer_Stop(&tI);
	XScuTimer_LoadTimer(&tI,333333333); //// 1 sec
	XScuTimer_DisableAutoReload(&tI);
  
}
 
 
void intrInit(){
    int status =0 ;
	iC = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
	status = XScuGic_CfgInitialize(&iI, iC, iC->CpuBaseAddress);
	if(status == XST_SUCCESS)
		xil_printf("GIC Init Successful\r\n");
	else
		xil_printf("GIC Init Failed\r\n");
 
 
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &iI);
	Xil_ExceptionEnable();
 
 
	XScuGic_Connect(&iI, XPAR_SCUTIMER_INTR, (Xil_InterruptHandler)tmrHandler, &tI); ///XPAR_SCUTIMER_INTR can be found in xparameters_ps.h
	XScuGic_Enable(&iI, XPAR_SCUTIMER_INTR);
  
	XScuTimer_EnableInterrupt(&tI);
  
}
  
 
int main()
{
    init_platform();
    tmrInit();
    intrInit();
 
 
    XScuTimer_Start(&tI);
 
    while(1);
 
 
 
    cleanup_platform();
    return 0;
}