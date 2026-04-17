// binary_counter.c

/*Features Summary:

  Binary Counter:

  - SW0: Pause/Resume
  - SW1: Count up/down
  - SW2-SW3: 4 speed levels
  - SW4: Reset to 0
  - SW5-SW7: 8 different counting patterns (binary, odd/even only, powers of 2, Fibonacci, etc.)
*/

  #include <stdio.h>
  #include <xstatus.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xgpio.h"
  #include "xparameters.h"
  #include "sleep.h"

  XGpio_Config *gpio_config;
  XGpio gpio;

  void gpio_init(){
      int status;
      gpio_config = XGpio_LookupConfig(0); // Device ID is 0 (first GPIO instance)
      status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);
      if (status == XST_SUCCESS)
          xil_printf("Device INIT Successful.\n");
      else
          xil_printf("Device INIT Failed.\n");
  }

  int main() {
      u8 counter = 0;
      u8 prevSwValue = 0;
      u8 swValue = 0;
      int speed = 500; // Default speed in ms
      int direction = 1; // 1 = up, -1 = down
      int paused = 0;

      init_platform();
      gpio_init();

      XGpio_SetDataDirection(&gpio, 1, 0xff); // Channel 1 = input (switches)
      XGpio_SetDataDirection(&gpio, 2, 0x00); // Channel 2 = output (LEDs)

      xil_printf("\n\r=== Binary Counter Application ===\n\r");
      xil_printf("SW0: Pause/Resume\n\r");
      xil_printf("SW1: Change Direction (Up/Down)\n\r");
      xil_printf("SW2-SW3: Speed Control (00=Fast, 01=Normal, 10=Slow, 11=Very Slow)\n\r");
      xil_printf("SW4: Reset Counter\n\r");
      xil_printf("SW5-SW7: Pattern Mode\n\r\n\r");

      while (1) {
          swValue = XGpio_DiscreteRead(&gpio, 1);

          // SW0: Pause/Resume (toggle on change)
          if ((swValue & 0x01) && !(prevSwValue & 0x01)) {
              paused = !paused;
              xil_printf("Counter %s\n\r", paused ? "PAUSED" : "RESUMED");
          }

          // SW1: Direction (toggle on change)
          if ((swValue & 0x02) && !(prevSwValue & 0x02)) {
              direction = -direction;
              xil_printf("Direction: %s\n\r", direction == 1 ? "UP" : "DOWN");
          }

          // SW2-SW3: Speed control
          int speedMode = (swValue >> 2) & 0x03;
          switch(speedMode) {
              case 0: speed = 100; break;   // Fast
              case 1: speed = 500; break;   // Normal
              case 2: speed = 1000; break;  // Slow
              case 3: speed = 2000; break;  // Very Slow
          }

          // SW4: Reset counter
          if (swValue & 0x10) {
              counter = 0;
              xil_printf("Counter RESET\n\r");
          }

          // SW5-SW7: Pattern modes
          int patternMode = (swValue >> 5) & 0x07;

          if (!paused) {
              switch(patternMode) {
                  case 0: // Normal binary count
                      counter = (counter + direction) & 0xFF;
                      break;
                  case 1: // Only odd numbers
                      counter = (counter + (direction * 2)) & 0xFF;
                      counter |= 0x01;
                      break;
                  case 2: // Only even numbers
                      counter = (counter + (direction * 2)) & 0xFE;
                      break;
                  case 3: // Powers of 2
                      if (direction == 1) {
                          if (counter == 0 || counter == 0x80) counter = 1;
                          else counter = counter << 1;
                      } else {
                          if (counter == 0 || counter == 1) counter = 0x80;
                          else counter = counter >> 1;
                      }
                      break;
                  case 4: // Fibonacci-like pattern (mod 256)
                      {
                          static u8 prev1 = 0, prev2 = 1;
                          u8 next = (prev1 + prev2) & 0xFF;
                          prev1 = prev2;
                          prev2 = next;
                          counter = next;
                      }
                      break;
                  default: // Normal count for other modes
                      counter = (counter + direction) & 0xFF;
                      break;
              }

              XGpio_DiscreteWrite(&gpio, 2, counter);
              xil_printf("Counter: %3d (0x%02X) | Mode: %d | Speed: %dms\n\r",
                         counter, counter, patternMode, speed);
          }

          prevSwValue = swValue;
          usleep(speed * 1000);
      }

      cleanup_platform();
      return 0;
  }
