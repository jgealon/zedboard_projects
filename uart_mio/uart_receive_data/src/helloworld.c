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
#include <xil_types.h>
#include "platform.h"
#include "xil_printf.h"
#include "xuartps.h"
#include "xparameters.h"
#include "xstatus.h"

XUartPs_Config *uart_config;
XUartPs uart;

void uart_init(){
    s32 status;

    // SDT mode: Use base address instead of device ID
    uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);

    if (uart_config == NULL) {
        xil_printf("UART config lookup FAILED!\n");
        return;
    }

    status = XUartPs_CfgInitialize(&uart, uart_config, uart_config->BaseAddress);
    XUartPs_SetBaudRate(&uart, 115200);
    XUartPs_SetOperMode(&uart, XUARTPS_OPER_MODE_NORMAL);
    if (status == XST_SUCCESS)
        xil_printf("UART init successful\n");
    else
        xil_printf("UART init failed\n");

    while(XUartPs_IsSending(&uart));
}
int main()
{
    u8 data[11] = "Hello World";
    u8 RecvData[11];

    s32 count = 0;
    uart_init();

    XUartPs_SetOperMode(&uart, XUARTPS_OPER_MODE_LOCAL_LOOP);
    XUartPs_Send(&uart, &data[0], 11);
    while(XUartPs_IsSending(&uart));

    while (count != 11){
        count = count + XUartPs_Recv(&uart, &RecvData[count], 11);
    }

    XUartPs_SetOperMode(&uart, XUARTPS_OPER_MODE_NORMAL);
    
    int i = 0;
     for (i = 0; i<11; i++)
    {
        xil_printf("%0c", RecvData[i]);
    }     



    cleanup_platform();
    return 0;
    
}
