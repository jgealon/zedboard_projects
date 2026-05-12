# SCU Watchdog Timer Examples - Complete Usage Guide

This guide provides **step-by-step instructions** for implementing and running the SCU Watchdog Timer examples with **expected results** for each.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Hardware Setup](#hardware-setup)
3. [Software Setup](#software-setup)
4. [Example 1: Simple UART Echo with Watchdog](#example-1-simple-uart-echo-with-watchdog)
5. [Example 2: UART Command Parser with Watchdog](#example-2-uart-command-parser-with-watchdog)
6. [Example 3: Data Logger with Watchdog](#example-3-data-logger-with-watchdog)
7. [Example 4: Reset Detection and Recovery](#example-4-reset-detection-and-recovery)
8. [Troubleshooting](#troubleshooting)
9. [Common Issues and Solutions](#common-issues-and-solutions)

---

## Prerequisites

### Hardware Required:
- ✅ Zedboard (or compatible Zynq-7000 board)
- ✅ USB cable for JTAG programming
- ✅ USB cable for UART communication
- ✅ Power supply for Zedboard

### Software Required:
- ✅ Vivado 2023.2 (or compatible version)
- ✅ Vitis IDE 2023.2 (or compatible version)
- ✅ Serial terminal (PuTTY, TeraTerm, or Minicom)
- ✅ Existing pvt_wdt platform project

### Knowledge Required:
- Basic understanding of Vitis IDE
- Familiarity with serial terminal usage
- Understanding of C programming

---

## Hardware Setup

### Step 1: Physical Connections

1. **Connect JTAG Cable**
   ```
   PC USB Port → Zedboard JTAG USB Port (J17)
   ```

2. **Connect UART Cable**
   ```
   PC USB Port → Zedboard UART USB Port (J14)
   ```

3. **Power Settings**
   - Set jumper JP7 to "USB" or connect external 12V supply
   - Ensure boot mode jumpers are set correctly:
     ```
     JP7-11: Set to JTAG mode
     MIO6: 0
     MIO5: 0
     MIO4: 0
     MIO3: 0
     MIO2: 1
     ```

4. **Power On**
   - Turn on power switch (SW8)
   - Blue "POWER GOOD" LED (LD13) should illuminate

### Step 2: Verify UART Connection

1. **Open Device Manager (Windows) / dmesg (Linux)**
   - Locate the Zedboard UART port
   - Windows: Look for "USB Serial Port (COMx)"
   - Linux: Look for "/dev/ttyUSBx"

2. **Note the COM Port Number**
   - Example: COM5, COM6, /dev/ttyUSB0, etc.

---

## Software Setup

### Step 1: Configure Serial Terminal

#### Using PuTTY (Windows):

1. Open PuTTY
2. Configure connection:
   ```
   Connection Type: Serial
   Serial Line: COM5 (your COM port)
   Speed: 115200
   ```

3. Configure terminal settings:
   - Category → Terminal
   - Set "Implicit CR in every LF" ✅
   - Set "Implicit LF in every CR" ✅

4. Click "Open"

#### Using TeraTerm (Windows):

1. Open TeraTerm
2. File → New Connection
3. Select "Serial" and your COM port
4. Setup → Serial Port:
   ```
   Baud Rate: 115200
   Data: 8 bit
   Parity: none
   Stop: 1 bit
   Flow Control: none
   ```

#### Using Minicom (Linux):

```bash
sudo minicom -D /dev/ttyUSB0 -b 115200
```

Configure settings (Ctrl+A then Z):
```
Add Carriage Return: Yes
Echo: On
```

### Step 2: Verify Vitis Setup

1. **Open Vitis IDE**
2. **Set Workspace**
   ```
   Workspace: C:/Users/JGEALON/Documents/ZedProjects/pvt_wdt
   ```

3. **Verify Platform**
   - Check that your platform project is visible
   - Platform should be built successfully

---

## Example 1: Simple UART Echo with Watchdog

### What This Example Does:
- Echoes back any character you type
- Displays periodic heartbeat messages
- Monitors system health with watchdog (5 second timeout)
- Toggles LED on received characters
- Auto-restarts if system hangs

### Implementation Steps:

#### Step 1: Create Application Project

1. In Vitis IDE, click **File → New → Application Project**

2. **Select Platform:**
   - Choose your existing platform: `platform`
   - Click **Next**

3. **Application Project Details:**
   ```
   Application project name: wdt_uart_echo
   System project name: wdt_uart_echo_system
   Target processor: ps7_cortexa9_0
   ```
   - Click **Next**

4. **Domain:**
   - Select existing domain or create new
   - OS: standalone
   - Click **Next**

5. **Template:**
   - Select **Empty Application (C)**
   - Click **Finish**

#### Step 2: Add Source Code

1. **Right-click on `wdt_uart_echo/src`** → **New → File**

2. **File name:** `main.c`

3. **Copy the code from SCU_WDT_CORRECTED_EXAMPLES.md Example 1**

4. **Important Modifications for Your Hardware:**
   ```c
   // If you don't have AXI GPIO, remove GPIO code or use PS GPIO
   // For PS GPIO on MIO pins:
   #define MIO_LED_PIN 7  // Change to your actual MIO pin
   ```

5. **Save the file** (Ctrl+S)

#### Step 3: Build the Project

1. **Right-click on `wdt_uart_echo`** → **Build Project**

2. **Wait for build to complete** (check Console for "Build Finished")

3. **Verify no errors** in Problems view

#### Step 4: Run the Application

1. **Power on Zedboard** and ensure UART terminal is open

2. **Right-click on `wdt_uart_echo`** → **Run As → Launch Hardware (Single Application Debug)**

3. **System will program FPGA and launch application**

### Expected Results:

#### Initial Output:
```
Watchdog initialized: 5 second timeout
PS UART initialized at 115200 baud

=== UART Echo with Watchdog ===
Type anything - it will echo back
Watchdog monitors system health

[0] System OK - Watchdog serviced
```

#### When You Type:
```
Hello                           ← You type this
Hello                           ← Echoed back
[100000] Characters received: 5
[200000] Characters received: 5
Testing watchdog                ← You type this
Testing watchdog                ← Echoed back
[300000] Characters received: 21
```

#### Behavior Verification:

| Action | Expected Result | Why |
|--------|----------------|-----|
| Type any character | Immediately echoed back | UART working |
| Wait without typing | Heartbeat messages every ~10s | Main loop running |
| Let run for 5 minutes | No reset | Watchdog being serviced |
| Disconnect JTAG during run | System continues | Standalone operation |

#### LED Behavior (if GPIO configured):
- LED toggles on/off each time you type a character
- Shows visual feedback of UART activity

### Success Criteria:
✅ Characters echo back correctly
✅ Heartbeat messages appear regularly
✅ System runs indefinitely without reset
✅ No error messages in console

---

## Example 2: UART Command Parser with Watchdog

### What This Example Does:
- Accepts text commands via UART
- Controls LEDs based on commands
- Can enable/disable watchdog
- Demonstrates timeout scenario
- 4-second watchdog timeout

### Implementation Steps:

#### Step 1: Create New Application

Follow same steps as Example 1, but name it: `wdt_command_parser`

#### Step 2: Add Source Code

Use code from **SCU_WDT_CORRECTED_EXAMPLES.md Example 2**

#### Step 3: Important Code Adjustments

```c
// Adjust these based on your hardware:
#define LED_BASE_PIN 7  // First MIO pin for LEDs

// If you don't have 8 LEDs, modify set_leds():
void set_leds(u8 pattern) {
    // Set only available LEDs
    for (int i = 0; i < 4; i++) {  // Change to your LED count
        XGpioPs_WritePin(&gpio_ps, LED_BASE_PIN + i, (pattern >> i) & 0x01);
    }
}
```

#### Step 4: Build and Run

1. Build project
2. Run on hardware
3. Open serial terminal

### Expected Results:

#### Initial Startup:
```
=== Watchdog Command Monitor ===
Using PS UART for commands
Type 'HELP' for command list

Ready>
```

#### Test Sequence 1: Help Command
```
Ready> HELP
```

**Expected Output:**
```
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

Ready>
```

#### Test Sequence 2: PING Command
```
Ready> PING
PONG

Ready>
```
- **LED Behavior:** Single LED turns on (pattern 0x01)

#### Test Sequence 3: STATUS Command
```
Ready> STATUS
System OK - Watchdog Active (4s timeout)

Ready>
```
- **LED Behavior:** Lower 4 LEDs turn on (pattern 0x0F)

#### Test Sequence 4: LED Control
```
Ready> LED ON
All LEDs ON

Ready> LED OFF
All LEDs OFF

Ready> LED 170
LED pattern set to: 0xAA

Ready>
```
- **LED Behavior:** All LEDs ON → All OFF → Alternating pattern (10101010)

#### Test Sequence 5: SLOW Command (Safe Long Task)
```
Ready> SLOW
Executing slow task (with watchdog service)...
  Progress: 1/8
  Progress: 2/8
  Progress: 3/8
  Progress: 4/8
  Progress: 5/8
  Progress: 6/8
  Progress: 7/8
  Progress: 8/8
Slow task complete

Ready>
```
- **Duration:** ~8 seconds (1 second per step)
- **System Behavior:** Does NOT reset (watchdog serviced during task)
- **LED Behavior:** Lights up sequentially (0x01, 0x02, 0x04, 0x08, etc.)

#### Test Sequence 6: HANG Command (Watchdog Timeout Test)
```
Ready> HANG
WARNING: Simulating hang - watchdog will timeout!
Still hanging...
Still hanging...
Still hanging...
[System resets after ~4 seconds]

=== Watchdog Command Monitor ===
Using PS UART for commands
Type 'HELP' for command list

Ready>
```
- **LED Behavior:** All LEDs ON, then system resets
- **Duration:** Reset occurs after 4 seconds
- **Important:** This proves watchdog is working!

### Verification Checklist:

| Test | Command | Expected Result | Pass/Fail |
|------|---------|----------------|-----------|
| Echo | `PING` | Returns `PONG` | ⬜ |
| Status | `STATUS` | Shows system info | ⬜ |
| LED Control | `LED 170` | LEDs show 0xAA pattern | ⬜ |
| Safe Long Task | `SLOW` | Completes without reset | ⬜ |
| Watchdog Timeout | `HANG` | System resets in ~4s | ⬜ |
| Backspace | Type then backspace | Characters erased | ⬜ |
| Unknown Command | `INVALID` | Shows error message | ⬜ |

### Success Criteria:
✅ All commands execute correctly
✅ SLOW command completes without reset (watchdog serviced)
✅ HANG command causes system reset (watchdog working)
✅ LEDs respond to commands
✅ Terminal shows proper formatting

---

## Example 3: Data Logger with Watchdog

### What This Example Does:
- Logs sensor data in CSV format over UART
- Reads real temperature from XADC
- Simulates additional sensor readings
- Logs at 1 Hz rate
- 10-second watchdog timeout

### Implementation Steps:

#### Step 1: Create Application
Name: `wdt_data_logger`

#### Step 2: Add Source Code
Use code from **SCU_WDT_CORRECTED_EXAMPLES.md Example 3**

#### Step 3: Build and Run

### Expected Results:

#### Initial Output:
```
=== Data Logger with Watchdog ===
Logging to UART at 1 Hz
Watchdog timeout: 10 seconds

Sample,Temp_C,Sensor1,Sensor2,Voltage
```

#### Continuous Logging (CSV Format):
```
0,45.23,512,768,1.842
1,45.21,493,801,1.856
2,45.24,601,723,1.839
3,45.22,558,692,1.847
4,45.25,487,751,1.834
5,45.23,623,689,1.858
6,45.22,541,744,1.841
7,45.24,499,812,1.845
8,45.21,634,697,1.852
9,45.23,518,778,1.838
10,45.25,595,701,1.847
```

#### Occasional Error Messages:
```
15,45.22,601,723,1.839
# Warning: Sensor read error (count: 1)
16,45.24,558,692,1.847
...
```
- Simulated ~5% error rate
- System continues logging (doesn't crash)

#### Status Messages:
```
59,45.21,487,751,1.834
# Status: 60 samples, 3 errors, WDT active
60,45.23,623,689,1.858
...
119,45.22,541,744,1.841
# Status: 120 samples, 6 errors, WDT active
```

### Data Analysis:

#### Capturing Data to File:

**Windows (PuTTY):**
1. Session → Logging
2. Choose "All session output"
3. Browse to save location: `C:\logs\sensor_data.csv`
4. Click "Start Logging"

**Linux (Minicom):**
```bash
minicom -D /dev/ttyUSB0 -b 115200 -C sensor_data.csv
```

#### Processing Data:

1. **Open in Excel/LibreOffice:**
   - Remove comment lines (starting with #)
   - Import as CSV
   - Create graphs of temperature, sensor values

2. **Python Analysis:**
   ```python
   import pandas as pd
   import matplotlib.pyplot as plt

   # Read CSV (skip comment lines)
   df = pd.read_csv('sensor_data.csv', comment='#')

   # Plot temperature over time
   plt.plot(df['Sample'], df['Temp_C'])
   plt.xlabel('Sample Number')
   plt.ylabel('Temperature (°C)')
   plt.title('FPGA Die Temperature')
   plt.show()
   ```

### Expected Temperature Range:
- **Normal Operation:** 40-50°C (typical for Zynq at room temp)
- **Under Load:** 50-60°C
- **Warning:** >70°C (check cooling)

### Verification Checklist:

| Metric | Expected | Status |
|--------|----------|--------|
| Sample Rate | 1 sample/second | ⬜ |
| Temperature Range | 40-60°C | ⬜ |
| CSV Format | Valid comma-separated | ⬜ |
| Error Handling | Continues after errors | ⬜ |
| Watchdog Active | No unexpected resets | ⬜ |
| Status Messages | Every 60 samples | ⬜ |

### Success Criteria:
✅ Data logs continuously at 1 Hz
✅ Temperature readings are realistic
✅ CSV format is valid
✅ System handles simulated errors gracefully
✅ No watchdog resets during normal operation

---

## Example 4: Reset Detection and Recovery

### What This Example Does:
- Detects if previous reset was caused by watchdog
- Implements recovery procedure after watchdog reset
- Allows manual triggering of watchdog timeout
- Visual feedback via LED
- 3-second watchdog timeout

### Implementation Steps:

#### Step 1: Create Application
Name: `wdt_reset_detection`

#### Step 2: Add Source Code
Use code from **SCU_WDT_CORRECTED_EXAMPLES.md Example 4**

#### Step 3: First Run (Clean Boot)

### Expected Results:

#### First Boot (Normal Power-On):
```
=== Watchdog Reset Detection Demo ===
Watchdog timeout: 3 seconds

Press:
  '1' - Normal operation (services watchdog)
  '2' - Trigger watchdog reset
  's' - Show status

Running in NORMAL mode
[0] Normal operation - WDT serviced
[1] Normal operation - WDT serviced
[2] Normal operation - WDT serviced
```

- **LED Behavior:** Slow blink (toggles every second)
- **System Behavior:** Running normally

#### Status Check:
```
(Press 's')

[STATUS]
  Mode: NORMAL
  Loop count: 15
  Watchdog active: Yes
  Timeout: 3 seconds
  Reset flag: CLEAR

[16] Normal operation - WDT serviced
```

#### Trigger Watchdog Reset:
```
(Press '2')

[MODE] Triggering watchdog timeout...
System will reset in ~3 seconds
[0] Waiting for timeout...
[1] Waiting for timeout...
[2] Waiting for timeout...
```

**Then system resets...**

#### Second Boot (After Watchdog Reset):
```
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! WATCHDOG TIMEOUT DETECTED        !!
!! System was reset by watchdog     !!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Recovery Actions:
  [1] Clearing watchdog reset flag
  [2] Performing system diagnostics
  [3] Restoring safe configuration

System recovered - resuming normal operation

=== Watchdog Reset Detection Demo ===
Watchdog timeout: 3 seconds

Press:
  '1' - Normal operation (services watchdog)
  '2' - Trigger watchdog reset
  's' - Show status

Running in NORMAL mode
[0] Normal operation - WDT serviced
```

- **LED Behavior:** Rapid flashing (10 times) during recovery, then normal blink
- **Duration:** Recovery takes ~2 seconds

#### Status Check After Recovery:
```
(Press 's')

[STATUS]
  Mode: NORMAL
  Loop count: 8
  Watchdog active: Yes
  Timeout: 3 seconds
  Reset flag: CLEAR    ← Cleared after recovery

[9] Normal operation - WDT serviced
```

### Test Sequence:

#### Complete Test Flow:

| Step | Action | Expected Behavior | LED Pattern | Duration |
|------|--------|-------------------|-------------|----------|
| 1 | Power on board | Normal startup, no recovery message | Slow blink | - |
| 2 | Press 's' | Shows "Reset flag: CLEAR" | Slow blink | - |
| 3 | Press '2' | Message: "Triggering timeout..." | Solid ON | - |
| 4 | Wait | System resets after 3 seconds | OFF | 3 seconds |
| 5 | Boot up | Recovery message displayed | Rapid flash (10x) | 2 seconds |
| 6 | After recovery | Returns to normal mode | Slow blink | - |
| 7 | Press 's' | Shows "Reset flag: CLEAR" (cleared) | Slow blink | - |
| 8 | Press '1' | Confirms normal mode | Slow blink | - |

### Verification Checklist:

| Test | Expected Result | Pass/Fail |
|------|----------------|-----------|
| Clean Boot | No recovery message | ⬜ |
| Status Check | Shows system info | ⬜ |
| Mode Switch | Press '1' works | ⬜ |
| Trigger Timeout | Press '2' causes reset | ⬜ |
| Reset Detection | Recovery message on reboot | ⬜ |
| LED Recovery Flash | 10 rapid flashes | ⬜ |
| Flag Clearing | Reset flag cleared after recovery | ⬜ |
| Resume Normal | Returns to normal operation | ⬜ |

### Success Criteria:
✅ Normal boot shows no recovery message
✅ Watchdog timeout can be triggered
✅ Reset detection works (recovery message appears)
✅ Reset flag is properly cleared
✅ System resumes normal operation after recovery
✅ LED provides visual feedback during recovery

---

## Troubleshooting

### Issue 1: No Output in Serial Terminal

**Symptoms:**
- Terminal is open but shows nothing
- Application runs in Vitis but no UART output

**Solutions:**

1. **Check COM Port:**
   ```
   Windows: Device Manager → Ports (COM & LPT)
   Linux: ls -l /dev/ttyUSB*
   ```

2. **Verify Baud Rate:**
   - Must be **115200**
   - Check terminal settings

3. **Check USB Cable:**
   - Use USB port labeled "UART" on Zedboard (J14)
   - Try different USB cable
   - Check cable supports data (not just power)

4. **Verify UART Initialization:**
   ```c
   // Add debug LED toggle to confirm code is running
   XGpioPs_WritePin(&gpio, LED_PIN, 1);  // LED should turn on
   ```

5. **Try Direct Printf:**
   ```c
   // Add at start of main():
   print("Hello World\r\n");  // Uses default UART
   ```

### Issue 2: System Resets Immediately

**Symptoms:**
- Application starts but immediately resets
- Continuous reset loop

**Causes & Solutions:**

1. **Watchdog Timeout Too Short:**
   ```c
   // Increase timeout value
   #define WDT_TIMEOUT (333333333 * 10)  // 10 seconds instead of 3
   ```

2. **Initialization Takes Too Long:**
   ```c
   // Service watchdog during initialization
   void init_all_devices() {
       init_watchdog();
       XScuWdt_RestartWdt(&wdt);  // ← Add this

       init_ps_gpio();
       XScuWdt_RestartWdt(&wdt);  // ← And this

       init_ps_uart();
       XScuWdt_RestartWdt(&wdt);  // ← And this
   }
   ```

3. **Watchdog Not Started Properly:**
   ```c
   // Check initialization order
   XScuWdt_Stop(&wdt);          // 1. Stop first
   XScuWdt_SetWdMode(&wdt);     // 2. Set mode
   XScuWdt_LoadWdt(&wdt, val);  // 3. Load counter
   XScuWdt_Start(&wdt);         // 4. Start last
   ```

### Issue 3: Characters Not Echoing

**Symptoms:**
- Can send characters but no echo
- Terminal shows no response

**Solutions:**

1. **Check UART Receive:**
   ```c
   // Add debug output
   if (XUartPs_IsReceiveData(uart.Config.BaseAddress)) {
       xil_printf("RX data available\r\n");  // Should print when you type
       XUartPs_Recv(&uart, &rx_byte, 1);
       xil_printf("Received: %c (0x%02X)\r\n", rx_byte, rx_byte);
   }
   ```

2. **Verify Terminal Settings:**
   - Local echo should be OFF (application does the echo)
   - If you see double characters, local echo is ON

3. **Check Receive Buffer:**
   ```c
   // Make sure receive is not blocking
   u32 received = XUartPs_Recv(&uart, &rx_byte, 1);
   if (received > 0) {
       // Process byte
   }
   ```

### Issue 4: LEDs Not Working

**Symptoms:**
- UART works but LEDs don't respond
- No visual feedback

**Solutions:**

1. **Check GPIO Initialization:**
   ```c
   // Verify pin direction and output enable
   XGpioPs_SetDirectionPin(&gpio_ps, LED_PIN, 1);     // 1 = output
   XGpioPs_SetOutputEnablePin(&gpio_ps, LED_PIN, 1);  // Enable

   // Test with direct toggle
   XGpioPs_WritePin(&gpio_ps, LED_PIN, 1);  // Should turn on
   sleep(1);
   XGpioPs_WritePin(&gpio_ps, LED_PIN, 0);  // Should turn off
   ```

2. **Verify MIO Pin Number:**
   ```c
   // Check your board's schematic for correct MIO pins
   // Zedboard typically uses MIO0-MIO15 for various peripherals
   // LEDs might be on specific MIO pins
   ```

3. **If Using AXI GPIO (PL):**
   - Check that AXI GPIO IP is in your hardware design
   - Verify base address in xparameters.h
   - Check that platform was rebuilt after hardware changes

### Issue 5: Build Errors

**Common Build Errors:**

#### Error: "xuartps.h: No such file or directory"
```
Solution:
- BSP needs to be regenerated
- Right-click platform → Build Project
- Clean and rebuild application
```

#### Error: "undefined reference to 'XUartPs_Send'"
```
Solution:
- Check linker settings
- Verify BSP libraries are linked
- Clean and rebuild entire workspace
```

#### Error: "XPAR_XUARTPS_0_DEVICE_ID undeclared"
```
Solution:
- xparameters.h not updated
- Regenerate BSP
- Hardware → Update Hardware Specification
```

### Issue 6: Watchdog Not Timing Out

**Symptoms:**
- HANG command doesn't cause reset
- System never resets even when not serviced

**Solutions:**

1. **Verify Watchdog is in Watchdog Mode:**
   ```c
   // Must call SetWdMode, not SetTimerMode
   XScuWdt_SetWdMode(&wdt);  // ← Correct for system reset
   // NOT: XScuWdt_SetTimerMode(&wdt);  // ← This only generates interrupts
   ```

2. **Check Watchdog is Started:**
   ```c
   // Add verification
   u32 ctrl_reg = XScuWdt_GetControlReg(&wdt);
   xil_printf("WDT Control Reg: 0x%08X\r\n", ctrl_reg);
   // Bit 0 should be 1 (enabled)
   ```

3. **Verify Counter Value:**
   ```c
   // Make sure counter is loaded
   xil_printf("WDT Timeout: %lu counts\r\n", WDT_TIMEOUT);
   xil_printf("Expected timeout: %lu seconds\r\n", WDT_TIMEOUT / 333333333);
   ```

### Issue 7: Data Logger Shows Incorrect Temperature

**Symptoms:**
- Temperature reading is 0°C or -273°C
- Temperature reading is constant
- Temperature reading is unrealistic (>100°C or <0°C at room temp)

**Solutions:**

1. **Check XADC Initialization:**
   ```c
   // Add status check
   s32 status = XXadcPs_CfgInitialize(&xadc, xadc_cfg, xadc_cfg->BaseAddress);
   if (status != XST_SUCCESS) {
       xil_printf("XADC Init Failed!\r\n");
   }
   ```

2. **Verify Temperature Channel:**
   ```c
   // Read raw value for debugging
   u16 temp_raw = XXadcPs_GetAdcData(&xadc, XADCPS_CH_TEMP);
   xil_printf("Raw XADC value: 0x%04X (%u)\r\n", temp_raw, temp_raw);

   // Raw value should be around 20000-30000 for room temp
   ```

3. **Check Conversion Formula:**
   ```c
   // Correct formula for Zynq-7000
   float temp_c = ((float)temp_raw * 503.975f / 65536.0f) - 273.15f;

   // Sanity check
   if (temp_c < 0 || temp_c > 100) {
       xil_printf("Warning: Temperature out of range: %.2f C\r\n", temp_c);
   }
   ```

---

## Common Issues and Solutions

### General Tips:

1. **Always Check UART First:**
   - If UART doesn't work, you can't debug
   - Use `xil_printf()` for simple debugging
   - Add LED toggles for visual confirmation

2. **Watchdog Timeout Values:**
   - Start with longer timeouts (10+ seconds) during development
   - Reduce timeout after code is stable
   - Remember: 333333333 counts = 1 second

3. **Clean Build After Hardware Changes:**
   ```
   1. Hardware → Update Hardware Specification
   2. Platform → Build Project (Clean first if needed)
   3. Application → Clean Project
   4. Application → Build Project
   ```

4. **Check Platform Status:**
   - Platform must be built successfully before application
   - Green checkmark should appear on platform project
   - Look for errors in Console during platform build

5. **Debugging Techniques:**
   ```c
   // Add checkpoint messages
   xil_printf("Checkpoint 1: Watchdog init\r\n");
   init_watchdog();

   xil_printf("Checkpoint 2: GPIO init\r\n");
   init_ps_gpio();

   xil_printf("Checkpoint 3: UART init\r\n");
   init_ps_uart();

   xil_printf("Checkpoint 4: Entering main loop\r\n");
   ```

### Performance Tips:

1. **Optimize UART Send:**
   ```c
   // Instead of sending byte-by-byte
   for (int i = 0; i < len; i++) {
       XUartPs_Send(&uart, &data[i], 1);  // Slow
   }

   // Send entire buffer at once
   XUartPs_Send(&uart, data, len);  // Fast
   ```

2. **Reduce Watchdog Service Overhead:**
   ```c
   // Don't service every loop iteration if loop is fast
   static u32 wdt_counter = 0;
   if (++wdt_counter > 10000) {
       XScuWdt_RestartWdt(&wdt);
       wdt_counter = 0;
   }
   ```

3. **Buffer UART Output:**
   ```c
   // For data logging, buffer multiple samples
   char buffer[1024];
   int buf_pos = 0;

   // Add samples to buffer
   buf_pos += sprintf(&buffer[buf_pos], "%lu,%.2f,...\r\n", ...);

   // Send when buffer is full
   if (buf_pos > 512) {
       XUartPs_Send(&uart, buffer, buf_pos);
       buf_pos = 0;
   }
   ```

---

## Summary Checklist

Before Running Examples:
- ⬜ Zedboard powered and connected (JTAG + UART)
- ⬜ Serial terminal configured (115200 baud, COM port correct)
- ⬜ Vitis workspace set to correct directory
- ⬜ Platform project built successfully
- ⬜ UART cable in correct port (J14)

During Testing:
- ⬜ Initial messages appear in terminal
- ⬜ Characters echo correctly (Example 1)
- ⬜ Commands execute as expected (Example 2)
- ⬜ Data logs at correct rate (Example 3)
- ⬜ Watchdog reset detection works (Example 4)

Success Indicators:
- ✅ UART communication bidirectional
- ✅ Watchdog services regularly (no unexpected resets)
- ✅ Watchdog timeout works when triggered
- ✅ LEDs respond to code (if configured)
- ✅ Temperature readings realistic (Example 3)

---

## Next Steps

After successfully running all examples:

1. **Modify for Your Application:**
   - Change watchdog timeout for your requirements
   - Add your own commands or sensors
   - Integrate with your application logic

2. **Add Error Handling:**
   - Check return values from all driver calls
   - Implement recovery strategies
   - Add logging for debugging

3. **Optimize Performance:**
   - Profile execution time
   - Adjust watchdog timeout accordingly
   - Buffer UART output if needed

4. **Test Edge Cases:**
   - Very short timeouts
   - High UART traffic
   - Long-running tasks
   - Power cycling

5. **Document Your Implementation:**
   - Record actual timeout values used
   - Document any hardware modifications
   - Note any issues encountered and solutions

---

## Additional Resources

- **Xilinx Documentation:**
  - UG585: Zynq-7000 Technical Reference Manual
  - UG1137: Zynq-7000 Software Developers Guide
  - Driver docs in Vitis install directory

- **Community Support:**
  - Xilinx Forums
  - Stack Overflow (tag: xilinx-zynq)
  - GitHub examples

- **Your Project Files:**
  - `UART_COMPARISON.md` - Understand UART options
  - `SCU_WDT_CORRECTED_EXAMPLES.md` - Source code
  - `README_CORRECTED.md` - Quick reference

---

**Good luck with your SCU Watchdog Timer implementation!**

If you encounter issues not covered here, check the console output in Vitis, verify hardware connections, and ensure all software versions match.
