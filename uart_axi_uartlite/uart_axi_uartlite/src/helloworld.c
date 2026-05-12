/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xuartlite.h"
#include "xparameters.h"

XUartLite_Config *uart_config1, *uart_config2;
XUartLite uart1, uart2;

void uart_init(){

    uart_config1 = XUartLite_LookupConfig(XPAR_XUARTLITE_0_BASEADDR);
    int status1 = XUartLite_CfgInitialize(&uart1, uart_config1, uart_config1->RegBaseAddr);

    uart_config2 = XUartLite_LookupConfig(XPAR_XUARTLITE_1_BASEADDR);
    int status2 = XUartLite_CfgInitialize(&uart2, uart_config2, uart_config2->RegBaseAddr);

    if ((status1 && status2) == XST_SUCCESS)
        xil_printf("UART INIT SUCCESSFUL.\n");
    else
        xil_printf("UART INIT FAILED.\n");

}

int main()
{
    init_platform();
    uart_init();

    u8 data1[]="UART 1->2";
    u8 data2[]="UART 2->1";
    u8 Rx[9];

    xil_printf("Sending Data from UART 0 -> UART 1\n");

    XUartLite_Send(&uart1, &data1[0], 9);
    while(XUartLite_IsSending(&uart1));

    int byteRcvd = 0;

    while (byteRcvd != 9){
        byteRcvd = byteRcvd + XUartLite_Recv(&uart2, &Rx[byteRcvd], 9);   
    }    
    
    xil_printf("Transmission complete UART0 -> UART1\n");

    for (int i = 0; i<9; i++)
    {
        xil_printf("%0c", Rx[i]);
    }
    xil_printf("\n");

    xil_printf("Sending data UART0 -> UART1\n");

    XUartLite_Send(&uart2, &data2[0], 9);
    while(XUartLite_IsSending(&uart2));

    byteRcvd = 0;

    while (byteRcvd != 9){
        byteRcvd = byteRcvd + XUartLite_Recv(&uart1, &Rx[byteRcvd], 9);   
    }    
    
    xil_printf("Transmission complete UART0 -> UART1\n");

    for (int i = 0; i<9; i++)
    {
        xil_printf("%0c", Rx[i]);
    }
    xil_printf("\n");

    xil_printf("Sending data UART1 -> UART0\n");


    cleanup_platform();
    return 0;
}
