// knight_rider.c                                                                                                                                                                                                 #include <stdio.h>
  #include <stdlib.h>  // Added for rand() function
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

  // LED patterns for different effects
  u8 getKnightRiderPattern(int position, int width) {
      u8 pattern = 0;
      for (int i = 0; i < width && (position + i) < 8; i++) {
          pattern |= (1 << (position + i));
      }
      return pattern;
  }

  u8 getCylonPattern(int position, int width) {
      // Center-weighted pattern
      u8 pattern = (1 << position);
      if (width > 1 && position > 0) pattern |= (1 << (position - 1));
      if (width > 2 && position < 7) pattern |= (1 << (position + 1));
      return pattern;
  }

  u8 getBouncingPattern(int position) {
      return (1 << position) | (1 << (7 - position));
  }

  int main() {
      int position = 0;
      int direction = 1; // 1 = right, -1 = left
      u8 swValue = 0;
      int speed = 100;
      int width = 1;
      int effect = 0;

      init_platform();
      gpio_init();

      // Seed random number generator
      srand(12345); // Use a fixed seed or use time-based if available

      XGpio_SetDataDirection(&gpio, 1, 0xff); // Channel 1 = input (switches)
      XGpio_SetDataDirection(&gpio, 2, 0x00); // Channel 2 = output (LEDs)

      xil_printf("\n\r=== Knight Rider / Cylon Scanner ===\n\r");
      xil_printf("SW0-SW2: Speed Control (000=Fastest ... 111=Slowest)\n\r");
      xil_printf("SW3-SW4: LED Width (00=1 LED, 01=2 LEDs, 10=3 LEDs, 11=4 LEDs)\n\r");
      xil_printf("SW5-SW7: Effect Mode\n\r");
      xil_printf("  000 = Classic Knight Rider\n\r");
      xil_printf("  001 = Cylon (center-weighted)\n\r");
      xil_printf("  010 = Bouncing Dual\n\r");
      xil_printf("  011 = Chase Left\n\r");
      xil_printf("  100 = Chase Right\n\r");
      xil_printf("  101 = Expand/Contract\n\r");
      xil_printf("  110 = Random Sparkle\n\r");
      xil_printf("  111 = All Patterns Cycle\n\r\n\r");

      while (1) {
          swValue = XGpio_DiscreteRead(&gpio, 1);

          // SW0-SW2: Speed control (0=fastest, 7=slowest)
          int speedMode = swValue & 0x07;
          speed = 50 + (speedMode * 50); // 50ms to 400ms

          // SW3-SW4: Width control
          width = ((swValue >> 3) & 0x03) + 1; // 1 to 4 LEDs

          // SW5-SW7: Effect selection
          effect = (swValue >> 5) & 0x07;

          u8 ledPattern = 0;

          switch(effect) {
              case 0: // Classic Knight Rider
                  ledPattern = getKnightRiderPattern(position, width);
                  break;

              case 1: // Cylon (center-weighted)
                  ledPattern = getCylonPattern(position, width);
                  break;

              case 2: // Bouncing Dual
                  ledPattern = getBouncingPattern(position);
                  break;

              case 3: // Chase Left
                  ledPattern = (0xFF >> (8 - width)) << position;
                  direction = -1;
                  break;

              case 4: // Chase Right
                  ledPattern = (0xFF >> (8 - width)) << position;
                  direction = 1;
                  break;

              case 5: // Expand/Contract
                  {
                      static int expanding = 1;
                      int center = 3;
                      ledPattern = 0;
                      for (int i = 0; i < position; i++) {
                          if (center - i >= 0) ledPattern |= (1 << (center - i));
                          if (center + i <= 7) ledPattern |= (1 << (center + i));
                      }
                      if (position >= 4) expanding = 0;
                      if (position <= 0) expanding = 1;
                      direction = expanding ? 1 : -1;
                  }
                  break;

              case 6: // Random Sparkle
                  ledPattern = (u8)(rand() % 256);
                  break;

              case 7: // Cycle through all effects
                  {
                      static int cycleEffect = 0;
                      static int cycleCount = 0;
                      cycleCount++;
                      if (cycleCount > 20) {
                          cycleEffect = (cycleEffect + 1) % 6;
                          cycleCount = 0;
                          xil_printf("Effect: %d\n\r", cycleEffect);
                      }
                      // Recursive-like pattern selection
                      switch(cycleEffect) {
                          case 0: ledPattern = getKnightRiderPattern(position, 2); break;
                          case 1: ledPattern = getCylonPattern(position, 3); break;
                          case 2: ledPattern = getBouncingPattern(position); break;
                          case 3: ledPattern = (1 << position); break;
                          case 4: ledPattern = (0x03 << position); break;
                          case 5: ledPattern = (0xFF << position) & 0xFF; break;
                      }
                  }
                  break;
          }

          // Write pattern to LEDs
          XGpio_DiscreteWrite(&gpio, 2, ledPattern);

          // Update position
          position += direction;

          // Bounce at edges
          if (position >= (8 - width + 1)) {
              position = 8 - width;
              direction = -1;
          } else if (position < 0) {
              position = 0;
              direction = 1;
          }

          usleep(speed * 1000);
      }

      cleanup_platform();
      return 0;
  }
