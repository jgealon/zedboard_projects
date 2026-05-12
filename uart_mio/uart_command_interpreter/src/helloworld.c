 #include <stdio.h>
  #include <string.h>
  #include <xil_types.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xuartps.h"
  #include "xparameters.h"
  #include "xstatus.h"

  XUartPs_Config *uart_config;
  XUartPs uart;

  #define CMD_BUFFER_SIZE 64
  char cmd_buffer[CMD_BUFFER_SIZE];
  u32 cmd_index = 0;
  u32 led_state = 0;
  u32 counter = 0;

  void uart_init() {
      s32 status;
      uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
      if (uart_config != NULL) {
          status = XUartPs_CfgInitialize(&uart, uart_config,
  uart_config->BaseAddress);
          if (status == XST_SUCCESS) {
              XUartPs_SetBaudRate(&uart, 115200);
          }
      }
  }

  void send_string(const char *str) {
      XUartPs_Send(&uart, (u8*)str, strlen(str));
  }

  void print_banner() {
      send_string("\r\n=================================\r\n");
      send_string("  ZedBoard Command Interpreter\r\n");
      send_string("=================================\r\n");
      send_string("Commands: help, led, status, count, reset\r\n");
      send_string("> ");
  }

  void process_command(char *cmd) {
      char response[128];

      if (strcmp(cmd, "help") == 0) {
          send_string("\r\nCommands:\r\n");
          send_string("  help   - Show this help\r\n");
          send_string("  led    - Toggle LED\r\n");
          send_string("  status - Show system status\r\n");
          send_string("  count  - Show counter value\r\n");
          send_string("  reset  - Reset counter\r\n\r\n");

      } else if (strcmp(cmd, "led") == 0) {
          led_state = !led_state;
          sprintf(response, "LED is now %s\r\n", led_state ? "ON" : "OFF");
          send_string(response);

      } else if (strcmp(cmd, "status") == 0) {
          send_string("\r\n--- System Status ---\r\n");
          sprintf(response, "LED State: %s\r\n", led_state ? "ON" : "OFF");
          send_string(response);
          sprintf(response, "Counter: %u\r\n", counter);
          send_string(response);
          send_string("---------------------\r\n");

      } else if (strcmp(cmd, "count") == 0) {
          sprintf(response, "Counter: %u\r\n", counter);
          send_string(response);

      } else if (strcmp(cmd, "reset") == 0) {
          counter = 0;
          send_string("Counter reset to 0\r\n");

      } else if (strlen(cmd) > 0) {
          send_string("Unknown command. Type 'help'\r\n");
      }

      send_string("> ");
  }

  int main() {
      u8 recv_byte;

      init_platform();
      uart_init();
      print_banner();

      while (1) {
          if (XUartPs_Recv(&uart, &recv_byte, 1) == 1) {

              if (recv_byte == '\r' || recv_byte == '\n') {
                  send_string("\r\n");
                  cmd_buffer[cmd_index] = '\0';
                  process_command(cmd_buffer);
                  cmd_index = 0;

              } else if (recv_byte == 0x7F || recv_byte == 0x08) {
                  // Backspace
                  if (cmd_index > 0) {
                      cmd_index--;
                      send_string("\b \b");
                  }

              } else if (recv_byte >= 32 && recv_byte <= 126) {
                  if (cmd_index < CMD_BUFFER_SIZE - 1) {
                      cmd_buffer[cmd_index++] = recv_byte;
                      XUartPs_Send(&uart, &recv_byte, 1);
                  }
              }
          }

          counter++;
      }

      cleanup_platform();
      return 0;
  }
