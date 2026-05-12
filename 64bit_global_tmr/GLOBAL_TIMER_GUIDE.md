# Zynq-7000 Global Timer Guide

Complete guide for using the 64-bit Global Timer in Zynq-7000 ZedBoard projects.

## Table of Contents
1. [Why No Driver in BSP?](#why-no-driver-in-bsp)
2. [Two Approaches](#two-approaches)
3. [Setup Instructions](#setup-instructions)
4. [Usage Examples](#usage-examples)
5. [Troubleshooting](#troubleshooting)

---

## Why No Driver in BSP?

When you check `bsp.yaml`, you'll see:
```yaml
ps7_globaltimer_0: None
```

**This is normal and expected!**

The Zynq-7000 Global Timer is part of the ARM Cortex-A9 MPCore's private peripherals. Unlike GPIO or UART, it doesn't have a separate "driver" package. Instead, it's accessed through:
1. Direct register access (manual)
2. Xilinx's built-in `xtime_l.h` functions (recommended)

There is **NO "time_l" driver** - you might be thinking of `xtime_l.h` (the header file).

---

## Two Approaches

### Approach 1: Use Xilinx's Built-in Functions (RECOMMENDED)

Xilinx provides `XTime_GetTime()` in the standalone BSP.

**Advantages:**
- ✅ Simple API
- ✅ Already tested and maintained
- ✅ Handles 64-bit counter correctly
- ✅ Part of standard BSP

**Requirements:**
- Add missing parameters to `xparameters.h` (see setup below)

### Approach 2: Direct Register Access (Custom)

Access Global Timer registers directly using `Xil_In32()` and `Xil_Out32()`.

**Advantages:**
- ✅ Full control
- ✅ No dependencies
- ✅ Educational - learn hardware details

**Requirements:**
- Implement your own functions

---

## Setup Instructions

### For Approach 1: Using Xilinx Functions

The Global Timer parameters are missing from the auto-generated `xparameters.h`. You need to add them manually.

#### Step 1: Add Parameters to xparameters.h

**File**: `64bit_global_tmr_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xparameters.h`

Add these lines near the top (after `#define XPARAMETERS_H`):

```c
/* Global Timer Parameters - Added manually for standalone BSP */
#define XPAR_GLOBAL_TMR_BASEADDR 0xF8F00200U
#define XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ 666666687
```

#### Step 2: Include the Header in Your Code

```c
#include "xtime_l.h"

// XTime is defined as u64 (unsigned long long)
XTime start, end;

// Get current timer value
XTime_GetTime(&start);
// ... do something ...
XTime_GetTime(&end);

// Calculate difference
XTime elapsed = end - start;
```

#### Step 3: Calculate Time in Microseconds/Milliseconds

```c
// Global Timer runs at CPU_FREQ / 2
#define GTIMER_FREQ_HZ (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2)

// Convert ticks to microseconds
unsigned long long us = (elapsed * 1000000ULL) / GTIMER_FREQ_HZ;

// Convert ticks to milliseconds
unsigned long long ms = (elapsed * 1000ULL) / GTIMER_FREQ_HZ;
```

### For Approach 2: Direct Register Access

Your current code already implements this correctly:

**Global Timer Registers:**
```c
#define GLOBAL_TMR_BASEADDR 0xF8F00200U
#define GTIMER_COUNTER_LOWER_OFFSET 0x00U  // Lower 32 bits
#define GTIMER_COUNTER_UPPER_OFFSET 0x04U  // Upper 32 bits
#define GTIMER_CONTROL_OFFSET 0x08U        // Control register
```

**Read 64-bit Counter:**
```c
void XTime_GetTime(XTime *Xtime_Global)
{
    u32 low, high;

    // Read with rollover protection
    do {
        high = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET);
        low = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_LOWER_OFFSET);
    } while(Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET) != high);

    *Xtime_Global = (((XTime) high) << 32U) | (XTime) low;
}
```

**Enable Global Timer:**
```c
void GlobalTimer_Start(void)
{
    // Enable Global Timer (bit 0 = enable)
    Xil_Out32(GLOBAL_TMR_BASEADDR + GTIMER_CONTROL_OFFSET, 0x1);
}
```

---

## Usage Examples

### Example 1: Simple Timing

```c
#include "xtime_l.h"

int main()
{
    XTime start, end;

    init_platform();

    // Timer is automatically started by boot code
    XTime_GetTime(&start);

    // Your code here
    for(int i = 0; i < 1000000; i++);

    XTime_GetTime(&end);

    xil_printf("Elapsed ticks: %llu\n", end - start);

    cleanup_platform();
    return 0;
}
```

### Example 2: Microsecond Delay

```c
void delay_us(unsigned int microseconds)
{
    XTime start, current;
    XTime delay_ticks = ((XTime)microseconds * GTIMER_FREQ_HZ) / 1000000ULL;

    XTime_GetTime(&start);
    do {
        XTime_GetTime(&current);
    } while ((current - start) < delay_ticks);
}

// Usage
delay_us(100);  // 100 microsecond delay
```

### Example 3: Measure Function Execution Time

```c
void measure_function_time(void)
{
    XTime start, end;
    unsigned long long elapsed_us;

    XTime_GetTime(&start);

    // Function to measure
    my_complex_function();

    XTime_GetTime(&end);

    elapsed_us = ((end - start) * 1000000ULL) / GTIMER_FREQ_HZ;
    xil_printf("Function took %llu microseconds\n", elapsed_us);
}
```

### Example 4: Continuous Monitoring

```c
void monitor_loop_timing(void)
{
    XTime prev_time, current_time;
    unsigned long long interval_ms;

    XTime_GetTime(&prev_time);

    while(1) {
        // Your loop code here
        process_data();

        XTime_GetTime(&current_time);
        interval_ms = ((current_time - prev_time) * 1000ULL) / GTIMER_FREQ_HZ;

        if(interval_ms > 10) {
            xil_printf("Warning: Loop took %llu ms\n", interval_ms);
        }

        prev_time = current_time;
    }
}
```

### Example 5: Timeout Implementation

```c
int wait_for_event_with_timeout(unsigned int timeout_ms)
{
    XTime start, current;
    XTime timeout_ticks = ((XTime)timeout_ms * GTIMER_FREQ_HZ) / 1000ULL;

    XTime_GetTime(&start);

    while(!event_occurred()) {
        XTime_GetTime(&current);
        if((current - start) > timeout_ticks) {
            return -1;  // Timeout
        }
    }

    return 0;  // Success
}
```

---

## Important Notes

### 1. Timer Frequency

The Global Timer runs at **half the CPU frequency**:
```c
GTIMER_FREQ = CPU_FREQ / 2
```

For Zynq-7000 running at 666.666687 MHz:
```c
GTIMER_FREQ = 666666687 / 2 = 333333343.5 Hz
```

### 2. Timer Resolution

- **Tick period**: ~3 nanoseconds (at 333 MHz)
- **Maximum time**: ~1750 years before 64-bit overflow
- **Precision**: Excellent for microsecond and millisecond timing

### 3. Counter Rollover

When reading the 64-bit counter:
1. Always read upper 32 bits first
2. Then read lower 32 bits
3. Re-read upper 32 bits to check for rollover
4. If changed, repeat

This prevents errors when the lower counter rolls over during read.

### 4. Timer is Always Running

The Global Timer starts automatically during boot. You don't need to initialize it unless you want to:
- Reset the counter to 0
- Change timer settings
- Use interrupts or compare features

### 5. Difference from sleep() Functions

```c
// sleep() uses TTC (Triple Timer Counter) or SCU Timer
sleep(1);  // May use different hardware timer

// Global Timer is always available and doesn't conflict
XTime_GetTime(&time);  // Uses Global Timer
```

---

## Troubleshooting

### Build Error: "XPAR_GLOBAL_TMR_BASEADDR undeclared"

**Problem**: Missing parameters in xparameters.h

**Solution**: Add these lines to `xparameters.h`:
```c
#define XPAR_GLOBAL_TMR_BASEADDR 0xF8F00200U
#define XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ 666666687
```

### Build Error: "xtime_l.h: No such file or directory"

**Problem**: Header path not included

**Solution**: The file exists in `libsrc/standalone/src/arm/cortexa9/xtime_l.h`. It should be automatically included. If not, check your BSP is built correctly.

### Runtime Issue: Timer Values Don't Increase

**Problem**: Timer not started or system clock issue

**Solution**:
1. Verify FPGA is programmed
2. Check CPU is running at expected frequency
3. Ensure timer control register bit 0 is set:
   ```c
   Xil_Out32(GLOBAL_TMR_BASEADDR + 0x08, 0x1);
   ```

### Timing Values Are Wrong

**Problem**: Incorrect frequency constant

**Solution**: Check your actual CPU frequency. Use:
```c
xil_printf("CPU Freq: %u Hz\n", XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ);
xil_printf("Timer Freq: %u Hz\n", GTIMER_FREQ_HZ);
```

Common Zynq-7000 CPU frequencies:
- 666.666687 MHz (most common)
- 667 MHz
- 533 MHz
- 800 MHz (overclocked)

### Values Overflow or Wrap Around

**Problem**: Using 32-bit variables for 64-bit timer

**Solution**: Always use `XTime` (u64 or unsigned long long):
```c
// WRONG
u32 time;  // Only 32 bits!

// CORRECT
XTime time;  // 64 bits
unsigned long long time;  // 64 bits
```

---

## Comparison: Global Timer vs Other Zynq Timers

| Timer | Type | Width | Frequency | Best For |
|-------|------|-------|-----------|----------|
| **Global Timer** | Free-running counter | 64-bit | CPU_FREQ/2 | High-precision timing, timestamps |
| **SCU Private Timer** | Countdown timer | 32-bit | CPU_FREQ/2 | Interrupts, periodic events |
| **TTC** | Triple Timer Counter | 16-bit | Variable | PWM, general timing, sleep() |
| **AXI Timer** | PL timer | 32/64-bit | Variable | FPGA logic timing |

**Global Timer Advantages:**
- ✅ Always available (part of Cortex-A9)
- ✅ 64-bit range (no overflow concerns)
- ✅ High frequency (3ns resolution)
- ✅ Simple to use
- ✅ No PL resources needed

---

## Summary

### Quick Start Checklist

For using Xilinx functions:

1. ✅ Add parameters to `xparameters.h`:
   ```c
   #define XPAR_GLOBAL_TMR_BASEADDR 0xF8F00200U
   #define XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ 666666687
   ```

2. ✅ Include header in your code:
   ```c
   #include "xtime_l.h"
   ```

3. ✅ Use the functions:
   ```c
   XTime start, end;
   XTime_GetTime(&start);
   // your code
   XTime_GetTime(&end);
   ```

4. ✅ Calculate elapsed time:
   ```c
   #define GTIMER_FREQ_HZ (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2)
   unsigned long long us = ((end - start) * 1000000ULL) / GTIMER_FREQ_HZ;
   ```

### Key Points

- **No driver needed** - this is normal for Global Timer
- **Two approaches** - Xilinx functions or direct access
- **Always running** - no initialization needed
- **64-bit counter** - no overflow worries
- **High precision** - ~3ns resolution

---

**Document Version**: 1.0
**Last Updated**: 2026-05-03
**Vitis Version**: 2023.2 Unified IDE
**Project**: 64bit_global_tmr (ZedBoard Zynq-7000)
