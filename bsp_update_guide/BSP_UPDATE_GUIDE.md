# BSP Update Guide for Vitis 2023.2

This guide provides step-by-step instructions to update `xparameters.h` using XSCT (Xilinx Software Command-Line Tool) and the provided TCL script.

## 📋 Table of Contents
- [Prerequisites](#prerequisites)
- [When to Update BSP](#when-to-update-bsp)
- [Method 1: Using XSCT Command Line](#method-1-using-xsct-command-line)
- [Method 2: Using Vitis GUI](#method-2-using-vitis-gui)
- [Troubleshooting](#troubleshooting)
- [What Gets Updated](#what-gets-updated)

---

## Prerequisites

### Required Software
- **Xilinx Vitis 2023.2** installed
- **Platform project** already created in your workspace
- **Hardware design (.xsa file)** exported from Vivado

### Required Files
- `update_xparam.tcl` - TCL script provided in this directory
- Updated `.xsa` file from Vivado (if hardware changed)

---

## When to Update BSP

You need to update the BSP (Board Support Package) and regenerate `xparameters.h` when:

✅ Hardware design changes (new IP cores added/removed)
✅ IP configuration changes (address changes, parameter updates)
✅ New peripherals added to the design
✅ GPIO configuration modified
✅ Memory map changes
✅ After importing a new `.xsa` file

---

## Method 1: Using XSCT Command Line

### Step 1: Prepare the TCL Script

1. Open `update_xparam.tcl` in a text editor
2. Locate line 2:
   ```tcl
   set platform_project_name    VITIS_PROJECT_NAME(CASE SENSITIVE)
   ```

3. Replace `VITIS_PROJECT_NAME(CASE SENSITIVE)` with your actual platform project name

   **Example:**
   ```tcl
   set platform_project_name    platform
   ```
   or
   ```tcl
   set platform_project_name    dual_channel_axi_gpio_platform
   ```

4. Save the file

### Step 2: Launch XSCT

**Windows:**
1. Open **Command Prompt** or **PowerShell**
2. Navigate to your Vitis installation:
   ```cmd
   cd C:\Xilinx\Vitis\2023.2\bin
   ```
3. Launch XSCT:
   ```cmd
   xsct.bat
   ```

**Linux:**
1. Open terminal
2. Source Vitis settings:
   ```bash
   source /tools/Xilinx/Vitis/2023.2/settings64.sh
   ```
3. Launch XSCT:
   ```bash
   xsct
   ```

You should see the XSCT prompt:
```
****** Xilinx Software Commandline Tool (XSCT) v2023.2
  **** Build date : Oct 11 2023-10:49:12
    ** Copyright 1986-2023 Xilinx, Inc. All Rights Reserved.

xsct%
```

### Step 3: Set Workspace

Navigate to your Vitis workspace:

```tcl
xsct% setws /path/to/your/vitis/workspace
```

**Example (Windows):**
```tcl
xsct% setws C:/Users/JGEALON/Documents/ZedProjects/dual_channel_axi_gpio
```

**Example (Linux):**
```tcl
xsct% setws /home/user/projects/zedboard_workspace
```

### Step 4: Run the TCL Script

Execute the update script:

```tcl
xsct% source /path/to/update_xparam.tcl
```

**Example (Windows):**
```tcl
xsct% source C:/Users/JGEALON/Documents/ZedProjects/bsp_update_guide/update_xparam.tcl
```

**Example (Linux):**
```tcl
xsct% source /home/user/bsp_update_guide/update_xparam.tcl
```

### Step 5: Verify Success

If successful, you should see:
```
--------------------------------------------
Successfully Updated xparameters.h file
--------------------------------------------
```

### Step 6: Exit XSCT

```tcl
xsct% exit
```

---

## Method 2: Using Vitis GUI

### Step 1: Update Hardware Specification

1. Open **Vitis IDE 2023.2**
2. Select your workspace
3. In **Explorer** view, right-click on your **platform project**
4. Select **Update Hardware Specification**
5. Browse to your updated `.xsa` file
6. Click **OK**

### Step 2: Regenerate Platform

1. Right-click on the **platform project**
2. Select **Build Project** or press **Ctrl+B**
3. Wait for the build to complete

### Step 3: Regenerate BSP (Alternative Method)

1. Expand your platform project
2. Navigate to:
   ```
   platform → [domain_name] → bsp
   ```
3. Right-click on **bsp** folder
4. Select **Re-generate BSP Sources**

### Step 4: Clean and Rebuild Applications

1. Right-click on each application project
2. Select **Clean Project**
3. Then select **Build Project**

---

## Troubleshooting

### Issue: "Platform not found"

**Solution:**
- Verify the platform name is spelled correctly (case-sensitive)
- Use `platform list` in XSCT to see available platforms:
  ```tcl
  xsct% platform list
  ```

### Issue: "Workspace not set"

**Solution:**
- Ensure you ran `setws` command with correct path
- Check the path exists and contains `.metadata` folder

### Issue: "Permission denied"

**Solution:**
- Close Vitis IDE before running XSCT script
- Ensure you have write permissions to workspace
- On Linux, check file ownership: `ls -la`

### Issue: "TCL script errors"

**Solution:**
- Check line 2 has correct platform name (no extra spaces)
- Ensure platform name doesn't contain special characters
- Verify TCL syntax (no typos)

### Issue: "xparameters.h not updated"

**Solution:**
1. Check if `.xsa` file was actually updated
2. Manually regenerate:
   ```tcl
   xsct% platform active platform_name
   xsct% platform generate
   ```
3. Check build console for errors

### Issue: XSCT command not found

**Solution:**
- Verify Vitis 2023.2 is installed
- Add Vitis bin directory to PATH:

  **Windows:**
  ```cmd
  set PATH=%PATH%;C:\Xilinx\Vitis\2023.2\bin
  ```

  **Linux:**
  ```bash
  export PATH=$PATH:/tools/Xilinx/Vitis/2023.2/bin
  ```

---

## What Gets Updated

When you run this script, the following files are regenerated:

### xparameters.h
Located in: `platform/[domain]/bsp/include/xparameters.h`

Contains:
- Device IDs (e.g., `XPAR_XGPIO_0_DEVICE_ID`)
- Base addresses (e.g., `XPAR_XGPIO_0_BASEADDR`)
- Peripheral configurations
- Interrupt IDs
- Memory map definitions

### Example Changes:
```c
// Before (if GPIO was added)
#define XPAR_XGPIO_NUM_INSTANCES 0

// After
#define XPAR_XGPIO_NUM_INSTANCES 1
#define XPAR_AXI_GPIO_0_BASEADDR 0x41200000
#define XPAR_AXI_GPIO_0_HIGHADDR 0x4120ffff
#define XPAR_XGPIO_0_DEVICE_ID 0
```

### Other Updated Files:
- `platform/[domain]/bsp/libsrc/*/src/*.h` - Driver headers
- `platform/hw/*.tcl` - Hardware description files
- Platform metadata files

---

## Manual XSCT Commands (Alternative)

If you prefer manual control, use these commands:

```tcl
# Set workspace
xsct% setws /path/to/workspace

# List platforms
xsct% platform list

# Activate platform
xsct% platform active platform_name

# List domains
xsct% domain list

# Activate domain
xsct% domain active standalone_ps7_cortexa9_0

# Regenerate BSP
xsct% bsp regenerate

# Generate platform
xsct% platform generate

# Build application (optional)
xsct% app build -name app_name
```

---

## Verification Steps

After updating, verify the changes:

### 1. Check xparameters.h

Open:
```
platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xparameters.h
```

or

```
platform/export/platform/sw/[domain]/include/xparameters.h
```

Look for your peripheral definitions.

### 2. Check Build Output

In Vitis console, look for:
```
Building file: ../src/platform.c
Invoking: ARM v7 gcc compiler
...
Finished building: ../src/platform.c
```

### 3. Test Application

Run your application to ensure:
- No compilation errors
- Correct device initialization
- Peripherals work as expected

---

## Quick Reference

### XSCT Launch Commands

| Platform | Command |
|----------|---------|
| Windows (CMD) | `C:\Xilinx\Vitis\2023.2\bin\xsct.bat` |
| Windows (PowerShell) | `& "C:\Xilinx\Vitis\2023.2\bin\xsct.bat"` |
| Linux | `xsct` (after sourcing settings64.sh) |

### Common XSCT Commands

| Command | Description |
|---------|-------------|
| `setws <path>` | Set workspace |
| `platform list` | List all platforms |
| `platform active <name>` | Activate platform |
| `domain list` | List domains |
| `bsp regenerate` | Regenerate BSP |
| `platform generate` | Generate platform |
| `app list` | List applications |
| `app build -name <app>` | Build application |
| `help` | Show help |
| `exit` | Exit XSCT |

---

## Best Practices

1. ✅ **Always backup** your workspace before running update scripts
2. ✅ **Close Vitis IDE** before running XSCT commands
3. ✅ **Version control** your platform project
4. ✅ **Document changes** in hardware design
5. ✅ **Test after update** to ensure applications still work
6. ✅ **Keep .xsa files** organized with version numbers
7. ✅ **Clean build** applications after BSP update

---

## Additional Resources

- [Vitis Unified Software Platform Documentation (UG1416)](https://docs.xilinx.com/r/en-US/ug1416-vitis-documentation)
- [XSCT Command Reference](https://docs.xilinx.com/r/en-US/ug1400-vitis-embedded/XSCT-Commands)
- [Vitis Embedded Software Development Flow](https://docs.xilinx.com/r/en-US/ug1400-vitis-embedded)

---

## Summary

**Quick Steps:**
1. Edit `update_xparam.tcl` → Set platform name
2. Launch XSCT → `xsct.bat` or `xsct`
3. Set workspace → `setws /path/to/workspace`
4. Run script → `source /path/to/update_xparam.tcl`
5. Verify → Check console output and xparameters.h
6. Exit → `exit`

**That's it!** Your `xparameters.h` and BSP are now updated. 🎉

---

*Last Updated: April 2026*
*Compatible with: Xilinx Vitis 2023.2*
