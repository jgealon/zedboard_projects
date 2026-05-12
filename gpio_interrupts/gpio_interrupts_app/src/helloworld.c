#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "sleep.h"
  
static u32 last_value = 0;


XGpio_Config *gpio_config;
XGpio gpio;

void intr_handler(){

    XGpio_InterruptDisable(&gpio, XGPIO_IR_CH1_MASK);

    u32 dataRead = XGpio_DiscreteRead(&gpio, 1);

    // Only print if value actually changed (debounce)
    if(dataRead != last_value) {
        xil_printf("SW VALUE READ : %0d\r\n", dataRead);
        last_value = dataRead;
    }

    XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);

    // Debounce delay - ignore bounces for 50ms
    usleep(50000);

    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);

}



void gpio_init(){

    gpio_config = XGpio_LookupConfig(XPAR_XGPIO_0_BASEADDR);
    int status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);

    if(status == XST_SUCCESS)
        xil_printf("GPIO INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GPIO INIT FAILED.\r\n");

    XGpio_SetDataDirection(&gpio, 1, 0xFF);

    // Read initial value to initialize last_value
    last_value = XGpio_DiscreteRead(&gpio, 1);
    xil_printf("Initial switch value: %0d\r\n", last_value);

}

XScuGic_Config *intc_config;
XScuGic intc;

void intc_init(){

    intc_config = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
    s32 status = XScuGic_CfgInitialize(&intc, intc_config, intc_config->CpuBaseAddress);

    if(status == XST_SUCCESS)
        xil_printf("GIC INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GIC INIT FAILED.\r\n");

    ////INIT and ENABLE EXCEPTION HANDLER
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &intc);
    Xil_ExceptionEnable();

    XScuGic_Connect(&intc, XPAR_FABRIC_AXI_GPIO_0_VEC_ID, (Xil_InterruptHandler) intr_handler, 0);
    XScuGic_Enable(&intc, XPAR_FABRIC_AXI_GPIO_0_VEC_ID);

    // Enable GPIO interrupt in the GPIO IP
      XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);
      XGpio_InterruptGlobalEnable(&gpio);

}



int main()
{
    init_platform();
    gpio_init();
    intc_init();

    while(1){

    }

    cleanup_platform();
    return 0;
}
