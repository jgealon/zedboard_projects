# Complete Guide: Adding llfifo Driver to Vitis Application

## Table of Contents
1. [Initial Problem](#initial-problem)
2. [Understanding the Hardware](#understanding-the-hardware)
3. [Finding the Driver](#finding-the-driver)
4. [Adding Driver to BSP](#adding-driver-to-bsp)
5. [Adding Driver to Application](#adding-driver-to-application)
6. [Fixing Application Code](#fixing-application-code)
7. [Building the Application](#building-the-application)
8. [Troubleshooting](#troubleshooting)

---

## Initial Problem

### Symptoms
When building the application, you get compilation errors like:
```
fatal error: xllfifo.h: No such file or directory
#include "xllfifo.h"
         ^~~~~~~~~~~
```

Or linker errors like:
```
undefined reference to 'XLlFifo_iRxOccupancy'
undefined reference to 'XLlFifo_RxGetWord'
```

### Root Cause
The application code uses the `llfifo` driver (for AXI FIFO MM-S IP), but:
1. The driver is not included in the BSP (Board Support Package)
2. The driver source files are not in the application
3. The device is not properly configured in xparameters.h

---

## Understanding the Hardware

### Step 1: Check Your Block Design
First, understand what IP blocks are in your Vivado block design.

**Bash Command:**
```bash
# Navigate to your project
cd /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo

# Check the block design file (if exported as Verilog wrapper)
cat axi4stream_data_fifo.gen/sources_1/bd/system/hdl/system_wrapper.v
```

### Step 2: Identify the IP Blocks
In this project, there are TWO FIFO-related IPs:

1. **axi_fifo_mm_s_0** - AXI FIFO Memory-Mapped to Stream
   - This is what we need the `llfifo` driver for
   - Controlled by PS (ARM processor) via memory-mapped registers
   - Converts memory-mapped interface to AXI-Stream

2. **axis_data_fifo_0** - AXI-Stream Data FIFO
   - Pure streaming FIFO (passthrough)
   - No driver needed (hardware handles it automatically)

### Step 3: Understand Which Driver You Need
The application code uses:
```c
#include "xllfifo.h"
XLlFifo FifoInstance;
```

This indicates you need the **llfifo driver** for the **axi_fifo_mm_s_0** IP block.

---

## Finding the Driver

### Step 1: Check BSP Configuration
The BSP (Board Support Package) contains all drivers and libraries for your hardware.

**Bash Command:**
```bash
# Navigate to your BSP directory
cd /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp

# Check the BSP configuration file
cat bsp.yaml
```

**Look for the `drv_info` section:**
```bash
# Search for your FIFO IP in the driver info
grep -A 3 "axi_fifo" bsp.yaml
```

**Expected output (BEFORE fix):**
```yaml
drv_info:
  axi_fifo_mm_s_0:
    driver: null
    ip_name: axi_fifo_mm_s
```

The `driver: null` means no driver is assigned!

### Step 2: Find Available Xilinx Drivers
All Xilinx drivers are installed with Vitis.

**Bash Command:**
```bash
# List all available drivers
ls /c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/
```

**Search for FIFO-related drivers:**
```bash
ls /c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/ | grep -i fifo
```

**Output:**
```
llfifo_v5_6          # <-- This is what we need!
```

### Step 3: Verify the Driver Location
```bash
# Check the driver directory
ls -la /c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/llfifo_v5_6/

# Output shows:
# data/       - Driver configuration data
# doc/        - Documentation
# examples/   - Example code
# src/        - Source files (what we need!)
```

### Step 4: List Driver Source Files
```bash
# List all source files in the driver
ls -la /c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/llfifo_v5_6/src/

# Output:
# xllfifo.c          - Main driver implementation
# xllfifo.h          - Header file with API declarations
# xllfifo_hw.h       - Hardware register definitions
# xllfifo_sinit.c    - Static initialization
# xstreamer.c        - Stream operations
# xstreamer.h        - Stream header
# Makefile           - Build configuration
```

---

## Adding Driver to BSP

### Step 1: Update bsp.yaml
The BSP configuration file tells Vitis which drivers to use for which IP blocks.

**Bash Command:**
```bash
# Navigate to BSP directory
cd /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp

# Backup the original
cp bsp.yaml bsp.yaml.backup

# Edit using your preferred editor (or use sed/awk)
# Find the axi_fifo_mm_s_0 entry and change it from:
#   axi_fifo_mm_s_0: None
# To:
#   axi_fifo_mm_s_0:
#     driver: llfifo
#     ip_name: axi_fifo_mm_s
#     path: C:\Xilinx\Vitis\2023.2\data\embeddedsw\XilinxProcessorIPLib\drivers\llfifo_v5_6
```

**Using sed (if you want to script it):**
```bash
# Note: This is complex with sed due to YAML formatting
# Recommend manual editing in this case
```

**Verify the change:**
```bash
grep -A 3 "axi_fifo_mm_s_0" bsp.yaml

# Output should show:
# axi_fifo_mm_s_0:
#   driver: llfifo
#   ip_name: axi_fifo_mm_s
#   path: C:\Xilinx\Vitis\2023.2\data\embeddedsw\XilinxProcessorIPLib\drivers\llfifo_v5_6
```

### Step 2: Copy Driver to BSP libsrc
The BSP needs the driver source files in its `libsrc` directory.

**Bash Commands:**
```bash
# Create driver directory in BSP libsrc
mkdir -p libsrc/llfifo_v5_6/src

# Copy all source files
cp -r /c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/llfifo_v5_6/src/* \
      libsrc/llfifo_v5_6/src/

# Verify files were copied
ls -la libsrc/llfifo_v5_6/src/

# Output:
# xllfifo.c
# xllfifo.h
# xllfifo_hw.h
# xllfifo_sinit.c
# xstreamer.c
# xstreamer.h
# Makefile
```

### Step 3: Copy Headers to BSP Include Directory
```bash
# Copy header files to the BSP include directory
cp libsrc/llfifo_v5_6/src/xllfifo.h include/
cp libsrc/llfifo_v5_6/src/xllfifo_hw.h include/
cp libsrc/llfifo_v5_6/src/xstreamer.h include/

# Verify headers are in place
ls -la include/xllfifo*.h include/xstreamer.h

# Output:
# include/xllfifo.h
# include/xllfifo_hw.h
# include/xstreamer.h
```

### Step 4: Update xparameters.h
The driver needs device IDs and base addresses defined in `xparameters.h`.

**Bash Commands:**
```bash
# Check current xparameters.h
grep -i "fifo" include/xparameters.h

# If nothing is found, you need to add definitions
# First, find the base address from Vivado Address Editor or system.hwh
```

**To find the base address:**
```bash
# If you have the XSA file or hwh file, you can extract it
# Or check Vivado Address Editor

# For this project, the address is 0x43C00000 (from Vivado)
```

**Add to xparameters.h (at the end before #endif):**
```c
/* Definitions for peripheral AXI_FIFO_MM_S_0 */
#define XPAR_AXI_FIFO_MM_S_0_DEVICE_ID 0
#define XPAR_AXI_FIFO_MM_S_0_BASEADDR 0x43C00000
#define XPAR_AXI_FIFO_MM_S_0_HIGHADDR 0x43C0FFFF

/* Canonical definitions for llfifo driver */
#define XPAR_LLFIFO_NUM_INSTANCES 1
#define XPAR_LLFIFO_0_DEVICE_ID XPAR_AXI_FIFO_MM_S_0_DEVICE_ID
#define XPAR_LLFIFO_0_BASEADDR XPAR_AXI_FIFO_MM_S_0_BASEADDR

/* Alias for application compatibility */
#define XPAR_AXI_FIFO_0_DEVICE_ID XPAR_AXI_FIFO_MM_S_0_DEVICE_ID
```

**Verify additions:**
```bash
grep -A 2 "AXI_FIFO_MM_S_0" include/xparameters.h
```

### Step 5: Update BSP CMakeLists.txt
The BSP build system needs to know about the new driver.

**Bash Commands:**
```bash
# Check current CMakeLists.txt
cat CMakeLists.txt

# Look for the line with BSP_LIBSRC_SUBDIRS
grep "BSP_LIBSRC_SUBDIRS" CMakeLists.txt

# Original:
# set (BSP_LIBSRC_SUBDIRS libsrc standalone xiltimer)

# Change to include llfifo_v5_6:
# set (BSP_LIBSRC_SUBDIRS libsrc standalone xiltimer llfifo_v5_6)
```

**Verify the change:**
```bash
grep "BSP_LIBSRC_SUBDIRS" CMakeLists.txt

# Output should show:
# set (BSP_LIBSRC_SUBDIRS libsrc standalone xiltimer llfifo_v5_6)
```

---

## Adding Driver to Application

The application also needs the driver source files directly (for Vitis 2023.2 build system).

### Step 1: Create Configuration File (xllfifo_g.c)
This file provides the device configuration table.

**Bash Commands:**
```bash
# Navigate to BSP libsrc driver directory
cd /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/llfifo_v5_6/src

# Create xllfifo_g.c
cat > xllfifo_g.c << 'EOF'
#include "xparameters.h"
#include "xllfifo.h"

XLlFifo_Config XLlFifo_ConfigTable[] = {
    {
        XPAR_AXI_FIFO_MM_S_0_DEVICE_ID,  /* DeviceId */
        XPAR_AXI_FIFO_MM_S_0_BASEADDR,   /* BaseAddress */
        0x1000,                           /* Tx FIFO Depth (4096 bytes) */
        0x1000,                           /* Rx FIFO Depth (4096 bytes) */
        0,                                /* Has Axis TDest */
        0                                 /* Has Axis TId */
    }
};
EOF

# Verify the file was created
cat xllfifo_g.c
```

### Step 2: Copy Driver Files to Application Source
```bash
# Navigate to application source directory
cd /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src

# Copy driver source files
cp /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/llfifo_v5_6/src/xllfifo.c .

cp /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/llfifo_v5_6/src/xllfifo_sinit.c .

cp /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/llfifo_v5_6/src/xstreamer.c .

cp /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/llfifo_v5_6/src/xllfifo_g.c .

# Verify all files are copied
ls -la *.c
```

**Expected output:**
```
helloworld.c
platform.c
xllfifo.c
xllfifo_sinit.c
xllfifo_g.c
xstreamer.c
```

### Step 3: Update Application CMakeLists.txt
```bash
# The CMakeLists.txt is in the src directory
cat CMakeLists.txt

# Find the section with "collect (PROJECT_LIB_SOURCES"
grep "PROJECT_LIB_SOURCES" CMakeLists.txt

# You need to add all the driver source files
```

**Original CMakeLists.txt:**
```cmake
collect (PROJECT_LIB_SOURCES helloworld.c)
collect (PROJECT_LIB_SOURCES platform.c)
```

**Updated CMakeLists.txt:**
```cmake
collect (PROJECT_LIB_SOURCES helloworld.c)
collect (PROJECT_LIB_SOURCES platform.c)
collect (PROJECT_LIB_SOURCES xllfifo.c)
collect (PROJECT_LIB_SOURCES xllfifo_sinit.c)
collect (PROJECT_LIB_SOURCES xllfifo_g.c)
collect (PROJECT_LIB_SOURCES xstreamer.c)
```

**Verify the change:**
```bash
grep "PROJECT_LIB_SOURCES" CMakeLists.txt

# Should show all 6 source files
```

### Step 4: Verify File Structure
```bash
# List all C source files
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/*.c

# Expected output (6 files):
# helloworld.c
# platform.c
# xllfifo.c
# xllfifo_sinit.c
# xllfifo_g.c
# xstreamer.c
```

---

## Fixing Application Code

### Step 1: Check for Xilinx Driver Typo
The Xilinx llfifo driver has a known typo in the function name.

**Bash Command:**
```bash
# Search for the LookupConfig function declaration
grep "LookupConfig" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xllfifo.h

# Output shows:
# XLlFifo_Config *XLlFfio_LookupConfig(UINTPTR BaseAddress);
#                 ^^^^^^^ Note the double 'f' - this is a Xilinx typo!
```

**In your application code, use:**
```c
Config = XLlFfio_LookupConfig(DeviceId);  // Note: XLlFfio NOT XLlFifo
```

### Step 2: Fix Logic Error in FifoRecv Function
There's a common logic error in FIFO receive code.

**Check the current code:**
```bash
# View the FifoRecv function
sed -n '133,162p' /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/helloworld.c
```

**WRONG CODE (reads twice per iteration):**
```c
for (int i = 0; i < RecvLen; i++)
{
    RecvWord = XLlFifo_RxGetWord(InstancePtr);      // Read #1
    if (XLlFifo_iRxOccupancy(InstancePtr))
        RecvWord = XLlFifo_RxGetWord(InstancePtr);  // Read #2 - overwrites #1!
    DstAddr[i] = RecvWord;
}
```

**Problem:** This reads TWO words per loop but only stores the second one, skipping half the data.

**CORRECT CODE:**
```c
for (int i = 0; i < RecvLen; i++)
{
    if (XLlFifo_iRxOccupancy(InstancePtr))  // Check if data available
    {
        RecvWord = XLlFifo_RxGetWord(InstancePtr);  // Read once
        DstAddr[i] = RecvWord;                      // Store it
    }
}
```

### Step 3: Verify Application Code
```bash
# Check that helloworld.c has correct includes
head -20 /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/helloworld.c

# Should include:
# #include <stdio.h>
# #include "xparameters.h"
# #include "xllfifo.h"
# #include "xstatus.h"
```

---

## Building the Application

### Step 1: Clean Build Directory (If Needed)
```bash
# Navigate to application directory
cd /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app

# Remove build directory if corrupted
rm -rf build

# Verify it's gone
ls -la build
# Should show: cannot access 'build': No such file or directory
```

### Step 2: Build in Vitis IDE
Open Vitis 2023.2 GUI and:
1. Right-click on `axi4stream_data_fifo_app` project
2. Select **Build Project**

Vitis will automatically:
- Run CMake to generate build files
- Compile all source files
- Link with BSP libraries
- Create the .elf executable

### Step 3: Monitor Build Progress
Check for successful compilation of driver files:
```bash
# After build starts, check if object files are being created
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/build/CMakeFiles/axi4stream_data_fifo_app.elf.dir/*.obj

# Should show:
# helloworld.c.obj
# platform.c.obj
# xllfifo.c.obj
# xllfifo_sinit.c.obj
# xllfifo_g.c.obj
# xstreamer.c.obj
```

### Step 4: Verify Build Success
```bash
# Check if ELF file was created
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/build/*.elf

# Should show:
# axi4stream_data_fifo_app.elf
```

---

## Troubleshooting

### Issue 1: "xllfifo.h: No such file or directory"

**Check:**
```bash
# Verify header is in BSP include
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xllfifo.h
```

**Solution:** Copy header from driver to BSP include directory (see Step 3 of "Adding Driver to BSP")

---

### Issue 2: "undefined reference to XLlFifo_iRxOccupancy"

**Check:**
```bash
# Verify driver source files are in application src
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/xllfifo*.c

# Verify CMakeLists.txt includes them
grep "xllfifo" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/CMakeLists.txt
```

**Solution:** Ensure all driver .c files are copied to src AND listed in CMakeLists.txt

---

### Issue 3: "implicit declaration of function 'XLlFifo_LookupConfig'"

**This is the Xilinx typo issue!**

**Check:**
```bash
# Search for the correct function name
grep "LookupConfig" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xllfifo.h
```

**Solution:** Use `XLlFfio_LookupConfig` (with double 'f') instead of `XLlFifo_LookupConfig`

---

### Issue 4: "XPAR_AXI_FIFO_0_DEVICE_ID undeclared"

**Check:**
```bash
# Search for device ID in xparameters.h
grep "FIFO.*DEVICE_ID" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xparameters.h
```

**Solution:** Add device ID definitions to xparameters.h (see Step 4 of "Adding Driver to BSP")

---

### Issue 5: CMake Configuration Fails

**Check CMake error log:**
```bash
cat /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/build/CMakeFiles/CMakeError.log | tail -50
```

**Common issue:** "undefined reference to '_exit'"
- This happens when CMake is misconfigured
- Solution: Remove build directory and let Vitis recreate it

```bash
rm -rf /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/build
# Then rebuild in Vitis
```

---

### Issue 6: Wrong Data Received

**Check:**
```bash
# Review the FifoRecv function
sed -n '144,151p' /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/helloworld.c
```

**Solution:** Fix the double-read logic error (see "Fixing Application Code" section)

---

## Complete File Checklist

Use these commands to verify all files are in place:

```bash
# BSP Configuration
echo "=== BSP Configuration ==="
grep -A 3 "axi_fifo_mm_s_0" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/bsp.yaml

# BSP Headers
echo "=== BSP Headers ==="
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xllfifo*.h

# BSP Driver Source
echo "=== BSP Driver Source ==="
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/llfifo_v5_6/src/

# BSP CMakeLists.txt
echo "=== BSP CMakeLists.txt ==="
grep "BSP_LIBSRC_SUBDIRS" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/CMakeLists.txt

# BSP xparameters.h
echo "=== BSP xparameters.h ==="
grep "FIFO.*DEVICE_ID\|FIFO.*BASEADDR" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xparameters.h

# Application Source Files
echo "=== Application Source Files ==="
ls -la /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/*.c

# Application CMakeLists.txt
echo "=== Application CMakeLists.txt ==="
grep "PROJECT_LIB_SOURCES" /c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo/axi4stream_data_fifo_app/src/CMakeLists.txt
```

---

## Quick Reference: All Commands in Sequence

Here's a complete script of all commands in order:

```bash
#!/bin/bash
# Complete llfifo driver setup script

PROJECT=/c/Users/JGEALON/Documents/ZedProjects/axi4stream_data_fifo
BSP=$PROJECT/axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp
APP=$PROJECT/axi4stream_data_fifo_app
XILINX_DRIVER=/c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/llfifo_v5_6

# 1. Check current BSP configuration
echo "Step 1: Checking BSP configuration..."
grep -A 3 "axi_fifo_mm_s_0" $BSP/bsp.yaml

# 2. Create driver directory in BSP
echo "Step 2: Creating driver directory in BSP..."
mkdir -p $BSP/libsrc/llfifo_v5_6/src

# 3. Copy driver source to BSP
echo "Step 3: Copying driver source to BSP..."
cp -r $XILINX_DRIVER/src/* $BSP/libsrc/llfifo_v5_6/src/

# 4. Copy headers to BSP include
echo "Step 4: Copying headers to BSP include..."
cp $BSP/libsrc/llfifo_v5_6/src/xllfifo.h $BSP/include/
cp $BSP/libsrc/llfifo_v5_6/src/xllfifo_hw.h $BSP/include/
cp $BSP/libsrc/llfifo_v5_6/src/xstreamer.h $BSP/include/

# 5. Create configuration file
echo "Step 5: Creating xllfifo_g.c..."
cat > $BSP/libsrc/llfifo_v5_6/src/xllfifo_g.c << 'EOF'
#include "xparameters.h"
#include "xllfifo.h"

XLlFifo_Config XLlFifo_ConfigTable[] = {
    {
        XPAR_AXI_FIFO_MM_S_0_DEVICE_ID,
        XPAR_AXI_FIFO_MM_S_0_BASEADDR,
        0x1000,
        0x1000,
        0,
        0
    }
};
EOF

# 6. Copy driver files to application
echo "Step 6: Copying driver files to application..."
cp $BSP/libsrc/llfifo_v5_6/src/xllfifo.c $APP/src/
cp $BSP/libsrc/llfifo_v5_6/src/xllfifo_sinit.c $APP/src/
cp $BSP/libsrc/llfifo_v5_6/src/xstreamer.c $APP/src/
cp $BSP/libsrc/llfifo_v5_6/src/xllfifo_g.c $APP/src/

# 7. Verify all files
echo "Step 7: Verifying all files..."
echo "Application source files:"
ls -la $APP/src/*.c

echo "BSP headers:"
ls -la $BSP/include/xllfifo*.h

echo "Done! Now:"
echo "1. Manually update $BSP/bsp.yaml to assign llfifo driver"
echo "2. Manually update $BSP/CMakeLists.txt to include llfifo_v5_6"
echo "3. Manually add device IDs to $BSP/include/xparameters.h"
echo "4. Manually update $APP/src/CMakeLists.txt to include driver sources"
echo "5. Fix application code (Xilinx typo and logic error)"
echo "6. Build in Vitis IDE"
```

---

## Summary

To add the llfifo driver to a Vitis application:

1. ✅ **Identify** - Check block design and BSP configuration to find missing driver
2. ✅ **Locate** - Find driver in Xilinx installation (llfifo_v5_6)
3. ✅ **BSP Setup** - Copy driver to BSP, update configuration files
4. ✅ **App Setup** - Copy driver source to application, update CMakeLists.txt
5. ✅ **Fix Code** - Handle Xilinx typo (XLlFfio) and logic errors
6. ✅ **Build** - Clean build directory and rebuild in Vitis

All bash commands and file locations are documented above for reference.
