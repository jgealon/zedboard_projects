# How to Check BSP Files in Vitis 2023.2

A comprehensive guide to viewing, inspecting, and verifying Board Support Package (BSP) files in Xilinx Vitis 2023.2.

## 📋 Table of Contents
- [BSP File Locations](#bsp-file-locations)
- [Method 1: Using Vitis GUI](#method-1-using-vitis-gui)
- [Method 2: Using File Explorer](#method-2-using-file-explorer)
- [Method 3: Using XSCT](#method-3-using-xsct)
- [Checking Platform Drivers](#checking-platform-drivers)
- [Key BSP Files to Check](#key-bsp-files-to-check)
- [Verifying BSP Configuration](#verifying-bsp-configuration)
- [Common Issues](#common-issues)

---

## BSP File Locations

In Vitis 2023.2, BSP files are organized as follows:

```
workspace/
├── platform_project/
│   ├── platform.spr                          # Platform project file
│   ├── hw/
│   │   └── design_wrapper.xsa                # Hardware specification
│   ├── export/
│   │   └── platform/
│   │       └── sw/
│   │           └── [domain_name]/
│   │               ├── include/
│   │               │   └── xparameters.h     # ✅ EXPORTED xparameters
│   │               └── lib/
│   └── [processor]/
│       └── [domain_name]/
│           └── bsp/
│               ├── include/                   # BSP headers
│               │   ├── xparameters.h         # ✅ Main parameters file
│               │   ├── xparameters_ps.h
│               │   ├── xgpio.h               # Peripheral headers
│               │   └── ...
│               └── libsrc/                    # Driver sources
│                   ├── common/
│                   ├── gpio/
│                   ├── standalone/
│                   └── ...
```

**Example Path:**
```
platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/
```

---

## Method 1: Using Vitis GUI

### Step 1: Open Vitis IDE

1. Launch **Vitis 2023.2**
2. Select your workspace

### Step 2: Locate Platform Project

In the **Explorer** pane (left side):

```
📁 [workspace_name]
  ├── 📁 platform_project_name
  │   ├── 📄 platform.spr
  │   ├── 📁 hw
  │   ├── 📁 ps7_cortexa9_0
  │   │   └── 📁 standalone_ps7_cortexa9_0
  │   │       ├── 📁 bsp                    ⬅️ BSP folder
  │   │       └── 📄 domain.bif
  │   └── 📁 export
```

### Step 3: View BSP Settings

#### Option A: Platform Settings Window

1. **Double-click** on `platform.spr` file
2. The **Platform Settings** window opens
3. Navigate through tabs:
   - **Overview** - Platform summary
   - **Hardware** - .xsa file information
   - **Domains** - List of software domains

4. In **Domains** section:
   - Click on a domain (e.g., `standalone_ps7_cortexa9_0`)
   - View BSP settings in the properties panel

#### Option B: BSP Settings Dialog

1. Right-click on **platform project**
2. Select **Board Support Package Settings**
3. A dialog window opens showing:
   - **Overview** tab - BSP information
   - **Standalone** tab - OS settings
   - **Drivers** tab - Peripheral drivers
   - **Libraries** tab - Available libraries

### Step 4: View BSP Files Directly

#### Expand BSP Folder:

```
📁 standalone_ps7_cortexa9_0
  └── 📁 bsp
      ├── 📁 include               ⬅️ Header files
      │   ├── 📄 xparameters.h     ⬅️ CHECK THIS
      │   ├── 📄 xgpio.h
      │   ├── 📄 xuartps.h
      │   └── ...
      ├── 📁 lib                    ⬅️ Compiled libraries
      │   └── 📄 libxil.a
      └── 📁 libsrc                 ⬅️ Driver source code
          ├── 📁 common
          ├── 📁 gpio
          ├── 📁 standalone
          └── ...
```

#### To View xparameters.h:

1. Navigate to: `bsp/include/xparameters.h`
2. **Double-click** to open in editor
3. Review device IDs, addresses, and configurations

---

## Method 2: Using File Explorer

### Step 1: Navigate to Workspace

Open Windows File Explorer (or Linux file manager):

```
C:\Users\JGEALON\Documents\ZedProjects\dual_channel_axi_gpio\platform\
```

### Step 2: Browse to BSP Directory

```
platform\ps7_cortexa9_0\standalone_ps7_cortexa9_0\bsp\
```

### Step 3: Key Files to Check

#### 📄 xparameters.h
**Location:** `bsp/include/xparameters.h`

Open with text editor to verify:
```c
// Check device instance counts
#define XPAR_XGPIO_NUM_INSTANCES 1

// Check base addresses
#define XPAR_AXI_GPIO_0_BASEADDR 0x41200000
#define XPAR_XGPIO_0_BASEADDR 0x41200000

// Check device IDs
#define XPAR_XGPIO_0_DEVICE_ID 0

// Check peripheral properties
#define XPAR_XGPIO_0_IS_DUAL 0x1
#define XPAR_XGPIO_0_GPIO_WIDTH 0x8
```

#### 📄 Makefile
**Location:** `bsp/libsrc/*/src/Makefile`

Shows compiler flags and build settings

#### 📄 xparameters_ps.h
**Location:** `bsp/include/xparameters_ps.h`

Contains Processing System specific parameters

#### 📁 libsrc Directory
**Location:** `bsp/libsrc/`

Contains source code for all drivers:
- `common/` - Common utility functions
- `gpio/` - GPIO driver
- `standalone/` - Standalone OS functions
- `scugic/` - Interrupt controller
- `uartps/` - UART driver
- etc.

---

## Method 3: Using XSCT

### Step 1: Launch XSCT

```bash
C:\Xilinx\Vitis\2023.2\bin\xsct.bat
```

### Step 2: Set Workspace

```tcl
xsct% setws C:/Users/JGEALON/Documents/ZedProjects/dual_channel_axi_gpio
```

### Step 3: List Platforms

```tcl
xsct% platform list
```

**Output:**
```
platform
```

### Step 4: Activate Platform

```tcl
xsct% platform active platform
```

### Step 5: List Domains

```tcl
xsct% domain list
```

**Output:**
```
standalone_ps7_cortexa9_0
zynq_fsbl
```

### Step 6: Activate Domain

```tcl
xsct% domain active standalone_ps7_cortexa9_0
```

### Step 7: Report BSP Information

```tcl
xsct% bsp config
```

**Output:** Lists all BSP configuration parameters

### Step 8: Check Specific Settings

```tcl
# Check stdin
xsct% bsp config stdin

# Check stdout
xsct% bsp config stdout

# Check compiler flags
xsct% bsp config extra_compiler_flags

# List all drivers
xsct% bsp listdrivers
```

### Step 9: Get BSP Details

```tcl
xsct% bsp report
```

Shows comprehensive BSP information including:
- OS type
- Processor type
- Driver versions
- Library settings

---

## Checking Platform Drivers

### Overview

Drivers are software components that provide APIs to control hardware peripherals. Each peripheral in your hardware design requires a corresponding driver in the BSP.

### Method 1: Check Drivers via Vitis GUI

#### Step 1: Open BSP Settings

1. Right-click on **platform project**
2. Select **Board Support Package Settings**
3. Click on the **Drivers** tab

#### Step 2: View Driver List

The **Drivers** tab shows:

| Column | Description | Example |
|--------|-------------|---------|
| **IP Name** | Hardware peripheral instance | `axi_gpio_0` |
| **Driver Name** | Software driver used | `gpio` |
| **Driver Version** | Version of driver | `v4.9` |
| **Driver Type** | Category of driver | `IO` |

#### Step 3: Check Driver Properties

1. Click on any driver in the list
2. View properties in the bottom panel:
   - Driver name
   - Version
   - Source directory
   - Configuration parameters

**Example Screenshot Reference:**
```
┌─────────────────────────────────────────────┐
│ IP Name         │ Driver Name │ Version     │
├─────────────────────────────────────────────┤
│ axi_gpio_0      │ gpio        │ v4.9        │
│ ps7_uart_1      │ uartps      │ v3.10       │
│ ps7_scugic_0    │ scugic      │ v4.8        │
│ ps7_cortexa9_0  │ cpu_cortexa9│ v2.11       │
└─────────────────────────────────────────────┘
```

### Method 2: Check Drivers via XSCT

#### List All Drivers

```tcl
xsct% setws /path/to/workspace
xsct% platform active platform_name
xsct% domain active standalone_ps7_cortexa9_0
xsct% bsp listdrivers
```

**Example Output:**
```
DRIVER NAME        VERSION    DESCRIPTION
----------------------------------------
gpio               4.9        AXI GPIO driver
uartps             3.10       PS UART driver
scugic             4.8        GIC interrupt controller
cpu_cortexa9       2.11       Cortex-A9 processor driver
dmaps              2.6        PS DMA controller
emacps             3.16       PS Ethernet MAC
gpiops             3.10       PS GPIO driver
qspips             3.9        PS Quad-SPI
scutimer           2.4        Private timer
scuwdt             2.3        Watchdog timer
sdps               3.12       SD controller
ttcps              3.15       Triple timer counter
xadcps             2.6        XADC
devcfg             3.7        Device configuration
standalone         8.1        Standalone OS
```

#### Get Detailed Driver Information

```tcl
# Get driver version
xsct% bsp config driver_version gpio

# Get all properties of a driver
xsct% bsp config -driver gpio

# List driver files
xsct% bsp listfiles gpio
```

#### Check Which Peripheral Uses Which Driver

```tcl
# Show hardware-to-driver mapping
xsct% bsp listips
```

**Example Output:**
```
IP INSTANCE         DRIVER         VERSION
------------------------------------------
axi_gpio_0          gpio           4.9
ps7_uart_1          uartps         3.10
ps7_cortexa9_0      cpu_cortexa9   2.11
ps7_scugic          scugic         4.8
ps7_ddr_0           (none)         -
```

### Method 3: Check Drivers via File System

#### Navigate to Driver Source

```
platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/
```

Each driver has its own directory:

```
libsrc/
├── gpio/               ← AXI GPIO driver
│   ├── data/
│   │   └── gpio.mdd    # Driver metadata
│   ├── doc/
│   │   └── html/       # Driver documentation
│   ├── examples/
│   │   └── *.c         # Example code
│   └── src/
│       ├── xgpio.c     # Driver implementation
│       ├── xgpio.h     # Driver header
│       └── Makefile
├── gpiops/             ← PS GPIO driver (different from AXI GPIO)
│   └── src/
├── uartps/             ← PS UART driver
│   └── src/
├── scugic/             ← Interrupt controller driver
│   └── src/
└── standalone/         ← Standalone OS support
    └── src/
```

#### Check Driver Version from .mdd File

```bash
# View driver metadata
cat platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/gpio/data/gpio.mdd
```

**Example gpio.mdd content:**
```tcl
OPTION psf_version = 2.1;

BEGIN driver gpio

  OPTION supported_peripherals = (axi_gpio);
  OPTION driver_state = ACTIVE;
  OPTION copyfiles = all;
  OPTION VERSION = 4.9;
  OPTION NAME = gpio;

END driver
```

### Method 4: Check Drivers in xparameters.h

The `xparameters.h` file shows which peripherals have drivers:

```c
// Number of instances indicates driver presence
#define XPAR_XGPIO_NUM_INSTANCES 1        // GPIO driver present
#define XPAR_XUARTPS_NUM_INSTANCES 1      // UART driver present
#define XPAR_XSCUGIC_NUM_INSTANCES 1      // GIC driver present

// If a peripheral has no driver, it won't have these definitions
```

### Common Platform Drivers

#### Zynq-7000 PS (Processing System) Drivers

| Driver Name | Purpose | Header File |
|-------------|---------|-------------|
| **cpu_cortexa9** | Cortex-A9 processor | `xil_cache.h`, `xil_mmu.h` |
| **scugic** | Generic Interrupt Controller | `xscugic.h` |
| **scutimer** | Private timer | `xscutimer.h` |
| **scuwdt** | Watchdog timer | `xscuwdt.h` |
| **uartps** | PS UART | `xuartps.h` |
| **gpiops** | PS GPIO (MIO) | `xgpiops.h` |
| **emacps** | Ethernet MAC | `xemacps.h` |
| **qspips** | Quad-SPI | `xqspips.h` |
| **sdps** | SD card controller | `xsdps.h` |
| **usbps** | USB controller | `xusbps.h` |
| **dmaps** | DMA controller | `xdmaps.h` |
| **devcfg** | Device configuration | `xdevcfg.h` |
| **xadcps** | Analog-to-Digital Converter | `xxadcps.h` |
| **ttcps** | Triple Timer Counter | `xttcps.h` |

#### AXI Peripheral Drivers (PL - Programmable Logic)

| Driver Name | Purpose | Header File |
|-------------|---------|-------------|
| **gpio** | AXI GPIO | `xgpio.h` |
| **uartlite** | AXI UART Lite | `xuartlite.h` |
| **tmrctr** | AXI Timer | `xtmrctr.h` |
| **intc** | AXI Interrupt Controller | `xintc.h` |
| **axidma** | AXI DMA | `xaxidma.h` |
| **axicdma** | AXI Central DMA | `xaxicdma.h` |
| **axivdma** | AXI Video DMA | `xaxivdma.h` |
| **axiethernet** | AXI Ethernet | `xaxiethernet.h` |
| **spi** | AXI SPI | `xspi.h` |
| **iic** | AXI IIC | `xiic.h` |

#### System Drivers

| Driver Name | Purpose | Header File |
|-------------|---------|-------------|
| **standalone** | Standalone OS | `xil_printf.h`, `sleep.h`, `xil_io.h` |
| **common** | Common utilities | `xstatus.h`, `xil_types.h` |
| **coresightps_dcc** | Debug Communication Channel | `xcoresightpsdcc.h` |

### Checking Driver Configuration

#### View Driver Parameters (XSCT)

```tcl
xsct% bsp config -driver gpio

# Example output:
C_ALL_INPUTS: 0
C_ALL_OUTPUTS: 0
C_DOUT_DEFAULT: 0x00000000
C_GPIO_WIDTH: 8
C_INTERRUPT_PRESENT: 0
C_IS_DUAL: 1
C_TRI_DEFAULT: 0xFFFFFFFF
```

#### Check Driver Examples

Each driver includes example code:

```
libsrc/gpio/examples/
├── xgpio_example.c           # Basic GPIO example
├── xgpio_intr_example.c      # Interrupt example
├── xgpio_selftest_example.c  # Self-test example
└── xgpio_tapp_example.c      # Test application
```

### Verifying Driver Installation

#### Checklist:

- [ ] Driver directory exists in `libsrc/`
- [ ] Driver header exists in `include/`
- [ ] Peripheral defined in `xparameters.h`
- [ ] Driver version matches requirements
- [ ] Example code compiles without errors

#### Quick Verification Script (XSCT)

Save as `check_drivers.tcl`:
```tcl
# check_drivers.tcl
proc check_platform_drivers {ws_path plat_name domain_name} {
    setws $ws_path
    platform active $plat_name
    domain active $domain_name

    puts "\n========================================="
    puts "Platform Driver Check"
    puts "=========================================\n"

    puts "Platform: $plat_name"
    puts "Domain: $domain_name\n"

    puts "--- Available Drivers ---"
    set drivers [bsp listdrivers]
    set count 0
    foreach {name version desc} $drivers {
        incr count
        puts [format "  %2d. %-20s v%-6s - %s" $count $name $version $desc]
    }

    puts "\n--- Hardware-to-Driver Mapping ---"
    puts [format "%-25s %-20s %s" "IP INSTANCE" "DRIVER" "VERSION"]
    puts "---------------------------------------------------------------"

    # This would need actual IP list parsing
    puts "\nTotal Drivers: [expr {$count}]"
    puts "=========================================\n"
}

# Usage: check_platform_drivers /path/to/ws platform_name domain_name
check_platform_drivers [lindex $argv 0] [lindex $argv 1] [lindex $argv 2]
```

**Run with:**
```bash
xsct check_drivers.tcl /path/to/workspace platform standalone_ps7_cortexa9_0
```

### Driver Version Compatibility

#### Check Driver Compatibility

Different Vitis versions include different driver versions. To verify compatibility:

1. **Check Release Notes**
   - Location: `libsrc/[driver]/doc/`
   - File: `ChangeLog` or `README.txt`

2. **Driver Version Format**
   ```
   v4.9 = Major.Minor
   v4.9.1 = Major.Minor.Patch
   ```

3. **Backward Compatibility**
   - Minor version updates: Usually compatible
   - Major version updates: May have API changes

#### Example: Checking GPIO Driver Version

```bash
# View changelog
cat platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/gpio/src/ChangeLog
```

### Troubleshooting Driver Issues

#### Issue: Driver Not Found

**Symptoms:**
```c
#include "xgpio.h"  // Error: file not found
```

**Check:**
1. Is peripheral in hardware design?
2. Is driver in `libsrc/` folder?
3. Is header in `include/` folder?

**Solution:**
```tcl
# Regenerate BSP
xsct% platform active platform_name
xsct% platform generate
```

#### Issue: Wrong Driver Version

**Symptoms:**
- Compilation errors
- Missing functions
- Deprecated warnings

**Solution:**
1. Check Vitis version compatibility
2. Update platform if needed
3. Regenerate BSP

#### Issue: Multiple Driver Versions

**Symptoms:**
- Conflicting definitions
- Linker errors

**Check:**
```bash
# Search for duplicate headers
find . -name "xgpio.h"
```

**Solution:**
- Keep only BSP version
- Remove user-copied headers

### Summary: Quick Driver Check Commands

```tcl
# Launch XSCT
xsct

# Set workspace
xsct% setws /path/to/workspace

# Activate platform and domain
xsct% platform active platform_name
xsct% domain active standalone_ps7_cortexa9_0

# List all drivers with versions
xsct% bsp listdrivers

# Check hardware-to-driver mapping
xsct% bsp listips

# Get full BSP report (includes drivers)
xsct% bsp report

# Exit
xsct% exit
```

---

## Key BSP Files to Check

### 1. xparameters.h ✅ MOST IMPORTANT

**Location:** `bsp/include/xparameters.h`

**What to check:**
```c
// ✅ Device instance counts
#define XPAR_XGPIO_NUM_INSTANCES 1

// ✅ Base addresses (must match hardware)
#define XPAR_XGPIO_0_BASEADDR 0x41200000

// ✅ Device IDs (for driver initialization)
#define XPAR_XGPIO_0_DEVICE_ID 0

// ✅ Interrupt IDs (if using interrupts)
#define XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR 61

// ✅ Memory ranges
#define XPAR_PS7_DDR_0_BASEADDRESS 0x100000
#define XPAR_PS7_DDR_0_HIGHADDRESS 0x20000000

// ✅ CPU clock frequency
#define XPAR_CPU_CORE_CLOCK_FREQ_HZ 666666687

// ✅ UART for printf
#define STDOUT_BASEADDRESS 0xe0001000
#define STDIN_BASEADDRESS 0xe0001000
```

### 2. Driver Header Files

**Location:** `bsp/include/*.h`

Common headers:
- `xgpio.h` - GPIO driver API
- `xuartps.h` - UART driver
- `xscugic.h` - Interrupt controller
- `xil_printf.h` - Printf functions
- `sleep.h` - Delay functions
- `xstatus.h` - Status codes

### 3. Platform Configuration

**Location:** `bsp/libsrc/standalone/src/`

Files:
- `platform_config.h` - Platform-specific configs
- `xil-crt0.S` - Startup assembly code
- `translation_table.S` - MMU settings

### 4. Linker Script

**Location:** Application `src/lscript.ld`

Check memory regions:
```ld
MEMORY
{
   ps7_ddr_0 : ORIGIN = 0x100000, LENGTH = 0x1FF00000
   ps7_ram_0 : ORIGIN = 0x0, LENGTH = 0x30000
   ps7_ram_1 : ORIGIN = 0xFFFF0000, LENGTH = 0xFE00
}
```

---

## Verifying BSP Configuration

### ✅ Checklist

Use this checklist to verify your BSP is correct:

#### 1. Hardware Match
- [ ] `.xsa` file is up-to-date
- [ ] `xparameters.h` has all expected peripherals
- [ ] Base addresses match Vivado address editor
- [ ] Device IDs are sequential (0, 1, 2...)

#### 2. Driver Availability
- [ ] All peripheral drivers are in `libsrc/`
- [ ] Driver headers exist in `include/`
- [ ] Driver versions are compatible

#### 3. STDIO Configuration
- [ ] `STDOUT_BASEADDRESS` points to correct UART
- [ ] `STDIN_BASEADDRESS` points to correct UART
- [ ] UART parameters match hardware

#### 4. Memory Map
- [ ] DDR address ranges are correct
- [ ] OCM/SRAM addresses are correct
- [ ] No overlapping regions

#### 5. Build Settings
- [ ] Compiler flags are appropriate
- [ ] Optimization level is set
- [ ] Standard library is selected

### Quick Verification Commands (XSCT)

```tcl
# Set workspace
xsct% setws /path/to/workspace

# List platforms
xsct% platform list

# Activate platform
xsct% platform active platform_name

# List domains
xsct% domain list

# Check BSP configuration
xsct% domain active standalone_ps7_cortexa9_0
xsct% bsp config stdin
xsct% bsp config stdout

# List drivers
xsct% bsp listdrivers

# Get full report
xsct% bsp report
```

---

## Common Issues

### ❌ Issue 1: xparameters.h is Empty or Incomplete

**Symptoms:**
- Missing device definitions
- Undefined macros in code

**Solution:**
```tcl
xsct% platform active platform_name
xsct% platform generate
```

or use GUI: Right-click platform → **Build Project**

### ❌ Issue 2: Wrong Device IDs

**Symptoms:**
```c
XPAR_AXI_GPIO_0_DEVICE_ID not defined
```

**Solution:**
- Use numeric IDs instead: `XGpio_LookupConfig(0)`
- Check `XPAR_XGPIO_NUM_INSTANCES` is > 0

### ❌ Issue 3: UART Not Working (No Console Output)

**Check:**
```c
// In xparameters.h
#define STDOUT_BASEADDRESS 0xe0001000  // Should be UART address
#define STDIN_BASEADDRESS 0xe0001000
```

**Solution (XSCT):**
```tcl
xsct% bsp config stdin ps7_uart_1
xsct% bsp config stdout ps7_uart_1
xsct% bsp regenerate
```

### ❌ Issue 4: Compilation Errors about Missing Headers

**Check:**
- Driver source exists in `libsrc/[peripheral]/src/`
- Header exists in `include/`
- BSP was regenerated after hardware changes

**Solution:**
Right-click platform → **Re-generate BSP Sources**

### ❌ Issue 5: Outdated BSP After Hardware Change

**Symptoms:**
- Old addresses in xparameters.h
- Missing new peripherals

**Solution:**
1. Update hardware spec:
   - Right-click platform → **Update Hardware Specification**
   - Browse to new `.xsa` file
2. Regenerate platform:
   - Right-click platform → **Build Project**

---

## BSP Information Summary Command

### Quick Info Script for XSCT

Create `check_bsp.tcl`:
```tcl
# check_bsp.tcl
set ws_path [lindex $argv 0]
set plat_name [lindex $argv 1]

setws $ws_path
platform active $plat_name

puts "\n========================================="
puts "BSP Information Summary"
puts "=========================================\n"

puts "Platform: $plat_name"
puts "Workspace: $ws_path\n"

puts "--- Domains ---"
set domains [domain list]
foreach dom $domains {
    puts "  - $dom"
}

puts "\n--- Active Domain Configuration ---"
domain active [lindex $domains 0]
puts "stdin: [bsp config stdin]"
puts "stdout: [bsp config stdout]"

puts "\n--- Available Drivers ---"
set drivers [bsp listdrivers]
foreach drv $drivers {
    puts "  - $drv"
}

puts "\n=========================================\n"
```

**Usage:**
```bash
xsct check_bsp.tcl /path/to/workspace platform_name
```

---

## Visual Studio Code BSP Browsing

If using VS Code:

1. Open workspace folder
2. Install **C/C++** extension
3. Navigate to BSP files:
   ```
   platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/
   ```
4. Use **Ctrl+P** to quickly open files:
   - Type: `xparameters.h`
   - Type: `xgpio.h`
5. Use **Ctrl+F** to search within files

---

## Summary

### To Check BSP in Vitis 2023.2:

| Method | Best For | Steps |
|--------|----------|-------|
| **Vitis GUI** | Visual inspection | Double-click `platform.spr` |
| **File Explorer** | Direct file access | Navigate to `bsp/include/` |
| **XSCT** | Automated checking | `bsp report` command |

### Most Important Files:

1. ✅ **xparameters.h** - Device definitions
2. ✅ **Driver headers** - API documentation
3. ✅ **libsrc/** - Driver implementations

### Quick Check:

```bash
# Open in text editor
C:\Users\JGEALON\Documents\ZedProjects\dual_channel_axi_gpio\
  platform\ps7_cortexa9_0\standalone_ps7_cortexa9_0\bsp\include\xparameters.h
```

Look for your peripheral definitions and verify addresses match hardware!

---

*Last Updated: April 2026*
*Compatible with: Xilinx Vitis 2023.2*
