#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"

XGpio led, sw;

void gpio_init(){
    int statusl, statuss;

    xil_printf("\n=== GPIO Initialization ===\n");

    // SDT mode: Use base addresses instead of device IDs
    // Initialize LED GPIO (base address 0x41200000)
    statusl = XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);
    xil_printf("LED init: %d\n", statusl);

    // Initialize Switch GPIO (base address 0x41210000)
    statuss = XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);
    xil_printf("SW init: %d\n", statuss);

    if (statusl == XST_SUCCESS && statuss == XST_SUCCESS)
        xil_printf("DEVICE INIT SUCCESSFUL.\n");
    else
        xil_printf("DEVICE INIT FAILED.\n");
}

int main()
{
    u32 swRead = 0;
    init_platform();
    gpio_init();

    // Set directions
    XGpio_SetDataDirection(&led, 1, 0x00);  // LED as output
    XGpio_SetDataDirection(&sw, 1, 0xFF);   // Switch as input

    // Test LED GPIO first - blink pattern
    xil_printf("\n=== Testing LED GPIO ===\n");
    xil_printf("Turning all LEDs ON...\n");
    XGpio_DiscreteWrite(&led, 1, 0xFF);
    sleep(2);
    xil_printf("Turning all LEDs OFF...\n");
    XGpio_DiscreteWrite(&led, 1, 0x00);
    sleep(2);

    xil_printf("\n=== Starting switch read loop ===\n");
    while(1){
        swRead = XGpio_DiscreteRead(&sw, 1);
        XGpio_DiscreteWrite(&led, 1, swRead);
        xil_printf("SwRead : 0x%02X (%d)\n", swRead, swRead);
        sleep(1);
    }


    cleanup_platform();
    return 0;
}
