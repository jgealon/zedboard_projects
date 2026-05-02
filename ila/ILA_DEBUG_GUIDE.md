# ILA (Integrated Logic Analyzer) Debug Guide for Vitis 2023.2

Complete guide to using ILA for hardware debug in your Zynq-7000 ZedBoard project.

## Table of Contents
1. [What is ILA?](#what-is-ila)
2. [Your Current ILA Setup](#your-current-ila-setup)
3. [The Correct Workflow (Vitis 2023.2)](#the-correct-workflow-vitis-20232)
4. [Understanding ILA Waveforms](#understanding-ila-waveforms)
5. [Setting Up Specific Triggers](#setting-up-specific-triggers)
6. [Practical Examples](#practical-examples)
7. [Troubleshooting](#troubleshooting)
8. [Quick Reference](#quick-reference)

---

## What is ILA?

**ILA (Integrated Logic Analyzer)** is a Vivado IP core that allows you to probe and monitor internal FPGA signals in real-time during hardware execution.

### Key Features
- Monitor internal FPGA signals without external test equipment
- Capture signal data at hardware clock speed
- Set trigger conditions to capture specific events
- View timing diagrams and waveforms
- Debug AXI transactions, GPIO signals, custom logic
- Non-intrusive hardware debugging via JTAG

### ILA vs Software Debugging

| Feature | Software Debug (XSDB) | Hardware Debug (ILA) |
|---------|----------------------|---------------------|
| **Target** | ARM processor (PS) | FPGA logic (PL) |
| **What it monitors** | CPU memory, registers, variables | Hardware signals, buses, state machines |
| **Use case** | C/C++ application debugging | Hardware interface debugging |
| **Speed** | Slow (JTAG limited) | Fast (captures at clock speed) |
| **Trigger on** | Breakpoints, watchpoints | Signal values, patterns, edges |

**Best Practice**: Use ILA for hardware/interface issues, XSDB for software issues.

---

## Your Current ILA Setup

Based on your block design (`design_1`), your System ILA has:

### Hardware Configuration
- **IP Core**: System ILA 1.1
- **Number of Slots**: 3 monitor slots
- **Clock**: 100 MHz (FCLK_CLK0)
- **Data Depth**: 1024 samples

### Monitored Signals

#### SLOT 0: GPIO Interface
```
Interface: axi_gpio_0/GPIO
Signals: GPIO_tri_o (GPIO output)
Purpose: Monitor LED output signals
```

#### SLOT 1: PS to Interconnect AXI Bus
```
Interface: processing_system7_0/M_AXI_GP0
Type: AXI Memory Mapped (AXI4)
Signals: AWADDR, AWVALID, AWREADY, WDATA, WVALID, WREADY,
         BRESP, BVALID, BREADY, ARADDR, ARVALID, ARREADY,
         RDATA, RVALID, RREADY, etc.
Purpose: Monitor CPU transactions to peripherals
```

#### SLOT 2: Interconnect to GPIO AXI Bus
```
Interface: ps7_0_axi_periph/M00_AXI to axi_gpio_0/S_AXI
Type: AXI Memory Mapped (AXI4-Lite)
Purpose: Monitor transactions reaching GPIO peripheral
```

### What You Can Debug
- ✅ GPIO pin state changes
- ✅ AXI read/write transactions from CPU
- ✅ AXI protocol timing and handshaking
- ✅ Data values being written to GPIO
- ✅ Transaction ordering and latency

---

## The Correct Workflow (Vitis 2023.2)

### Simple 5-Step Process

**Step 1: Start Vitis Debug Session**
1. In Vitis, press **F5** (or click Debug button)
2. Your application loads and halts at main()
3. FPGA is programmed, software is loaded

**Step 2: Run Your Application**
1. Press **F8** (Resume/Continue)
2. Your application runs in the background
3. Keep it running - don't terminate!

**Step 3: Close Vitis**
1. Close the Vitis IDE
2. Your application continues running on the board
3. This releases JTAG control

**Step 4: Open Vivado and Connect**
1. Launch Vivado
2. **Flow Navigator** → **Open Hardware Manager**
3. Click **Open target** → **Auto Connect**
4. You should see `hw_ila_1` under xc7z020_1 device

**Step 5: Trigger ILA and Capture**
1. Right-click **hw_ila_1** → **Run Trigger Immediate**
2. ILA captures whatever is currently happening
3. Waveforms appear automatically!
4. Analyze your signals

---

## Understanding The Workflow

### Why This Works

**The Problem**: JTAG can only be controlled by one tool at a time
- Vitis debugger uses JTAG to control software
- Vivado Hardware Manager uses JTAG to control ILA
- They conflict if both are open

**The Solution**:
1. Vitis programs FPGA and starts software running
2. Close Vitis → releases JTAG lock
3. Software keeps running on ARM processor (independent)
4. Vivado connects and controls ILA
5. ILA captures the running hardware activity

### Immediate Trigger Explained

**Immediate Trigger** = Capture whatever is happening right now
- No conditions needed
- Captures current bus activity
- Perfect for seeing if your hardware is working
- Shows actual signal values and timing

---

## Understanding ILA Waveforms

### Opening the Waveform Window

After triggering, waveforms appear automatically. If not:
- Right-click `hw_ila_1` → **Display Captured Waveform**
- Or look for the **Waveform** tab in Vivado

### Waveform Window Features

#### Zoom and Pan
- **Zoom In**: Ctrl + Mouse wheel up, or click **+** button
- **Zoom Out**: Ctrl + Mouse wheel down, or click **-** button
- **Zoom to Fit**: Click **Zoom Full** button
- **Pan**: Click and drag on time ruler

#### Change Radix (Number Format)
1. Right-click on signal name
2. **Waveform Style** → **Radix** → Select:
   - Binary
   - Hexadecimal
   - Decimal
   - Unsigned Decimal
   - ASCII

#### Measure Timing
1. Click on waveform at first point
2. Drag to second point
3. Tooltip shows time difference and sample count

### Reading AXI Transactions

#### AXI Write Transaction Sequence

```
Time   AWVALID  AWADDR      AWREADY  WVALID  WDATA       WREADY  BVALID  BRESP
0      0        X           X        0       X           X       0       X
10     1        0x41200000  0        0       X           0       0       X
20     1        0x41200000  1        1       0x00000005  0       0       X
30     0        X           0        1       0x00000005  1       0       X
40     0        X           0        0       X           0       1       0
```

**Analysis**:
- At time 10: CPU starts write to address 0x41200000
- At time 20: Address accepted (AWREADY), data presented (WDATA = 0x05)
- At time 30: Data accepted (WREADY)
- At time 40: Write response OK (BVALID, BRESP = 0)

**Look for these signals in your capture**:
- `net_slot_2_axi_awvalid` = Write address valid
- `net_slot_2_axi_awaddr` = Address being written to
- `net_slot_2_axi_wdata` = Data being written
- `net_slot_2_axi_awready` = Slave ready for address
- `net_slot_2_axi_wready` = Slave ready for data

---

## Setting Up Specific Triggers

Instead of immediate trigger, you can set conditions to capture specific events.

### Method 1: Using GUI

**Before arming the trigger:**

1. In Hardware Manager, select `hw_ila_1`
2. In the **Trigger Setup** window (top section), you'll see a table with probe names
3. Click on the probe you want to trigger on
4. Set the **Compare Value** (e.g., `== 1`)
5. Set the **Radix** (Binary, Hex, etc.)
6. Click **Run Trigger** (green play button)

### Method 2: Using Tcl Console

```tcl
# Get ILA handle
set ila [get_hw_ilas hw_ila_1]

# Set trigger on write transactions
set probe [get_hw_probes design_1_i/system_ila_0/inst/net_slot_2_axi_awvalid -of_objects $ila]
set_property TRIGGER_COMPARE_VALUE eq1'b1 $probe

# Arm trigger
run_hw_ila $ila
```

### Common Trigger Examples

#### Trigger on Any AXI Write to GPIO
```tcl
set probe [get_hw_probes design_1_i/system_ila_0/inst/net_slot_2_axi_awvalid -of_objects $ila]
set_property TRIGGER_COMPARE_VALUE eq1'b1 $probe
run_hw_ila $ila
```

#### Trigger on Specific Address
```tcl
set probe [get_hw_probes design_1_i/system_ila_0/inst/net_slot_2_axi_awaddr -of_objects $ila]
set_property TRIGGER_COMPARE_VALUE eq32'h41200000 $probe
run_hw_ila $ila
```

#### Trigger on Specific Data Value
```tcl
set probe [get_hw_probes design_1_i/system_ila_0/inst/net_slot_2_axi_wdata -of_objects $ila]
set_property TRIGGER_COMPARE_VALUE eq32'h00000005 $probe
run_hw_ila $ila
```

### Trigger Position

Control how much data before/after trigger is captured:

```tcl
# Capture 256 samples before trigger, 768 after (total 1024)
set_property CONTROL.TRIGGER_POSITION 256 [get_hw_ilas hw_ila_1]
```

---

## Practical Examples

### Example 1: Verify GPIO Writes

**Goal**: Confirm your software is writing to AXI GPIO.

**Your Code**:
```c
XGpio_DiscreteWrite(&gpio, 1, 0x5);
```

**Steps**:
1. Start Vitis debug, press F8 to run
2. Close Vitis
3. Open Vivado Hardware Manager, connect
4. Right-click hw_ila_1 → **Run Trigger Immediate**
5. Look for `net_slot_2_axi_wdata` signal
6. Should see value `0x00000005`

### Example 2: Measure AXI Latency

**Goal**: Measure time from write request to completion.

**In Waveform**:
1. Find when `net_slot_2_axi_awvalid` goes high (write starts)
2. Find when `net_slot_2_axi_bvalid` goes high (write completes)
3. Click and drag between these points
4. Note the sample count

**Calculate**:
```
Latency = (Sample_End - Sample_Start) × Clock_Period
Clock_Period = 1 / 100MHz = 10ns
Example: 25 samples × 10ns = 250ns latency
```

### Example 3: Debug Missing Transactions

**Problem**: Your code runs but nothing happens.

**Debug Process**:
1. Capture with immediate trigger
2. Check `net_slot_1_axi_awvalid` (PS side):
   - If **no activity**: CPU not generating transactions → software bug
3. Check `net_slot_2_axi_awvalid` (GPIO side):
   - If **activity on slot 1 but not slot 2**: Address routing issue
4. Check `SLOT_0_GPIO_tri_o_1`:
   - If **transactions but GPIO doesn't change**: GPIO config issue

### Example 4: Continuous Monitoring

**Goal**: Capture multiple data sets over time.

**Script**:
```tcl
# In Vivado Tcl Console
set ila [get_hw_ilas hw_ila_1]

for {set i 0} {$i < 5} {incr i} {
    puts "Capture $i..."

    # Arm and trigger immediately
    run_hw_ila $ila
    after 100

    # Wait for trigger
    wait_on_hw_ila $ila

    # Upload and save data
    set data [upload_hw_ila_data $ila]
    set filename "C:/Users/JGEALON/Documents/ZedProjects/ila/capture_${i}.csv"
    write_hw_ila_data -csv_file -file $filename $data

    puts "Saved to $filename"
    after 1000
}

puts "All captures complete!"
```

---

## Troubleshooting

### ILA Not Appearing in Hardware Manager

**Symptoms**: After connecting, no `hw_ila_1` shown.

**Solutions**:
1. Verify bitstream has ILA:
   - Check that you programmed the correct .bit file
   - ILA must be in the hardware design
2. Refresh hardware device:
   ```tcl
   refresh_hw_device [get_hw_devices xc7z020_1]
   ```
3. Check FPGA is programmed:
   - LED should be on
   - DONE pin should be high

### Waveform Window Empty

**Symptoms**: Trigger executes but no waveforms appear.

**Solutions**:
1. Check ILA actually triggered:
   - Look for status message in Tcl console
2. Manually upload data:
   ```tcl
   set data [upload_hw_ila_data [get_hw_ilas hw_ila_1]]
   display_hw_ila_data $data
   ```
3. Try immediate trigger to verify ILA works

### No Activity on Bus

**Symptoms**: Waveforms show but all signals are flat/zero.

**Possible Causes**:
1. **Software not running**: Make sure you pressed F8 in Vitis before closing
2. **Wrong peripheral**: Code using PS GPIO instead of AXI GPIO
3. **Clock not running**: Check if clock signal is toggling
4. **Reset stuck**: Check reset signals are released

**Verify software is using AXI GPIO**:
```c
// CORRECT - Uses AXI GPIO
#include "xgpio.h"
XGpio gpio;
XGpio_Initialize(&gpio, 0);
XGpio_DiscreteWrite(&gpio, 1, 0x5);

// WRONG - Uses PS GPIO (MIO)
#include "xgpiops.h"
XGpioPs gpio;
XGpioPs_WritePin(&gpio, 1, 0x5);
```

### Trigger Never Fires (Specific Trigger)

**Symptoms**: Using specific trigger condition, but ILA never triggers.

**Solutions**:
1. **Use immediate trigger first** to verify hardware is active
2. **Check trigger value matches actual data**:
   - Capture with immediate trigger
   - Look at actual signal values
   - Adjust trigger condition to match
3. **Simplify trigger condition**:
   - Start with single signal
   - Add more conditions after verifying basic trigger works

### JTAG Connection Lost

**Symptoms**: "Target disconnected" or connection errors.

**Solutions**:
1. Close Vivado
2. Power cycle the board
3. Reconnect USB cable
4. Restart from Step 1 of workflow

---

## Quick Reference

### The Simple Workflow

```
1. Vitis: F5 (Debug) → F8 (Resume)
2. Close Vitis (software keeps running)
3. Vivado: Open Hardware Manager → Connect
4. Right-click hw_ila_1 → Run Trigger Immediate
5. View waveforms!
```

### Essential Tcl Commands

```tcl
# Connect to hardware
open_hw_manager
connect_hw_server
open_hw_target

# Get ILA handle
set ila [get_hw_ilas hw_ila_1]

# Trigger immediately
run_hw_ila $ila

# Wait for trigger to complete
wait_on_hw_ila $ila

# Upload and display waveforms
set data [upload_hw_ila_data $ila]
display_hw_ila_data $data

# Save to CSV
write_hw_ila_data -csv_file -file "capture.csv" $data

# List all available probes
get_hw_probes -of_objects $ila
```

### Important File Locations

```
Bitstream: ila_app/_ide/bitstream/design_1_wrapper.bit
ELF File:  ila_app/build/ila_app.elf
XSA File:  design_1_wrapper.xsa
```

### Key Probe Names (Your Design)

```
SLOT 0 (GPIO Output):
  - design_1_i/system_ila_0/inst/SLOT_0_GPIO_tri_o_1

SLOT 1 (PS to Interconnect):
  - net_slot_1_axi_awvalid (Write address valid)
  - net_slot_1_axi_awaddr  (Write address)
  - net_slot_1_axi_wdata   (Write data)
  - net_slot_1_axi_wready  (Write ready)

SLOT 2 (Interconnect to GPIO):
  - net_slot_2_axi_awvalid (Write address valid)
  - net_slot_2_axi_awaddr  (Write address)
  - net_slot_2_axi_wdata   (Write data)
  - net_slot_2_axi_wready  (Write ready)
```

### Common Issues Quick Fix

| Problem | Quick Fix |
|---------|-----------|
| ILA won't trigger | Use immediate trigger instead |
| No waveforms | Check software is running (press F8) |
| All signals zero | Using wrong GPIO (use XGpio not XGpioPs) |
| JTAG busy | Close Vitis first |
| Can't see hw_ila_1 | Refresh device or reconnect |

---

## Exporting Data

### Export to CSV

**Method 1: GUI**
1. Right-click on waveform
2. **Export Waveform** → **CSV**
3. Choose file location
4. Open in Excel for analysis

**Method 2: Tcl**
```tcl
set data [get_hw_ila_data hw_ila_data_1]
write_hw_ila_data -csv_file -file "C:/path/to/capture.csv" $data
```

### Save Waveform Configuration

Save your trigger setup and waveform layout:
```tcl
write_hw_ila_data -wcfg_file -file "ila_setup.wcfg" [get_hw_ila_data hw_ila_data_1]

# Reload later
read_hw_ila_data "ila_setup.wcfg" hw_ila_1
```

---

## Advanced Tips

### 1. Understanding Your Application

Make sure your software is actually writing to AXI GPIO:

**Correct approach**:
```c
#include "xgpio.h"      // AXI GPIO driver
#include "xparameters.h"

XGpio gpio;

int main() {
    init_platform();

    // Initialize AXI GPIO (not PS GPIO!)
    XGpio_Initialize(&gpio, 0);
    XGpio_SetDataDirection(&gpio, 1, 0x00000000); // All outputs

    // Write loop
    while(1) {
        XGpio_DiscreteWrite(&gpio, 1, 0x0);
        XGpio_DiscreteWrite(&gpio, 1, 0x1);
        XGpio_DiscreteWrite(&gpio, 1, 0x2);
        // ... continues running
    }

    cleanup_platform();
    return 0;
}
```

**Key points**:
- Use `xgpio.h` (AXI GPIO), NOT `xgpiops.h` (PS GPIO)
- Use infinite loop or long delay so software keeps running
- This generates AXI transactions that ILA can capture

### 2. Direct Memory Access (Alternative)

If driver doesn't work, write directly to GPIO address:

```c
volatile unsigned int *gpio_addr = (unsigned int *)0x41200000;

while(1) {
    *gpio_addr = 0x0;
    *gpio_addr = 0x1;
    *gpio_addr = 0x2;
    // ... etc
}
```

This bypasses the driver and writes directly to the AXI bus.

### 3. Automated Capture Script

Save this script for repeated captures:

```tcl
# auto_capture.tcl
set ila [get_hw_ilas hw_ila_1]

# Set sample depth
set_property CONTROL.DATA_DEPTH 1024 $ila
set_property CONTROL.TRIGGER_POSITION 256 $ila

# Immediate trigger
puts "Running immediate trigger..."
run_hw_ila $ila

# Wait for completion
wait_on_hw_ila $ila

# Upload and save
set timestamp [clock format [clock seconds] -format %Y%m%d_%H%M%S]
set filename "C:/Users/JGEALON/Documents/ZedProjects/ila/capture_${timestamp}.csv"

set data [upload_hw_ila_data $ila]
write_hw_ila_data -csv_file -file $filename $data

puts "Saved to: $filename"
display_hw_ila_data $data
```

**Run it**:
```tcl
source C:/Users/JGEALON/Documents/ZedProjects/ila/auto_capture.tcl
```

---

## Summary

### The Correct Process (Vitis 2023.2)

1. **Vitis**: Debug (F5) → Resume (F8) → Close Vitis
2. **Vivado**: Open Hardware Manager → Connect
3. **Capture**: Right-click hw_ila_1 → Run Trigger Immediate
4. **Analyze**: View waveforms, zoom, measure timing

### Why This Works

- Vitis programs FPGA and starts software
- Software keeps running after Vitis closes
- Vivado connects and captures running hardware
- ILA shows actual AXI transactions in real-time

### Key Insight

**ILA monitors hardware signals, not software execution**. As long as your software is running and generating AXI transactions, ILA can capture them - regardless of which tool started the software.

---

**Document Version**: 2.0
**Last Updated**: 2026-05-03
**Vitis Version**: 2023.2 Unified IDE
**Vivado Version**: 2023.2
**Project**: ila (ZedBoard Zynq-7000)
**Hardware**: System ILA with 3 monitor slots (GPIO + AXI buses)
