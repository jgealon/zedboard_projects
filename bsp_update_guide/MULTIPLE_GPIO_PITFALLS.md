# Multiple Instance AXI GPIO Pitfalls Guide

## Critical Issue: SDT vs Traditional Mode

### The Problem

In Vitis 2023.2 and newer, BSPs can be compiled in two different modes:
1. **Traditional Mode** - Uses Device IDs (0, 1, 2...)
2. **SDT Mode (System Device Tree)** - Uses Base Addresses (0x41200000, 0x41210000...)

**The driver functions change behavior** based on compilation mode, but **error messages don't tell you** which mode is active!

---

## How to Detect SDT Mode

### Method 1: Check Driver Source Code

Look at `platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/gpio/src/xgpio_sinit.c`:

```c
#ifndef SDT
XGpio_Config *XGpio_LookupConfig(u16 DeviceId)  // Traditional mode
#else
XGpio_Config *XGpio_LookupConfig(UINTPTR BaseAddress)  // SDT mode
#endif
```

### Method 2: Check xgpio.h Header

Look at `platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/gpio/src/xgpio.h`:

```c
typedef struct {
#ifndef SDT
    u16 DeviceId;      // Traditional mode - has DeviceId field
#else
    char *Name;        // SDT mode - uses Name instead
#endif
    UINTPTR BaseAddress;
    // ...
} XGpio_Config;
```

### Method 3: Check Config Table

Look at `platform/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/gpio/src/xgpio_g.c`:

**Traditional Mode:**
```c
XGpio_Config XGpio_ConfigTable[] = {
    {
        0,              /* DeviceId - PRESENT */
        0x41200000,     /* BaseAddress */
        // ...
    }
};
```

**SDT Mode:**
```c
XGpio_Config XGpio_ConfigTable[] = {
    {
        "xlnx,axi-gpio-2.0",  /* Name - NO DeviceId */
        0x41200000,           /* BaseAddress */
        // ...
    }
};
```

---

## Common Pitfalls

### Pitfall #1: Using Device IDs in SDT Mode

❌ **WRONG (SDT Mode):**
```c
XGpio led, sw;

// Initialize with device IDs
XGpio_Initialize(&led, 0);   // Looks for base address 0x0 - FAILS!
XGpio_Initialize(&sw, 1);    // Looks for base address 0x1 - FAILS!
```

**Error:** `XGpio_LookupConfig(0)` returns NULL because it's looking for a GPIO at base address `0x00000000`, which doesn't exist!

✅ **CORRECT (SDT Mode):**
```c
XGpio led, sw;

// Initialize with base addresses from xparameters.h
XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);  // 0x41200000
XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);   // 0x41210000
```

### Pitfall #2: Using Type Declaration Instead of Value

❌ **WRONG:**
```c
uart_config = XUartPs_LookupConfig(u16 DeviceId);
```

This is using the **type declaration** `u16` instead of an actual value!

✅ **CORRECT (Traditional Mode):**
```c
uart_config = XUartPs_LookupConfig(0);  // Device ID 0
```

✅ **CORRECT (SDT Mode):**
```c
uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);  // 0xe0001000
```

### Pitfall #3: Assuming Device IDs Match Vivado Names

In Vivado, you might see:
- `axi_gpio_0` (your first GPIO IP block)
- `axi_gpio_1` (your second GPIO IP block)

**This does NOT mean:**
- Device ID 0 = `axi_gpio_0`
- Device ID 1 = `axi_gpio_1`

**The truth:**
- Device ID is determined by the **order in xparameters.h**
- Vivado instance names are irrelevant to driver code
- In SDT mode, device IDs don't exist at all!

### Pitfall #4: Confusing Dual-Channel vs Multiple Instances

**Dual-Channel GPIO (1 IP, 2 channels):**
```c
XGpio gpio;  // ONE instance

XGpio_Initialize(&gpio, 0);  // Traditional mode
// OR
XGpio_Initialize(&gpio, XPAR_XGPIO_0_BASEADDR);  // SDT mode

// Channel 1 for switches, Channel 2 for LEDs
XGpio_SetDataDirection(&gpio, 1, 0xFF);  // Channel 1 = input
XGpio_SetDataDirection(&gpio, 2, 0x00);  // Channel 2 = output

u32 sw = XGpio_DiscreteRead(&gpio, 1);   // Read from channel 1
XGpio_DiscreteWrite(&gpio, 2, sw);       // Write to channel 2
```

**Multiple Instances (2 IPs, 1 channel each):**
```c
XGpio led, sw;  // TWO instances

// SDT Mode:
XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);  // First GPIO IP
XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);   // Second GPIO IP

// Both use Channel 1 (only channel available)
XGpio_SetDataDirection(&led, 1, 0x00);  // LED = output
XGpio_SetDataDirection(&sw, 1, 0xFF);   // Switch = input

u32 swRead = XGpio_DiscreteRead(&sw, 1);    // Read from sw instance
XGpio_DiscreteWrite(&led, 1, swRead);       // Write to led instance
```

---

## Diagnostic Procedure

### Step 1: Determine Your Mode

1. Check `xgpio_g.c` - does the config table have DeviceId or Name?
2. Check function signature in `xgpio_sinit.c`

### Step 2: Check xparameters.h

```bash
# Look for GPIO definitions
grep XGPIO xparameters.h
```

Expected output:
```c
#define XPAR_XGPIO_NUM_INSTANCES 2
#define XPAR_XGPIO_0_BASEADDR 0x41200000
#define XPAR_XGPIO_0_HIGHADDR 0x4120ffff
#define XPAR_XGPIO_1_BASEADDR 0x41210000
#define XPAR_XGPIO_1_HIGHADDR 0x4121ffff
```

**Note:** No `XPAR_AXI_GPIO_0_DEVICE_ID` macros in Vitis 2023.2+

### Step 3: Diagnostic Test Code

```c
#include "xgpio.h"
#include "xparameters.h"
#include "xil_printf.h"

XGpio gpio1, gpio2;

void test_gpio_init() {
    int status1, status2;

    xil_printf("\n=== GPIO Diagnostic ===\n");
    xil_printf("Number of GPIO instances: %d\n", XPAR_XGPIO_NUM_INSTANCES);
    xil_printf("GPIO 0 Base: 0x%08X\n", XPAR_XGPIO_0_BASEADDR);
    xil_printf("GPIO 1 Base: 0x%08X\n", XPAR_XGPIO_1_BASEADDR);

    // Try SDT mode (base addresses)
    xil_printf("\n--- Testing SDT Mode ---\n");
    status1 = XGpio_Initialize(&gpio1, XPAR_XGPIO_0_BASEADDR);
    xil_printf("GPIO 1 init (SDT): %d\n", status1);

    status2 = XGpio_Initialize(&gpio2, XPAR_XGPIO_1_BASEADDR);
    xil_printf("GPIO 2 init (SDT): %d\n", status2);

    if (status1 == XST_SUCCESS && status2 == XST_SUCCESS) {
        xil_printf("\n✓ SDT Mode confirmed - use base addresses!\n");
    } else {
        xil_printf("\n✗ SDT Mode failed - BSP might be Traditional mode\n");

        // Try Traditional mode (device IDs)
        xil_printf("\n--- Testing Traditional Mode ---\n");
        status1 = XGpio_Initialize(&gpio1, 0);
        xil_printf("GPIO 1 init (ID=0): %d\n", status1);

        status2 = XGpio_Initialize(&gpio2, 1);
        xil_printf("GPIO 2 init (ID=1): %d\n", status2);

        if (status1 == XST_SUCCESS && status2 == XST_SUCCESS) {
            xil_printf("\n✓ Traditional Mode confirmed - use device IDs!\n");
        }
    }
}
```

---

## Complete Working Examples

### Traditional Mode (Device IDs)

```c
#include "xgpio.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "sleep.h"

XGpio led, sw;

void gpio_init() {
    int statusl, statuss;

    // Traditional mode - use device IDs
    statusl = XGpio_Initialize(&led, 0);
    statuss = XGpio_Initialize(&sw, 1);

    if (statusl == XST_SUCCESS && statuss == XST_SUCCESS)
        xil_printf("GPIO init successful\n");
    else
        xil_printf("GPIO init failed\n");
}

int main() {
    u32 swRead;

    init_platform();
    gpio_init();

    XGpio_SetDataDirection(&led, 1, 0x00);  // Output
    XGpio_SetDataDirection(&sw, 1, 0xFF);   // Input

    while(1) {
        swRead = XGpio_DiscreteRead(&sw, 1);
        XGpio_DiscreteWrite(&led, 1, swRead);
        sleep(1);
    }

    cleanup_platform();
    return 0;
}
```

### SDT Mode (Base Addresses)

```c
#include "xgpio.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "sleep.h"

XGpio led, sw;

void gpio_init() {
    int statusl, statuss;

    // SDT mode - use base addresses
    statusl = XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);
    statuss = XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);

    if (statusl == XST_SUCCESS && statuss == XST_SUCCESS)
        xil_printf("GPIO init successful\n");
    else
        xil_printf("GPIO init failed\n");
}

int main() {
    u32 swRead;

    init_platform();
    gpio_init();

    XGpio_SetDataDirection(&led, 1, 0x00);  // Output
    XGpio_SetDataDirection(&sw, 1, 0xFF);   // Input

    while(1) {
        swRead = XGpio_DiscreteRead(&sw, 1);
        XGpio_DiscreteWrite(&led, 1, swRead);
        sleep(1);
    }

    cleanup_platform();
    return 0;
}
```

---

## Applies to Other Peripherals Too!

This SDT vs Traditional mode issue affects **ALL Xilinx drivers**:

### UART (PS)
```c
// Traditional:
XUartPs_LookupConfig(0);

// SDT:
XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);  // 0xe0001000
```

### SPI
```c
// Traditional:
XSpiPs_LookupConfig(0);

// SDT:
XSpiPs_LookupConfig(XPAR_XSPIPS_0_BASEADDR);
```

### I2C
```c
// Traditional:
XIicPs_LookupConfig(0);

// SDT:
XIicPs_LookupConfig(XPAR_XIICPS_0_BASEADDR);
```

---

## Quick Reference Table

| Mode | XGpio_Config Field | LookupConfig() Parameter | Initialize() Parameter |
|------|-------------------|-------------------------|----------------------|
| **Traditional** | `u16 DeviceId` | `u16 DeviceId` (0, 1, 2...) | `u16 DeviceId` (0, 1, 2...) |
| **SDT** | `char *Name` | `UINTPTR BaseAddress` | `UINTPTR BaseAddress` |

---

## Troubleshooting

### "Config Lookup Failed" Error

**Symptom:**
```
LED config found at 0x41200000
LED init status: 0
SW config lookup FAILED!
```

**Cause:** Using device ID in SDT mode

**Solution:** Check if you're using base addresses:
```c
// Wrong:
XGpio_Initialize(&sw, 1);  // Looking for address 0x1

// Correct:
XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);  // 0x41210000
```

### "Device Init Failed" Error

**Symptom:**
```
DEVICE INIT FAILED.
```

**Causes:**
1. Wrong parameter type (device ID vs base address)
2. BSP not regenerated after hardware changes
3. Wrong base address macro used

**Solution:**
1. Verify mode (SDT vs Traditional)
2. Regenerate platform in Vitis
3. Check xparameters.h for correct macros

### LEDs Don't Respond to Switches

**Possible Causes:**
1. Wrong GPIO instance mapping
2. Wrong channel numbers
3. Incorrect data direction

**Debug Steps:**
```c
// Test LED GPIO independently
XGpio_DiscreteWrite(&led, 1, 0xFF);  // All ON
sleep(2);
XGpio_DiscreteWrite(&led, 1, 0x00);  // All OFF
sleep(2);

// Read and print switch values
u32 sw_val = XGpio_DiscreteRead(&sw, 1);
xil_printf("Switch: 0x%02X (%d)\n", sw_val, sw_val);
```

---

## Best Practices

1. **Always check driver source** to determine SDT vs Traditional mode
2. **Use macros from xparameters.h** - never hardcode addresses
3. **Add diagnostic prints** during initialization
4. **Test each GPIO independently** before combining
5. **Regenerate platform** after any hardware changes
6. **Document your BSP mode** in project README

---

## Summary

The #1 cause of "device not found" or "init failed" errors in multiple GPIO projects:

**Using device IDs (0, 1, 2) when BSP is compiled in SDT mode (expects base addresses)**

**Solution:** Always verify your BSP mode and use the appropriate parameter type!

---

**Document Version:** 1.0
**Created:** 2026-04-18
**Author:** Claude AI Assistant
**Project:** Multiple Instance AXI GPIO Troubleshooting
