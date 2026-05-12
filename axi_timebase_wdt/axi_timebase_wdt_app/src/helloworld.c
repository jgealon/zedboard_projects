
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xwdttb.h"

XWdtTb_Config *wdt_config;
XWdtTb wdt;

void wdt_init(){

    wdt_config = XWdtTb_LookupConfig(XPAR_XWDTTB_0_BASEADDR);
    s32 status = XWdtTb_CfgInitialize(&wdt, wdt_config, wdt_config->BaseAddr);

    if (status == XST_SUCCESS)
        xil_printf("WDT INIT SUCCESSFUL.\r\n");
    else
        xil_printf("WDT INIT FAILED.\r\n");
    
    status = XWdtTb_SelfTest(&wdt);
    
    if (status == XST_SUCCESS)
        xil_printf("WDT SELF TEST SUCCESSFUL.\r\n");
    else
        xil_printf("WDT SELF TEST FAILED.\r\n");

}

int main()
{
    init_platform();
    wdt_init();

    XWdtTb_Stop(&wdt);

    XWdtTb_Start(&wdt);    

    while (!XWdtTb_IsWdtExpired(&wdt));
    xil_printf("WDT Expired.\n");

  
  

    cleanup_platform();
    return 0;
}
