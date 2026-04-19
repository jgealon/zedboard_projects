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
    XUartPs_SetBaudRate(&uart, 9600);
    if (status == XST_SUCCESS)
        xil_printf("UART init successful\n");
    else
        xil_printf("UART init failed\n");
}
int main()
{
    u8 data[11] = "Hello World";
    s32 count = 0;
    init_platform();
    uart_init();

    while (count != 11){
    count = XUartPs_Send(&uart, &data[0], 11);
    xil_printf("\nCount : %0d\n", count);
    }

    xil_printf("DATA SENT SUCCESSFUL\n");
    cleanup_platform();
    return 0;
    
}
