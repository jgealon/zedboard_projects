# MIO LED Blink with TTC

## 📁 Project Description

This project demonstrates **LED blinking using TTC (Triple Timer Counter)** and **GPIO MIO pins** on Zynq-7000 boards (Zedboard/Zybo).

## ✅ Features

- TTC configured for 1 Hz (1-second period)
- LED toggles every second
- Uses MIO GPIO pins (no PL required!)
- Polling-based implementation
- Clean UART output showing LED state

## 🎯 Expected Output

```
========================================
   MIO LED Blink with TTC
========================================

Using MIO Pin: 7

TTC Init Successful
TTC Self Test Successful
GPIO Init Successful
GPIO Pin 7 configured as output

=== Configuring TTC ===
Target frequency: 1 Hz
Calculated interval: 6944444
Calculated prescaler: 3

=== Starting LED Blink Loop ===
LED will blink every 1 second

[1] LED ON
[2] LED OFF
[3] LED ON
[4] LED OFF
[5] LED ON
...
```

## 🔧 MIO Pin Configuration

### Zedboard

**Default:** MIO7 (defined in code)

If MIO7 doesn't work, try:
- **MIO0** - Alternative LED pin

To change the pin, edit line 68 in `helloworld.c`:
```c
#define LED_PIN  7   // Change to 0 if MIO7 doesn't work
```

### Zybo

**Default:** MIO7 ✅ (confirmed working)

## 📊 How It Works

### 1. TTC Configuration

```c
// Configure for 1 Hz (1 second period)
XTtcPs_CalcIntervalFromFreq(&ttc, 1, &interval, &prescaler);
XTtcPs_SetPrescaler(&ttc, prescaler);      // Calculated: 3 (÷16)
XTtcPs_SetInterval(&ttc, interval);        // Calculated: 6944444
```

**Result:** TTC counts down from 6,944,444 to 0 at 6.944 MHz (111 MHz ÷ 16)

### 2. GPIO Configuration

```c
XGpioPs_SetDirectionPin(&gpio, LED_PIN, 1);      // Set as output
XGpioPs_SetOutputEnablePin(&gpio, LED_PIN, 1);  // Enable output
```

### 3. LED Toggle Loop

```c
while(1) {
    // Start timer
    XTtcPs_Start(&ttc);

    // Wait for countdown to 0
    while(XTtcPs_GetCounterValue(&ttc));

    XTtcPs_Stop(&ttc);

    // Toggle LED
    led_state = !led_state;
    XGpioPs_WritePin(&gpio, LED_PIN, led_state);

    // Reset for next iteration
    XTtcPs_ResetCounterValue(&ttc);
}
```

## 🔑 Key Code Sections

### Complete LED Toggle Pattern

```c
// 1. Initialize
gpio_init(LED_PIN);    // Setup GPIO
ttc_init();            // Setup TTC

// 2. Configure TTC for 1 second
XTtcPs_CalcIntervalFromFreq(&ttc, 1, &interval, &prescaler);
XTtcPs_SetPrescaler(&ttc, prescaler);
XTtcPs_SetInterval(&ttc, interval);

// 3. Start with LED OFF
XGpioPs_WritePin(&gpio, LED_PIN, 0);

// 4. Toggle loop
while(1) {
    XTtcPs_Start(&ttc);
    while(XTtcPs_GetCounterValue(&ttc));  // Wait 1 second
    XTtcPs_Stop(&ttc);

    led_state = !led_state;
    XGpioPs_WritePin(&gpio, LED_PIN, led_state);

    XTtcPs_ResetCounterValue(&ttc);
}
```

## 🚀 How to Build and Run

### 1. Build in Vitis

```
1. Open Vitis IDE
2. Build mio_led_with_ttc platform (if not already built)
3. Build mio_led_with_ttc_app application
4. Verify no build errors
```

### 2. Program Hardware

```
1. Connect Zedboard/Zybo:
   - JTAG cable (programming)
   - UART cable (115200 baud)
   - Power supply

2. Open serial terminal (PuTTY/TeraTerm) at 115200 baud

3. Right-click mio_led_with_ttc_app → Run As → Launch Hardware
```

### 3. Observe

- **UART**: See LED state messages
- **Hardware**: Watch LED blinking every 1 second

## 🐛 Troubleshooting

### LED Not Blinking

**Problem:** LED doesn't blink
**Solutions:**
1. **Check MIO pin number** - Try changing `LED_PIN` to 0
2. **Verify GPIO init** - Check UART output shows "GPIO Init Successful"
3. **Check board LED** - Verify which LED is connected to MIO pins

### Wrong MIO Pin

If MIO7 doesn't work on your board:

```c
// In helloworld.c, line 68:
#define LED_PIN  0   // Try MIO0 instead

// Or try other MIO pins:
#define LED_PIN  9   // Alternative
#define LED_PIN  10  // Alternative
```

**How to find the correct pin:**
1. Check your board's schematic
2. Look for LED connections to PS MIO pins
3. Try common pins: 0, 7, 9, 10

### No UART Output

**Problem:** Terminal shows nothing
**Solution:** Program is waiting 2 seconds for terminal - make sure terminal is open before programming

### Wrong Blink Speed

**Problem:** LED blinks too fast/slow
**Solution:** Change frequency in line 74:

```c
ttc_setup data = {1,0,0,0};  // Change 1 to desired Hz
// Examples:
// {2,0,0,0} = 0.5 second (2 Hz)
// {0.5,0,0,0} = 2 seconds (0.5 Hz)
```

**Note:** Use integer frequencies for best accuracy.

## 📖 Related Documentation

- **TTC_USAGE_GUIDE.md** - Complete TTC documentation
- **SCU_TIMER_USAGE_GUIDE.md** - SCU Timer comparison
- **SCU_TIMER_VS_WATCHDOG_COMPARISON.md** - Timer types

## 💡 Understanding MIO vs EMIO

### MIO (Multiplexed I/O)
- ✅ **This project uses MIO**
- Direct PS GPIO pins
- No PL (FPGA fabric) required
- Limited number of pins
- Fast and simple

### EMIO (Extended MIO)
- Requires PL configuration
- More flexible routing
- Used when MIO pins exhausted
- Requires hardware platform changes

**This project uses MIO only - no PL programming needed!**

## 🎓 What You'll Learn

1. **TTC Timer Usage**
   - Automatic frequency calculation
   - Interval mode with auto-reload
   - Polling-based timing

2. **GPIO Control**
   - MIO pin configuration
   - Direction and enable settings
   - Digital output control

3. **Integration**
   - Combining timers and GPIO
   - Polling loops
   - Clean state management

## 🔬 Advanced Modifications

### 1. Faster Blink (0.5 second)

```c
ttc_setup data = {2,0,0,0};  // 2 Hz = 0.5 seconds
```

### 2. Multiple LEDs

```c
#define LED_PIN1  7
#define LED_PIN2  0

// In gpio_init, configure both:
XGpioPs_SetDirectionPin(&gpio, LED_PIN1, 1);
XGpioPs_SetDirectionPin(&gpio, LED_PIN2, 1);
XGpioPs_SetOutputEnablePin(&gpio, LED_PIN1, 1);
XGpioPs_SetOutputEnablePin(&gpio, LED_PIN2, 1);

// Alternate them:
XGpioPs_WritePin(&gpio, LED_PIN1, led_state);
XGpioPs_WritePin(&gpio, LED_PIN2, !led_state);
```

### 3. PWM-Style Dimming

```c
// Use match mode for PWM
// See TTC_USAGE_GUIDE.md for PWM examples
```

### 4. Interrupt-Driven (Non-Blocking)

```c
// Instead of polling, use TTC interrupts
// See TTC_USAGE_GUIDE.md "Use Case 3"
```

## 📁 Project Structure

```
mio_led_with_ttc/
├── mio_led_with_ttc_app/
│   └── src/
│       └── helloworld.c          ← Main application
├── mio_led_with_ttc/             ← Platform (hardware)
├── design_1_wrapper.xsa          ← Hardware export
└── README.md                     ← This file
```

## ✨ Key Takeaways

1. **TTC provides accurate timing** - 1 Hz = exactly 1 second
2. **MIO GPIO is simple** - No PL configuration needed
3. **Polling works** - No interrupts required for simple blink
4. **Tight loops are OK** - TTC handles polling well (unlike SCU Timer)

## 🔗 Next Steps

Want to learn more?

1. **Try different frequencies** - Modify `outputFreq`
2. **Add more LEDs** - Use multiple MIO pins
3. **Use interrupts** - See TTC_USAGE_GUIDE.md
4. **Generate PWM** - Dim the LED with duty cycle

---

**Status:** Fully functional LED blink with TTC ✅

*Working on Zedboard and Zybo with MIO7*
