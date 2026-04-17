 // binary_trainer.c
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

  // Simple PRNG
  static u32 rng_state = 54321;

  u32 simple_rand() {
      rng_state = (rng_state * 1103515245 + 12345) & 0x7fffffff;
      return rng_state;
  }

  u8 getRandomNumber(int max) {
      return (u8)(simple_rand() % max);
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

  void displayBinary(u8 value) {
      for (int i = 7; i >= 0; i--) {
          xil_printf("%d", (value >> i) & 1);
      }
  }

  void celebrationPattern() {
      for (int i = 0; i < 3; i++) {
          XGpio_DiscreteWrite(&gpio, 2, 0xFF);
          usleep(100000);
          XGpio_DiscreteWrite(&gpio, 2, 0x00);
          usleep(100000);
      }
  }

  int main() {
      u8 targetNumber = 0;
      u8 swValue = 0;
      u8 prevSwValue = 0;
      int score = 0;
      int attempts = 0;
      int gameMode = 0;
      int difficulty = 0;
      int waiting = 0;
      u32 lastChangeCounter = 0;
      u32 currentCounter = 0;
      int settled = 0;

      init_platform();
      gpio_init();

      XGpio_SetDataDirection(&gpio, 1, 0xff);
      XGpio_SetDataDirection(&gpio, 2, 0x00);

      xil_printf("\n\r======================================\n\r");
      xil_printf("   BINARY TRAINER / QUIZ GAME\n\r");
      xil_printf("======================================\n\r\n\r");

      xil_printf("GAME MODES (SW6-SW7):\n\r");
      xil_printf("  00 = Decimal->Binary: Set switches to match number\n\r");
      xil_printf("  01 = Binary->Decimal: What number do LEDs show?\n\r");
      xil_printf("  10 = Free Play: LEDs mirror switches\n\r");
      xil_printf("  11 = Pattern Match: Copy the LED pattern\n\r");
      xil_printf("\n\rDIFFICULTY (SW5):\n\r");
      xil_printf("  DOWN = Easy (0-15, 4 bits)\n\r");
      xil_printf("  UP = Hard (0-255, 8 bits)\n\r");
      xil_printf("\n\rCONTROLS:\n\r");
      xil_printf("  SW0 = Submit Answer / Next Question\n\r");
      xil_printf("  SW1 = Show Answer / Hint\n\r");
      xil_printf("  SW2-SW4 = (Not used in all modes)\n\r");
      xil_printf("\n\rPress SW0 to START!\n\r\n\r");

      // Wait for start
      while (!(XGpio_DiscreteRead(&gpio, 1) & 0x01)) {
          usleep(100000);
      }
      while (XGpio_DiscreteRead(&gpio, 1) & 0x01) {
          usleep(100000);
      }

      xil_printf("GAME START!\n\r\n\r");
      sleep(1);

      // Generate first question
      targetNumber = getRandomNumber(16);

      while (1) {
          swValue = XGpio_DiscreteRead(&gpio, 1);
          currentCounter++;

          // Get game mode and difficulty
          gameMode = (swValue >> 6) & 0x03;
          difficulty = (swValue >> 5) & 0x01;

          // Update max range based on difficulty
          int maxNum = difficulty ? 256 : 16;

          // Mode 0: Decimal to Binary
          if (gameMode == 0) {
              if (!waiting) {
                  targetNumber = getRandomNumber(maxNum);
                  xil_printf("----------------------------------\n\r");
                  xil_printf("Question %d: Convert %d to binary\n\r", attempts + 1, targetNumber);
                  xil_printf("Set switches to match this number!\n\r");
                  if (!difficulty) {
                      xil_printf("(Using only SW2-SW5 for 4-bit mode)\n\r");
                  }
                  xil_printf("Press SW0 when ready to submit.\n\r\n\r");
                  waiting = 1;
                  settled = 0;
              }

              // Display current switch setting
              u8 currentInput = difficulty ? (swValue & 0xFC) : ((swValue >> 2) & 0x0F);
              xil_printf("\rCurrent: %3d = ", currentInput);
              displayBinary(currentInput);
              xil_printf("  ");

              // Show on LEDs
              XGpio_DiscreteWrite(&gpio, 2, currentInput << (difficulty ? 0 : 2));

              // Detect switch changes
              if (swValue != prevSwValue) {
                  lastChangeCounter = currentCounter;
                  settled = 0;
              }

              // Check if settled (counter difference threshold)
              if ((currentCounter - lastChangeCounter) > 50) {
                  settled = 1;
              }

              // Submit answer with SW0
              if ((swValue & 0x01) && (prevSwValue & 0x01) == 0 && settled) {
                  u8 answer = difficulty ? (swValue & 0xFC) : ((swValue >> 2) & 0x0F);
                  u8 target = difficulty ? targetNumber : (targetNumber & 0x0F);

                  xil_printf("\n\r");
                  attempts++;

                  if (answer == target) {
                      score++;
                      xil_printf("CORRECT! %d = ", target);
                      displayBinary(target);
                      xil_printf("\n\r");
                      celebrationPattern();
                  } else {
                      xil_printf("WRONG! You entered: %d = ", answer);
                      displayBinary(answer);
                      xil_printf("\n\r");
                      xil_printf("Correct answer: %d = ", target);
                      displayBinary(target);
                      xil_printf("\n\r");
                  }

                  xil_printf("Score: %d/%d (%d%%)\n\r\n\r", score, attempts,
                            (score * 100) / attempts);

                  sleep(2);
                  waiting = 0;
              }

              // Hint with SW1
              if ((swValue & 0x02) && !(prevSwValue & 0x02)) {
                  xil_printf("\n\rHINT: ");
                  displayBinary(difficulty ? targetNumber : (targetNumber & 0x0F));
                  xil_printf("\n\r\n\r");
              }
          }

          // Mode 1: Binary to Decimal
          else if (gameMode == 1) {
              if (!waiting) {
                  targetNumber = getRandomNumber(maxNum);
                  XGpio_DiscreteWrite(&gpio, 2, targetNumber);
                  xil_printf("----------------------------------\n\r");
                  xil_printf("Question %d: What number is shown?\n\r", attempts + 1);
                  xil_printf("LEDs show: ");
                  displayBinary(targetNumber);
                  xil_printf("\n\rUse SW2-SW7 to enter decimal value.\n\r");
                  xil_printf("Press SW0 to submit.\n\r\n\r");
                  waiting = 1;
              }

              // Read answer from switches
              u8 currentInput = (swValue >> 2) & 0x3F; // 6 bits = 0-63
              xil_printf("\rYour answer: %3d  ", currentInput);

              if ((swValue & 0x01) && !(prevSwValue & 0x01)) {
                  xil_printf("\n\r");
                  attempts++;

                  if (currentInput == targetNumber) {
                      score++;
                      xil_printf("CORRECT! ");
                      displayBinary(targetNumber);
                      xil_printf(" = %d\n\r", targetNumber);
                      celebrationPattern();
                  } else {
                      xil_printf("WRONG! You entered: %d\n\r", currentInput);
                      xil_printf("Correct answer: ");
                      displayBinary(targetNumber);
                      xil_printf(" = %d\n\r", targetNumber);
                  }

                  xil_printf("Score: %d/%d (%d%%)\n\r\n\r", score, attempts,
                            (score * 100) / attempts);

                  sleep(2);
                  waiting = 0;
              }
          }

          // Mode 2: Free Play
          else if (gameMode == 2) {
              u8 display = swValue;
              XGpio_DiscreteWrite(&gpio, 2, display);
              xil_printf("\rBinary: ");
              displayBinary(display);
              xil_printf(" = Decimal: %3d  ", display);
          }

          // Mode 3: Pattern Match
          else if (gameMode == 3) {
              if (!waiting) {
                  targetNumber = getRandomNumber(maxNum);
                  xil_printf("----------------------------------\n\r");
                  xil_printf("Pattern Match %d!\n\r", attempts + 1);
                  xil_printf("Memorize this pattern:\n\r");

                  // Show pattern 3 times
                  for (int i = 0; i < 3; i++) {
                      XGpio_DiscreteWrite(&gpio, 2, targetNumber);
                      displayBinary(targetNumber);
                      xil_printf(" ");
                      sleep(1);
                      XGpio_DiscreteWrite(&gpio, 2, 0x00);
                      sleep(1);
                  }

                  xil_printf("\n\rSet switches to match!\n\r");
                  xil_printf("Press SW0 to submit.\n\r\n\r");
                  waiting = 1;
              }

              u8 currentInput = swValue & 0xFC;
              xil_printf("\rYour pattern: ");
              displayBinary(currentInput);
              xil_printf("  ");
              XGpio_DiscreteWrite(&gpio, 2, currentInput);

              if ((swValue & 0x01) && !(prevSwValue & 0x01)) {
                  xil_printf("\n\r");
                  attempts++;

                  if (currentInput == targetNumber) {
                      score++;
                      xil_printf("PERFECT MATCH!\n\r");
                      celebrationPattern();
                  } else {
                      xil_printf("NO MATCH!\n\r");
                      xil_printf("Correct: ");
                      displayBinary(targetNumber);
                      xil_printf("\n\r");
                      XGpio_DiscreteWrite(&gpio, 2, targetNumber);
                      sleep(2);
                  }

                  xil_printf("Score: %d/%d (%d%%)\n\r\n\r", score, attempts,
                            (score * 100) / attempts);

                  sleep(1);
                  waiting = 0;
              }
          }

          prevSwValue = swValue;
          usleep(10000); // 10ms loop delay
      }

      cleanup_platform();
      return 0;
  }
