# SCU Watchdog Timer - Documentation Summary

## ⚠️ Important Correction

The original `SCU_WDT_EXAMPLES.md` incorrectly used **AXI UARTLite**, which is not present in your pvt_wdt hardware design.

## ✅ Corrected Documentation

### 1. **UART_COMPARISON.md** - Read This First!
Explains the three UART options in Zynq:
- **PS UART (XUartPs)** - ✅ What you should use
- **AXI UARTLite (XUartLite)** - ❌ Not in your design
- **AXI UART 16550** - Rarely used

**Key Takeaway:** Use **PS UART (XUartPs)** for pvt_wdt because:
- It's already available in the Processing System
- No FPGA resources needed
- Better performance and features
- Matches well with SCU WDT (also a PS peripheral)

### 2. **SCU_WDT_CORRECTED_EXAMPLES.md** - Use These Examples!
Updated versions of the watchdog examples using **PS UART** instead of UARTLite:
- ✅ Example 1: Simple UART Echo with Watchdog
- ✅ Example 2: UART Command Parser with Watchdog
- ✅ Example 3: Data Logger with Watchdog
- ✅ Example 4: Reset Detection and Recovery

All examples use:
```c
#include "xuartps.h"    // PS UART driver
XUartPs uart;           // PS UART instance
```

### 3. **SCU_WDT_EXAMPLES.md** - Reference Only
Original examples document - contains good SCU WDT explanations but uses wrong UART driver.
- ✅ Use it for: Understanding SCU WDT concepts, timing calculations
- ❌ Don't copy UART code: It uses XUartLite which isn't in your design

---

## Quick Comparison

| What | UARTLite (Wrong) | PS UART (Right) |
|------|------------------|-----------------|
| **Hardware** | Must add IP in Vivado | Already in PS7 |
| **Include** | `xuartlite.h` | `xuartps.h` |
| **Type** | `XUartLite` | `XUartPs` |
| **Device ID** | `XPAR_AXI_UARTLITE_0_DEVICE_ID` | `XPAR_XUARTPS_0_DEVICE_ID` |
| **Init** | `XUartLite_Initialize()` | `XUartPs_CfgInitialize()` + lookup |
| **Baud Rate** | Fixed in hardware | `XUartPs_SetBaudRate()` |
| **Resources** | Uses FPGA LUTs/BRAM | No PL resources |

---

## What to Do Next

### Option 1: Just Use xil_printf (Simplest)
If you only need to print messages, no extra code needed:
```c
#include "xil_printf.h"

int main() {
    init_platform();

    xil_printf("Hello from PS UART!\r\n");  // That's it!

    cleanup_platform();
    return 0;
}
```

### Option 2: Full PS UART Control (Advanced)
If you need to receive data or full UART control, use examples from `SCU_WDT_CORRECTED_EXAMPLES.md`.

---

## Files in This Directory

```
pvt_wdt/
├── README_CORRECTED.md              ← You are here
├── UART_COMPARISON.md               ← Read first: Explains UART options
├── SCU_WDT_CORRECTED_EXAMPLES.md    ← Use these examples (PS UART)
└── SCU_WDT_EXAMPLES.md              ← Reference only (wrong UART)
```

---

## Hardware Setup

Your pvt_wdt design currently has:
- ✅ Processing System (PS7) with SCU Watchdog
- ✅ PS UART (UART0) - Available through MIO pins
- ❌ No AXI GPIO in PL (would need to add in Vivado)
- ❌ No AXI UARTLite in PL

To use the LED examples, you would need to either:
1. Add AXI GPIO IP in Vivado for PL-based LEDs, OR
2. Use PS GPIO (XGpioPs) for MIO-based LEDs

The corrected examples now show PS GPIO usage which works without modifying your hardware design!

---

## Summary

✅ **Use:** `SCU_WDT_CORRECTED_EXAMPLES.md` for working code
✅ **Read:** `UART_COMPARISON.md` to understand why
✅ **Reference:** `SCU_WDT_EXAMPLES.md` for WDT concepts only

The key fix: **XUartLite → XUartPs**
