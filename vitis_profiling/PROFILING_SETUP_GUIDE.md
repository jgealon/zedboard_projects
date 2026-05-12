# Vitis Bare-Metal Profiling Guide

Guide for profiling bare-metal applications in Vitis 2023.2 Unified IDE (ZedBoard Zynq-7000).

---

## ⚠️ Important: gprof Not Supported in Bare-Metal

**Software intrusive profiling with gprof (`-pg` flag) is NOT supported in bare-metal standalone BSP.**

### Why gprof doesn't work:

The ARM bare-metal toolchain (`gcc-arm-none-eabi`) lacks required libraries:
- ❌ `libc_p.a` - Profiling version of C library
- ❌ `libgmon.a` - gprof monitoring library
- ❌ Cannot generate `gmon.out` file

**Error when using `-pg` flag:**
```
cannot find -lc_p: No such file or directory
```

### Where gprof DOES work:

- ✅ **Linux applications** (using `gcc-arm-linux-gnueabi` toolchain)
- ✅ **Older Vitis versions** with special profiling BSP libraries
- ✅ **Simulation environments** with full glibc support

---

## Alternative Profiling Methods for Bare-Metal

### Method 1: Manual Timing with XTime (Recommended)

Use the Zynq-7000 Global Timer for precise function timing.

**Advantages:**
- ✅ Works immediately, no setup required
- ✅ Very low overhead (<1%)
- ✅ Nanosecond precision (~3ns resolution)
- ✅ No external tools needed

#### Implementation:

```c
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_io.h"

/* Global Timer Registers */
#define GLOBAL_TMR_BASEADDR 0xF8F00200U
#define GTIMER_COUNTER_LOWER_OFFSET 0x00U
#define GTIMER_COUNTER_UPPER_OFFSET 0x04U
#define XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ 666666687

typedef unsigned long long XTime;

void XTime_GetTime(XTime *Xtime_Global)
{
    u32 low, high;

    /* Read with rollover protection */
    do {
        high = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET);
        low = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_LOWER_OFFSET);
    } while(Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET) != high);

    *Xtime_Global = (((XTime) high) << 32U) | (XTime) low;
}

/* Timer runs at CPU_FREQ / 2 */
#define GTIMER_FREQ_HZ (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2)

void profile_function(void)
{
    XTime start, end;
    unsigned long long elapsed_us;

    XTime_GetTime(&start);

    // Your function to profile
    my_function();

    XTime_GetTime(&end);

    elapsed_us = ((end - start) * 1000000ULL) / GTIMER_FREQ_HZ;
    xil_printf("Function took %llu microseconds\n\r", elapsed_us);
}
```

#### Complete Example:

```c
void function_a(void)
{
    volatile int sum = 0;
    for(int i = 0; i < 100000; i++) {
        sum += i;
    }
}

void function_b(void)
{
    volatile int product = 1;
    for(int i = 1; i < 10000; i++) {
        product = (product * i) % 1000000;
    }
}

int main()
{
    XTime start, end;
    unsigned long long elapsed_us;

    init_platform();

    xil_printf("=== Performance Profiling ===\n\r");

    // Profile function_a
    XTime_GetTime(&start);
    function_a();
    XTime_GetTime(&end);
    elapsed_us = ((end - start) * 1000000ULL) / GTIMER_FREQ_HZ;
    xil_printf("function_a: %llu us\n\r", elapsed_us);

    // Profile function_b
    XTime_GetTime(&start);
    function_b();
    XTime_GetTime(&end);
    elapsed_us = ((end - start) * 1000000ULL) / GTIMER_FREQ_HZ;
    xil_printf("function_b: %llu us\n\r", elapsed_us);

    cleanup_platform();
    return 0;
}
```

**Output:**
```
=== Performance Profiling ===
function_a: 1234 us
function_b: 5678 us
```

---

### Method 2: Function Call Counting with Instrumentation

Add manual counters to track function calls:

```c
#include <stdio.h>
#include "xil_printf.h"

// Global counters
unsigned int func_a_calls = 0;
unsigned int func_b_calls = 0;
unsigned long long func_a_total_time = 0;
unsigned long long func_b_total_time = 0;

void function_a(void)
{
    XTime start, end;
    func_a_calls++;

    XTime_GetTime(&start);

    // Your code
    volatile int sum = 0;
    for(int i = 0; i < 100000; i++) {
        sum += i;
    }

    XTime_GetTime(&end);
    func_a_total_time += (end - start);
}

void function_b(void)
{
    XTime start, end;
    func_b_calls++;

    XTime_GetTime(&start);

    // Your code
    volatile int product = 1;
    for(int i = 1; i < 10000; i++) {
        product = (product * i) % 1000000;
    }

    XTime_GetTime(&end);
    func_b_total_time += (end - start);
}

void print_profile_report(void)
{
    unsigned long long func_a_avg_us, func_b_avg_us;

    if(func_a_calls > 0) {
        func_a_avg_us = ((func_a_total_time / func_a_calls) * 1000000ULL) / GTIMER_FREQ_HZ;
    }
    if(func_b_calls > 0) {
        func_b_avg_us = ((func_b_total_time / func_b_calls) * 1000000ULL) / GTIMER_FREQ_HZ;
    }

    xil_printf("\n=== Profile Report ===\n\r");
    xil_printf("function_a: %u calls, avg %llu us\n\r", func_a_calls, func_a_avg_us);
    xil_printf("function_b: %u calls, avg %llu us\n\r", func_b_calls, func_b_avg_us);
}

int main()
{
    init_platform();

    // Call functions multiple times
    for(int i = 0; i < 100; i++) {
        function_a();
    }

    for(int i = 0; i < 50; i++) {
        function_b();
    }

    print_profile_report();

    cleanup_platform();
    return 0;
}
```

**Output:**
```
=== Profile Report ===
function_a: 100 calls, avg 1234 us
function_b: 50 calls, avg 5678 us
```

---

### Method 3: Macro-Based Profiling Framework

Create reusable profiling macros:

```c
#include "xil_types.h"
#include "xil_io.h"
#include "xil_printf.h"

/* Global Timer Configuration */
#define GLOBAL_TMR_BASEADDR 0xF8F00200U
#define GTIMER_COUNTER_LOWER_OFFSET 0x00U
#define GTIMER_COUNTER_UPPER_OFFSET 0x04U
#define XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ 666666687
#define GTIMER_FREQ_HZ (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2)

typedef unsigned long long XTime;

void XTime_GetTime(XTime *Xtime_Global)
{
    u32 low, high;
    do {
        high = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET);
        low = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_LOWER_OFFSET);
    } while(Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET) != high);
    *Xtime_Global = (((XTime) high) << 32U) | (XTime) low;
}

/* Profiling Macros */
#define PROFILE_ENABLED 1

#if PROFILE_ENABLED
    #define PROFILE_START(name) \
        XTime profile_##name##_start; \
        XTime_GetTime(&profile_##name##_start);

    #define PROFILE_END(name) \
        do { \
            XTime profile_##name##_end; \
            XTime_GetTime(&profile_##name##_end); \
            unsigned long long us = ((profile_##name##_end - profile_##name##_start) * 1000000ULL) / GTIMER_FREQ_HZ; \
            xil_printf("[PROFILE] %s: %llu us\n\r", #name, us); \
        } while(0)
#else
    #define PROFILE_START(name)
    #define PROFILE_END(name)
#endif

/* Usage Example */
int main()
{
    init_platform();

    PROFILE_START(main_loop);

    PROFILE_START(section_a);
    function_a();
    PROFILE_END(section_a);

    PROFILE_START(section_b);
    function_b();
    PROFILE_END(section_b);

    PROFILE_END(main_loop);

    cleanup_platform();
    return 0;
}
```

**Output:**
```
[PROFILE] section_a: 1234 us
[PROFILE] section_b: 5678 us
[PROFILE] main_loop: 6912 us
```

---

### Method 4: Statistical Sampling (Advanced)

Use timer interrupts to sample program counter periodically:

```c
#include "xscutimer.h"
#include "xscugic.h"

#define SAMPLE_INTERVAL_US 1000  // Sample every 1ms

XScuTimer TimerInstance;
XScuGic GicInstance;

unsigned int function_a_samples = 0;
unsigned int function_b_samples = 0;

void timer_interrupt_handler(void *CallBackRef)
{
    // Read program counter (requires debug mode)
    // Increment counter for current function

    // Simple approach: use function markers
    // In each function, set a global "current_function" variable
}

// Then analyze sample counts to determine where CPU time is spent
```

---

## Comparison: Profiling Methods

| Method | Setup | Overhead | Precision | Call Graph | Best For |
|--------|-------|----------|-----------|------------|----------|
| **XTime Manual** | None | <1% | ~3ns | No | Quick measurements |
| **Call Counting** | Manual | Low | ~3ns | No | Function statistics |
| **Macro Framework** | Medium | Low | ~3ns | No | Reusable profiling |
| **Statistical Sampling** | Complex | Medium | 1-10ms | Yes | Hotspot detection |
| **gprof (-pg)** | N/A | N/A | N/A | N/A | ❌ Not available |

---

## Build Configuration (Current Setup)

### BSP Settings (bsp.yaml)

```yaml
os_config:
  standalone:
    enable_sw_intrusive_profile:
      value: true  # Kept for compatibility, but has no effect

proc_config:
  ps7_cortexa9_0:
    proc_extra_compiler_flags:
      value: ' -g -Wall -Wextra -fno-tree-loop-distribute-patterns'
      # NOTE: -pg flag removed (not supported in bare-metal)
```

### Application Settings (UserConfig.cmake)

```cmake
# Debug level
set(USER_COMPILE_DEBUG_LEVEL -g3)

# Optimization level (lower for profiling)
set(USER_COMPILE_OPTIMIZATION_LEVEL -O0)

# -pg flag NOT used (missing libc_p library)
set(USER_COMPILE_OTHER_FLAGS )

# Workaround for CMake path bug
set(USER_LINK_DIRECTORIES ".")
```

---

## Building the Application

1. **Rebuild Platform**:
   - Right-click **vitis_profiling_pfrm** → Clean → Build Project

2. **Rebuild Application**:
   - Right-click **vitis_profiling_app** → Clean → Build Project

3. **Run and Profile**:
   - Use XTime functions in your code
   - View timing results over UART

---

## Example: Complete Profiling Application

See `helloworld.c` for a complete example using XTime-based profiling.

**Key functions:**
- `XTime_GetTime()` - Read 64-bit timer
- Calculate elapsed time in microseconds
- Print results over UART

---

## Tips for Effective Profiling

### 1. Reduce Optimization During Profiling

```cmake
set(USER_COMPILE_OPTIMIZATION_LEVEL -O0)  # No optimization
```

### 2. Use Volatile for Timing Loops

```c
volatile int sum = 0;  // Prevents compiler optimization
```

### 3. Run Multiple Iterations

```c
// Get average performance
for(int i = 0; i < 100; i++) {
    profile_function();
}
```

### 4. Account for Timer Overhead

```c
XTime start, end;
XTime_GetTime(&start);
XTime_GetTime(&end);
unsigned long long overhead = end - start;
xil_printf("Timer overhead: %llu ticks\n\r", overhead);
```

### 5. Use Debug Optimization for Accurate Results

```cmake
set(USER_COMPILE_DEBUG_LEVEL -g3)  # Maximum debug info
```

---

## Troubleshooting

### Build Error: "cannot find -lc_p"

**Problem**: `-pg` flag is enabled

**Solution**: Remove `-pg` from:
- `bsp.yaml` → proc_extra_compiler_flags
- `UserConfig.cmake` → USER_COMPILE_OTHER_FLAGS

### Build Error: "cannot find Files/Git/"

**Problem**: Empty USER_LINK_DIRECTORIES causing linker path bug

**Solution**: Set in `UserConfig.cmake`:
```cmake
set(USER_LINK_DIRECTORIES ".")
```

### Timing Values Seem Wrong

**Problem**: Incorrect CPU frequency constant

**Solution**: Verify your CPU frequency:
```c
xil_printf("CPU Freq: %u Hz\n", XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ);
xil_printf("Timer Freq: %u Hz\n", GTIMER_FREQ_HZ);
```

### Functions Execute Too Fast to Measure

**Problem**: Sub-microsecond execution times

**Solution**:
- Run function multiple times in a loop
- Use nanosecond calculation instead of microseconds
- Check for compiler optimization removing code

---

## Summary

### ❌ What Doesn't Work:
- gprof profiling (`-pg` flag)
- `gmon.out` file generation
- Automatic call graph generation
- GUI "Enable profiling (gprof)" checkbox (if it exists, it won't work for bare-metal)

### ✅ What Does Work:
- Manual timing with XTime API
- Function call counting
- Custom profiling frameworks
- Statistical sampling (advanced)

### Recommended Approach:

Use **XTime manual timing** for bare-metal profiling:
1. Simple to implement
2. Low overhead
3. Precise measurements
4. No external dependencies

---

**Document Version**: 2.0
**Last Updated**: 2026-05-03
**Vitis Version**: 2023.2 Unified IDE
**Target**: ZedBoard Zynq-7000 (Bare-Metal Standalone BSP)
**Status**: gprof not supported, XTime profiling recommended
