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
C:\Users\JGEALON\Documents\ZedProjects\axi_shift_register\xsdb /*run xsdb*/
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
xsdb% fpga fpga C:/Users/JGEALON/Documents/ZedProjects/axi_shift_register/axi_shift_register.runs/impl_1/system_wrapper.bit
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
xsdb% source C:/Users/JGEALON/Documents/ZedProjects/axi_shift_register/axi_shift_register.gen/sources_1/bd/system/ip/system_processing_system7_0_0/ps7_init.tcl
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
Memory write error at 0x41200000. Blocked address 0x40000000.
Cannot access FPGA: Level shifters are not enabled
```

### 6. Access GPIO Registers

Use the `-force` flag to bypass memory map checks:

```tcl
# Write to AXI MM2C (assuming base address 0x40000000)
xsdb% mwr -force 0x40000000 0x3 //0x3=0b011->DIR=0;DIN=1;EN=1 (shift left)
xsdb% mwr -force 0x40000000 0x1
xsdb% mwr -force 0x40000000 0x1
xsdb% mwr -force 0x40000000 0x3

# Read back shift register value
xsdb% mrd -force 0x40000004
40000004:   00000009 //terminal return 

# Write data again to AXI MM2C (assuming base address 0x40000000)
xsdb% mwr -force 0x40000000 0x5 //0x5=0b101->DIR=1;DIN=0;EN=1
xsdb% mwr -force 0x40000000 0x5
xsdb% mwr -force 0x40000000 0x7 //0x7=0b111->DIR=1;DIN=1;EN=1 (shift right)
xsdb% mwr -force 0x40000000 0x7

# Read back shift register value
xsdb% mrd -force 0x40000004
40000004:   0000000C

```

**Why `-force` is needed:**

Without `-force`, you may get:
```
Memory write error at 0x40000000. PL AXI slave ports access is not allowed.
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
xsdb% fpga system_wrapper.bit
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
xsdb% fpga system_wrapper.bit
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
xsdb% fpga system_wrapper.bit
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
xsdb% mwr 0x40000000 0x3    # ❌ Memory map error
```

### ✅ CORRECT: Always Use -force for GPIO

```tcl
xsdb% mwr -force 0x40000000 0x3    # ✅ Works!
```

### ❌ WRONG: Relative Paths Without cd

```tcl
xsdb% fpga system_wrapper.bit    # ❌ File not found
```

### ✅ CORRECT: Use Full Absolute Paths

```tcl
xsdb% fpga C:/Users/JGEALON/Documents/ZedProjects/axi_shift_register/axi_shift_register.runs/impl_1/system_wrapper.bit    # ✅ Works!
```


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
