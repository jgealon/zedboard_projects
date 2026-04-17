#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sleep.h"

XGpio led, sw;

void gpio_init(){
    int status1, status2;

    // Initialize GPIO instance 0 (for LEDs)
    // Device ID = 0 (first GPIO instance)
    status1 = XGpio_Initialize(&led, 0);
    if (status1 != XST_SUCCESS) {
        xil_printf("GPIO 0 (LED) initialization failed!\n\r");
        return;
    }

    // Initialize GPIO instance 1 (for Switches)
    // Device ID = 1 (second GPIO instance)
    status2 = XGpio_Initialize(&sw, 1);
    if (status2 != XST_SUCCESS) {
        xil_printf("GPIO 1 (SW) initialization failed!\n\r");
        return;
    }

    // Set data direction for GPIO 0 (LEDs - Output)
    XGpio_SetDataDirection(&led, 1, 0x00); // Channel 1 = Output

    // Set data direction for GPIO 1 (Switches - Input)
    XGpio_SetDataDirection(&sw, 1, 0xFF); // Channel 1 = Input

    xil_printf("GPIO initialization successful!\n\r");
    xil_printf("GPIO 0 (LED) at 0x%08X\n\r", XPAR_XGPIO_0_BASEADDR);
    xil_printf("GPIO 1 (SW)  at 0x%08X\n\r", XPAR_XGPIO_1_BASEADDR);
}

int main()
{
    u8 swValue = 0;

    init_platform();

    xil_printf("\n\r=== Multiple Instance GPIO Test ===\n\r");
    gpio_init();

    xil_printf("\n\rMirroring switches to LEDs...\n\r");
    xil_printf("Toggle switches to see LEDs change.\n\r\n\r");

    // Main loop - mirror switches to LEDs
    while (1) {
        // Read switches from GPIO 1
        swValue = XGpio_DiscreteRead(&sw, 1);

        // Write to LEDs on GPIO 0
        XGpio_DiscreteWrite(&led, 1, swValue);

        // Print status every 2 seconds
        xil_printf("Switch: 0x%02X -> LED: 0x%02X\n\r", swValue, swValue);
        sleep(2);
    }

    cleanup_platform();
    return 0;
}
