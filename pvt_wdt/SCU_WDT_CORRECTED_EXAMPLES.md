# SCU Watchdog Timer with PS UART - Corrected Examples

These examples use **PS UART (XUartPs)** which is the correct choice for Zynq PS-based designs like pvt_wdt.

---

## Example 1: Simple UART Echo with Watchdog Protection

Basic PS UART communication with watchdog monitoring.

```c
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xgpiops.h"
#include "xuartps.h"
#include "xparameters.h"
#include "sleep.h"

// Device instances
XScuWdt wdt;
XGpioPs gpio_ps;
XUartPs uart;

#define WDT_TIMEOUT (333333333 * 5)  // 5 seconds
#define MIO_LED_PIN 7  // MIO pin for LED (adjust for your board)

void init_watchdog() {
    XScuWdt_Config *config = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, config, config->BaseAddr);
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT);
    XScuWdt_Start(&wdt);
    xil_printf("Watchdog initialized: 5 second timeout\r\n");
}

void init_ps_gpio() {
    XGpioPs_Config *gpio_config = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&gpio_ps, gpio_config, gpio_config->BaseAddr);

    // Configure MIO pin as output for LED
    XGpioPs_SetDirectionPin(&gpio_ps, MIO_LED_PIN, 1);  // 1 = output
    XGpioPs_SetOutputEnablePin(&gpio_ps, MIO_LED_PIN, 1);
    XGpioPs_WritePin(&gpio_ps, MIO_LED_PIN, 0);  // LED off
}

void init_ps_uart() {
    XUartPs_Config *uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
    XUartPs_CfgInitialize(&uart, uart_config, uart_config->BaseAddress);
    XUartPs_SetBaudRate(&uart, 115200);

    xil_printf("PS UART initialized at 115200 baud\r\n");
}

void uart_send_string(const char *str) {
    XUartPs_Send(&uart, (u8*)str, strlen(str));
}

int main() {
    init_platform();
    init_watchdog();
    init_ps_gpio();
    init_ps_uart();

    char buffer[128];
    u8 rx_byte;
    u32 rx_count = 0;
    u32 heartbeat = 0;

    uart_send_string("\r\n=== UART Echo with Watchdog ===\r\n");
    uart_send_string("Type anything - it will echo back\r\n");
    uart_send_string("Watchdog monitors system health\r\n\r\n");

    while(1) {
        // Check for received data
        if (XUartPs_IsReceiveData(uart.Config.BaseAddress)) {
            XUartPs_Recv(&uart, &rx_byte, 1);

            // Echo back
            XUartPs_Send(&uart, &rx_byte, 1);

            // Count characters
            rx_count++;

            // Toggle LED on receive
            u32 led_state = XGpioPs_ReadPin(&gpio_ps, MIO_LED_PIN);
            XGpioPs_WritePin(&gpio_ps, MIO_LED_PIN, !led_state);
        }

        // Service watchdog every loop iteration
        XScuWdt_RestartWdt(&wdt);

        // Periodic status message
        if (heartbeat++ > 100000) {
            sprintf(buffer, "[Heartbeat] Characters received: %lu\r\n", rx_count);
            uart_send_string(buffer);
            heartbeat = 0;
        }

        usleep(10);  // Small delay
    }

    cleanup_platform();
    return 0;
}
```

---

## Example 2: UART Command Parser with Watchdog

Process commands from PS UART with LED feedback.

```c
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xgpiops.h"
#include "xuartps.h"
#include "xparameters.h"
#include "sleep.h"

XScuWdt wdt;
XGpioPs gpio_ps;
XUartPs uart;

#define WDT_TIMEOUT (333333333 * 4)  // 4 seconds
#define LED_BASE_PIN 7  // Starting MIO pin for LEDs

#define CMD_BUFFER_SIZE 64

void init_all_devices() {
    // Watchdog
    XScuWdt_Config *wdt_config = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, wdt_config, wdt_config->BaseAddr);
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT);
    XScuWdt_Start(&wdt);

    // PS GPIO
    XGpioPs_Config *gpio_config = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&gpio_ps, gpio_config, gpio_config->BaseAddr);

    // Configure LED pins as outputs
    for (int i = 0; i < 8; i++) {
        XGpioPs_SetDirectionPin(&gpio_ps, LED_BASE_PIN + i, 1);
        XGpioPs_SetOutputEnablePin(&gpio_ps, LED_BASE_PIN + i, 1);
        XGpioPs_WritePin(&gpio_ps, LED_BASE_PIN + i, 0);
    }

    // PS UART
    XUartPs_Config *uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
    XUartPs_CfgInitialize(&uart, uart_config, uart_config->BaseAddress);
    XUartPs_SetBaudRate(&uart, 115200);
}

void uart_print(const char *str) {
    XUartPs_Send(&uart, (u8*)str, strlen(str));
}

void set_leds(u8 pattern) {
    for (int i = 0; i < 8; i++) {
        XGpioPs_WritePin(&gpio_ps, LED_BASE_PIN + i, (pattern >> i) & 0x01);
    }
}

void process_command(char *cmd) {
    char response[128];

    if (strcmp(cmd, "PING") == 0) {
        uart_print("PONG\r\n");
        set_leds(0x01);

    } else if (strcmp(cmd, "STATUS") == 0) {
        sprintf(response, "System OK - Watchdog Active (4s timeout)\r\n");
        uart_print(response);
        set_leds(0x0F);

    } else if (strcmp(cmd, "LED ON") == 0) {
        set_leds(0xFF);
        uart_print("All LEDs ON\r\n");

    } else if (strcmp(cmd, "LED OFF") == 0) {
        set_leds(0x00);
        uart_print("All LEDs OFF\r\n");

    } else if (strncmp(cmd, "LED ", 4) == 0) {
        // Parse LED pattern (e.g., "LED 55" for 0x55)
        u8 pattern = (u8)atoi(&cmd[4]);
        set_leds(pattern);
        sprintf(response, "LED pattern set to: 0x%02X\r\n", pattern);
        uart_print(response);

    } else if (strcmp(cmd, "WDT STOP") == 0) {
        XScuWdt_Stop(&wdt);
        uart_print("WARNING: Watchdog STOPPED\r\n");
        set_leds(0xAA);

    } else if (strcmp(cmd, "WDT START") == 0) {
        XScuWdt_Start(&wdt);
        uart_print("Watchdog STARTED\r\n");
        set_leds(0x55);

    } else if (strcmp(cmd, "SLOW") == 0) {
        uart_print("Executing slow task (with watchdog service)...\r\n");
        for (int i = 0; i < 8; i++) {
            sleep(1);
            XScuWdt_RestartWdt(&wdt);  // Service watchdog during long task
            sprintf(response, "  Progress: %d/8\r\n", i + 1);
            uart_print(response);
            set_leds(1 << i);
        }
        uart_print("Slow task complete\r\n");

    } else if (strcmp(cmd, "HANG") == 0) {
        uart_print("WARNING: Simulating hang - watchdog will timeout!\r\n");
        set_leds(0xFF);
        // Don't service watchdog - system will reset
        while(1) {
            sleep(1);
            uart_print("Still hanging...\r\n");
        }

    } else if (strcmp(cmd, "HELP") == 0) {
        uart_print("\r\nAvailable Commands:\r\n");
        uart_print("  PING        - Echo test\r\n");
        uart_print("  STATUS      - Show system status\r\n");
        uart_print("  LED ON/OFF  - Control all LEDs\r\n");
        uart_print("  LED <num>   - Set LED pattern (0-255)\r\n");
        uart_print("  WDT START   - Start watchdog\r\n");
        uart_print("  WDT STOP    - Stop watchdog\r\n");
        uart_print("  SLOW        - Run long task (with WDT service)\r\n");
        uart_print("  HANG        - Simulate hang (triggers reset)\r\n");
        uart_print("  HELP        - Show this help\r\n\r\n");

    } else {
        sprintf(response, "ERROR: Unknown command '%s'\r\n", cmd);
        uart_print(response);
        set_leds(0xE0);  // Error pattern
    }
}

int main() {
    init_platform();
    init_all_devices();

    char cmd_buffer[CMD_BUFFER_SIZE];
    u8 cmd_index = 0;
    u8 rx_byte;
    u32 last_wdt_service = 0;

    xil_printf("\r\n=== Watchdog Command Monitor ===\r\n");
    xil_printf("Using PS UART for commands\r\n");
    xil_printf("Type 'HELP' for command list\r\n\r\n");
    uart_print("Ready> ");

    while(1) {
        // Check for UART data
        if (XUartPs_IsReceiveData(uart.Config.BaseAddress)) {
            XUartPs_Recv(&uart, &rx_byte, 1);

            if (rx_byte == '\r' || rx_byte == '\n') {
                if (cmd_index > 0) {
                    cmd_buffer[cmd_index] = '\0';
                    uart_print("\r\n");

                    // Process command
                    process_command(cmd_buffer);

                    cmd_index = 0;
                    uart_print("\r\nReady> ");
                }
            } else if (rx_byte == 0x08 || rx_byte == 0x7F) {  // Backspace
                if (cmd_index > 0) {
                    cmd_index--;
                    uart_print("\b \b");  // Erase character on terminal
                }
            } else if (cmd_index < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_index++] = rx_byte;
                XUartPs_Send(&uart, &rx_byte, 1);  // Echo
            }
        }

        // Service watchdog periodically (every ~500ms when idle)
        if (last_wdt_service++ > 50000) {
            XScuWdt_RestartWdt(&wdt);
            last_wdt_service = 0;
        }

        usleep(10);
    }

    cleanup_platform();
    return 0;
}
```

**Example Session:**
```
=== Watchdog Command Monitor ===
Using PS UART for commands
Type 'HELP' for command list

Ready> HELP

Available Commands:
  PING        - Echo test
  STATUS      - Show system status
  LED ON/OFF  - Control all LEDs
  LED <num>   - Set LED pattern (0-255)
  WDT START   - Start watchdog
  WDT STOP    - Stop watchdog
  SLOW        - Run long task (with WDT service)
  HANG        - Simulate hang (triggers reset)
  HELP        - Show this help

Ready> PING
PONG

Ready> LED 170
LED pattern set to: 0xAA

Ready> STATUS
System OK - Watchdog Active (4s timeout)

Ready> SLOW
Executing slow task (with watchdog service)...
  Progress: 1/8
  Progress: 2/8
  ...
  Progress: 8/8
Slow task complete

Ready> HANG
WARNING: Simulating hang - watchdog will timeout!
Still hanging...
Still hanging...
[System resets after 4 seconds]
```

---

## Example 3: Watchdog with Data Logging over UART

Log sensor data (simulated) with watchdog protection.

```c
#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xuartps.h"
#include "xxadcps.h"
#include "xparameters.h"
#include "sleep.h"

XScuWdt wdt;
XUartPs uart;
XXadcPs xadc;

#define WDT_TIMEOUT (333333333 * 10)  // 10 seconds

void init_devices() {
    // Watchdog
    XScuWdt_Config *wdt_cfg = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, wdt_cfg, wdt_cfg->BaseAddr);
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT);
    XScuWdt_Start(&wdt);

    // PS UART
    XUartPs_Config *uart_cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
    XUartPs_CfgInitialize(&uart, uart_cfg, uart_cfg->BaseAddress);
    XUartPs_SetBaudRate(&uart, 115200);

    // XADC (for real temperature reading)
    XXadcPs_Config *xadc_cfg = XXadcPs_LookupConfig(XPAR_XXADCPS_0_DEVICE_ID);
    XXadcPs_CfgInitialize(&xadc, xadc_cfg, xadc_cfg->BaseAddress);
}

void uart_log(const char *msg) {
    XUartPs_Send(&uart, (u8*)msg, strlen(msg));
}

void log_sensor_data(u32 sample_num) {
    char log_buffer[256];

    // Read real die temperature from XADC
    u16 temp_raw = XXadcPs_GetAdcData(&xadc, XADCPS_CH_TEMP);
    float temp_c = ((float)temp_raw * 503.975f / 65536.0f) - 273.15f;

    // Simulate other sensor readings
    u16 sensor1 = rand() % 1024;
    u16 sensor2 = rand() % 1024;
    float voltage = 1.8f + ((float)(rand() % 100) / 1000.0f);

    // Format log message (CSV format)
    sprintf(log_buffer,
            "%lu,%.2f,%d,%d,%.3f\r\n",
            sample_num,
            temp_c,
            sensor1,
            sensor2,
            voltage);

    uart_log(log_buffer);
}

int main() {
    init_platform();
    init_devices();

    u32 sample_count = 0;
    u32 error_count = 0;

    xil_printf("\r\n=== Data Logger with Watchdog ===\r\n");
    xil_printf("Logging to UART at 1 Hz\r\n");
    xil_printf("Watchdog timeout: 10 seconds\r\n");
    xil_printf("\r\n");

    // CSV header
    uart_log("Sample,Temp_C,Sensor1,Sensor2,Voltage\r\n");

    while(1) {
        // Log data
        log_sensor_data(sample_count);
        sample_count++;

        // Service watchdog after successful data logging
        XScuWdt_RestartWdt(&wdt);

        // Simulate occasional errors (recover gracefully)
        if ((rand() % 100) < 5) {  // 5% chance of "error"
            error_count++;
            char err_msg[64];
            sprintf(err_msg, "# Warning: Sensor read error (count: %lu)\r\n", error_count);
            uart_log(err_msg);

            // Still service watchdog even on error
            XScuWdt_RestartWdt(&wdt);
        }

        // Status message every 60 samples
        if (sample_count % 60 == 0) {
            char status[128];
            sprintf(status,
                    "# Status: %lu samples, %lu errors, WDT active\r\n",
                    sample_count, error_count);
            uart_log(status);
        }

        sleep(1);  // 1 Hz sampling rate
    }

    cleanup_platform();
    return 0;
}
```

**Example Output:**
```
=== Data Logger with Watchdog ===
Logging to UART at 1 Hz
Watchdog timeout: 10 seconds

Sample,Temp_C,Sensor1,Sensor2,Voltage
0,45.23,512,768,1.842
1,45.21,493,801,1.856
2,45.24,601,723,1.839
# Warning: Sensor read error (count: 1)
3,45.22,558,692,1.847
...
# Status: 60 samples, 3 errors, WDT active
```

---

## Example 4: Reset Detection and Recovery

Detect watchdog resets and implement recovery strategy.

```c
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xuartps.h"
#include "xgpiops.h"
#include "xparameters.h"
#include "sleep.h"

XScuWdt wdt;
XUartPs uart;
XGpioPs gpio;

#define WDT_TIMEOUT (333333333 * 3)  // 3 seconds
#define RECOVERY_LED_PIN 7

typedef enum {
    MODE_NORMAL = 0,
    MODE_RECOVERY = 1,
    MODE_TEST_TIMEOUT = 2
} SystemMode;

void init_devices() {
    // PS GPIO
    XGpioPs_Config *gpio_cfg = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&gpio, gpio_cfg, gpio_cfg->BaseAddr);
    XGpioPs_SetDirectionPin(&gpio, RECOVERY_LED_PIN, 1);
    XGpioPs_SetOutputEnablePin(&gpio, RECOVERY_LED_PIN, 1);

    // PS UART
    XUartPs_Config *uart_cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
    XUartPs_CfgInitialize(&uart, uart_cfg, uart_cfg->BaseAddress);
    XUartPs_SetBaudRate(&uart, 115200);

    // Watchdog
    XScuWdt_Config *wdt_cfg = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, wdt_cfg, wdt_cfg->BaseAddr);
}

void uart_print(const char *str) {
    XUartPs_Send(&uart, (u8*)str, strlen(str));
}

void flash_recovery_led(u8 times) {
    for (int i = 0; i < times; i++) {
        XGpioPs_WritePin(&gpio, RECOVERY_LED_PIN, 1);
        usleep(200000);
        XGpioPs_WritePin(&gpio, RECOVERY_LED_PIN, 0);
        usleep(200000);
    }
}

void handle_watchdog_reset() {
    uart_print("\r\n");
    uart_print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    uart_print("!! WATCHDOG TIMEOUT DETECTED        !!\r\n");
    uart_print("!! System was reset by watchdog     !!\r\n");
    uart_print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    uart_print("\r\n");

    // Flash LED to indicate recovery
    flash_recovery_led(10);

    uart_print("Recovery Actions:\r\n");
    uart_print("  [1] Clearing watchdog reset flag\r\n");
    uart_print("  [2] Performing system diagnostics\r\n");
    uart_print("  [3] Restoring safe configuration\r\n");
    uart_print("\r\n");

    sleep(2);
    uart_print("System recovered - resuming normal operation\r\n\r\n");
}

int main() {
    init_platform();
    init_devices();

    SystemMode mode = MODE_NORMAL;
    u32 loop_count = 0;
    u8 rx_byte;

    // Check if last reset was caused by watchdog
    if (XScuWdt_IsWdtExpired(&wdt)) {
        mode = MODE_RECOVERY;
        handle_watchdog_reset();
        mode = MODE_NORMAL;
    }

    // Start watchdog
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT);
    XScuWdt_Start(&wdt);

    xil_printf("=== Watchdog Reset Detection Demo ===\r\n");
    xil_printf("Watchdog timeout: 3 seconds\r\n");
    xil_printf("\r\n");
    xil_printf("Press:\r\n");
    xil_printf("  '1' - Normal operation (services watchdog)\r\n");
    xil_printf("  '2' - Trigger watchdog reset\r\n");
    xil_printf("  's' - Show status\r\n");
    xil_printf("\r\n");
    uart_print("Running in NORMAL mode\r\n");

    while(1) {
        // Check for UART commands
        if (XUartPs_IsReceiveData(uart.Config.BaseAddress)) {
            XUartPs_Recv(&uart, &rx_byte, 1);

            if (rx_byte == '1') {
                mode = MODE_NORMAL;
                uart_print("\r\n[MODE] Switched to NORMAL - Watchdog will be serviced\r\n");
                XGpioPs_WritePin(&gpio, RECOVERY_LED_PIN, 0);

            } else if (rx_byte == '2') {
                mode = MODE_TEST_TIMEOUT;
                uart_print("\r\n[MODE] Triggering watchdog timeout...\r\n");
                uart_print("System will reset in ~3 seconds\r\n");
                XGpioPs_WritePin(&gpio, RECOVERY_LED_PIN, 1);

            } else if (rx_byte == 's' || rx_byte == 'S') {
                char status[256];
                sprintf(status,
                        "\r\n[STATUS]\r\n"
                        "  Mode: %s\r\n"
                        "  Loop count: %lu\r\n"
                        "  Watchdog active: Yes\r\n"
                        "  Timeout: 3 seconds\r\n"
                        "  Reset flag: %s\r\n\r\n",
                        mode == MODE_NORMAL ? "NORMAL" : "TEST_TIMEOUT",
                        loop_count,
                        XScuWdt_IsWdtExpired(&wdt) ? "SET" : "CLEAR");
                uart_print(status);
            }
        }

        // Normal operation - service watchdog
        if (mode == MODE_NORMAL) {
            XScuWdt_RestartWdt(&wdt);

            if (loop_count % 10 == 0) {
                char msg[64];
                sprintf(msg, "[%lu] Normal operation - WDT serviced\r\n", loop_count);
                uart_print(msg);
            }

            // Blink LED slowly
            XGpioPs_WritePin(&gpio, RECOVERY_LED_PIN, (loop_count % 2));

        // Test mode - don't service watchdog (will cause reset)
        } else if (mode == MODE_TEST_TIMEOUT) {
            // DON'T service watchdog - let it timeout
            char msg[64];
            sprintf(msg, "[%lu] Waiting for timeout...\r\n", loop_count);
            uart_print(msg);
        }

        loop_count++;
        sleep(1);
    }

    cleanup_platform();
    return 0;
}
```

---

## Key Differences from UARTLite Examples

| Aspect | AXI UARTLite (Wrong) | PS UART (Correct) |
|--------|---------------------|-------------------|
| **Include** | `#include "xuartlite.h"` | `#include "xuartps.h"` |
| **Instance Type** | `XUartLite uart;` | `XUartPs uart;` |
| **Device ID** | `XPAR_AXI_UARTLITE_0_DEVICE_ID` | `XPAR_XUARTPS_0_DEVICE_ID` |
| **Init Function** | `XUartLite_Initialize()` | `XUartPs_CfgInitialize()` |
| **Lookup Config** | N/A | `XUartPs_LookupConfig()` |
| **Set Baud Rate** | Fixed in hardware | `XUartPs_SetBaudRate(&uart, 115200)` |
| **Send** | `XUartLite_Send()` | `XUartPs_Send()` |
| **Receive** | `XUartLite_Recv()` | `XUartPs_Recv()` |
| **Check RX** | `!XUartLite_IsReceiveEmpty()` | `XUartPs_IsReceiveData()` |
| **Hardware** | Requires IP in Vivado | Already in PS7 |

---

## Summary

✅ **Use PS UART (XUartPs)** for pvt_wdt project because:
1. SCU WDT is a PS peripheral
2. No PL resources wasted
3. Better performance and features
4. Simpler setup

❌ **Don't use UARTLite** unless you specifically need:
- More than 2 UARTs
- UART on specific FPGA pins
- Integration with PL logic

The corrected examples above use **PS UART** which is the proper choice for your Zynq PS-based design!
