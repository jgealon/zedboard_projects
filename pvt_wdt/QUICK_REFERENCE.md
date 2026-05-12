# SCU Watchdog Timer - Quick Reference Card

## 🚀 Quick Start (5 Minutes)

```bash
1. Connect Zedboard (JTAG + UART)
2. Open serial terminal (115200 baud)
3. Create application in Vitis
4. Copy example code from SCU_WDT_CORRECTED_EXAMPLES.md
5. Build and Run
```

---

## 📋 Hardware Checklist

```
✅ Zedboard powered on
✅ JTAG cable: PC → J17
✅ UART cable: PC → J14
✅ Serial terminal: 115200, 8-N-1
✅ COM port identified (Device Manager / dmesg)
```

---

## 🔧 Serial Terminal Settings

| Setting | Value |
|---------|-------|
| **Baud Rate** | 115200 |
| **Data Bits** | 8 |
| **Parity** | None |
| **Stop Bits** | 1 |
| **Flow Control** | None |
| **Line Endings** | CR+LF |

---

## 💻 Example Summary

### Example 1: UART Echo ⏱️ Time: 15 min
**Purpose:** Basic UART + Watchdog
**Timeout:** 5 seconds
**Test:** Type characters → should echo back
**Success:** Continuous operation without reset

### Example 2: Command Parser ⏱️ Time: 20 min
**Purpose:** Command interface + LED control
**Timeout:** 4 seconds
**Test Commands:**
- `PING` → Returns PONG
- `LED 170` → Sets LED pattern
- `SLOW` → Long task (no reset)
- `HANG` → Triggers reset (proof it works!)

### Example 3: Data Logger ⏱️ Time: 15 min
**Purpose:** CSV logging + XADC
**Timeout:** 10 seconds
**Output:** CSV format, 1 sample/sec
**Success:** Continuous logging without reset

### Example 4: Reset Detection ⏱️ Time: 20 min
**Purpose:** Detect and recover from WDT reset
**Timeout:** 3 seconds
**Test:** Press '2' → triggers reset → recovery message
**Success:** Proper recovery procedure executed

---

## 🐛 Quick Troubleshooting

| Problem | Quick Fix |
|---------|-----------|
| **No UART output** | Check COM port, baud rate (115200), cable |
| **Immediate reset** | Increase WDT timeout to 10+ seconds |
| **No echo** | Check terminal local echo is OFF |
| **LEDs not working** | Verify MIO pin numbers for your board |
| **Build errors** | Rebuild platform, clean application |
| **WDT not timing out** | Verify `SetWdMode()` not `SetTimerMode()` |

---

## 📝 Essential Code Snippets

### Initialize Watchdog
```c
XScuWdt wdt;
XScuWdt_Config *cfg = XScuWdt_LookupConfig(XPAR_SCUWDT_0_DEVICE_ID);
XScuWdt_CfgInitialize(&wdt, cfg, cfg->BaseAddr);
XScuWdt_Stop(&wdt);
XScuWdt_SetWdMode(&wdt);
XScuWdt_LoadWdt(&wdt, 333333333 * 5);  // 5 seconds
XScuWdt_Start(&wdt);
```

### Service (Kick) Watchdog
```c
XScuWdt_RestartWdt(&wdt);  // Call before timeout!
```

### Initialize PS UART
```c
XUartPs uart;
XUartPs_Config *cfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
XUartPs_CfgInitialize(&uart, cfg, cfg->BaseAddress);
XUartPs_SetBaudRate(&uart, 115200);
```

### Send/Receive UART
```c
// Send
XUartPs_Send(&uart, (u8*)str, strlen(str));

// Receive (non-blocking)
if (XUartPs_IsReceiveData(uart.Config.BaseAddress)) {
    u8 byte;
    XUartPs_Recv(&uart, &byte, 1);
}
```

### Check Reset Cause
```c
if (XScuWdt_IsWdtExpired(&wdt)) {
    xil_printf("Last reset was caused by watchdog!\r\n");
    // Perform recovery actions
}
```

### PS GPIO (MIO LEDs)
```c
XGpioPs gpio;
XGpioPs_Config *cfg = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
XGpioPs_CfgInitialize(&gpio, cfg, cfg->BaseAddr);

// Configure as output
XGpioPs_SetDirectionPin(&gpio, LED_PIN, 1);
XGpioPs_SetOutputEnablePin(&gpio, LED_PIN, 1);

// Write
XGpioPs_WritePin(&gpio, LED_PIN, 1);  // On
XGpioPs_WritePin(&gpio, LED_PIN, 0);  // Off
```

---

## ⏱️ Timeout Calculations

**Formula:** `Timeout (seconds) = Counter / 333333333`

| Counter Value | Timeout | Use Case |
|---------------|---------|----------|
| `333333333 * 1` | 1 second | Fast response |
| `333333333 * 3` | 3 seconds | Interactive apps |
| `333333333 * 5` | 5 seconds | **Recommended default** |
| `333333333 * 10` | 10 seconds | Long tasks |
| `333333333 * 30` | 30 seconds | Very long tasks |

**Private Timer Clock:** 333.333 MHz (half of 666.666 MHz CPU)

---

## 🔍 Expected Outputs Quick View

### Example 1 Output:
```
Watchdog initialized: 5 second timeout
=== UART Echo with Watchdog ===
Type anything - it will echo back
[Heartbeat messages every ~10s]
```

### Example 2 Output:
```
=== Watchdog Command Monitor ===
Ready> HELP
  PING        - Echo test
  STATUS      - Show system status
  ...
Ready> PING
PONG
Ready>
```

### Example 3 Output:
```
=== Data Logger with Watchdog ===
Sample,Temp_C,Sensor1,Sensor2,Voltage
0,45.23,512,768,1.842
1,45.21,493,801,1.856
...
```

### Example 4 Output (After Reset):
```
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! WATCHDOG TIMEOUT DETECTED        !!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Recovery Actions:
  [1] Clearing watchdog reset flag
  ...
```

---

## ✅ Success Criteria

### Example 1: ✓ Checklist
- ⬜ Characters echo immediately
- ⬜ Heartbeat messages appear
- ⬜ No unexpected resets
- ⬜ Runs indefinitely

### Example 2: ✓ Checklist
- ⬜ All commands respond correctly
- ⬜ `SLOW` completes without reset
- ⬜ `HANG` causes system reset
- ⬜ LEDs respond to commands

### Example 3: ✓ Checklist
- ⬜ CSV data logs at 1 Hz
- ⬜ Temperature is 40-60°C
- ⬜ No unexpected resets
- ⬜ Error handling works

### Example 4: ✓ Checklist
- ⬜ Normal boot: no recovery message
- ⬜ Trigger timeout with '2'
- ⬜ Recovery message after reset
- ⬜ Reset flag cleared properly

---

## 📚 Document Guide

| Document | When to Use |
|----------|-------------|
| **QUICK_REFERENCE.md** | Quick lookup (you are here!) |
| **HOW_TO_USE_EXAMPLES.md** | Step-by-step instructions |
| **SCU_WDT_CORRECTED_EXAMPLES.md** | Copy source code |
| **UART_COMPARISON.md** | Understand UART choices |
| **SCU_WDT_EXAMPLES.md** | WDT concepts (UART code wrong) |
| **README_CORRECTED.md** | Project overview |

---

## 🎯 Common Use Cases

### Use Case 1: Prevent System Hang
```c
while(1) {
    do_critical_task();
    XScuWdt_RestartWdt(&wdt);  // Keep alive
}
```

### Use Case 2: Timeout Detection
```c
// If this hangs, system will reset
perform_risky_operation();
// Watchdog not serviced → reset
```

### Use Case 3: Task Monitoring
```c
void slow_task() {
    for (int i = 0; i < 10; i++) {
        process_step(i);
        XScuWdt_RestartWdt(&wdt);  // Service during long task
    }
}
```

### Use Case 4: Recovery After Crash
```c
int main() {
    init_watchdog();

    if (XScuWdt_IsWdtExpired(&wdt)) {
        recover_from_crash();  // Last run crashed!
    }

    normal_operation();
}
```

---

## ⚠️ Important Notes

### DO ✅
- Service watchdog regularly in main loop
- Set timeout longer than worst-case execution time
- Check reset flag at startup
- Test timeout scenario during development
- Use PS UART (XUartPs) for pvt_wdt project

### DON'T ❌
- Service watchdog in interrupt handler
- Use timeout shorter than initialization time
- Forget to check `XScuWdt_IsWdtExpired()` at startup
- Use `SetTimerMode()` when you want system reset
- Use AXI UARTLite (not in pvt_wdt hardware)

---

## 🔗 Quick Links

- **Full Guide:** HOW_TO_USE_EXAMPLES.md
- **Source Code:** SCU_WDT_CORRECTED_EXAMPLES.md
- **UART Info:** UART_COMPARISON.md
- **Xilinx Docs:** UG585 (Zynq TRM), UG1137 (Software Guide)

---

## 📞 Getting Help

1. **Check HOW_TO_USE_EXAMPLES.md** → Troubleshooting section
2. **Verify hardware connections** → UART cable in J14, JTAG in J17
3. **Check Console output** in Vitis for build errors
4. **Test simple example first** → Example 1 (UART Echo)
5. **Add debug messages:** `xil_printf("Checkpoint X\r\n");`

---

## 🎓 Learning Path

```
Beginner:  Example 1 (Echo) → Understand basics
           ↓
Intermediate: Example 2 (Commands) → Add features
              ↓
Advanced:  Example 3 (Logger) → Real application
           Example 4 (Recovery) → Production-ready
```

---

**Keep this card handy while developing! 📌**

*Last Updated: 2026-04-23*
