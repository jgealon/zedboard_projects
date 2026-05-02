#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xbram.h"
#include "xstatus.h"


XBram_Config *bram_config;
XBram bram;

void bram_init(){
    bram_config = XBram_LookupConfig(XPAR_AXI_BRAM_CTRL_0_DEVICE_ID);
    int status = XBram_CfgInitialize(&bram, bram_config, bram_config->CtrlBaseAddress);
    
    if(status == XST_SUCCESS)
        xil_printf("BRAM INIT SUCCESSFUL.\r\n");
    else
        xil_printf("BRAM INIT FAILED.\r\n");
    
}


int main()
{
    init_platform();
    bram_init();

    volatile int *baseAddr;
    baseAddr = (int *)XPAR_AXI_BRAM_CTRL_0_BASEADDR;

    int i = 0;

    for(i = 0; i < 50; i++)
    {
        *(baseAddr + i) = 5*i;
    }
    
    cleanup_platform();
    return 0;
}
