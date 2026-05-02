#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"

XGpio gpio;

void gpio_init(){

    // Use device ID 0 (first GPIO instance)
    int status = XGpio_Initialize(&gpio, 0);

    if(status == XST_SUCCESS)
        xil_printf("AXI GPIO INIT SUCCESSFUL.\r\n");
    else
        xil_printf("AXI GPIO INIT FAILED.\r\n");

    XGpio_SetDataDirection(&gpio, 1, 0x00000000); // All outputs

}



int main()
{
    init_platform();

    xil_printf("\r\n=== Starting ILA Test ===\r\n");

    gpio_init();

    xil_printf("Starting GPIO writes...\r\n");

    // Write directly to AXI GPIO address - bypass driver
    volatile unsigned int *gpio_addr = (unsigned int *)0x41200000;

    int count = 0;
    while(1) {
        *gpio_addr = 0x0;
        *gpio_addr = 0x1;
        *gpio_addr = 0x2;
        *gpio_addr = 0x3;
        *gpio_addr = 0x4;
        *gpio_addr = 0x5;
        *gpio_addr = 0x6;
        *gpio_addr = 0x7;

        count++;
        if(count % 1000 == 0) {
            xil_printf("Wrote %d cycles\r\n", count);
        }
    }

    cleanup_platform();
    return 0;
}
