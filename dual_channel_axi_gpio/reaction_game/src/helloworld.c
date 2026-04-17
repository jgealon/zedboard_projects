 // reaction_game.c
  #include <stdio.h>
  #include <stdlib.h>
  #include <xstatus.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xgpio.h"
  #include "xparameters.h"
  #include "sleep.h"

  XGpio_Config *gpio_config;
  XGpio gpio;

  // Simple counter for pseudo-random
  static u32 random_seed = 12345;

  u32 simple_rand() {
      random_seed = (random_seed * 1103515245 + 12345) & 0x7fffffff;
      return random_seed;
  }

  u32 getRandomDelay() {
      return (simple_rand() % 2000) + 1000; // 1-3 seconds in ms
  }

  u8 getRandomLED() {
      return (1 << (simple_rand() % 8));
  }

  void gpio_init(){
      int status;
      gpio_config = XGpio_LookupConfig(0);
      status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);
      if (status == XST_SUCCESS)
          xil_printf("Device INIT Successful.\n");
      else
          xil_printf("Device INIT Failed.\n");
  }

  // Simple millisecond counter (approximate)
  volatile u32 millisCounter = 0;

  u32 getMillis() {
      return millisCounter;
  }

  int main() {
      u8 targetLED = 0;
      u8 swValue = 0;
      u8 prevSwValue = 0;
      int score = 0;
      int misses = 0;
      int gameMode = 0;
      int round = 0;
      int difficulty = 1;
      u32 startTime, endTime;
      u32 reactionTimeMs;
      int waitingForResponse = 0;
      u32 timeout = 3000; // Changed to u32

      init_platform();
      gpio_init();

      XGpio_SetDataDirection(&gpio, 1, 0xff);
      XGpio_SetDataDirection(&gpio, 2, 0x00);

      xil_printf("\n\r=== REACTION TIME GAME ===\n\r");
      xil_printf("Instructions:\n\r");
      xil_printf("- A random LED will light up\n\r");
      xil_printf("- Flip the corresponding switch UP as fast as you can!\n\r");
      xil_printf("- Flip it back DOWN for next round\n\r");
      xil_printf("\n\rDifficulty (SW6-SW7):\n\r");
      xil_printf("  00 = Easy (5s timeout)\n\r");
      xil_printf("  01 = Normal (3s timeout)\n\r");
      xil_printf("  10 = Hard (2s timeout)\n\r");
      xil_printf("  11 = Expert (1s timeout)\n\r");
      xil_printf("\n\rGame Mode (SW5):\n\r");
      xil_printf("  DOWN = Sequential (LED0->LED7)\n\r");
      xil_printf("  UP = Random LEDs\n\r");
      xil_printf("\n\rPress SW0 to START!\n\r");
      xil_printf("Press SW1 to RESET score\n\r\n\r");

      // Wait for start button
      while (1) {
          swValue = XGpio_DiscreteRead(&gpio, 1);
          if (swValue & 0x01) {
              xil_printf("GAME START!\n\r\n\r");
              sleep(1);
              break;
          }
      }

      millisCounter = 0;

      while (1) {
          swValue = XGpio_DiscreteRead(&gpio, 1);
          millisCounter++; // Increment counter (approximate timing)

          // Check for reset (SW1)
          if ((swValue & 0x02) && !(prevSwValue & 0x02)) {
              score = 0;
              misses = 0;
              round = 0;
              xil_printf("\n\r=== SCORE RESET ===\n\r\n\r");
          }

          // Get difficulty from SW6-SW7
          difficulty = (swValue >> 6) & 0x03;
          timeout = 5000 - (difficulty * 1000); // Changed to u32: 5000, 4000, 3000, 2000
          if (difficulty == 3) timeout = 1000; // Expert mode

          // Get game mode from SW5
          gameMode = (swValue >> 5) & 0x01;

          if (!waitingForResponse) {
              // New round - light up LED
              round++;

              if (gameMode == 0) {
                  // Sequential mode
                  targetLED = 1 << (round % 8);
              } else {
                  // Random mode
                  targetLED = getRandomLED();
              }

              // Random delay before showing LED
              u32 delay = getRandomDelay();
              xil_printf("Round %d: Get ready", round);
              for (int i = 0; i < 3; i++) {
                  xil_printf(".");
                  usleep(delay * 333); // divide delay into 3 parts
              }
              xil_printf("\n\r");

              // Show target LED
              XGpio_DiscreteWrite(&gpio, 2, targetLED);
              startTime = millisCounter;
              waitingForResponse = 1;

              // Count leading zeros to find LED position
              int ledPos = 0;
              u8 temp = targetLED;
              while (!(temp & 1)) {
                  ledPos++;
                  temp >>= 1;
              }
              xil_printf("GO! Hit LED position %d (SW%d)\n\r", ledPos, ledPos);
          }

          if (waitingForResponse) {
              swValue = XGpio_DiscreteRead(&gpio, 1);

              // Check if correct switch was flipped
              if (swValue & targetLED) {
                  endTime = millisCounter;
                  reactionTimeMs = (endTime - startTime) / 10; // Approximate ms

                  score++;
                  xil_printf("*** CORRECT! *** Reaction Time: ~%lu ms\n\r", reactionTimeMs);
                  xil_printf("Score: %d | Misses: %d | Accuracy: %d%%\n\r\n\r",
                            score, misses, (score * 100) / (score + misses));

                  // Turn off LED
                  XGpio_DiscreteWrite(&gpio, 2, 0x00);

                  // Wait for switch to be released
                  xil_printf("Release switch for next round...\n\r");
                  while (XGpio_DiscreteRead(&gpio, 1) & targetLED) {
                      usleep(100000);
                  }

                  waitingForResponse = 0;
                  sleep(1);

              } else {
                  // Check timeout
                  u32 elapsed = (millisCounter - startTime) / 10; // Approximate ms (now u32)

                  if (elapsed > timeout) { // Both u32 now - no warning!
                      misses++;
                      xil_printf("*** TIMEOUT! *** Too slow!\n\r");
                      if (score + misses > 0) {
                          xil_printf("Score: %d | Misses: %d | Accuracy: %d%%\n\r\n\r",
                                    score, misses, (score * 100) / (score + misses));
                      }

                      XGpio_DiscreteWrite(&gpio, 2, 0x00);
                      waitingForResponse = 0;
                      sleep(1);
                  }

                  // Check if wrong switch was hit
                  if ((swValue & 0xFC) && !(swValue & targetLED)) {
                      misses++;
                      xil_printf("*** WRONG! *** Hit wrong switch!\n\r");
                      xil_printf("Score: %d | Misses: %d | Accuracy: %d%%\n\r\n\r",
                                score, misses, (score * 100) / (score + misses));

                      XGpio_DiscreteWrite(&gpio, 2, 0x00);
                      waitingForResponse = 0;
                      sleep(1);
                  }
              }
          }

          prevSwValue = swValue;
          usleep(100); // 100us loop delay (gives ~10000 counts per second)
      }

      cleanup_platform();
      return 0;
  }

