# How to Check BSP Files in Vitis 2023.2

A comprehensive guide to viewing, inspecting, and verifying Board Support Package (BSP) files in Xilinx Vitis 2023.2.

## 📋 Table of Contents
- [BSP File Locations](#bsp-file-locations)
- [Method 1: Using Vitis GUI](#method-1-using-vitis-gui)
- [Method 2: Using File Explorer](#method-2-using-file-explorer)
- [Method 3: Using XSCT](#method-3-using-xsct)
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
