#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xparameters.h"
#include "unistd.h"


XScuWdt_Config *wdt_config;
XScuWdt wdt;

void wdt_init(){
    wdt_config = XScuWdt_LookupConfig(XPAR_XSCUWDT_0_BASEADDR);
s32 status = XScuWdt_CfgInitialize(&wdt, wdt_config, wdt_config->BaseAddr);
if(status == XST_SUCCESS)
    xil_printf("WDT INIT SUCCESSFUL.\n");
else
    xil_printf("WDT INIT FAILED.\n");

status = XScuWdt_SelfTest(&wdt);
if(status == XST_SUCCESS)
    xil_printf("WDT SELF TEST SUCCESSFUL.\n");
else
    xil_printf("WDT SELF TEST FAILED.\n");
}


int main()
{
    init_platform();
    wdt_init();
    u32 count = 333333333 * 5;
    u32 temp = 0;
    
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, count);
    XScuWdt_Start(&wdt);

    while(temp < 20){
        sleep(1);
        xil_printf("Temp : %0d\n", temp);
        temp++;
        XScuWdt_RestartWdt(&wdt);
                
    }
    cleanup_platform();
    return 0;
}
