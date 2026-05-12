  #include <stdio.h>
  #include <xil_types.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xuartps.h"
  #include "xparameters.h"
  #include "xstatus.h"
  #include "sleep.h"

  XUartPs_Config *uart_config;
  XUartPs uart;

  typedef struct {
      u32 sensor_value;
      u32 temperature;
      u32 voltage;
      u32 timestamp;
  } SensorData;

  SensorData data = {0, 25, 3300, 0};

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

  void print_header() {
      send_string("\r\n");
      send_string("================================================\r\n");
      send_string("       ZedBoard Sensor Data Monitor\r\n");
      send_string("================================================\r\n");
      send_string("Monitoring system parameters every 2 seconds...\r\n");
      send_string("Press 'p' to pause, 'r' to resume, 'q' to quit\r\n");
      send_string("------------------------------------------------\r\n\r\n");
      send_string("Time(s) | Sensor | Temp(C) | Voltage(mV) | Status\r\n");
      send_string("--------|--------|---------|-------------|--------\r\n");
  }

  void simulate_sensors() {
      // Simulate sensor readings
      data.sensor_value = (data.sensor_value + 10) % 1024;
      data.temperature = 20 + (data.sensor_value % 15);
      data.voltage = 3000 + (data.sensor_value % 600);
      data.timestamp++;
  }

  void print_data_row() {
      char buffer[128];
      const char *status;

      // Determine status based on temperature
      if (data.temperature < 25) {
          status = "NORMAL";
      } else if (data.temperature < 30) {
          status = "WARM  ";
      } else {
          status = "HOT!  ";
      }

      sprintf(buffer, "%7u | %6u | %7u | %11u | %s\r\n",
              data.timestamp,
              data.sensor_value,
              data.temperature,
              data.voltage,
              status);

      send_string(buffer);
  }

  void print_summary() {
      char buffer[128];

      send_string("\r\n");
      send_string("=== Data Summary ===\r\n");
      sprintf(buffer, "Total Samples: %u\r\n", data.timestamp);
      send_string(buffer);
      sprintf(buffer, "Last Sensor:   %u\r\n", data.sensor_value);
      send_string(buffer);
      sprintf(buffer, "Last Temp:     %u C\r\n", data.temperature);
      send_string(buffer);
      sprintf(buffer, "Last Voltage:  %u mV\r\n", data.voltage);
      send_string(buffer);
      send_string("====================\r\n\r\n");
  }

  int main() {
      u8 recv_byte;
      u32 paused = 0;
      u32 running = 1;
      u32 loop_count = 0;

      init_platform();
      uart_init();
      print_header();

      while (running) {
          // Check for user input
          if (XUartPs_Recv(&uart, &recv_byte, 1) == 1) {
              if (recv_byte == 'p' || recv_byte == 'P') {
                  paused = 1;
                  send_string("\r\n[PAUSED] Press 'r' to resume...\r\n");

              } else if (recv_byte == 'r' || recv_byte == 'R') {
                  paused = 0;
                  send_string("[RESUMED]\r\n");
                  send_string("Time(s) | Sensor | Temp(C) | Voltage(mV) | Status\r\n");

  send_string("--------|--------|---------|-------------|--------\r\n");

              } else if (recv_byte == 'q' || recv_byte == 'Q') {
                  running = 0;
                  send_string("\r\n[STOPPING] Generating summary...\r\n");
              }
          }

          if (!paused && running) {
              // Update sensors every ~2 seconds
              if (loop_count % 20 == 0) {
                  simulate_sensors();
                  print_data_row();
              }
          }

          loop_count++;
          usleep(100000);  // 100ms delay
      }

      print_summary();
      send_string("Program terminated.\r\n");

      cleanup_platform();
      return 0;
  }

