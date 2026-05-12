#include <stdint.h>
#include <stdio.h>
#include <sleep.h>
#include "xparameters.h"

uint32_t *led_p, *sw_p;

int main()
{
    

    printf("AXI GPIO\n");
    printf("Successfully ran LED and SW\n");
    
    led_p = (uint32_t *)XPAR_AXI_GPIO_0_BASEADDR;
    sw_p = (uint32_t *)XPAR_AXI_GPIO_1_BASEADDR;

    while (1){

        if (*(sw_p + 0) == 0xFF)
        {
            *(led_p + 0) = 0x5;
            sleep(1);
            *(led_p + 0) = 0xA;
            sleep(1);
            *(led_p + 0) = 0xFF;  
            sleep(1);      
            *(led_p + 0) = 0x245A;
            sleep(1);
        }
        else {
            *(led_p + 0) = 0x0;    
        }
    }
    return 0;
}
