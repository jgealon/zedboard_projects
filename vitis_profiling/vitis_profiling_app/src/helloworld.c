#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
 
 
XGpio_Config *gpio_c;
XGpio gpio;
 
void gpio_init(){
 
	gpio_c = XGpio_LookupConfig(XPAR_XGPIO_0_BASEADDR);
	int status = XGpio_CfgInitialize(&gpio, gpio_c, gpio_c->BaseAddress );
	if(status == XST_SUCCESS)
		 xil_printf("GPIO SUCCESSFULLY INITIALIZED\r\n");
	else
		xil_printf("GPIO INIT FAILED.\r\n");
 
    XGpio_SetDataDirection(&gpio, 1, 0x0000);
}
 
void led_1(){
	XGpio_DiscreteWrite(&gpio, 1, 0xffff);
	XGpio_DiscreteWrite(&gpio, 1, 0x0000);
}
 
 
void led_3(){
	XGpio_DiscreteWrite(&gpio, 1, 0xffff);
	XGpio_DiscreteWrite(&gpio, 1, 0x0000);
}
 
 
void led_5(){
	XGpio_DiscreteWrite(&gpio, 1, 0xffff);
	XGpio_DiscreteWrite(&gpio, 1, 0x0000);
}
 
 
int main()
{
    init_platform();
    gpio_init();
    led_1();
    led_3();
    led_5();
    print("Hello World\n\r");
 
    cleanup_platform();
    return 0;
}
