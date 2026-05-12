 #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include <xil_types.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xuartps.h"
  #include "xuartlite.h"
  #include "xgpio.h"
  #include "xparameters.h"
  #include "xstatus.h"
  #include "sleep.h"

  XUartPs_Config *uart_ps_config;
  XUartPs uart_ps;
  XUartLite_Config *uart_config1, *uart_config2;
  XUartLite uart_lite1, uart_lite2;
  XGpio led, sw;

  typedef struct {
      u32 score;
      u32 questions;
      u32 level;
      u32 streak;
  } GameState;

  GameState game = {0, 0, 1, 0};

  void uart_ps_init() {
      s32 status;
      uart_ps_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
      if (uart_ps_config != NULL) {
          status = XUartPs_CfgInitialize(&uart_ps, uart_ps_config,
  uart_ps_config->BaseAddress);
          if (status == XST_SUCCESS) {
              XUartPs_SetBaudRate(&uart_ps, 115200);
          }
      }
  }

  void uart_lite_init() {
      uart_config1 = XUartLite_LookupConfig(XPAR_XUARTLITE_0_BASEADDR);
      int status1 = XUartLite_CfgInitialize(&uart_lite1, uart_config1,
  uart_config1->RegBaseAddr);

      uart_config2 = XUartLite_LookupConfig(XPAR_XUARTLITE_1_BASEADDR);
      int status2 = XUartLite_CfgInitialize(&uart_lite2, uart_config2,
  uart_config2->RegBaseAddr);

      if ((status1 == XST_SUCCESS) && (status2 == XST_SUCCESS)) {
          xil_printf("[OK] UARTLite loopback initialized\n\r");
      } else {
          xil_printf("[ERROR] UARTLite init failed\n\r");
      }
  }

  void gpio_init() {
      XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);
      XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);
      XGpio_SetDataDirection(&led, 1, 0x00);
      XGpio_SetDataDirection(&sw, 1, 0xFF);
  }

  void send_string(const char *str) {
      XUartPs_Send(&uart_ps, (u8*)str, strlen(str));
  }

  void transmit_score_loopback() {
      char score_data[64];
      sprintf(score_data, "SCORE:%u/%u,LVL:%u,STREAK:%u\n", game.score, game.questions, game.level, game.streak);

      u32 len = strlen(score_data);

      // Send via UARTLite1
      XUartLite_Send(&uart_lite1, (u8*)score_data, len);
      while (XUartLite_IsSending(&uart_lite1));

      // Receive via UARTLite2 (loopback verification)
      u8 echo_buf[64];
      u32 byteRcvd = 0;
      while (byteRcvd != len) {
          byteRcvd = byteRcvd + XUartLite_Recv(&uart_lite2, &echo_buf[byteRcvd],
   len);
      }
      echo_buf[byteRcvd] = '\0';

      // Verify loopback
      if (memcmp(score_data, echo_buf, len) == 0) {
          send_string("\r\n[UARTLite] Score transmitted & verified: ");
          send_string((char*)echo_buf);
      } else {
          send_string("\r\n[UARTLite] Loopback verification failed!\r\n");
      }
  }

  void flash_leds(u8 pattern, int times) {
      for (int i = 0; i < times; i++) {
          XGpio_DiscreteWrite(&led, 1, pattern);
          usleep(200000);
          XGpio_DiscreteWrite(&led, 1, 0x00);
          usleep(200000);
      }
  }

  void show_binary_on_leds(u8 value, int duration_ms) {
      XGpio_DiscreteWrite(&led, 1, value);
      usleep(duration_ms * 1000);
  }

  void print_banner() {
      send_string("\r\n");

  send_string("========================================================\r\n");
      send_string("  Binary Quiz Game with Dual UARTLite Score Loopback\r\n");

  send_string("========================================================\r\n");
      send_string("HOW TO PLAY:\r\n");
      send_string("1. Watch the LEDs show a BINARY number\r\n");
      send_string("2. Calculate the DECIMAL value\r\n");
      send_string("3. Set switches to that BINARY value\r\n");
      send_string("4. Type 'check' to verify\r\n");
      send_string("\r\n");
      send_string("NEW: Scores transmitted & verified via UARTLite  loopback!\r\n");
      send_string("\r\n");
      send_string("Commands: start, check, hint, score, quit\r\n");
      send_string("========================================================\r\n\
  r\n");
  }

  u8 generate_question() {
      u8 max_val = (game.level == 1) ? 15 : (game.level == 2) ? 63 : 255;
      return (rand() % max_val) + 1;
  }

  void ask_question(u8 answer) {
      char msg[128];

      send_string("\r\n--- NEW QUESTION ---\r\n");
      sprintf(msg, "Level %u - Question #%u (Streak: %u)\r\n", game.level, game.questions + 1, game.streak);
      send_string(msg);
      send_string("Watch the LEDs!\r\n");

      show_binary_on_leds(answer, 3000);
      XGpio_DiscreteWrite(&led, 1, 0x00);

      send_string("Set switches and type 'check'\r\n");
      send_string("> ");
  }

  void check_answer(u8 correct, u8 user_answer) {
      char msg[128];

      game.questions++;

      if (user_answer == correct) {
          send_string("\r\n===========================================\r\n");
          send_string("      ✓ CORRECT! Great job!               \r\n");
          send_string("===========================================\r\n");
          game.score++;
          game.streak++;

          sprintf(msg, "Current Streak: %u\r\n", game.streak);
          send_string(msg);

          // Transmit score via loopback
          transmit_score_loopback();

          flash_leds(0xFF, 3);

          if (game.score % 5 == 0 && game.level < 3) {
              game.level++;
              sprintf(msg, "\r\n*** LEVEL UP! Now at Level %u ***\r\n", game.level);
              send_string(msg);
              flash_leds(0xAA, 5);
          }
      } else {
          send_string("\r\n===========================================\r\n");
          send_string("      ✗ INCORRECT - Try again!           \r\n");
          send_string("===========================================\r\n");
          sprintf(msg, "Correct: %u, Your answer: %u\r\n", correct,
  user_answer);
          send_string(msg);
          game.streak = 0;

          // Transmit score via loopback
          transmit_score_loopback();

          flash_leds(0x81, 3);
      }

      sprintf(msg, "\r\nScore: %u/%u (%.1f%%)\r\n", game.score, game.questions, (game.questions > 0) ? (100.0 * game.score / game.questions) : 0.0);
      send_string(msg);
  }

  int main() {
      u8 recv_byte;
      char cmd[32];
      u32 cmd_idx = 0;
      u8 current_answer = 0;
      u32 game_active = 0;

      init_platform();
      uart_ps_init();
      uart_lite_init();
      gpio_init();

      srand(12345);

      print_banner();
      send_string("Type 'start' to begin!\r\n> ");

      while (1) {
          if (XUartPs_Recv(&uart_ps, &recv_byte, 1) == 1) {
              if (recv_byte == '\r' || recv_byte == '\n') {
                  send_string("\r\n");
                  cmd[cmd_idx] = '\0';

                  if (strcmp(cmd, "start") == 0) {
                      game_active = 1;
                      game.score = 0;
                      game.questions = 0;
                      game.level = 1;
                      game.streak = 0;
                      current_answer = generate_question();

                      // Send game start notification
                      u8 start_msg[] = "GAME_START\n";
                      XUartLite_Send(&uart_lite1, start_msg, sizeof(start_msg) -
   1);
                      while (XUartLite_IsSending(&uart_lite1));

                      ask_question(current_answer);

                  } else if (strcmp(cmd, "check") == 0 && game_active) {
                      u8 user_answer = XGpio_DiscreteRead(&sw, 1);
                      check_answer(current_answer, user_answer);
                      current_answer = generate_question();
                      usleep(2000000);
                      ask_question(current_answer);

                  } else if (strcmp(cmd, "hint") == 0 && game_active) {
                      send_string("\r\nHINT: Answer is between ");
                      char hint[64];
                      u8 lower = (current_answer > 10) ? current_answer - 10 :
  0;
                      u8 upper = (current_answer < 245) ? current_answer + 10 :
  255;
                      sprintf(hint, "%u and %u\r\n> ", lower, upper);
                      send_string(hint);

                  } else if (strcmp(cmd, "score") == 0) {
                      char msg[128];
                        sprintf(msg, "\r\nScore: %u/%u (%.1f%%) - Level %u -  Streak: %u\r\n> ", game.score, game.questions, (game.questions > 0) ? (100.0 * game.score / game.questions) : 0.0,game.level, game.streak);
                        send_string(msg);
                        transmit_score_loopback();

                  } else if (strcmp(cmd, "quit") == 0) {
                      send_string("\r\nThanks for playing!\r\n");
                      char final[64];
                      sprintf(final, "Final Score: %u/%u\r\n", game.score, game.questions);
                      send_string(final);

                      // Send game end notification
                      u8 end_msg[] = "GAME_END\n";
                      XUartLite_Send(&uart_lite1, end_msg, sizeof(end_msg) - 1);
                      while (XUartLite_IsSending(&uart_lite1));

                      transmit_score_loopback();
                      game_active = 0;
                      send_string("\r\nType 'start' to play again!\r\n> ");

                  } else if (strlen(cmd) > 0) {
                      send_string("Unknown command. Try: start, check, hint, score, quit\r\n> ");
                  }

                  cmd_idx = 0;
              } else if (recv_byte == 0x7F || recv_byte == 0x08) {
                  if (cmd_idx > 0) {
                      cmd_idx--;
                      send_string("\b \b");
                  }
              } else if (recv_byte >= 32 && recv_byte <= 126) {
                  if (cmd_idx < 31) {
                      cmd[cmd_idx++] = recv_byte;
                      XUartPs_Send(&uart_ps, &recv_byte, 1);
                  }
              }
          }
      }

      cleanup_platform();
      return 0;
  }
