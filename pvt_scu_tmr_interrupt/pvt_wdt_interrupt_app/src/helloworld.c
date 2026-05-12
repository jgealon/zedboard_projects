#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xscuwdt.h"
#include "xil_exception.h"
 
XScuWdt_Config *wdt_config;
XScuWdt wdt;
 
XScuGic_Config *gic_config;
XScuGic gic;
 

void wdtHandler(){
 
 
	if(XScuWdt_IsTimerExpired(&wdt))
	{
		xil_printf("We have an Interrupt at 5 Sec\r\n");
	}
  
}
  
void wdtInit(){
	int status = 0;
	wdt_config = XScuWdt_LookupConfig(XPAR_XSCUWDT_0_BASEADDR);
	status = XScuWdt_CfgInitialize(&wdt,wdt_config,wdt_config->BaseAddr);
	if(status == XST_SUCCESS)
		xil_printf("WDT Init Successful\r\n");
	else
		xil_printf("WDT Init Failed\r\n");
  
	    XScuWdt_Stop(&wdt);
	    XScuWdt_SetTimerMode(&wdt);
	    XScuWdt_LoadWdt(&wdt,333333333*5); //3sec
		u32 opt = XScuWdt_GetControlReg(&wdt);
		opt = opt | XSCUWDT_CONTROL_IT_ENABLE_MASK | XSCUWDT_CONTROL_AUTO_RELOAD_MASK;
		XScuWdt_SetControlReg(&wdt,opt);
 
}
  
void gicInit(){
    int status = 0;
	gic_config = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
	status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);
	if(status == XST_SUCCESS)
		xil_printf("GIC Init Successful\r\n");
	else
		xil_printf("GIC Init Failed\r\n");
 
 
     	Xil_ExceptionInit();
		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &gic);
		Xil_ExceptionEnable();
 
 
	XScuGic_Connect(&gic, XPAR_SCUWDT_INTR, (Xil_InterruptHandler)wdtHandler, &wdt);
	XScuGic_Enable(&gic, XPAR_SCUWDT_INTR);
   
}
 
 
int main()
{
    init_platform();
    wdtInit();
    gicInit();
     XScuWdt_Start(&wdt);
 
    while(1);
  
    cleanup_platform();
    return 0;
}