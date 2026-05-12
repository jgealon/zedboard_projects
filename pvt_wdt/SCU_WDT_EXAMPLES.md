# SCU Watchdog Timer (SCUWDT) Guide

## What is SCU WDT?

The **SCU Watchdog Timer (SCUWDT)** is part of the ARM Cortex-A9 processor's private peripherals in the Zynq-7000 SoC. Despite the "SCU" (Snoop Control Unit) prefix, it's actually an independent watchdog timer module in the Application Processor Unit (APU).

### Key Features

1. **Dual Operating Modes**:
   - **Watchdog Mode**: System reset on timeout
   - **Timer Mode**: Interrupt generation on timeout

2. **Down Counter**: Counts down from a loaded value to 0

3. **Auto-Reload**: In timer mode, automatically reloads from load register

4. **System Reset Detection**: Can determine if last reset was caused by watchdog timeout

---

## Use Cases

### 1. System Reliability
- Automatically reset system if software hangs or crashes
- Detect and recover from infinite loops or deadlocks
- Ensure critical tasks execute within time constraints

### 2. Periodic Task Monitoring
- Monitor that periodic tasks complete on time
- Implement system health checks
- Timeout detection for communication protocols

### 3. Development and Debugging
- Identify code sections that take too long
- Test system recovery mechanisms
- Monitor application responsiveness

---

## How It Works

### Watchdog Mode
```
1. Load counter with timeout value
2. Start watchdog
3. Application must periodically "kick" (restart) watchdog
4. If counter reaches 0 before restart → System Reset
5. Reset flag set in status register (survives reset)
```

### Timer Mode
```
1. Load counter with period value
2. Start timer
3. When counter reaches 0 → Interrupt generated
4. If auto-reload enabled → Counter reloads automatically
5. Event flag set in interrupt status register
```

---

## Basic API Functions

```c
// Configuration and Initialization
XScuWdt_Config *XScuWdt_LookupConfig(u16 DeviceId);
s32 XScuWdt_CfgInitialize(XScuWdt *InstancePtr, XScuWdt_Config *ConfigPtr, u32 EffectiveAddress);

// Mode Configuration
void XScuWdt_SetWdMode(XScuWdt *InstancePtr);      // Set watchdog mode
void XScuWdt_SetTimerMode(XScuWdt *InstancePtr);   // Set timer mode

// Control Functions
void XScuWdt_Start(XScuWdt *InstancePtr);          // Start the timer
void XScuWdt_Stop(XScuWdt *InstancePtr);           // Stop the timer
void XScuWdt_RestartWdt(XScuWdt *InstancePtr);     // "Kick" the watchdog
void XScuWdt_LoadWdt(XScuWdt *InstancePtr, u32 Value); // Load counter value

// Status Functions
u32 XScuWdt_IsWdtExpired(XScuWdt *InstancePtr);    // Check if watchdog caused reset
u32 XScuWdt_IsTimerExpired(XScuWdt *InstancePtr);  // Check if timer expired
u32 XScuWdt_GetControlReg(XScuWdt *InstancePtr);   // Read control register
```

---

## Timing Calculations

The watchdog uses the CPU private timer clock (typically half of CPU frequency).

For Zynq-7000 with 666.666 MHz CPU clock:
- Private timer clock = 333.333 MHz
- Counter decrements at 333.333 MHz
- Each count = ~3 nanoseconds

**Timeout Formula:**
```
Timeout (seconds) = Counter_Value / 333333333
```

**Examples:**
- Counter = 333333333 → Timeout = 1 second
- Counter = 1666666665 → Timeout = 5 seconds
- Counter = 16666666 → Timeout = 50 milliseconds

---

## Example 1: Watchdog with LED Status Indicator

Monitor system health with LED feedback via UART status messages.

```c
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xgpio.h"
#include "xparameters.h"
#include "sleep.h"

// Device instances
XScuWdt wdt;
XGpio gpio_leds;

// Watchdog timeout: 2 seconds
#define WDT_TIMEOUT_COUNT (333333333 * 2)

void init_watchdog() {
    XScuWdt_Config *config = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, config, config->BaseAddr);

    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT_COUNT);
    XScuWdt_Start(&wdt);

    xil_printf("Watchdog initialized: 2 second timeout\r\n");
}

void init_leds() {
    XGpio_Initialize(&gpio_leds, XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_SetDataDirection(&gpio_leds, 1, 0x00); // All outputs
    XGpio_DiscreteWrite(&gpio_leds, 1, 0x00);    // All LEDs off
}

int main() {
    init_platform();
    init_watchdog();
    init_leds();

    u8 led_pattern = 0x01;
    u32 loop_count = 0;

    xil_printf("\r\n=== System Health Monitor ===\r\n");
    xil_printf("Watchdog active. System will reset if not serviced.\r\n\r\n");

    while(1) {
        // Update LED pattern (rotating)
        XGpio_DiscreteWrite(&gpio_leds, 1, led_pattern);
        led_pattern = (led_pattern << 1) | (led_pattern >> 7);

        // Service watchdog
        XScuWdt_RestartWdt(&wdt);

        // Status message via UART
        xil_printf("[%d] System OK - Watchdog serviced - LED: 0x%02X\r\n",
                   loop_count++, led_pattern);

        sleep(1);  // 1 second delay

        // Simulate occasional longer task (still within timeout)
        if (loop_count % 5 == 0) {
            xil_printf("  [Processing extended task...]\r\n");
            usleep(500000);  // 500ms additional delay
        }
    }

    cleanup_platform();
    return 0;
}
```

**Expected Output:**
```
=== System Health Monitor ===
Watchdog active. System will reset if not serviced.

[0] System OK - Watchdog serviced - LED: 0x01
[1] System OK - Watchdog serviced - LED: 0x02
[2] System OK - Watchdog serviced - LED: 0x04
[3] System OK - Watchdog serviced - LED: 0x08
[4] System OK - Watchdog serviced - LED: 0x10
  [Processing extended task...]
[5] System OK - Watchdog serviced - LED: 0x20
```

---

## Example 2: Switch-Controlled Watchdog Enable/Disable

Use switches to control watchdog operation and display status on LEDs.

```c
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xgpio.h"
#include "xparameters.h"
#include "sleep.h"

XScuWdt wdt;
XGpio gpio_leds;
XGpio gpio_switches;

#define WDT_TIMEOUT_COUNT (333333333 * 3)  // 3 seconds
#define SW0_WDT_ENABLE    (1 << 0)
#define SW1_LONG_TASK     (1 << 1)

void init_watchdog() {
    XScuWdt_Config *config = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, config, config->BaseAddr);
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT_COUNT);
}

void init_gpio() {
    XGpio_Initialize(&gpio_leds, XPAR_AXI_GPIO_LEDS_DEVICE_ID);
    XGpio_SetDataDirection(&gpio_leds, 1, 0x00);

    XGpio_Initialize(&gpio_switches, XPAR_AXI_GPIO_SWITCHES_DEVICE_ID);
    XGpio_SetDataDirection(&gpio_switches, 1, 0xFF);  // All inputs
}

int main() {
    init_platform();
    init_watchdog();
    init_gpio();

    u8 wdt_enabled = 0;
    u8 prev_switches = 0;
    u32 heartbeat_counter = 0;

    xil_printf("\r\n=== Watchdog Control System ===\r\n");
    xil_printf("SW0: Enable/Disable Watchdog\r\n");
    xil_printf("SW1: Simulate long task (will timeout if WDT enabled)\r\n");
    xil_printf("LEDs: Heartbeat pattern when running\r\n\r\n");

    while(1) {
        u8 switches = XGpio_DiscreteRead(&gpio_switches, 1);

        // Check if SW0 changed (watchdog enable/disable)
        if ((switches & SW0_WDT_ENABLE) && !(prev_switches & SW0_WDT_ENABLE)) {
            if (!wdt_enabled) {
                XScuWdt_Start(&wdt);
                wdt_enabled = 1;
                xil_printf("[INFO] Watchdog ENABLED (3 sec timeout)\r\n");
                XGpio_DiscreteWrite(&gpio_leds, 1, 0xFF);  // All LEDs on briefly
                usleep(200000);
            }
        } else if (!(switches & SW0_WDT_ENABLE) && (prev_switches & SW0_WDT_ENABLE)) {
            if (wdt_enabled) {
                XScuWdt_Stop(&wdt);
                wdt_enabled = 0;
                xil_printf("[INFO] Watchdog DISABLED\r\n");
                XGpio_DiscreteWrite(&gpio_leds, 1, 0x00);  // All LEDs off
                usleep(200000);
            }
        }

        prev_switches = switches;

        // Check if SW1 is active (simulate long task)
        if (switches & SW1_LONG_TASK) {
            xil_printf("[WARNING] Long task started (5 seconds)...\r\n");
            XGpio_DiscreteWrite(&gpio_leds, 1, 0xAA);  // Pattern indicating long task

            if (wdt_enabled) {
                xil_printf("[WARNING] This will cause watchdog timeout!\r\n");
            }

            sleep(5);  // 5 seconds - exceeds 3 second watchdog timeout!
            xil_printf("[INFO] Long task completed\r\n");
        }

        // Normal operation
        if (wdt_enabled) {
            XScuWdt_RestartWdt(&wdt);  // Kick the watchdog
        }

        // Heartbeat LED pattern
        u8 heartbeat = (1 << (heartbeat_counter % 8));
        XGpio_DiscreteWrite(&gpio_leds, 1, heartbeat);
        heartbeat_counter++;

        xil_printf(".");  // Simple heartbeat on UART
        if (heartbeat_counter % 10 == 0) {
            xil_printf(" [%d] WDT:%s\r\n",
                       heartbeat_counter,
                       wdt_enabled ? "ON" : "OFF");
        }

        usleep(300000);  // 300ms delay
    }

    cleanup_platform();
    return 0;
}
```

---

## Example 3: UART Command Monitor with Watchdog Protection

Monitor UART commands with watchdog protection and LED status feedback.

```c
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xgpio.h"
#include "xuartlite.h"
#include "xparameters.h"
#include "sleep.h"

XScuWdt wdt;
XGpio gpio_leds;
XUartLite uart;

#define WDT_TIMEOUT_COUNT (333333333 * 5)  // 5 seconds
#define BUFFER_SIZE 32

typedef enum {
    STATUS_OK = 0x01,
    STATUS_WAITING = 0x02,
    STATUS_PROCESSING = 0x04,
    STATUS_ERROR = 0x08,
    STATUS_TIMEOUT_WARNING = 0x10
} SystemStatus;

void init_devices() {
    // Watchdog
    XScuWdt_Config *config = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, config, config->BaseAddr);
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT_COUNT);
    XScuWdt_Start(&wdt);

    // LEDs
    XGpio_Initialize(&gpio_leds, XPAR_AXI_GPIO_LEDS_DEVICE_ID);
    XGpio_SetDataDirection(&gpio_leds, 1, 0x00);

    // UART
    XUartLite_Initialize(&uart, XPAR_AXI_UARTLITE_0_DEVICE_ID);
}

void update_status_leds(SystemStatus status) {
    XGpio_DiscreteWrite(&gpio_leds, 1, status);
}

void process_command(char *cmd) {
    update_status_leds(STATUS_PROCESSING);

    if (strncmp(cmd, "PING", 4) == 0) {
        xil_printf("PONG\r\n");
        update_status_leds(STATUS_OK);

    } else if (strncmp(cmd, "STATUS", 6) == 0) {
        xil_printf("System OK - Watchdog Active\r\n");
        update_status_leds(STATUS_OK);

    } else if (strncmp(cmd, "SLOW", 4) == 0) {
        xil_printf("Processing slow task...\r\n");
        update_status_leds(STATUS_TIMEOUT_WARNING);

        // Simulate slow task with multiple watchdog kicks
        for (int i = 0; i < 10; i++) {
            sleep(1);
            XScuWdt_RestartWdt(&wdt);  // Keep kicking watchdog
            xil_printf("  Progress: %d0%%\r\n", i+1);
        }

        xil_printf("Slow task complete\r\n");
        update_status_leds(STATUS_OK);

    } else if (strncmp(cmd, "HANG", 4) == 0) {
        xil_printf("WARNING: Simulating hang (watchdog will timeout)...\r\n");
        update_status_leds(STATUS_ERROR);

        // Intentionally don't kick watchdog - system will reset
        while(1) {
            sleep(1);
            xil_printf("Hanging... (no watchdog service)\r\n");
        }

    } else {
        xil_printf("ERROR: Unknown command\r\n");
        update_status_leds(STATUS_ERROR);
        sleep(1);
        update_status_leds(STATUS_OK);
    }
}

int main() {
    init_platform();
    init_devices();

    char rx_buffer[BUFFER_SIZE];
    u8 rx_index = 0;

    xil_printf("\r\n=== UART Command Monitor with Watchdog ===\r\n");
    xil_printf("Watchdog timeout: 5 seconds\r\n");
    xil_printf("\nCommands:\r\n");
    xil_printf("  PING   - Simple echo test\r\n");
    xil_printf("  STATUS - System status\r\n");
    xil_printf("  SLOW   - Long task with watchdog service\r\n");
    xil_printf("  HANG   - Simulate hang (triggers watchdog reset)\r\n");
    xil_printf("\nReady> ");

    update_status_leds(STATUS_WAITING);
    u32 last_kick_time = 0;

    while(1) {
        // Check for UART data
        if (!XUartLite_IsReceiveEmpty(uart.RegBaseAddress)) {
            char received;
            XUartLite_Recv(&uart, (u8*)&received, 1);

            if (received == '\r' || received == '\n') {
                if (rx_index > 0) {
                    rx_buffer[rx_index] = '\0';
                    xil_printf("\r\n");

                    process_command(rx_buffer);

                    rx_index = 0;
                    xil_printf("\nReady> ");
                    update_status_leds(STATUS_WAITING);
                }
            } else if (rx_index < BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = received;
                xil_printf("%c", received);  // Echo
            }
        }

        // Periodic watchdog service (every 500ms when waiting)
        if (last_kick_time++ > 5000) {
            XScuWdt_RestartWdt(&wdt);
            last_kick_time = 0;
        }

        usleep(100);  // Small delay
    }

    cleanup_platform();
    return 0;
}
```

**Example UART Session:**
```
=== UART Command Monitor with Watchdog ===
Watchdog timeout: 5 seconds

Commands:
  PING   - Simple echo test
  STATUS - System status
  SLOW   - Long task with watchdog service
  HANG   - Simulate hang (triggers watchdog reset)

Ready> PING
PONG

Ready> STATUS
System OK - Watchdog Active

Ready> SLOW
Processing slow task...
  Progress: 10%
  Progress: 20%
  Progress: 30%
  ...
  Progress: 100%
Slow task complete

Ready> HANG
WARNING: Simulating hang (watchdog will timeout)...
Hanging... (no watchdog service)
Hanging... (no watchdog service)
Hanging... (no watchdog service)
[System resets here]
```

---

## Example 4: Multi-Task Monitor with Priority Watchdog Service

Monitor multiple tasks with different priorities and LED status.

```c
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xgpio.h"
#include "xuartlite.h"
#include "xparameters.h"
#include "sleep.h"

XScuWdt wdt;
XGpio gpio_leds;
XGpio gpio_switches;
XUartLite uart;

#define WDT_TIMEOUT (333333333 * 4)  // 4 seconds

typedef struct {
    const char *name;
    u32 execution_count;
    u32 max_time_ms;
    u8 enabled;
    u8 led_bit;
} Task;

Task tasks[] = {
    {"HeartBeat",  0, 100,  1, 0x01},
    {"DataAcq",    0, 500,  1, 0x02},
    {"Processing", 0, 1000, 1, 0x04},
    {"UartTx",     0, 200,  1, 0x08}
};

#define NUM_TASKS (sizeof(tasks) / sizeof(Task))

void init_system() {
    // Watchdog
    XScuWdt_Config *config = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, config, config->BaseAddr);
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT);
    XScuWdt_Start(&wdt);

    // GPIO
    XGpio_Initialize(&gpio_leds, XPAR_AXI_GPIO_LEDS_DEVICE_ID);
    XGpio_SetDataDirection(&gpio_leds, 1, 0x00);

    XGpio_Initialize(&gpio_switches, XPAR_AXI_GPIO_SWITCHES_DEVICE_ID);
    XGpio_SetDataDirection(&gpio_switches, 1, 0xFF);

    // UART
    XUartLite_Initialize(&uart, XPAR_AXI_UARTLITE_0_DEVICE_ID);
}

void execute_task(Task *task) {
    u8 current_leds = XGpio_DiscreteRead(&gpio_leds, 1);
    XGpio_DiscreteWrite(&gpio_leds, 1, current_leds | task->led_bit);

    // Simulate task execution
    usleep(task->max_time_ms * 1000);

    task->execution_count++;

    // Turn off task LED
    current_leds = XGpio_DiscreteRead(&gpio_leds, 1);
    XGpio_DiscreteWrite(&gpio_leds, 1, current_leds & ~task->led_bit);
}

void print_status_report() {
    xil_printf("\r\n=== System Status Report ===\r\n");
    for (int i = 0; i < NUM_TASKS; i++) {
        xil_printf("  %-12s: %s (Runs: %lu, MaxTime: %lums)\r\n",
                   tasks[i].name,
                   tasks[i].enabled ? "ENABLED " : "DISABLED",
                   tasks[i].execution_count,
                   tasks[i].max_time_ms);
    }
    xil_printf("===========================\r\n\r\n");
}

int main() {
    init_platform();
    init_system();

    u32 cycle_count = 0;

    xil_printf("\r\n=== Multi-Task Watchdog Monitor ===\r\n");
    xil_printf("Watchdog timeout: 4 seconds\r\n");
    xil_printf("Each LED represents a task\r\n");
    xil_printf("System must complete all tasks within timeout\r\n\r\n");

    while(1) {
        cycle_count++;

        xil_printf("[Cycle %lu] Starting task execution...\r\n", cycle_count);

        u32 total_time = 0;

        // Execute all enabled tasks
        for (int i = 0; i < NUM_TASKS; i++) {
            if (tasks[i].enabled) {
                xil_printf("  -> %s (Est: %lums)...",
                           tasks[i].name, tasks[i].max_time_ms);

                execute_task(&tasks[i]);
                total_time += tasks[i].max_time_ms;

                xil_printf(" Done\r\n");
            }
        }

        xil_printf("[Cycle %lu] All tasks complete (Total: %lums)\r\n",
                   cycle_count, total_time);

        // Service watchdog after all critical tasks
        XScuWdt_RestartWdt(&wdt);
        xil_printf("[Cycle %lu] Watchdog serviced\r\n\r\n", cycle_count);

        // Check switches for task enable/disable
        u8 switches = XGpio_DiscreteRead(&gpio_switches, 1);
        for (int i = 0; i < NUM_TASKS && i < 4; i++) {
            tasks[i].enabled = (switches & (1 << i)) ? 1 : 0;
        }

        // Status report every 10 cycles
        if (cycle_count % 10 == 0) {
            print_status_report();
        }

        // Check if total time is approaching watchdog timeout
        if (total_time > 3000) {  // More than 3 seconds (timeout is 4)
            xil_printf("[WARNING] Task execution time approaching watchdog timeout!\r\n");
            u8 current_leds = XGpio_DiscreteRead(&gpio_leds, 1);
            XGpio_DiscreteWrite(&gpio_leds, 1, 0xFF);  // All LEDs on as warning
            usleep(500000);
            XGpio_DiscreteWrite(&gpio_leds, 1, current_leds);
        }

        sleep(1);  // Delay between cycles
    }

    cleanup_platform();
    return 0;
}
```

---

## Example 5: Watchdog Reset Detection and Recovery

Detect watchdog-caused resets and take appropriate recovery actions.

```c
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xscuwdt.h"
#include "xgpio.h"
#include "xuartlite.h"
#include "xparameters.h"
#include "sleep.h"

XScuWdt wdt;
XGpio gpio_leds;
XUartLite uart;

#define WDT_TIMEOUT (333333333 * 3)  // 3 seconds
#define RECOVERY_LED_PATTERN 0xAA

void init_system() {
    XGpio_Initialize(&gpio_leds, XPAR_AXI_GPIO_LEDS_DEVICE_ID);
    XGpio_SetDataDirection(&gpio_leds, 1, 0x00);

    XUartLite_Initialize(&uart, XPAR_AXI_UARTLITE_0_DEVICE_ID);
}

void init_watchdog() {
    XScuWdt_Config *config = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
    XScuWdt_CfgInitialize(&wdt, config, config->BaseAddr);
}

void handle_watchdog_reset() {
    xil_printf("\r\n");
    xil_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    xil_printf("!! WATCHDOG RESET DETECTED            !!\r\n");
    xil_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    xil_printf("\r\n");

    // Flash LEDs to indicate recovery
    for (int i = 0; i < 5; i++) {
        XGpio_DiscreteWrite(&gpio_leds, 1, RECOVERY_LED_PATTERN);
        usleep(200000);
        XGpio_DiscreteWrite(&gpio_leds, 1, ~RECOVERY_LED_PATTERN);
        usleep(200000);
    }

    xil_printf("System recovering from watchdog timeout...\r\n");
    xil_printf("Performing recovery actions:\r\n");
    xil_printf("  1. Clearing watchdog reset flag\r\n");
    xil_printf("  2. Reinitializing peripherals\r\n");
    xil_printf("  3. Restoring normal operation\r\n");
    xil_printf("\r\n");

    sleep(2);
}

int main() {
    init_platform();
    init_system();
    init_watchdog();

    // Check if last reset was caused by watchdog
    if (XScuWdt_IsWdtExpired(&wdt)) {
        handle_watchdog_reset();
        // Clear the reset flag - IMPORTANT!
        // The flag persists across resets until manually cleared
    }

    xil_printf("=== Watchdog Reset Detection Demo ===\r\n");
    xil_printf("Commands via UART:\r\n");
    xil_printf("  '1' - Normal operation (services watchdog)\r\n");
    xil_printf("  '2' - Induce watchdog reset (stop servicing)\r\n");
    xil_printf("  '3' - Show reset history\r\n");
    xil_printf("\r\n");

    // Start watchdog
    XScuWdt_Stop(&wdt);
    XScuWdt_SetWdMode(&wdt);
    XScuWdt_LoadWdt(&wdt, WDT_TIMEOUT);
    XScuWdt_Start(&wdt);

    u8 mode = 1;  // Default: normal operation
    u32 loop_count = 0;

    while(1) {
        // Check for UART command
        if (!XUartLite_IsReceiveEmpty(uart.RegBaseAddress)) {
            char cmd;
            XUartLite_Recv(&uart, (u8*)&cmd, 1);

            switch(cmd) {
                case '1':
                    mode = 1;
                    xil_printf("\r\n[Mode] Normal operation - Watchdog serviced\r\n");
                    break;

                case '2':
                    mode = 2;
                    xil_printf("\r\n[Mode] Inducing watchdog reset...\r\n");
                    xil_printf("System will reset in ~3 seconds...\r\n");
                    break;

                case '3':
                    xil_printf("\r\n[Reset History]\r\n");
                    if (XScuWdt_IsWdtExpired(&wdt)) {
                        xil_printf("  Watchdog reset flag: SET\r\n");
                        xil_printf("  (Previous reset was caused by watchdog timeout)\r\n");
                    } else {
                        xil_printf("  Watchdog reset flag: CLEAR\r\n");
                        xil_printf("  (No recent watchdog timeouts)\r\n");
                    }
                    break;
            }
        }

        // Mode 1: Normal operation
        if (mode == 1) {
            XScuWdt_RestartWdt(&wdt);  // Service watchdog

            u8 led_pattern = 1 << (loop_count % 8);
            XGpio_DiscreteWrite(&gpio_leds, 1, led_pattern);

            if (loop_count % 5 == 0) {
                xil_printf("[%lu] Normal operation - WDT serviced\r\n", loop_count);
            }
        }
        // Mode 2: Stop servicing watchdog (will cause reset)
        else if (mode == 2) {
            // Don't service watchdog - let it timeout
            XGpio_DiscreteWrite(&gpio_leds, 1, 0xFF);

            xil_printf("[%lu] Waiting for watchdog timeout...\r\n", loop_count);
        }

        loop_count++;
        sleep(1);
    }

    cleanup_platform();
    return 0;
}
```

---

## Hardware Requirements

### For Examples 1-5:

**Vivado Block Design:**
```
- Processing System (PS7)
  - Enable SCU Watchdog Timer

- AXI GPIO (LEDs)
  - Connect to Zedboard LEDs (LD0-LD7)
  - Channel 1: 8-bit output

- AXI GPIO (Switches) - Examples 2, 4
  - Connect to Zedboard Switches (SW0-SW7)
  - Channel 1: 8-bit input

- AXI UARTLite - Examples 3, 4, 5
  - Connect to USB-UART on Zedboard
  - Baud rate: 115200
```

**Key Point**: The SCU Watchdog Timer is part of the PS7 (Processing System) and doesn't require a separate IP core in Vivado. Just ensure it's enabled in the PS7 configuration.

---

## Common Pitfalls and Best Practices

### ❌ Don't Do This:
```c
// TOO SHORT - May timeout during normal operation
XScuWdt_LoadWdt(&wdt, 1000000);  // ~3ms timeout

// FORGOTTEN SERVICE - Watchdog will timeout
while(1) {
    do_work();
    // Forgot to call XScuWdt_RestartWdt()!
}

// WRONG MODE - Using timer mode but expecting system reset
XScuWdt_SetTimerMode(&wdt);  // This won't cause system reset!
```

### ✅ Do This:
```c
// Appropriate timeout for application
#define WDT_TIMEOUT (333333333 * 5)  // 5 seconds

// Service watchdog in main loop
while(1) {
    do_critical_tasks();
    XScuWdt_RestartWdt(&wdt);  // Kick the dog
    do_other_tasks();
}

// Check reset cause at startup
if (XScuWdt_IsWdtExpired(&wdt)) {
    handle_recovery();
}
```

---

## Debugging Tips

1. **Calculate timeout carefully** based on worst-case execution time
2. **Add logging** before long operations to track watchdog service points
3. **Use LEDs** to indicate system state during debugging
4. **Test timeout scenarios** in controlled environment
5. **Check reset flag** after unexpected resets to confirm cause

---

## Additional Resources

- **Xilinx UG585**: Zynq-7000 Technical Reference Manual (Chapter 8: Private Timers and Watchdog)
- **Xilinx UG1137**: Zynq-7000 Software Developers Guide
- **Driver Documentation**: `xscuwdt.h` header file contains detailed API documentation
- **Example Code**: Xilinx SDK includes watchdog examples in BSP

---

## Summary

The SCU Watchdog Timer is a critical component for building reliable embedded systems:

- **Watchdog Mode**: Automatic system recovery from hangs/crashes
- **Timer Mode**: Periodic interrupt generation
- **Integration**: Combines easily with GPIO and UART for comprehensive monitoring
- **Debugging**: Reset detection helps identify system failures

Use the examples above as starting points for implementing robust watchdog protection in your Zynq applications!
