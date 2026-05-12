# XSDB GPIO Access Guide for Zynq-7000

## Overview

This guide shows how to use **XSDB (Xilinx System Debugger)** to directly access AXI GPIO registers on the Zynq-7000 without running software. This is useful for:
- Quick hardware testing
- Debugging GPIO connections
- Verifying bitstream functionality
- Register-level testing

**XSDB** is the command-line debugger that replaced XSCT/XMD in Vitis 2019.2+. It provides direct memory access to the Zynq PS and PL peripherals.

---

## Prerequisites

- Zedboard connected via USB JTAG
- Vivado/Vitis 2023.2 installed
- Bitstream (.bit file) generated
- Cable drivers installed

---

## Step-by-Step Procedure

### 1. Launch XSDB

Launch Vitis IDE:
Open the workspace the project created. Then "Terminal -> New Terminal"
C:\Users\JGEALON\Documents\ZedProjects\axi4lite_gpio\xsdb /*run xsdb*/
```

You should see:
```
****** System Debugger (XSDB) v2023.2
  **** Build date : Oct 13 2023-21:31:41
xsdb%
```

### 2. Connect to Zedboard

```tcl
xsdb% connect
```

**Output:**
```
attempting to launch hw_server
****** Xilinx hw_server v2023.2
INFO: hw_server application started
INFO: To connect to this hw_server instance use url: TCP:127.0.0.1:3121
tcfchan#0
```

### 3. List Available Targets

```tcl
xsdb% targets
```

**Expected Output:**
```
  1  APU
     2  ARM Cortex-A9 MPCore #0 (Running)
     3  ARM Cortex-A9 MPCore #1 (Running)
  4  xc7z020
```

**Target Description:**
- **Target 1 (APU)**: Application Processing Unit (container)
- **Target 2 (Core #0)**: Primary ARM processor - use for memory access
- **Target 3 (Core #1)**: Secondary ARM processor
- **Target 4 (xc7z020)**: FPGA fabric - use for bitstream download

### 4. Program FPGA Bitstream

```tcl
xsdb% targets 4
xsdb% fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit
```

**Output:**
```
100%    5MB   0.5MB/s  00:10
```

**Note:** Use forward slashes `/` in paths, even on Windows.

### 5. Initialize Processing System (PS)

Switch to ARM core and initialize the PS:

```tcl
xsdb% targets 2
xsdb% source C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config
```

**What This Does:**
- Enables clocks (FCLK_CLK0, etc.)
- Configures AXI interfaces
- Enables PS-PL level shifters
- Sets up DDR controller

**Without PS initialization, you'll get this error:**
```
Memory write error at 0x41200000. Blocked address 0x41200000.
Cannot access FPGA: Level shifters are not enabled
```

### 6. Access GPIO Registers

Use the `-force` flag to bypass memory map checks:

```tcl
# Write to LED GPIO (assuming base address 0x41200000)
xsdb% mwr -force 0x41200000 0xFF

# Read back LED GPIO value
xsdb% mrd -force 0x41200000

# Read switch GPIO (assuming base address 0x41210000)
xsdb% mrd -force 0x41210000
```

**Why `-force` is needed:**

Without `-force`, you may get:
```
Memory write error at 0x41200000. PL AXI slave ports access is not allowed.
This address has not been added to the memory map
```

The `-force` flag bypasses XSDB's memory map protection, allowing direct hardware access.

---

## Critical: Common Mistakes to Avoid

### ❌ WRONG: Skipping PS Initialization Before ELF Download

```tcl
# This will FAIL!
xsdb% connect
xsdb% targets 4
xsdb% fpga design_1_wrapper.bit
xsdb% targets 2
xsdb% dow application.elf    # ❌ ERROR: MMU page translation fault
```

**Error you'll get:**
```
Memory write error at 0x100000. MMU page translation fault
```

### ✅ CORRECT: Always Initialize PS Before ELF Download

```tcl
# Method 1: Using ps7_init
xsdb% connect
xsdb% targets 4
xsdb% fpga design_1_wrapper.bit
xsdb% targets 2
xsdb% stop
xsdb% rst -processor
xsdb% source <path>/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config
xsdb% dow application.elf     # ✅ Works!
xsdb% con

# Method 2: Using FSBL (Recommended)
xsdb% connect
xsdb% targets 4
xsdb% fpga design_1_wrapper.bit
xsdb% targets 2
xsdb% dow fsbl.elf
xsdb% con
xsdb% after 2000
xsdb% stop
xsdb% dow application.elf     # ✅ Works!
xsdb% con
```

### ❌ WRONG: Not Resetting Before Re-initialization

```tcl
# If PS init already ran but in bad state
xsdb% ps7_init               # ❌ May not work properly
xsdb% dow application.elf    # ❌ Still fails
```

### ✅ CORRECT: Always Reset Before Re-initialization

```tcl
xsdb% targets 2
xsdb% stop
xsdb% rst -processor         # ✅ Reset first!
xsdb% source <path>/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config
xsdb% dow application.elf    # ✅ Works!
```

### ❌ WRONG: Forgetting -force Flag for GPIO Access

```tcl
xsdb% mwr 0x41200000 0xFF    # ❌ Memory map error
```

### ✅ CORRECT: Always Use -force for GPIO

```tcl
xsdb% mwr -force 0x41200000 0xFF    # ✅ Works!
```

### ❌ WRONG: Relative Paths Without cd

```tcl
xsdb% fpga design_1_wrapper.bit    # ❌ File not found
```

### ✅ CORRECT: Use Full Absolute Paths

```tcl
xsdb% fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit    # ✅ Works!
```

---

## Common GPIO Operations

### LED Control Examples

Assuming LED GPIO base address: `0x41200000`

```tcl
# Turn on all LEDs
xsdb% mwr -force 0x41200000 0xFF

# Turn off all LEDs
xsdb% mwr -force 0x41200000 0x00

# Alternating pattern (01010101)
xsdb% mwr -force 0x41200000 0x55

# Alternating pattern (10101010)
xsdb% mwr -force 0x41200000 0xAA

# Individual LED control (LED0 and LED7 on)
xsdb% mwr -force 0x41200000 0x81

# Read current LED state
xsdb% mrd -force 0x41200000
```

### Switch Reading Examples

Assuming Switch GPIO base address: `0x41210000`

```tcl
# Read switch values
xsdb% mrd -force 0x41210000

# Output example:
41210000:   000000A5
```

This means switches are in pattern: `10100101` (SW7-SW0)

### GPIO Direction Register

For bidirectional GPIO, set direction using offset `0x4`:
- `0` = Output
- `1` = Input

```tcl
# Configure all GPIO pins as outputs
xsdb% mwr -force 0x41200004 0x00000000

# Configure all GPIO pins as inputs
xsdb% mwr -force 0x41200004 0xFFFFFFFF

# Mixed: GPIO[7:4] = input, GPIO[3:0] = output
xsdb% mwr -force 0x41200004 0xF0
```

---

## AXI GPIO Register Map

| Offset | Register Name | Description |
|--------|--------------|-------------|
| 0x00   | GPIO_DATA    | GPIO data register |
| 0x04   | GPIO_TRI     | GPIO direction register (0=output, 1=input) |
| 0x08   | GPIO2_DATA   | GPIO2 data register (if dual channel) |
| 0x0C   | GPIO2_TRI    | GPIO2 direction register |

---

## Complete Testing Script

Here's a complete sequence for testing your GPIO design:

```tcl
# ===== CONNECTION =====
connect
targets

# ===== FPGA PROGRAMMING =====
targets 4
fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit

# ===== PS INITIALIZATION =====
targets 2
source C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl
ps7_init
ps7_post_config

# ===== GPIO BASE ADDRESSES =====
# Update these based on your Address Editor in Vivado
set GPIO_LED_BASE  0x41200000
set GPIO_SW_BASE   0x41210000

# ===== LED TESTING =====
puts "Testing LED patterns..."

# Pattern 1: All on
mwr -force $GPIO_LED_BASE 0xFF
after 1000

# Pattern 2: Alternating
mwr -force $GPIO_LED_BASE 0x55
after 1000

# Pattern 3: Alternating reverse
mwr -force $GPIO_LED_BASE 0xAA
after 1000

# Pattern 4: Walking LED
for {set i 0} {$i < 8} {incr i} {
    set pattern [expr 1 << $i]
    mwr -force $GPIO_LED_BASE $pattern
    after 500
}

# ===== SWITCH READING =====
puts "Reading switch values..."
set sw_val [mrd -force -value $GPIO_SW_BASE]
puts [format "Switch value: 0x%02X" $sw_val]

# ===== CLEANUP =====
puts "Test complete!"
```

---

## XSDB Command Reference

### Connection Commands

| Command | Description |
|---------|-------------|
| `connect` | Connect to hardware server |
| `connect -url TCP:localhost:3121` | Connect to specific server |
| `disconnect` | Disconnect from target |
| `targets` | List available targets |
| `jtag targets` | List JTAG chain devices |

### Target Commands

| Command | Description |
|---------|-------------|
| `targets <id>` | Select target by ID |
| `targets 2` | Select ARM Core #0 |
| `targets 4` | Select FPGA fabric |

### FPGA Commands

| Command | Description |
|---------|-------------|
| `fpga <bitstream.bit>` | Program FPGA |
| `fpga -state` | Check FPGA programming state |

### Memory Access Commands

| Command | Description |
|---------|-------------|
| `mwr <addr> <value>` | Write 32-bit word to address |
| `mwr -force <addr> <value>` | Force write (bypass checks) |
| `mrd <addr>` | Read 32-bit word from address |
| `mrd -force <addr>` | Force read (bypass checks) |
| `mrd -value <addr>` | Read and return value only |
| `mrd -size b <addr>` | Read byte (8-bit) |
| `mrd -size h <addr>` | Read halfword (16-bit) |
| `mrd -size w <addr>` | Read word (32-bit, default) |

### PS Initialization

| Command | Description |
|---------|-------------|
| `source ps7_init.tcl` | Load PS initialization script |
| `ps7_init` | Initialize processing system |
| `ps7_post_config` | Post-configuration setup |

### Program Execution

| Command | Description |
|---------|-------------|
| `dow <elf>` | Download ELF file to memory |
| `con` | Continue execution |
| `stop` | Stop execution |
| `rst -system` | System reset |
| `rst -processor` | Processor reset |

---

## Troubleshooting

### Issue 1: No Targets Found

**Symptom:**
```
xsdb% targets
xsdb%
```

**Solution:**
1. Check board power (power LED on)
2. Verify USB JTAG cable connection
3. Install cable drivers:
   ```bash
   cd C:\Xilinx\Vivado\2023.2\data\xicom\cable_drivers\nt64\digilent
   install_digilent.exe
   ```
4. Try in Vivado Hardware Manager first

### Issue 2: Level Shifters Not Enabled

**Symptom:**
```
Memory write error at 0x41200000. Cannot access FPGA: Level shifters are not enabled
```

**Solution:**
Run PS initialization:
```tcl
targets 2
source <path>/ps7_init.tcl
ps7_init
ps7_post_config
```

### Issue 3: Address Not in Memory Map

**Symptom:**
```
Memory write error at 0x41200000. This address has not been added to the memory map
```

**Solution:**
Use `-force` flag:
```tcl
mwr -force 0x41200000 0xFF
```

### Issue 4: Wrong Base Address

**Symptom:**
No error, but LEDs don't respond

**Solution:**
1. Open Vivado project
2. Go to **Address Editor** tab
3. Find your AXI GPIO base address
4. Update commands with correct address

### Issue 5: Bitstream Not Found

**Symptom:**
```
couldn't open "design_1_wrapper.bit": no such file or directory
```

**Solution:**
Use full absolute path with forward slashes:
```tcl
fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit
```

### Issue 6: MMU Page Translation Fault (ELF Download Fails)

**Symptom:**
```
xsdb% dow C:/Users/.../axi4lite_gpio_app.elf
Downloading Program -- C:/Users/.../axi4lite_gpio_app.elf
        section, .text: 0x00100000 - 0x0010545f
        ...
aborting, 1 pending requests...
Failed to download C:/Users/.../axi4lite_gpio_app.elf
Memory write error at 0x100000. MMU page translation fault
```

**Cause:** The processor's MMU (Memory Management Unit) and DDR memory controller are not properly initialized. The ELF file needs to be loaded into DDR RAM starting at address `0x00100000`, but the memory isn't accessible yet.

**Solution 1: Reset and Re-initialize PS**

The PS initialization may have failed or the processor is in a bad state. Reset first:

```tcl
# Stop the processor
xsdb% targets 2
xsdb% stop

# Reset the processor
xsdb% rst -processor

# Re-initialize PS
xsdb% source C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config

# Now download ELF
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_app/build/axi4lite_gpio_app.elf

# Run
xsdb% con
```

**Solution 2: Use FSBL (Recommended - Most Reliable)**

The FSBL (First Stage Boot Loader) handles all hardware initialization automatically, including DDR, MMU, caches, and clocks. This is the most reliable method:

```tcl
# Target ARM core
xsdb% targets 2

# Stop and reset processor
xsdb% stop
xsdb% rst -processor

# Download and run FSBL first
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_pfrm/zynq_fsbl/build/fsbl.elf
xsdb% con

# Wait for FSBL to complete (typically 1-2 seconds)
xsdb% after 2000
xsdb% stop

# Now download your application
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_app/build/axi4lite_gpio_app.elf
xsdb% con
```

**Solution 3: Full System Reset**

If the above doesn't work, start completely fresh:

```tcl
# Disconnect and reconnect
xsdb% disconnect
xsdb% connect

# Re-program FPGA
xsdb% targets 4
xsdb% fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit

# Target core, stop, and system reset
xsdb% targets 2
xsdb% stop
xsdb% rst -system

# Wait for reset to complete
xsdb% after 1000

# Initialize PS
xsdb% source C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config

# Download and run
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_app/build/axi4lite_gpio_app.elf
xsdb% con
```

**Why This Happens:**

The Zynq-7000 boot sequence requires:
1. **DDR Controller Initialization** - Configure DDR3 memory timing and parameters
2. **MMU Setup** - Configure virtual-to-physical memory translation
3. **Cache Configuration** - Enable L1/L2 caches
4. **Clock Configuration** - Set up CPU and peripheral clocks
5. **Level Shifter Enable** - Allow PS-PL communication

The `ps7_init` script or FSBL handles all of these. If you try to download an ELF before this initialization:
- The DDR memory at `0x00100000` is not accessible
- The MMU doesn't have valid page tables
- Memory writes fail with "MMU page translation fault"

**Best Practice:**

Always use this sequence:
1. Program FPGA (`targets 4; fpga <bit>`)
2. Initialize PS (`targets 2; ps7_init; ps7_post_config`) **OR** run FSBL
3. Download application (`dow <elf>`)
4. Run (`con`)

**Never skip step 2!**

### Issue 7: PS Initialization Seems to Run But Still Fails

**Symptom:**
You run `ps7_init` and `ps7_post_config` with no errors, but ELF download still fails with MMU fault.

**Solution:**
The processor may have been in a bad state when you ran the initialization. The initialization scripts expect the processor to be in a clean reset state.

**Always reset before initializing:**
```tcl
xsdb% targets 2
xsdb% stop
xsdb% rst -processor
xsdb% source <path>/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config
```

Or use FSBL which handles this automatically.

---

## Downloading and Running ELF Applications

### Finding Your ELF Files

Your compiled applications are typically located at:
- **Application ELF**: `axi4lite_gpio_app/build/axi4lite_gpio_app.elf`
- **FSBL ELF**: `axi4lite_gpio_pfrm/zynq_fsbl/build/fsbl.elf`

### Method 1: Direct Application Download (Quick Test)

This method downloads your application directly without running FSBL. Best for quick testing.

```tcl
# Connect and program FPGA
xsdb% connect
xsdb% targets 4
xsdb% fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit

# Initialize PS
xsdb% targets 2
xsdb% source C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config

# Download and run your application
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_app/build/axi4lite_gpio_app.elf
xsdb% con
```

**Output:** Your application's `printf()` output will appear in the XSDB console.

### Method 2: With FSBL (Full Boot Sequence)

This method runs FSBL first, then loads your application. Recommended for production.

```tcl
# Connect and program FPGA
xsdb% connect
xsdb% targets 4
xsdb% fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit

# Target ARM core
xsdb% targets 2

# Download and run FSBL
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_pfrm/zynq_fsbl/build/fsbl.elf
xsdb% con
xsdb% after 2000
xsdb% stop

# Download and run your application
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_app/build/axi4lite_gpio_app.elf
xsdb% con
```

### ELF Download Commands Reference

| Command | Description |
|---------|-------------|
| `dow <elf_file>` | Download ELF file to target memory |
| `con` | Continue/run execution from current PC |
| `stop` | Stop execution |
| `rst -processor` | Reset processor only (soft reset) |
| `rst -system` | Full system reset (hard reset) |
| `rrd` | Read all CPU registers (PC, SP, LR, etc.) |
| `rrd <register>` | Read specific register (e.g., `rrd pc`) |
| `rwr <reg> <value>` | Write to CPU register |
| `stp` | Single step execution |

### Controlling Your Application

```tcl
# Stop running application
xsdb% stop

# View current registers (PC, SP, LR, etc.)
xsdb% rrd

# Resume execution
xsdb% con

# Reset and restart
xsdb% rst -processor
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_app/build/axi4lite_gpio_app.elf
xsdb% con
```

### Debugging Your Application

```tcl
# Set breakpoint at address (e.g., main function)
xsdb% bpadd 0x00100400

# List breakpoints
xsdb% bplist

# Remove breakpoint
xsdb% bpremove 1

# Continue until breakpoint
xsdb% con

# Single step when stopped
xsdb% stp

# View local variables (requires debug symbols)
xsdb% locals

# View stack backtrace
xsdb% bt
```

### Monitoring Application While Running

```tcl
# Read GPIO registers while app is running (use -force)
xsdb% mrd -force 0x41200000

# Read memory at specific address
xsdb% mrd 0x00100000

# Stop application temporarily to inspect
xsdb% stop

# Check program counter
xsdb% rrd pc

# Resume
xsdb% con
```

### Example: Complete Debug Session

```tcl
# 1. Setup
xsdb% connect
xsdb% targets 4
xsdb% fpga C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit
xsdb% targets 2
xsdb% source C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl
xsdb% ps7_init
xsdb% ps7_post_config

# 2. Load application
xsdb% dow C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio/axi4lite_gpio_app/build/axi4lite_gpio_app.elf

# 3. Set breakpoint at main
xsdb% bpadd *main

# 4. Run to breakpoint
xsdb% con

# 5. Step through code
xsdb% stp
xsdb% stp

# 6. Check GPIO value
xsdb% mrd -force 0x41200000

# 7. Continue execution
xsdb% con
```

---

## Finding Your GPIO Base Addresses

### Method 1: Vivado Address Editor

1. Open your Vivado project
2. Open Block Design
3. Go to **Address Editor** tab
4. Look for your AXI GPIO instances:
   ```
   axi_gpio_0: 0x41200000 - 0x4120FFFF
   axi_gpio_1: 0x41210000 - 0x4121FFFF
   ```

### Method 2: Check Design Files

```bash
grep -r "41200000" axi4lite_gpio.gen/sources_1/bd/design_1/
```

### Method 3: View Block Design TCL

Open `design_1.tcl` in your block design folder and search for GPIO address assignments.

---

## Quick Reference Card

```
=== QUICK START (Manual GPIO Access) ===
xsdb% connect
xsdb% targets 4
xsdb% fpga <path/to/bitstream.bit>
xsdb% targets 2
xsdb% source <path/to/ps7_init.tcl>
xsdb% ps7_init
xsdb% ps7_post_config
xsdb% mwr -force 0x41200000 0xFF    # LEDs on
xsdb% mrd -force 0x41210000         # Read switches

=== QUICK START (Run Application) ===
xsdb% connect
xsdb% targets 4
xsdb% fpga <path/to/bitstream.bit>
xsdb% targets 2
xsdb% source <path/to/ps7_init.tcl>
xsdb% ps7_init
xsdb% ps7_post_config
xsdb% dow <path/to/application.elf>
xsdb% con

=== LED PATTERNS ===
All on:     mwr -force 0x41200000 0xFF
All off:    mwr -force 0x41200000 0x00
Pattern 1:  mwr -force 0x41200000 0x55
Pattern 2:  mwr -force 0x41200000 0xAA

=== READ OPERATIONS ===
Switches:   mrd -force 0x41210000
LEDs:       mrd -force 0x41200000

=== APPLICATION CONTROL ===
Load:       dow <path/to/app.elf>
Run:        con
Stop:       stop
Restart:    rst -processor; dow <elf>; con
Registers:  rrd
Breakpoint: bpadd <address>
Step:       stp
```

---

## Advanced: Creating Reusable Scripts

### Script 1: Manual GPIO Testing

Save this as `gpio_test.tcl`:

```tcl
#!/usr/bin/tclsh
# GPIO Test Script for XSDB - Manual GPIO Access

set PROJECT_DIR "C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio"
set BITSTREAM "$PROJECT_DIR/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit"
set PS7_INIT "$PROJECT_DIR/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl"
set GPIO_LED 0x41200000
set GPIO_SW  0x41210000

proc init_hardware {} {
    global BITSTREAM PS7_INIT
    puts "Connecting to board..."
    connect

    puts "Programming FPGA..."
    targets 4
    fpga $BITSTREAM

    puts "Initializing PS..."
    targets 2
    stop
    rst -processor
    source $PS7_INIT
    ps7_init
    ps7_post_config
    puts "Hardware initialized!"
}

proc test_leds {} {
    global GPIO_LED
    puts "Testing LED patterns..."
    set patterns {0xFF 0x00 0x55 0xAA 0x0F 0xF0}
    foreach pattern $patterns {
        puts "  Pattern: $pattern"
        mwr -force $GPIO_LED $pattern
        after 500
    }
}

proc read_switches {} {
    global GPIO_SW
    set val [mrd -force -value $GPIO_SW]
    puts [format "Switch value: 0x%02X" $val]
    return $val
}

# Main execution
init_hardware
test_leds
read_switches
puts "Test complete!"
```

Run with:
```bash
xsdb gpio_test.tcl
```

### Script 2: Application Download and Run

Save this as `run_app.tcl`:

```tcl
#!/usr/bin/tclsh
# Application Download Script with FSBL

set PROJECT_DIR "C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio"
set BITSTREAM "$PROJECT_DIR/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit"
set FSBL "$PROJECT_DIR/axi4lite_gpio_pfrm/zynq_fsbl/build/fsbl.elf"
set APP "$PROJECT_DIR/axi4lite_gpio_app/build/axi4lite_gpio_app.elf"

puts "=== XSDB Application Loader ==="

# Connect
puts "\n1. Connecting to board..."
connect

# Program FPGA
puts "\n2. Programming FPGA..."
targets 4
fpga $BITSTREAM
puts "   FPGA programmed successfully"

# Target ARM core
targets 2

# Run FSBL
puts "\n3. Running FSBL..."
stop
rst -processor
dow $FSBL
con
after 2000
stop
puts "   FSBL completed"

# Download application
puts "\n4. Downloading application..."
dow $APP
puts "   Application loaded"

# Run application
puts "\n5. Running application..."
puts "   (Use Ctrl+C to stop, or type 'stop' in XSDB)\n"
con
```

Run with:
```bash
xsdb run_app.tcl
```

### Script 3: Quick Reset and Reload

Save this as `reload_app.tcl`:

```tcl
#!/usr/bin/tclsh
# Quick reload script (assumes FPGA already programmed)

set PROJECT_DIR "C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio"
set PS7_INIT "$PROJECT_DIR/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl"
set APP "$PROJECT_DIR/axi4lite_gpio_app/build/axi4lite_gpio_app.elf"

puts "=== Quick Application Reload ==="

# Target ARM core
targets 2

# Stop and reset
puts "Resetting processor..."
stop
rst -processor

# Initialize PS
puts "Initializing PS..."
source $PS7_INIT
ps7_init
ps7_post_config

# Download and run
puts "Loading application..."
dow $APP
puts "Running...\n"
con
```

Run with:
```bash
xsdb reload_app.tcl
```

### Script 4: Complete Debug Session

Save this as `debug_app.tcl`:

```tcl
#!/usr/bin/tclsh
# Debug application with breakpoints

set PROJECT_DIR "C:/Users/JGEALON/Documents/ZedProjects/axi4lite_gpio"
set BITSTREAM "$PROJECT_DIR/axi4lite_gpio.runs/impl_1/design_1_wrapper.bit"
set PS7_INIT "$PROJECT_DIR/axi4lite_gpio.gen/sources_1/bd/design_1/ip/design_1_processing_system7_0_0/ps7_init.tcl"
set APP "$PROJECT_DIR/axi4lite_gpio_app/build/axi4lite_gpio_app.elf"

puts "=== XSDB Debug Session ==="

# Setup hardware
connect
targets 4
fpga $BITSTREAM
targets 2
stop
rst -processor
source $PS7_INIT
ps7_init
ps7_post_config

# Load application
puts "Loading application..."
dow $APP

# Set breakpoint at main
puts "Setting breakpoint at main..."
bpadd *main

# Run to breakpoint
puts "Running to main...\n"
con

puts "=== Stopped at main ==="
puts "Available commands:"
puts "  con       - Continue execution"
puts "  stp       - Single step"
puts "  rrd       - Read registers"
puts "  bplist    - List breakpoints"
puts "  stop      - Stop execution"
```

Run with:
```bash
xsdb debug_app.tcl
```

---

## Summary

### For Manual GPIO Access (No Software)

1. **Connect**: `connect`
2. **Program FPGA**: `targets 4; fpga <bitstream.bit>`
3. **Initialize PS**: `targets 2; stop; rst -processor; source ps7_init.tcl; ps7_init; ps7_post_config`
4. **Access GPIO**: `mwr -force <addr> <value>` and `mrd -force <addr>`
5. **Always use `-force`**: Bypass memory map checks for GPIO registers

### For Running Applications (With Software)

**Method 1: Using ps7_init.tcl**
```tcl
connect
targets 4; fpga <bitstream.bit>
targets 2
stop; rst -processor
source <path>/ps7_init.tcl
ps7_init; ps7_post_config
dow <application.elf>
con
```

**Method 2: Using FSBL (Recommended)**
```tcl
connect
targets 4; fpga <bitstream.bit>
targets 2
stop; rst -processor
dow <fsbl.elf>; con; after 2000; stop
dow <application.elf>
con
```

### Key Points to Remember

✅ **DO:**
- Always reset processor before PS initialization: `stop; rst -processor`
- Use full absolute paths with forward slashes: `C:/Users/.../file.bit`
- Use `-force` flag for GPIO register access
- Initialize PS before downloading ELF files
- Use FSBL for reliable initialization

❌ **DON'T:**
- Skip PS initialization before ELF download (causes MMU fault)
- Re-run ps7_init without resetting first (causes bad state)
- Use relative paths without changing directory first
- Forget the `-force` flag for GPIO operations
- Try to access DDR memory (`0x00100000+`) without initialization

### Most Common Error and Fix

**Error:**
```
Memory write error at 0x100000. MMU page translation fault
```

**Fix:**
```tcl
targets 2
stop
rst -processor
source <path>/ps7_init.tcl
ps7_init
ps7_post_config
dow <elf>
con
```

---

## References

- **XSCT Reference Guide**: UG1208 (ug1208-xsct-reference-guide.pdf)
- **Zynq-7000 Technical Reference Manual**: UG585
- **AXI GPIO Product Guide**: PG144
- **Vivado Design Suite User Guide**: UG973

---

**Document Version:** 2.0
**Date:** 2025
**Last Updated:** Added MMU fault troubleshooting, FSBL workflow, and common mistakes section
**Tested with:** Vivado/Vitis 2023.2, Zedboard (Zynq-7020)
