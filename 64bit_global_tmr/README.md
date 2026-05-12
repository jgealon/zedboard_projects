# Zynq-7000 64-bit Global Timer Usage Guide

## Overview

Your `64bit_global_tmr` project now demonstrates how to use the **Cortex-A9 64-bit Global Timer** directly in Zynq-7000. The global timer is a hardware timer built into the ARM Cortex-A9 CPU that provides 64-bit timing capabilities.

## What Was the Problem?

The default BSP configuration uses:
- **TTC (Triple Timer Counter)** for sleep/delay functions
- **SCU Timer** for tick/interrupt functions

You wanted to use the **64-bit Global Timer** instead, but there wasn't a dedicated "time_l driver" - the global timer functions exist but weren't enabled in the default configuration.

## Solution: Direct Global Timer Access

The updated `helloworld.c` shows how to access the 64-bit global timer directly through its memory-mapped registers at `0xF8F00200`.

### Key Global Timer Features

- **64-bit counter** (vs 32-bit in SCU Timer, 16-bit in TTC)
- **High precision** (runs at CPU frequency / 2)
- **No driver overhead** - direct register access
- **Always available** in Cortex-A9 cores

### Register Map

```
Base Address: 0xF8F00200
+0x00: Counter Lower 32 bits
+0x04: Counter Upper 32 bits
+0x08: Control Register
+0x0C: Interrupt Status
+0x10: Comparator Value Lower
+0x14: Comparator Value Upper
+0x18: Auto-increment Register
```

## How to Use

### 1. Basic Timer Reading

```c
XTime current_time;
XTime_GetTime(&current_time);  // Get 64-bit timestamp
```

### 2. Timing Measurements

```c
XTime start, end;
XTime_GetTime(&start);
// ... your code ...
XTime_GetTime(&end);
unsigned long long elapsed_us = GetElapsedTime_us(start, end);
```

### 3. Delays

```c
GlobalTimer_Delay_ms(1000);    // 1 second delay
GlobalTimer_Delay_us(500);     // 500 microsecond delay
```

### 4. Timer Control

```c
GlobalTimer_Start();     // Enable timer
GlobalTimer_Stop();      // Disable timer
GlobalTimer_Reset();     // Reset counter to 0
```

## Expected Output

```
========================================
   Zynq-7000 64-bit Global Timer Demo
========================================

Global Timer started!

=== Basic Timer Reading ===
Start time: 0x0000000000000000
End time:   0x00000000002FAF08
Elapsed: 100000 microseconds (100 milliseconds)

=== Longer Timing Test ===
Starting 1-second timing test...
Elapsed time: 1000 ms

=== Continuous Timer Reading ===
Timer[0]: 0x0000000000A00000
Timer[1]: 0x0000000000B00000
...

=== Timer Reset Test ===
Before reset: 0x0000000000C00000
After reset:  0x0000000000000000

Global Timer demo completed!
```

## Alternative: Enable Global Timer in BSP

If you want the global timer to be used by default for `sleep()`, `usleep()`, etc., you would need to modify the BSP configuration to define `XTIMER_IS_DEFAULT_TIMER` instead of using TTC/SCU timers.

However, direct access as shown here gives you full control over the 64-bit timer capabilities.

## Performance Notes

- **Frequency**: Global Timer runs at CPU frequency ÷ 2 (typically ~333 MHz on Zynq-7000)
- **Resolution**: Better than 1 nanosecond
- **Range**: Can count for centuries without rollover
- **Overhead**: Minimal (direct register access)

## Applications

The 64-bit global timer is ideal for:
- High-precision timing measurements
- Long-duration timing (seconds to years)
- Performance profiling
- Real-time system timing
- Benchmarking code execution

## Building and Running

1. Build the application in Vitis
2. Program your Zynq-7000 board
3. Open UART terminal at 115200 baud
4. Run the application
5. Observe the timer readings and measurements

The global timer will continue running even when the application exits, so subsequent runs will show accumulated time values.