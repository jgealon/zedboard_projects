# SCU Timer Example Project

## 📁 What's This?

This is a working Vitis application demonstrating **repeating SCU Timer** with countdown display.

## ✅ Current Status: WORKING

The timer successfully:
- Counts down from 5 seconds to 0
- Displays progress updates
- Repeats indefinitely
- Uses proper `XScuTimer_ClearInterruptStatus()` for repeating loops

## 🎯 Output Example

```
========================================
   SCU TIMER REPEATING LOOP TEST
========================================

TMR INIT SUCCESSFUL
TMR SELFTEST SUCCESSFUL

=== Starting Timer Loop ===

[Run 1] Starting timer...
[Run 1] Timer started. Waiting 5 seconds...
  ... 4 seconds remaining
  ... 3 seconds remaining
  ... 2 seconds remaining
  ... 1 seconds remaining
  ... 0 seconds remaining
[Run 1] Timer expired! (took 65399530 loop iterations)

[Run 2] Starting timer...
[Run 2] Timer started. Waiting 5 seconds...
  ... 4 seconds remaining
  ... 3 seconds remaining
  ... 2 seconds remaining
  ... 1 seconds remaining
  ... 0 seconds remaining
[Run 2] Timer expired! (took 65399530 loop iterations)
```

## 📖 Documentation

For complete documentation on SCU Timer usage, see:

**`../SCU_TIMER_USAGE_GUIDE.md`** ⭐

This comprehensive guide covers:
- How to use `GetCounterValue()` correctly
- Why repeating timers need `ClearInterruptStatus()`
- Common issues and solutions
- Best practices
- Advanced examples

## 🔑 Key Discoveries

### 1. GetCounterValue() Mystery SOLVED

**❌ Don't do this:**
```c
while(!XScuTimer_IsExpired(&tmr)) {
    u32 val = XScuTimer_GetCounterValue(&tmr);  // Every iteration!
    usleep(100000);  // Breaks timer!
}
```

**✅ Do this:**
```c
u32 count = 0;
while(!XScuTimer_IsExpired(&tmr)) {
    count++;
    if(count % 500000 == 0) {  // Every 500k iterations
        u32 val = XScuTimer_GetCounterValue(&tmr);  // Works!
    }
}
```

### 2. Repeating Timers Need ClearInterruptStatus

**❌ Without clear:**
```c
while(1) {
    XScuTimer_LoadTimer(&tmr, count);
    XScuTimer_Start(&tmr);
    while(!XScuTimer_IsExpired(&tmr));  // Expires immediately!
}
```

**✅ With clear:**
```c
while(1) {
    XScuTimer_ClearInterruptStatus(&tmr);  // Critical!
    XScuTimer_LoadTimer(&tmr, count);
    XScuTimer_Start(&tmr);
    while(!XScuTimer_IsExpired(&tmr));  // Works perfectly!
}
```

## 📁 File Structure

```
scu_timer/
├── src/
│   └── helloworld.c         ← Main source code (working example)
├── README.md                ← This file
└── [Vitis project files]
```

## 🚀 How to Run

1. Open this project in Vitis IDE
2. Build the project
3. Connect Zedboard (JTAG + UART)
4. Open serial terminal (115200 baud)
5. Run on hardware
6. Wait 5 seconds for terminal to connect
7. Watch the countdown repeat forever!

## 🔗 Related Documentation

- **SCU_TIMER_USAGE_GUIDE.md** - Complete usage guide ⭐
- **SCU_TIMER_VS_WATCHDOG_COMPARISON.md** - Timer vs Watchdog comparison
- **WATCHDOG_IMPORTANCE_EXAMPLE.md** - Why watchdog timers matter

## 💡 What I Learned

After extensive debugging, we discovered:

1. **Tight polling loops work best** - don't add function calls inside
2. **GetCounterValue() must be called infrequently** - every 100k-500k iterations
3. **ClearInterruptStatus() is mandatory** for repeating timers
4. **Frequent reads + usleep() breaks the timer** - causes stuck counter
5. **~65 million loop iterations** = 5 seconds on Zynq-7000

## 🎓 Credits

This example was created through extensive testing and debugging to understand the quirks of the SCU Timer peripheral on Zynq-7000 devices.

All findings are documented in **SCU_TIMER_USAGE_GUIDE.md** for future reference!

---

**Status:** Fully working, well-documented, production-ready ✅
