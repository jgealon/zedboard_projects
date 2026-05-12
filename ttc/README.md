# TTC (Triple Timer Counter) Project

## 📁 What's This?

This is a working Vitis application demonstrating the **TTC (Triple Timer Counter)** on Zynq-7000 with interval mode and polling.

## ✅ Current Status: WORKING

The TTC successfully:
- Initializes without errors
- Configures for 1 Hz (1-second period)
- Counts down to 0 using interval mode
- Repeats indefinitely
- Uses decrement mode with auto-reload

## 🎯 Output Example

```
========================================
   TTC (Triple Timer Counter) Test
========================================

TTC Init Successful
Self Test Successful

=== Configuring TTC ===
Target frequency: 1 Hz
Calculated interval: 6944444
Calculated prescaler: 3

=== Starting 1-second timer loop ===

[Iteration 1] 1-second delay achieved!
[Iteration 2] 1-second delay achieved!
[Iteration 3] 1-second delay achieved!
...
```

## 📖 Documentation

For complete documentation on TTC usage, see:

**`../pvt_wdt/TTC_USAGE_GUIDE.md`** ⭐

This comprehensive guide covers:
- TTC vs SCU Timer comparison
- How to use interval mode, match mode, PWM
- Interrupt-driven examples
- Event counting and input capture
- Best practices and common issues
- Advanced use cases

## 🔑 Key Discoveries

### 1. Automatic Frequency Calculation

TTC has a convenient function to calculate prescaler and interval:

```c
XInterval interval;
u8 prescaler;
XTtcPs_CalcIntervalFromFreq(&ttc, frequency_hz, &interval, &prescaler);
```

### 2. 16-bit Counter Limitation

TTC counter is only 16-bit (max 65535), so for long periods:
- Use higher prescaler values
- The driver automatically selects appropriate prescaler

### 3. Interval Mode Auto-Reload

With `XTTCPS_OPTION_INTERVAL_MODE`, the counter automatically reloads:

```c
XTtcPs_SetOptions(&ttc, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_DECREMENT);
XTtcPs_SetInterval(&ttc, interval);

// Counter counts: interval → 0 → interval → 0 (repeats automatically)
```

### 4. No XST_DEVICE_IS_STARTED Issue (After Fix)

Initially got error 5 (`XST_DEVICE_IS_STARTED`), but this was resolved by ensuring the TTC driver properly stops any running timer before initialization.

## 📁 File Structure

```
ttc/
├── ttc_app/
│   └── src/
│       └── helloworld.c         ← Main application (working)
├── ttc_platform/                ← Hardware platform
└── README.md                    ← This file
```

## 🚀 How to Run

1. Open this project in Vitis IDE
2. Build the project
3. Connect Zedboard (JTAG + UART)
4. Open serial terminal (115200 baud)
5. Run on hardware
6. Observe 1-second periodic messages

## 🔗 Related Documentation

- **TTC_USAGE_GUIDE.md** - Complete TTC usage guide ⭐
- **SCU_TIMER_USAGE_GUIDE.md** - SCU Timer comparison
- **SCU_TIMER_VS_WATCHDOG_COMPARISON.md** - Timer types explained

## 💡 What I Learned

### TTC vs SCU Timer

| Feature | TTC | SCU Timer |
|---------|-----|-----------|
| **Counter width** | 16-bit | 32-bit |
| **Number of timers** | 3 (TTC0/1/2) | 1 global + 1 per CPU |
| **Auto freq calc** | ✅ Yes | ❌ No |
| **PWM capable** | ✅ Yes | ❌ No |
| **Match mode** | ✅ 3 match points | ❌ None |
| **Best for** | Flexible timing, PWM | Simple delays |

### Key Code Pattern

```c
// 1. Initialize
ttc_config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_BASEADDR);
XTtcPs_CfgInitialize(&ttc, ttc_config, ttc_config->BaseAddress);

// 2. Configure frequency
XInterval interval;
u8 prescaler;
XTtcPs_CalcIntervalFromFreq(&ttc, 1, &interval, &prescaler);  // 1 Hz
XTtcPs_SetPrescaler(&ttc, prescaler);
XTtcPs_SetInterval(&ttc, interval);

// 3. Set options
u32 options = XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_DECREMENT;
XTtcPs_SetOptions(&ttc, options);

// 4. Use in loop
while(1) {
    XTtcPs_Start(&ttc);
    while(XTtcPs_GetCounterValue(&ttc));  // Wait for 0
    XTtcPs_Stop(&ttc);
    xil_printf("1 second elapsed!\r\n");
    XTtcPs_ResetCounterValue(&ttc);
}
```

## 🎓 Credits

This example was created to understand TTC operation on Zynq-7000, exploring:
- Interval mode timing
- Automatic frequency calculation
- Prescaler operation
- Polling vs interrupt-driven approaches

All findings are documented in **TTC_USAGE_GUIDE.md** for future reference!

## 🆚 When to Use TTC vs SCU Timer?

**Use TTC when:**
- ✅ Need PWM or waveform output
- ✅ Need multiple independent timers
- ✅ Need match/compare functionality
- ✅ Want convenient frequency calculation
- ✅ 16-bit counter is sufficient

**Use SCU Timer when:**
- ✅ Need 32-bit counter (longer max period)
- ✅ Need highest precision (333 MHz clock)
- ✅ Simple delays without extra features
- ✅ Want prescaler demonstration

## 🔬 Advanced Features

The TTC has many advanced features not demonstrated in this basic example:

1. **PWM Generation** - Create precise PWM signals
2. **Interrupt Mode** - Background timing without polling
3. **Match Mode** - Trigger events at specific counts
4. **External Clock** - Count external events
5. **Waveform Output** - Hardware output pin toggling

See **TTC_USAGE_GUIDE.md** for examples of all these features!

---

**Status:** Fully working, well-documented, ready for advanced exploration ✅
