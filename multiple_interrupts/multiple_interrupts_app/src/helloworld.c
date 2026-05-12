#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "sleep.h"

static u32 last_value = 0;


XGpio slide, btn;

void slide_handler(){

    XGpio_InterruptDisable(&slide, XGPIO_IR_CH1_MASK);

    u32 dataRead = XGpio_DiscreteRead(&slide, 1);

    // Only print if value actually changed (debounce)
    if(dataRead != last_value) {
        xil_printf("SLIDE SW VALUE READ : %0d\r\n", dataRead);
        last_value = dataRead;
    }

    XGpio_InterruptClear(&slide, XGPIO_IR_CH1_MASK);

    // Debounce delay - ignore bounces for 50ms
    usleep(50000);

    XGpio_InterruptEnable(&slide, XGPIO_IR_CH1_MASK);

}

void btn_handler(){

    XGpio_InterruptDisable(&btn, XGPIO_IR_CH1_MASK);

    u32 dataRead = XGpio_DiscreteRead(&btn, 1);

    // Only print if value actually changed (debounce)
    if(dataRead != last_value) {
        xil_printf("BUTTON VALUE READ : %0d\r\n", dataRead);
        last_value = dataRead;
    }

    XGpio_InterruptClear(&btn, XGPIO_IR_CH1_MASK);

    // Debounce delay - ignore bounces for 50ms
    usleep(50000);

    XGpio_InterruptEnable(&btn, XGPIO_IR_CH1_MASK);

}



void gpio_init(){

    
    int statuss = XGpio_Initialize(&slide, XPAR_XGPIO_0_BASEADDR);
    int statusb = XGpio_Initialize(&btn, XPAR_XGPIO_1_BASEADDR);

    
    if((statuss & statusb) == XST_SUCCESS)
        xil_printf("GPIO INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GPIO INIT FAILED.\r\n");

    XGpio_SetDataDirection(&slide, 1, 0xFF);
    XGpio_SetDataDirection(&btn, 1, 0xFF);
    // Read initial value to initialize last_value
    last_value = XGpio_DiscreteRead(&slide, 1);
    xil_printf("Initial switch value: %0d\r\n", last_value);

}

XScuGic_Config *gic_config;
XScuGic gic;

void gic_init(){

    gic_config = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
    s32 status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);

    if(status == XST_SUCCESS)
        xil_printf("GIC INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GIC INIT FAILED.\r\n");

    ////INIT and ENABLE EXCEPTION HANDLER
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &gic);
    Xil_ExceptionEnable();

    XScuGic_Connect(&gic, 61, (Xil_InterruptHandler) slide_handler, &slide);
    XScuGic_Connect(&gic, 62, (Xil_InterruptHandler) btn_handler, &btn);    
    XScuGic_Enable(&gic, 61);//slide switch
    XScuGic_Enable(&gic, 62);//btn switch

    // Enable GPIO interrupt in the GPIO IP
      XGpio_InterruptEnable(&slide, XGPIO_IR_CH1_MASK);
      XGpio_InterruptEnable(&btn, XGPIO_IR_CH1_MASK);
      XGpio_InterruptGlobalEnable(&slide);
      XGpio_InterruptGlobalEnable(&btn);

}



int main()
{
    init_platform();
    gpio_init();
    gic_init();

    while(1){

    }

    cleanup_platform();
    return 0;
}
