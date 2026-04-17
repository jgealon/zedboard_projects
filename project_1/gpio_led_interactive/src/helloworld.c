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
  #include <xstatus.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xgpio.h"
  #include "xparameters.h"

  XGpio gpio;
  u8 led_state = 0x0;

  void gpio_init(){
      int status = XGpio_Initialize(&gpio, 0);
      if (status == XST_SUCCESS)
          xil_printf("Device INIT Successful.\n\r");
      else
          xil_printf("Device INIT Failed.\n\r");
  }

  void print_menu(){
      xil_printf("\n\r=== LED Control Menu ===\n\r");
      xil_printf("Press 1-8 to toggle LED 1-8\n\r");
      xil_printf("Press 0 to turn off all LEDs\n\r");
      xil_printf("Press 9 to turn on all LEDs\n\r");
      xil_printf("Current LED state: 0x%02x\n\r", led_state);
      xil_printf("Enter command: ");
  }

  int main()
  {
      char input;
      u8 led_bit;

      init_platform();
      gpio_init();
      XGpio_SetDataDirection(&gpio, 1, 0x0);
      XGpio_DiscreteWrite(&gpio, 1, 0x0);

      xil_printf("\n\r*** ZedBoard Interactive LED Controller ***\n\r");
      print_menu();

      while (1){
          input = getchar();

          if (input >= '1' && input <= '8') {
              led_bit = input - '1';
              led_state ^= (1 << led_bit);
              XGpio_DiscreteWrite(&gpio, 1, led_state);
              xil_printf("%c\n\rLED %d toggled. State: 0x%02x\n\r", input, led_bit + 1,
  led_state);
          }
          else if (input == '0') {
              led_state = 0x0;
              XGpio_DiscreteWrite(&gpio, 1, led_state);
              xil_printf("0\n\rAll LEDs OFF. State: 0x%02x\n\r", led_state);
          }
          else if (input == '9') {
              led_state = 0xff;
              XGpio_DiscreteWrite(&gpio, 1, led_state);
              xil_printf("9\n\rAll LEDs ON. State: 0x%02x\n\r", led_state);
          }
          else if (input == 'm' || input == 'M') {
              print_menu();
          }
          else if (input != '\r' && input != '\n') {
              xil_printf("%c\n\rInvalid input! Press 'm' for menu.\n\r", input);
          }

          xil_printf("> ");
      }

      return 0;
  }


