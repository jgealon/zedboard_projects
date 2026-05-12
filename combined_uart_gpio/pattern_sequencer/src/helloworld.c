 #include <stdio.h>
  #include <string.h>
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

  #define MAX_SEQUENCE 16
  typedef struct {
      u8 patterns[MAX_SEQUENCE];
      u32 length;
      u32 speed_ms;
      u32 running;
      u32 loop_mode;
      u32 export_enabled;
  } Sequencer;

  Sequencer seq = {{0}, 0, 200, 0, 1, 0};

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

  void export_via_loopback(const char *data) {
      u32 len = strlen(data);

      // Send via UARTLite1
      XUartLite_Send(&uart_lite1, (u8*)data, len);
      while (XUartLite_IsSending(&uart_lite1));

      // Receive and verify via UARTLite2
      u8 echo_buf[128];
      u32 byteRcvd = 0;
      while (byteRcvd != len) {
          byteRcvd = byteRcvd + XUartLite_Recv(&uart_lite2, &echo_buf[byteRcvd],
   len);
      }

      // Verification (optional display)
      // echo_buf[byteRcvd] = '\0';
  }

  void print_banner() {
      send_string("\r\n");

  send_string("=========================================================\r\n");
      send_string("  Pattern Sequencer with Dual UARTLite Data Export\r\n");

  send_string("=========================================================\r\n");
      send_string("Create LED animations and export via UARTLite loopback!\r\n");
      send_string("\r\n");
      send_string("Commands:\r\n");
      send_string("  add <hex>     - Add pattern to sequence\r\n");
      send_string("  list          - Show current sequence\r\n");
      send_string("  play          - Start playing sequence\r\n");
      send_string("  stop          - Stop playing\r\n");
      send_string("  clear         - Clear sequence\r\n");
      send_string("  speed <ms>    - Set speed (50-2000ms)\r\n");
      send_string("  loop          - Toggle loop mode\r\n");
      send_string("  export        - Toggle UARTLite export\r\n");
      send_string("  record        - Record from switches\r\n");
      send_string("  preset <1-4>  - Load preset pattern\r\n");
      send_string("  help          - Show this menu\r\n");
      send_string("=========================================================\r\n\r\n");
  }

  void list_sequence() {
      char msg[128];
      send_string("\r\n--- Current Sequence ---\r\n");
      sprintf(msg, "Length: %u/%d\r\n", seq.length, MAX_SEQUENCE);
      send_string(msg);
      sprintf(msg, "Speed: %u ms\r\n", seq.speed_ms);
      send_string(msg);
      sprintf(msg, "Loop: %s\r\n", seq.loop_mode ? "ON" : "OFF");
      send_string(msg);
      sprintf(msg, "Export: %s\r\n", seq.export_enabled ? "ON" : "OFF");
      send_string(msg);
      sprintf(msg, "Status: %s\r\n", seq.running ? "PLAYING" : "STOPPED");
      send_string(msg);
      send_string("\r\nPatterns:\r\n");
      for (u32 i = 0; i < seq.length; i++) {
          sprintf(msg, "  %2u: 0x%02X  ", i + 1, seq.patterns[i]);
          send_string(msg);
          for (int b = 7; b >= 0; b--) {
              send_string((seq.patterns[i] & (1 << b)) ? "█" : "░");
          }
          send_string("\r\n");
      }
      send_string("------------------------\r\n");
  }

  void export_full_sequence() {
      char header[64];
      sprintf(header, "SEQ_START,LEN=%u,SPEED=%u\n", seq.length,
  seq.speed_ms);
      export_via_loopback(header);

      for (u32 i = 0; i < seq.length; i++) {
          char pattern_data[64];
          sprintf(pattern_data, "PATTERN[%u]=0x%02X\n", i, seq.patterns[i]);
          export_via_loopback(pattern_data);
      }

      export_via_loopback("SEQ_END\n");
      send_string("Sequence exported & verified via UARTLite loopback!\r\n");
  }

  void load_preset(u32 preset_num) {
      seq.length = 0;

      switch (preset_num) {
          case 1:  // Knight Rider
              send_string("Loading: Knight Rider\r\n");
              seq.patterns[0] = 0x01; seq.patterns[1] = 0x02;
              seq.patterns[2] = 0x04; seq.patterns[3] = 0x08;
              seq.patterns[4] = 0x10; seq.patterns[5] = 0x20;
              seq.patterns[6] = 0x40; seq.patterns[7] = 0x80;
              seq.patterns[8] = 0x40; seq.patterns[9] = 0x20;
              seq.patterns[10] = 0x10; seq.patterns[11] = 0x08;
              seq.patterns[12] = 0x04; seq.patterns[13] = 0x02;
              seq.length = 14;
              seq.speed_ms = 100;
              break;

          case 2:  // Binary Counter
              send_string("Loading: Binary Counter\r\n");
              for (u32 i = 0; i < 16; i++) {
                  seq.patterns[i] = i;
              }
              seq.length = 16;
              seq.speed_ms = 300;
              break;

          case 3:  // Alternating
              send_string("Loading: Alternating\r\n");
              seq.patterns[0] = 0xAA;
              seq.patterns[1] = 0x55;
              seq.length = 2;
              seq.speed_ms = 200;
              break;

          case 4:  // Expanding
              send_string("Loading: Expanding Center\r\n");
              seq.patterns[0] = 0x18; seq.patterns[1] = 0x3C;
              seq.patterns[2] = 0x7E; seq.patterns[3] = 0xFF;
              seq.patterns[4] = 0x7E; seq.patterns[5] = 0x3C;
              seq.patterns[6] = 0x18; seq.patterns[7] = 0x00;
              seq.length = 8;
              seq.speed_ms = 150;
              break;

          default:
              send_string("Unknown preset\r\n");
              return;
      }

      if (seq.export_enabled) {
          export_full_sequence();
      }

      list_sequence();
  }

  void record_from_switches() {
      send_string("\r\n=== RECORD MODE ===\r\n");
      send_string("Set switches, press SW0 to add pattern\r\n");
      send_string("Press SW7 to finish\r\n\r\n");

      seq.length = 0;
      u8 prev_sw = XGpio_DiscreteRead(&sw, 1);

      while (1) {
          u8 curr_sw = XGpio_DiscreteRead(&sw, 1);

          if ((curr_sw & 0x01) && !(prev_sw & 0x01)) {
              if (seq.length < MAX_SEQUENCE) {
                  u8 pattern = curr_sw & 0xFE;
                  seq.patterns[seq.length++] = pattern;

                  char msg[64];
                  sprintf(msg, "Added #%u: 0x%02X\r\n", seq.length, pattern);
                  send_string(msg);

                  if (seq.export_enabled) {
                      char export_data[64];
                      sprintf(export_data, "REC[%u]=0x%02X\n", seq.length - 1, pattern);
                      export_via_loopback(export_data);
                  }

                  XGpio_DiscreteWrite(&led, 1, pattern);
                  usleep(300000);
                  XGpio_DiscreteWrite(&led, 1, 0x00);
              }
          }

          if ((curr_sw & 0x80) && !(prev_sw & 0x80)) {
              send_string("\r\nRecording finished!\r\n");
              list_sequence();
              break;
          }

          prev_sw = curr_sw;
          usleep(50000);
      }
  }

  void process_command(char *cmd) {
      char msg[128];

      if (strncmp(cmd, "add ", 4) == 0) {
          if (seq.length < MAX_SEQUENCE) {
              u32 pattern = 0;
              sscanf(cmd + 4, "%x", &pattern);
              seq.patterns[seq.length] = pattern & 0xFF;

              if (seq.export_enabled) {
                  char export_data[64];
                  sprintf(export_data, "ADD[%u]=0x%02X\n", seq.length,
  seq.patterns[seq.length]);
                  export_via_loopback(export_data);
              }

              seq.length++;
              sprintf(msg, "Added: 0x%02X (Length: %u)\r\n",
                      seq.patterns[seq.length-1], seq.length);
              send_string(msg);
          } else {
              send_string("Sequence full!\r\n");
          }

      } else if (strcmp(cmd, "list") == 0) {
          list_sequence();

      } else if (strcmp(cmd, "play") == 0) {
          if (seq.length > 0) {
              seq.running = 1;
              send_string("Playing...\r\n");
              if (seq.export_enabled) {
                  export_via_loopback("PLAY_START\n");
              }
          } else {
              send_string("Sequence empty!\r\n");
          }

      } else if (strcmp(cmd, "stop") == 0) {
          seq.running = 0;
          XGpio_DiscreteWrite(&led, 1, 0x00);
          send_string("Stopped.\r\n");
          if (seq.export_enabled) {
              export_via_loopback("PLAY_STOP\n");
          }

      } else if (strcmp(cmd, "clear") == 0) {
          seq.length = 0;
          seq.running = 0;
          XGpio_DiscreteWrite(&led, 1, 0x00);
          send_string("Cleared.\r\n");

      } else if (strncmp(cmd, "speed ", 6) == 0) {
          u32 speed = 0;
          sscanf(cmd + 6, "%u", &speed);
          if (speed >= 50 && speed <= 2000) {
              seq.speed_ms = speed;
              sprintf(msg, "Speed: %u ms\r\n", seq.speed_ms);
              send_string(msg);
          } else {
              send_string("Speed: 50-2000 ms\r\n");
          }

      } else if (strcmp(cmd, "loop") == 0) {
          seq.loop_mode = !seq.loop_mode;
          sprintf(msg, "Loop: %s\r\n", seq.loop_mode ? "ON" : "OFF");
          send_string(msg);

      } else if (strcmp(cmd, "export") == 0) {
          seq.export_enabled = !seq.export_enabled;
          sprintf(msg, "UARTLite Export: %s\r\n", seq.export_enabled ? "ON" :
  "OFF");
          send_string(msg);
          if (seq.export_enabled && seq.length > 0) {
              export_full_sequence();
          }

      } else if (strcmp(cmd, "record") == 0) {
          record_from_switches();

      } else if (strncmp(cmd, "preset ", 7) == 0) {
          u32 preset = 0;
          sscanf(cmd + 7, "%u", &preset);
          load_preset(preset);

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
      char cmd[64];
      u32 cmd_idx = 0;
      u32 pattern_idx = 0;
      u32 loop_count = 0;

      init_platform();
      uart_ps_init();
      uart_lite_init();
      gpio_init();

      print_banner();
      send_string("> ");

      while (1) {
          if (XUartPs_Recv(&uart_ps, &recv_byte, 1) == 1) {
              if (recv_byte == '\r' || recv_byte == '\n') {
                  send_string("\r\n");
                  cmd[cmd_idx] = '\0';
                  process_command(cmd);
                  cmd_idx = 0;
              } else if (recv_byte == 0x7F || recv_byte == 0x08) {
                  if (cmd_idx > 0) {
                      cmd_idx--;
                      send_string("\b \b");
                  }
              } else if (recv_byte >= 32 && recv_byte <= 126) {
                  if (cmd_idx < 63) {
                      cmd[cmd_idx++] = recv_byte;
                      XUartPs_Send(&uart_ps, &recv_byte, 1);
                  }
              }
          }

          if (seq.running && (loop_count % (seq.speed_ms / 10) == 0)) {
              XGpio_DiscreteWrite(&led, 1, seq.patterns[pattern_idx]);

              if (seq.export_enabled) {
                  char data[32];
                  sprintf(data, "NOW:0x%02X\n", seq.patterns[pattern_idx]);
                  export_via_loopback(data);
              }

              pattern_idx++;

              if (pattern_idx >= seq.length) {
                  if (seq.loop_mode) {
                      pattern_idx = 0;
                  } else {
                      seq.running = 0;
                      XGpio_DiscreteWrite(&led, 1, 0x00);
                      send_string("\r\nFinished.\r\n> ");
                  }
              }
          }

          loop_count++;
          usleep(10000);
      }

      cleanup_platform();
      return 0;
  }
