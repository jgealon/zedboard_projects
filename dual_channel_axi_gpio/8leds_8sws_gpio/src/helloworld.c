#include <stdio.h> 
#include <xstatus.h> 
#include "platform.h" 
#include "xil_printf.h" 
#include "xgpio.h" 
#include "xparameters.h" 
#include "sleep.h" //Add this for sleep() 

XGpio_Config *gpio_config; 

XGpio gpio; 

void gpio_init(){
    int status;
    gpio_config = XGpio_LookupConfig(0); /* Device ID is 0 (first GPIO instance). XGpio_LookupConfig(XPAR_AXI_GPIO_0_DEVICE_ID); The problem is clear: XPAR_AXI_GPIO_0_DEVICE_ID doesn't exist in your
  xparameters.h file. In the Vitis flow, you should use the canonical device ID
  which is simply 0 (the instance number).*/

    status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);
    if (status == XST_SUCCESS)
        xil_printf("Device INIT Successful.\n");
    else
        xil_printf("Device INIT Failed.\n");

    } 
    
    int main() { 
        u32 swValue = 0; 
        init_platform(); 
        gpio_init(); 
        XGpio_SetDataDirection(&gpio, 1, 0xff); //Channel 1 = input (switches) 
        XGpio_SetDataDirection(&gpio, 2, 0x00); //Channel 2 = output (LEDs)
        
        while (1){ 
            swValue = XGpio_DiscreteRead(&gpio, 1); 
            XGpio_DiscreteWrite(&gpio, 2, swValue); 
            xil_printf("Switch Value: 0x%02X\n", swValue);
            sleep(2); 
            } 
            
            cleanup_platform(); 
            return 0; //Never reached, but good practice

    }