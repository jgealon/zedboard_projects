# UART Examples Guide

This document describes two cool UART examples for the ZedBoard demonstrating interactive communication.

---

## Example 1: UART Command Interpreter

### Description

A simple command-line interface over UART that allows users to send commands and receive responses. This example demonstrates:
- Command parsing and processing
- Interactive user input
- Echo functionality
- System state management
- Real-time command execution

### Features

- **Interactive Shell**: Type commands and get immediate responses
- **Command Echo**: Characters are echoed back as you type
- **Backspace Support**: Delete mistakes while typing
- **LED Control**: Toggle virtual LED on/off
- **Status Monitoring**: View system status and counter values
- **Counter Management**: Track and reset counter values

### Supported Commands

| Command | Description | Example Output |
|---------|-------------|----------------|
| `help` | Display available commands | Shows command list |
| `led` | Toggle LED state | "LED is now ON" |
| `status` | Show system status | Shows LED state and counter |
| `count` | Display counter value | "Counter: 12345" |
| `reset` | Reset counter to zero | "Counter reset to 0" |

### How to Use

1. **Build and Run**:
   - Copy `uart_command_interpreter.c` to your project's `src` folder
   - Rename or remove `helloworld.c` to avoid conflicts
   - Build and run the application in Vitis

2. **Connect Terminal**:
   - Open PuTTY or Vitis Serial Terminal
   - Settings: COM port, 115200 baud, 8N1
   - Connect to the board

3. **Interact**:
   - You'll see a welcome banner
   - Type `help` and press Enter
   - Try commands like `led`, `status`, `count`, etc.
   - Use backspace to correct typos

### Expected Output

```
=================================
  ZedBoard Command Interpreter
=================================
Commands: help, led, status, count, reset
> help

Commands:
  help   - Show this help
  led    - Toggle LED
  status - Show system status
  count  - Show counter value
  reset  - Reset counter

> led
LED is now ON

> status

--- System Status ---
LED State: ON
Counter: 45678
---------------------

> count
Counter: 45698

> reset
Counter reset to 0

>
```

### Code Highlights

**Command Buffer and Echo**:
```c
if (recv_byte == '\r' || recv_byte == '\n') {
    // Process complete command
    cmd_buffer[cmd_index] = '\0';
    process_command(cmd_buffer);
    cmd_index = 0;
} else if (recv_byte >= 32 && recv_byte <= 126) {
    // Echo printable characters
    cmd_buffer[cmd_index++] = recv_byte;
    XUartPs_Send(&uart, &recv_byte, 1);
}
```

**Backspace Handling**:
```c
if (recv_byte == 0x7F || recv_byte == 0x08) {
    if (cmd_index > 0) {
        cmd_index--;
        send_string("\b \b");  // Erase character on terminal
    }
}
```

---

## Example 2: UART Data Monitor

### Description

A real-time data monitoring system that continuously displays simulated sensor readings over UART. This example demonstrates:
- Periodic data transmission
- Formatted table output
- Interactive control (pause/resume)
- Data simulation
- Summary statistics

### Features

- **Live Monitoring**: Updates sensor data every 2 seconds
- **Formatted Display**: Clean tabular output
- **Interactive Control**:
  - Press `p` to pause monitoring
  - Press `r` to resume
  - Press `q` to quit and show summary
- **Simulated Sensors**: Temperature, voltage, and raw sensor readings
- **Status Indicators**: Color-coded status based on temperature
- **Summary Statistics**: Final report when exiting

### Simulated Sensors

| Sensor | Range | Description |
|--------|-------|-------------|
| Raw Sensor | 0-1023 | Simulated ADC reading |
| Temperature | 20-35°C | Simulated temperature sensor |
| Voltage | 3000-3600mV | Simulated voltage measurement |

### Status Levels

- **NORMAL**: Temperature < 25°C
- **WARM**: Temperature 25-30°C
- **HOT!**: Temperature > 30°C

### How to Use

1. **Build and Run**:
   - Copy `uart_data_monitor.c` to your project's `src` folder
   - Rename or remove other main files to avoid conflicts
   - Build and run the application in Vitis

2. **Connect Terminal**:
   - Open PuTTY or Vitis Serial Terminal
   - Settings: COM port, 115200 baud, 8N1
   - Connect to the board

3. **Monitor Data**:
   - Data automatically starts streaming
   - Watch sensor values update every 2 seconds
   - Press `p` to pause when needed
   - Press `r` to resume monitoring
   - Press `q` to quit and see summary

### Expected Output

```
================================================
       ZedBoard Sensor Data Monitor
================================================
Monitoring system parameters every 2 seconds...
Press 'p' to pause, 'r' to resume, 'q' to quit
------------------------------------------------

Time(s) | Sensor | Temp(C) | Voltage(mV) | Status
--------|--------|---------|-------------|--------
      1 |     10 |      20 |        3010 | NORMAL
      2 |     20 |      20 |        3020 | NORMAL
      3 |     30 |      20 |        3030 | NORMAL
      4 |     40 |      21 |        3040 | NORMAL
      5 |     50 |      21 |        3050 | NORMAL
      6 |     60 |      22 |        3060 | NORMAL
      7 |     70 |      22 |        3070 | NORMAL
      8 |     80 |      23 |        3080 | NORMAL
      9 |     90 |      23 |        3090 | NORMAL
     10 |    100 |      24 |        3100 | NORMAL
     11 |    110 |      24 |        3110 | NORMAL
     12 |    120 |      25 |        3120 | WARM
     13 |    130 |      25 |        3130 | WARM

[PAUSED] Press 'r' to resume...
[RESUMED]
Time(s) | Sensor | Temp(C) | Voltage(mV) | Status
--------|--------|---------|-------------|--------
     14 |    140 |      26 |        3140 | WARM
     15 |    150 |      26 |        3150 | WARM

[STOPPING] Generating summary...

=== Data Summary ===
Total Samples: 15
Last Sensor:   150
Last Temp:     26 C
Last Voltage:  3150 mV
====================

Program terminated.
```

### Code Highlights

**Sensor Simulation**:
```c
void simulate_sensors() {
    data.sensor_value = (data.sensor_value + 10) % 1024;
    data.temperature = 20 + (data.sensor_value % 15);
    data.voltage = 3000 + (data.sensor_value % 600);
    data.timestamp++;
}
```

**Interactive Control**:
```c
if (XUartPs_Recv(&uart, &recv_byte, 1) == 1) {
    if (recv_byte == 'p' || recv_byte == 'P') {
        paused = 1;
        send_string("\r\n[PAUSED] Press 'r' to resume...\r\n");
    } else if (recv_byte == 'r' || recv_byte == 'R') {
        paused = 0;
        send_string("[RESUMED]\r\n");
    } else if (recv_byte == 'q' || recv_byte == 'Q') {
        running = 0;
    }
}
```

**Periodic Updates**:
```c
if (!paused && running) {
    if (loop_count % 20 == 0) {  // Every ~2 seconds
        simulate_sensors();
        print_data_row();
    }
}
loop_count++;
usleep(100000);  // 100ms delay
```

---

## Common Code Elements

Both examples share common UART initialization code:

### UART Initialization (SDT Mode)

```c
void uart_init() {
    s32 status;
    uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
    if (uart_config != NULL) {
        status = XUartPs_CfgInitialize(&uart, uart_config, uart_config->BaseAddress);
        if (status == XST_SUCCESS) {
            XUartPs_SetBaudRate(&uart, 115200);
        }
    }
}
```

### Sending Strings

```c
void send_string(const char *str) {
    XUartPs_Send(&uart, (u8*)str, strlen(str));
}
```

### Receiving Bytes (Non-blocking)

```c
u8 recv_byte;
if (XUartPs_Recv(&uart, &recv_byte, 1) == 1) {
    // Process received byte
}
```

---

## Building and Running

### Method 1: Replace Main File

1. Navigate to your project: `uart_mio/uart_mio/src/`
2. Rename `helloworld.c` to `helloworld.c.bak`
3. Copy one of the example files and rename it to match your build settings
4. Build project in Vitis
5. Run on hardware

### Method 2: Create New Application

1. Right-click on `uart_mio` system in Vitis
2. Select `New → Application Component`
3. Name it (e.g., `uart_cmd` or `uart_monitor`)
4. Copy the example code to the new application's `src` folder
5. Build and run

### Method 3: Modify Existing

1. Open `helloworld.c`
2. Replace entire content with example code
3. Build and run

---

## Troubleshooting

### Issue: No Output in Terminal

**Check:**
- ✓ Correct COM port selected
- ✓ Baud rate set to 115200
- ✓ USB cable connected to J14 (USB-UART)
- ✓ Board powered on
- ✓ Application running

**Solution:**
```c
// Add debug output at start of main():
xil_printf("UART Example Starting...\n\r");
```

### Issue: Garbage Characters

**Cause:** Baud rate mismatch

**Solution:** Verify both terminal and code use 115200:
```c
XUartPs_SetBaudRate(&uart, 115200);
```

### Issue: Characters Not Echoed (Example 1)

**Check:** Echo code is present:
```c
// This line echoes character back:
XUartPs_Send(&uart, &recv_byte, 1);
```

### Issue: Commands Not Working (Example 1)

**Check:**
- Press Enter after typing command
- Use lowercase (or modify code for case-insensitive)
- Command buffer not overflowing

### Issue: Monitor Updates Too Fast/Slow (Example 2)

**Adjust timing:**
```c
// Change update frequency (current: every 20 loops × 100ms = 2 seconds)
if (loop_count % 20 == 0) {  // Change 20 to adjust
    simulate_sensors();
    print_data_row();
}

// Change loop delay
usleep(100000);  // Change 100000 (100ms) to adjust
```

---

## Customization Ideas

### Example 1 Enhancements

1. **Add More Commands**:
   ```c
   else if (strcmp(cmd, "info") == 0) {
       send_string("ZedBoard Rev D - Zynq-7000\r\n");
   }
   ```

2. **Add Real LED Control**:
   ```c
   #include "xgpio.h"
   XGpio led_gpio;
   // Initialize GPIO and use XGpio_DiscreteWrite()
   ```

3. **Add Command History**: Store last N commands

4. **Add Tab Completion**: Auto-complete commands

5. **Add Command Arguments**:
   ```c
   // Parse "set led 5" to set specific LED
   char *token = strtok(cmd, " ");
   ```

### Example 2 Enhancements

1. **Real Sensor Integration**:
   ```c
   #include "xadcps.h"
   // Read actual XADC temperature and voltage
   ```

2. **Data Logging**: Save readings to memory or SD card

3. **Graphing**: Output data in format for plotting tools

4. **Alarm Thresholds**: Alert when values exceed limits

5. **Multiple Sampling Rates**: Let user adjust update frequency

---

## Advanced Features

### Adding Color to Terminal Output

Use ANSI escape codes:
```c
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_RESET   "\033[0m"

// Example usage:
send_string(ANSI_RED "HOT!" ANSI_RESET);
```

### Adding Clear Screen

```c
send_string("\033[2J\033[H");  // ANSI clear screen and home cursor
```

### Adding Progress Bar

```c
void print_progress_bar(u32 percent) {
    send_string("[");
    for (int i = 0; i < 50; i++) {
        if (i < percent / 2) send_string("=");
        else send_string(" ");
    }
    send_string("]\r\n");
}
```

---

## Comparison Table

| Feature | Command Interpreter | Data Monitor |
|---------|-------------------|--------------|
| **Interaction** | User command input | One-way display + control |
| **Update Rate** | On-demand | Periodic (2 seconds) |
| **Data Flow** | Bidirectional | Mostly unidirectional |
| **Best For** | Control systems | Logging/monitoring |
| **Complexity** | Medium | Low |
| **User Input** | Full commands | Single key press |

---

## Learning Objectives

### Example 1 Teaches:
- ✓ String parsing and command processing
- ✓ Interactive user interfaces over UART
- ✓ Buffered input with echo
- ✓ Character-by-character processing
- ✓ Backspace and special key handling
- ✓ State machine design

### Example 2 Teaches:
- ✓ Periodic data transmission
- ✓ Formatted output (tables)
- ✓ Non-blocking user input
- ✓ Pause/resume control
- ✓ Data simulation
- ✓ Summary statistics generation

---

## Testing Checklist

### Example 1 - Command Interpreter
- [ ] Banner displays on startup
- [ ] Prompt appears (">")
- [ ] Characters echo as typed
- [ ] Backspace removes characters
- [ ] `help` command shows menu
- [ ] `led` command toggles state
- [ ] `status` command displays info
- [ ] `count` command shows counter
- [ ] `reset` command clears counter
- [ ] Unknown commands show error
- [ ] Empty enter just shows prompt

### Example 2 - Data Monitor
- [ ] Header displays on startup
- [ ] Data updates every ~2 seconds
- [ ] Timestamp increments
- [ ] Sensor values change
- [ ] Temperature varies
- [ ] Voltage varies
- [ ] Status updates (NORMAL/WARM/HOT)
- [ ] `p` pauses display
- [ ] `r` resumes display
- [ ] `q` quits and shows summary
- [ ] Summary shows correct values

---

## Quick Start Summary

1. **Choose an example** (Command Interpreter or Data Monitor)
2. **Copy code** to your `uart_mio/uart_mio/src/` folder
3. **Replace or rename** `helloworld.c`
4. **Build** project in Vitis
5. **Connect** terminal (115200 baud)
6. **Run** application on hardware
7. **Interact** and enjoy!

---

## Additional Resources

### Related Documents
- `MULTIPLE_GPIO_PITFALLS.md` - Understanding SDT mode
- `XPAR_NAMING_GUIDE.md` - XPAR macro conventions
- `VITIS_UART_TERMINAL_GUIDE.md` - Setting up terminals

### Reference Material
- XUartPs Driver Documentation: Vitis install path → `docs/xilinx_drivers/`
- Zynq-7000 TRM Chapter 19: UART Controller
- UART Protocol: https://www.circuitbasics.com/basics-uart-communication/

---

**Document Version:** 1.0
**Created:** 2026-04-19
**Author:** Claude AI Assistant
**Tested On:** ZedBoard Rev D, Vitis 2023.2
