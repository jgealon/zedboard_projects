#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xuartps.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xuartlite.h"
#include "xstatus.h"
#include "sleep.h"


// UART instances
  XUartPs_Config *uart_config;
  XUartPs uart_ps;              // PS UART for control
  XUartLite_Config *uart_config1, *uart_config2;
  XUartLite uart_lite;  // Two UARTLite for loopback

  // GPIO instances
  XGpio led, sw;

 // Command buffer
  #define CMD_SIZE 64
  char cmd_buffer[CMD_SIZE];
  u32 cmd_index = 0;

  // Loopback stats
  u32 bytes_sent = 0;
  u32 bytes_received = 0;

 void uart_ps_init() {
      s32 status;
      uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
      if (uart_config != NULL) {
          status = XUartPs_CfgInitialize(&uart_ps, uart_config, uart_config->BaseAddress);
          if (status == XST_SUCCESS) {
              XUartPs_SetBaudRate(&uart_ps, 115200);
          }
      }
  }

 void uart_lite_init() {
      s32 status;
      status = XUartLite_Initialize(&uart_lite, XPAR_XUARTLITE_0_BASEADDR);
      if (status == XST_SUCCESS) {
          xil_printf("[OK] UARTLite initialized for loopback\n\r");
      } else {
          xil_printf("[ERROR] UARTLite init failed\n\r");
      }
  }

  void gpio_init() {
      XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);
      XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);
      XGpio_SetDataDirection(&led, 1, 0x00);  // Output
      XGpio_SetDataDirection(&sw, 1, 0xFF);   // Input
  }

  void send_string(const char *str) {
      XUartPs_Send(&uart_ps, (u8*)str, strlen(str));
  }

  void print_banner() {
      send_string("\r\n");
      send_string("===================================================\r\n");
      send_string("  LED Controller with UARTLite Loopback Monitor\r\n");
      send_string("===================================================\r\n");
      send_string("Two UART interfaces:\r\n");
      send_string("  1. PS UART  - Command & Control (this terminal)\r\n");
      send_string("  2. UARTLite - Loopback testing\r\n");
      send_string("\r\n");
      send_string("Commands:\r\n");
      send_string("  led <0-255>   - Set LED pattern (hex)\r\n");
      send_string("  test <text>   - Send text to UARTLite loopback\r\n");
      send_string("  stream        - Stream switch data to loopback\r\n");
      send_string("  stats         - Show loopback statistics\r\n");
      send_string("  mirror        - Mirror switches to LEDs\r\n");
      send_string("  status        - Show system status\r\n");
      send_string("  help          - Show this menu\r\n");

  send_string("===================================================\r\n\r\n");
  }

  void test_loopback(const char *text) {
      char msg[128];
      u32 len = strlen(text);
      u8 recv_buffer[128] = {0};

      send_string("\r\n--- Loopback Test ---\r\n");
      sprintf(msg, "Sending: \"%s\" (%u bytes)\r\n", text, len);
      send_string(msg);

      // Send to UARTLite
      XUartLite_Send(&uart_lite, (u8*)text, len);
      bytes_sent += len;

      // Wait and receive
      usleep(100000);  // 100ms delay
      u32 received = XUartLite_Recv(&uart_lite, recv_buffer, len);
      bytes_received += received;

      recv_buffer[received] = '\0';

      sprintf(msg, "Received: \"%s\" (%u bytes)\r\n", recv_buffer, received);
      send_string(msg);

      if (received == len && memcmp(text, recv_buffer, len) == 0) {
          send_string("Result: ✓ PASS - Loopback successful!\r\n");
          // Flash LEDs on success
          XGpio_DiscreteWrite(&led, 1, 0xFF);
          usleep(200000);
          XGpio_DiscreteWrite(&led, 1, 0x00);
      } else {
          send_string("Result: ✗ FAIL - Data mismatch!\r\n");
          // Flash error pattern
          for (int i = 0; i < 3; i++) {
              XGpio_DiscreteWrite(&led, 1, 0x81);
              usleep(100000);
              XGpio_DiscreteWrite(&led, 1, 0x00);
              usleep(100000);
          }
      }
      send_string("---------------------\r\n");
  }

  void stream_switch_data() {
      char msg[128];
      send_string("\r\n--- Streaming Switch Data ---\r\n");
      send_string("Press any key to stop...\r\n\r\n");

      u8 dummy;
      while (XUartPs_Recv(&uart_ps, &dummy, 1) != 1) {
          u8 sw_val = XGpio_DiscreteRead(&sw, 1);

          // Create data packet
          sprintf(msg, "SW:0x%02X,", sw_val);
          u32 len = strlen(msg);

          // Send to loopback
          XUartLite_Send(&uart_lite, (u8*)msg, len);
          bytes_sent += len;

          // Receive echo
          u8 recv_buf[32];
          usleep(10000);
          u32 received = XUartLite_Recv(&uart_lite, recv_buf, len);
          bytes_received += received;

          // Display on LEDs
          XGpio_DiscreteWrite(&led, 1, sw_val);

          usleep(100000);  // 100ms between samples
      }

      send_string("\r\nStreaming stopped.\r\n");
  }

  void process_command(char *cmd) {
      char response[128];

      if (strncmp(cmd, "led ", 4) == 0) {
          u32 value = 0;
          sscanf(cmd + 4, "%x", &value);
          u8 led_val = value & 0xFF;
          XGpio_DiscreteWrite(&led, 1, led_val);
          sprintf(response, "LED set to 0x%02X\r\n", led_val);
          send_string(response);

      } else if (strncmp(cmd, "test ", 5) == 0) {
          test_loopback(cmd + 5);

      } else if (strcmp(cmd, "stream") == 0) {
          stream_switch_data();

      } else if (strcmp(cmd, "stats") == 0) {
          send_string("\r\n--- UARTLite Statistics ---\r\n");
          sprintf(response, "Bytes Sent:     %u\r\n", bytes_sent);
          send_string(response);
          sprintf(response, "Bytes Received: %u\r\n", bytes_received);
          send_string(response);
          sprintf(response, "Success Rate:   %.1f%%\r\n",
                  (bytes_sent > 0) ? (100.0 * bytes_received / bytes_sent) :
  0.0);
          send_string(response);
          send_string("---------------------------\r\n");

      } else if (strcmp(cmd, "mirror") == 0) {
          send_string("\r\nMirroring switches to LEDs...\r\n");
          send_string("Press any key to stop...\r\n");
          u8 dummy;
          while (XUartPs_Recv(&uart_ps, &dummy, 1) != 1) {
              u8 sw_val = XGpio_DiscreteRead(&sw, 1);
              XGpio_DiscreteWrite(&led, 1, sw_val);
              usleep(50000);
          }
          send_string("Mirror stopped.\r\n");

      } else if (strcmp(cmd, "status") == 0) {
          u8 sw_val = XGpio_DiscreteRead(&sw, 1);
          u8 led_val = XGpio_DiscreteRead(&led, 1);
          send_string("\r\n--- System Status ---\r\n");
          sprintf(response, "LEDs:     0x%02X\r\n", led_val);
          send_string(response);
          sprintf(response, "Switches: 0x%02X\r\n", sw_val);
          send_string(response);
          sprintf(response, "Loopback: %u bytes sent, %u received\r\n",
                  bytes_sent, bytes_received);
          send_string(response);
          send_string("---------------------\r\n");

      } else if (strcmp(cmd, "help") == 0) {
          print_banner();
          return;

      } else if (strlen(cmd) > 0) {
          send_string("Unknown command. Type 'help'\r\n");
      }

      send_string("> ");
  }

  int main() {
      u8 recv_byte;

      init_platform();
      uart_ps_init();
      uart_lite_init();
      gpio_init();

      print_banner();
      send_string("Type 'test hello' to test loopback!\r\n> ");

      while (1) {
          if (XUartPs_Recv(&uart_ps, &recv_byte, 1) == 1) {
              if (recv_byte == '\r' || recv_byte == '\n') {
                  send_string("\r\n");
                  cmd_buffer[cmd_index] = '\0';
                  process_command(cmd_buffer);
                  cmd_index = 0;
              } else if (recv_byte == 0x7F || recv_byte == 0x08) {
                  if (cmd_index > 0) {
                      cmd_index--;
                      send_string("\b \b");
                  }
              } else if (recv_byte >= 32 && recv_byte <= 126) {
                  if (cmd_index < CMD_SIZE - 1) {
                      cmd_buffer[cmd_index++] = recv_byte;
                      XUartPs_Send(&uart_ps, &recv_byte, 1);
                  }
              }
          }
      }

      cleanup_platform();
      return 0;
  }

