# XPAR Macro Naming Convention Guide

## Understanding xparameters.h Macro Names

### Standard Naming Pattern

```
XPAR_X<DRIVER>_<INSTANCE>_<PROPERTY>
│    │        │          │
│    │        │          └─ Property (BASEADDR, HIGHADDR, etc.)
│    │        └──────────── Instance number (0, 1, 2...)
│    └───────────────────── Driver name (matches driver struct)
└────────────────────────── Xilinx Parameter prefix (always XPAR)
```

---

## Common Driver Names

### Processing System (PS) Peripherals

These are **built into the ARM CPU** on Zynq devices (MIO pins):

| Peripheral | Driver Struct | Macro Prefix | Example |
|-----------|--------------|-------------|---------|
| PS UART | `XUartPs` | `XPAR_XUARTPS_` | `XPAR_XUARTPS_0_BASEADDR` |
| PS SPI | `XSpiPs` | `XPAR_XSPIPS_` | `XPAR_XSPIPS_0_BASEADDR` |
| PS I2C | `XIicPs` | `XPAR_XIICPS_` | `XPAR_XIICPS_0_BASEADDR` |
| PS GPIO | `XGpioPs` | `XPAR_XGPIOPS_` | `XPAR_XGPIOPS_0_BASEADDR` |
| PS CAN | `XCanPs` | `XPAR_XCANPS_` | `XPAR_XCANPS_0_BASEADDR` |
| PS Ethernet | `XEmacPs` | `XPAR_XEMACPS_` | `XPAR_XEMACPS_0_BASEADDR` |
| PS USB | `XUsbPs` | `XPAR_XUSBPS_` | `XPAR_XUSBPS_0_BASEADDR` |
| PS SD | `XSdPs` | `XPAR_XSDPS_` | `XPAR_XSDPS_0_BASEADDR` |
| PS QSPI | `XQspiPs` | `XPAR_XQSPIPS_` | `XPAR_XQSPIPS_0_BASEADDR` |

**Note:** `Ps` suffix = **Processing System**

### Programmable Logic (PL) Peripherals

These are **soft IP cores** in the FPGA fabric:

| Peripheral | Driver Struct | Macro Prefix | Example |
|-----------|--------------|-------------|---------|
| AXI GPIO | `XGpio` | `XPAR_XGPIO_` | `XPAR_XGPIO_0_BASEADDR` |
| AXI Timer | `XTmrCtr` | `XPAR_XTMRCTR_` | `XPAR_XTMRCTR_0_BASEADDR` |
| AXI UART Lite | `XUartLite` | `XPAR_XUARTLITE_` | `XPAR_XUARTLITE_0_BASEADDR` |
| AXI IIC | `XIic` | `XPAR_XIIC_` | `XPAR_XIIC_0_BASEADDR` |
| AXI SPI | `XSpi` | `XPAR_XSPI_` | `XPAR_XSPI_0_BASEADDR` |
| AXI Interrupt Ctrl | `XIntc` | `XPAR_XINTC_` | `XPAR_XINTC_0_BASEADDR` |
| AXI DMA | `XAxiDma` | `XPAR_XAXIDMA_` | `XPAR_XAXIDMA_0_BASEADDR` |

---

## Example from Real xparameters.h

### UART Example

```c
#define XPAR_XUARTPS_NUM_INSTANCES 1

/* Definitions for peripheral UART1 */
#define XPAR_UART1_COMPATIBLE xlnx,xuartps
#define XPAR_UART1_BASEADDR 0xe0001000          // ← Vivado instance name
#define XPAR_UART1_HIGHADDR 0xe0001fff
#define XPAR_UART1_CLOCK_FREQ 0x2faf080

/* Canonical definitions for peripheral UART1 */
#define XPAR_XUARTPS_0_BASEADDR 0xe0001000      // ← USE THIS! (canonical)
#define XPAR_XUARTPS_0_HIGHADDR 0xe0001fff
#define XPAR_XUARTPS_0_COMPATIBLE xlnx,xuartps
#define XPAR_XUARTPS_0_CLOCK_FREQ 0x2faf080
```

**Which one to use?**
- ✅ `XPAR_XUARTPS_0_BASEADDR` - **Canonical name (USE THIS)**
- ❌ `XPAR_UART1_BASEADDR` - Instance name from Vivado (can change)

### GPIO Example

```c
#define XPAR_XGPIO_NUM_INSTANCES 2

/* Definitions for peripheral AXI_GPIO_0 */
#define XPAR_AXI_GPIO_0_BASEADDR 0x41200000     // ← Vivado instance name
#define XPAR_AXI_GPIO_0_HIGHADDR 0x4120ffff

/* Canonical definitions for peripheral AXI_GPIO_0 */
#define XPAR_XGPIO_0_BASEADDR 0x41200000        // ← USE THIS! (canonical)
#define XPAR_XGPIO_0_HIGHADDR 0x4120ffff

/* Definitions for peripheral AXI_GPIO_1 */
#define XPAR_AXI_GPIO_1_BASEADDR 0x41210000     // ← Vivado instance name

/* Canonical definitions for peripheral AXI_GPIO_1 */
#define XPAR_XGPIO_1_BASEADDR 0x41210000        // ← USE THIS! (canonical)
#define XPAR_XGPIO_1_HIGHADDR 0x4121ffff
```

---

## Common Properties

### Base Address Properties

| Macro Suffix | Description | Example Value |
|-------------|-------------|---------------|
| `_BASEADDR` | Start address of peripheral registers | `0x41200000` |
| `_HIGHADDR` | End address of peripheral registers | `0x4120ffff` |

### Configuration Properties

| Macro Suffix | Description | Example Value |
|-------------|-------------|---------------|
| `_NUM_INSTANCES` | Total count of this peripheral type | `2` |
| `_COMPATIBLE` | Device tree compatible string | `"xlnx,axi-gpio-2.0"` |
| `_CLOCK_FREQ` | Clock frequency in Hz | `0x2faf080` (50 MHz) |
| `_INTERRUPTS` | Interrupt ID | `0x4032` |
| `_INTERRUPT_PARENT` | Interrupt controller address | `0xf8f01000` |

### GPIO-Specific Properties

| Macro Suffix | Description | Example Value |
|-------------|-------------|---------------|
| `_GPIO_WIDTH` | Width of GPIO bus (bits) | `0x8` (8 bits) |
| `_IS_DUAL` | Dual channel support | `0x1` (true) / `0x0` (false) |
| `_INTERRUPT_PRESENT` | Interrupt support | `0x1` (yes) / `0x0` (no) |

---

## Naming Rules

### Rule 1: Match Driver Struct Name

The macro prefix **must match** the driver struct:

```c
// Driver struct: XUartPs
XUartPs uart;
XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);  // ✓ Matches

// Driver struct: XGpio
XGpio gpio;
XGpio_LookupConfig(XPAR_XGPIO_0_BASEADDR);      // ✓ Matches

// Driver struct: XSpiPs
XSpiPs spi;
XSpiPs_LookupConfig(XPAR_XSPIPS_0_BASEADDR);    // ✓ Matches
```

### Rule 2: Use Canonical Names

Always use the **canonical** macro (starts with `XPAR_X<DRIVER>_`):

```c
// ✅ CORRECT - Canonical name
#define XPAR_XUARTPS_0_BASEADDR 0xe0001000
XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);

// ❌ AVOID - Instance name (may change if you rename in Vivado)
#define XPAR_UART1_BASEADDR 0xe0001000
XUartPs_LookupConfig(XPAR_UART1_BASEADDR);  // Works but not recommended
```

### Rule 3: Instance Numbers Start at 0

```c
XPAR_XGPIO_0_BASEADDR    // First GPIO instance
XPAR_XGPIO_1_BASEADDR    // Second GPIO instance
XPAR_XGPIO_2_BASEADDR    // Third GPIO instance
```

**Important:** Instance numbers are based on **order in xparameters.h**, NOT Vivado instance names!

---

## Quick Lookup Method

### Method 1: Search by Driver Name

```bash
# Find all UART macros
grep XUARTPS xparameters.h

# Find all GPIO macros
grep XGPIO xparameters.h

# Find all SPI macros
grep XSPIPS xparameters.h
```

### Method 2: Find Base Address

```bash
# Find base address for UART
grep "XUARTPS.*BASEADDR" xparameters.h

# Output:
# #define XPAR_XUARTPS_0_BASEADDR 0xe0001000
```

### Method 3: Count Instances

```bash
# How many GPIOs?
grep "XGPIO_NUM_INSTANCES" xparameters.h

# Output:
# #define XPAR_XGPIO_NUM_INSTANCES 2
```

---

## SDT Mode vs Traditional Mode

### Traditional Mode (OLD)

Uses **Device ID** field:

```c
// xparameters.h (Traditional):
#define XPAR_XUARTPS_0_DEVICE_ID 0

// Code:
XUartPs_Config *cfg = XUartPs_LookupConfig(0);  // Device ID
```

### SDT Mode (NEW - Vitis 2023.2+)

Uses **Base Address**:

```c
// xparameters.h (SDT):
#define XPAR_XUARTPS_0_BASEADDR 0xe0001000
// NO DEVICE_ID macro!

// Code:
XUartPs_Config *cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
```

**How to tell which mode you're in:**

```bash
# Look for DEVICE_ID macros
grep "DEVICE_ID" xparameters.h

# If empty output → SDT mode (use base addresses)
# If found → Traditional mode (use device IDs)
```

---

## Code Examples

### UART Initialization (SDT Mode)

```c
#include "xuartps.h"
#include "xparameters.h"
#include "xstatus.h"

XUartPs uart;
XUartPs_Config *uart_cfg;

void uart_init() {
    int status;

    // Use base address (SDT mode)
    uart_cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);

    if (uart_cfg == NULL) {
        xil_printf("UART config not found!\n");
        return;
    }

    status = XUartPs_CfgInitialize(&uart, uart_cfg, uart_cfg->BaseAddress);

    if (status == XST_SUCCESS) {
        xil_printf("UART init successful\n");
    }
}
```

### GPIO Initialization (SDT Mode)

```c
#include "xgpio.h"
#include "xparameters.h"

XGpio led, sw;

void gpio_init() {
    int status1, status2;

    // Use base addresses (SDT mode)
    status1 = XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);
    status2 = XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);

    if (status1 == XST_SUCCESS && status2 == XST_SUCCESS) {
        xil_printf("GPIO init successful\n");
    }
}
```

### Multiple UARTs

```c
// If you have 2 UART instances:
XUartPs uart0, uart1;

XUartPs_Initialize(&uart0, XPAR_XUARTPS_0_BASEADDR);  // First UART
XUartPs_Initialize(&uart1, XPAR_XUARTPS_1_BASEADDR);  // Second UART
```

---

## Common Mistakes

### Mistake 1: Using Type Instead of Value

❌ **WRONG:**
```c
uart_cfg = XUartPs_LookupConfig(u16 DeviceId);
```

This passes the **type** `u16` instead of an actual value!

✅ **CORRECT:**
```c
uart_cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
```

### Mistake 2: Using Instance Name Macros

❌ **AVOID:**
```c
XGpio_Initialize(&gpio, XPAR_AXI_GPIO_0_BASEADDR);  // Vivado instance name
```

✅ **PREFER:**
```c
XGpio_Initialize(&gpio, XPAR_XGPIO_0_BASEADDR);     // Canonical name
```

### Mistake 3: Hardcoding Addresses

❌ **WRONG:**
```c
XGpio_Initialize(&gpio, 0x41200000);  // Hardcoded!
```

✅ **CORRECT:**
```c
XGpio_Initialize(&gpio, XPAR_XGPIO_0_BASEADDR);  // Use macro
```

**Why?** If hardware changes, you only need to regenerate the platform, not modify code!

### Mistake 4: Assuming Vivado Names = Instance Numbers

**Vivado block design names DON'T determine instance numbers!**

```
Vivado names:        axi_gpio_led    axi_gpio_switch
Instance numbers:    Can be 0 or 1   Can be 1 or 0
Depends on:          Order in xparameters.h (NOT Vivado names!)
```

Always check xparameters.h to see which is which!

---

## Debugging Tips

### Tip 1: Print All Macros

```c
xil_printf("UART Base: 0x%08X\n", XPAR_XUARTPS_0_BASEADDR);
xil_printf("GPIO0 Base: 0x%08X\n", XPAR_XGPIO_0_BASEADDR);
xil_printf("GPIO1 Base: 0x%08X\n", XPAR_XGPIO_1_BASEADDR);
```

### Tip 2: Verify Config Lookup

```c
XUartPs_Config *cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);

if (cfg == NULL) {
    xil_printf("ERROR: Config lookup failed!\n");
    xil_printf("Looking for base: 0x%08X\n", XPAR_XUARTPS_0_BASEADDR);
} else {
    xil_printf("Config found at: 0x%08X\n", cfg->BaseAddress);
}
```

### Tip 3: List All Available Macros

```bash
# In your project directory:
grep "^#define XPAR_X" platform/export/platform/sw/standalone_ps7_cortexa9_0/include/xparameters.h | sort
```

---

## Summary Table

| If you're using... | Look for macro... | Example |
|-------------------|------------------|---------|
| PS UART | `XPAR_XUARTPS_N_BASEADDR` | `XPAR_XUARTPS_0_BASEADDR` |
| PS SPI | `XPAR_XSPIPS_N_BASEADDR` | `XPAR_XSPIPS_0_BASEADDR` |
| PS I2C | `XPAR_XIICPS_N_BASEADDR` | `XPAR_XIICPS_0_BASEADDR` |
| AXI GPIO | `XPAR_XGPIO_N_BASEADDR` | `XPAR_XGPIO_0_BASEADDR` |
| AXI Timer | `XPAR_XTMRCTR_N_BASEADDR` | `XPAR_XTMRCTR_0_BASEADDR` |
| AXI UART Lite | `XPAR_XUARTLITE_N_BASEADDR` | `XPAR_XUARTLITE_0_BASEADDR` |

**Where `N` = instance number (0, 1, 2...)**

---

## Quick Reference Card

```
┌─────────────────────────────────────────────────────────┐
│  XPAR NAMING PATTERN                                    │
├─────────────────────────────────────────────────────────┤
│  XPAR_X<DRIVER>_<N>_<PROPERTY>                         │
│                                                         │
│  Example: XPAR_XUARTPS_0_BASEADDR                      │
│           │    │      │  │                              │
│           │    │      │  └─ Property                    │
│           │    │      └──── Instance (0-based)          │
│           │    └─────────── Driver (matches struct)     │
│           └──────────────── Xilinx Parameter            │
│                                                         │
│  Rules:                                                 │
│  1. Always use canonical name (XPAR_X<DRIVER>_...)     │
│  2. Match driver struct name (XUartPs → XUARTPS)       │
│  3. Use macros, never hardcode addresses               │
│  4. Check xparameters.h for available macros           │
└─────────────────────────────────────────────────────────┘
```

---

**Document Version:** 1.0
**Created:** 2026-04-19
**Author:** Claude AI Assistant
**Related:** MULTIPLE_GPIO_PITFALLS.md, DEVICE_ID_GUIDE.md
