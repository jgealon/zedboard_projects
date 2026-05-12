# GPIO Interrupt Setup Guide - Vitis 2023.2

Complete guide for setting up AXI GPIO interrupts in Vitis 2023.2 Unified IDE (ZedBoard Zynq-7000).

---

## Problem: Missing XPAR_FABRIC_*_VEC_ID Macros

### In Classic Vitis (Pre-2023):

The BSP automatically generated interrupt vector ID macros:

```c
#define XPAR_FABRIC_AXI_GPIO_0_VEC_ID 61
```

### In Vitis 2023.2 (SDT Mode):

**These macros are NOT generated!** The new SDT (System Device Tree) mode only provides:

```c
#define XPAR_XGPIO_0_INTERRUPTS 0x401d
```

This breaks tutorial code that uses `XPAR_FABRIC_GPIO_0_VEC_ID`.

---

## Understanding Zynq-7000 Interrupt IDs

### GIC (Generic Interrupt Controller) Structure:

The Zynq-7000 uses ARM Cortex-A9 GIC with:

| ID Range | Type | Description |
|----------|------|-------------|
| 0-15 | SGI | Software Generated Interrupts |
| 16-31 | PPI | Private Peripheral Interrupts |
| 32-95 | SPI | Shared Peripheral Interrupts (including PL fabric) |

### Fabric Interrupts (PL to PS):

- **IRQ_F2P[0]** = GIC ID 61 (SPI interrupt 29)
- **IRQ_F2P[1]** = GIC ID 62 (SPI interrupt 30)
- ...
- **IRQ_F2P[15]** = GIC ID 76 (SPI interrupt 44)

### Decoding XPAR_*_INTERRUPTS Value:

The value `0x401d` encodes the interrupt information:

```
0x401d breakdown:
- 0x40 = flags (indicates PL fabric interrupt)
- 0x1d = 29 decimal (fabric interrupt line number)

Actual GIC Interrupt ID = 29 + 32 = 61
```

**Formula:**
```c
GIC_INTR_ID = (XPAR_*_INTERRUPTS & 0xFF) + 32
```

---

## Solution: Add Missing Macros to xparameters.h

### File Location:

```
gpio_interrupts_pfrm/export/gpio_interrupts_pfrm/sw/standalone_ps7_cortexa9_0/include/xparameters.h
```

### Add After GPIO Definitions:

Find the GPIO section (around line 88-96):

```c
/* Canonical definitions for peripheral AXI_GPIO_0 */
#define XPAR_XGPIO_0_BASEADDR 0x41200000
#define XPAR_XGPIO_0_HIGHADDR 0x4120ffff
#define XPAR_XGPIO_0_COMPATIBLE xlnx,axi-gpio-2.0
#define XPAR_XGPIO_0_GPIO_WIDTH 0x8
#define XPAR_XGPIO_0_INTERRUPT_PRESENT 0x1
#define XPAR_XGPIO_0_IS_DUAL 0x0
#define XPAR_XGPIO_0_INTERRUPTS 0x401d
#define XPAR_XGPIO_0_INTERRUPT_PARENT 0xf8f01000
```

**Add these lines:**

```c
/* Legacy FABRIC interrupt vector ID (for Classic Vitis compatibility) */
/* Calculate: (XPAR_XGPIO_0_INTERRUPTS & 0xFF) + 32 = 0x1d + 32 = 61 */
#define XPAR_FABRIC_AXI_GPIO_0_VEC_ID 61
#define XPAR_FABRIC_GPIO_0_VEC_ID 61  /* Alternative naming */
```

### Why Both Macros?

Different tutorials use different naming conventions:
- `XPAR_FABRIC_AXI_GPIO_0_VEC_ID` - includes IP name
- `XPAR_FABRIC_GPIO_0_VEC_ID` - shorter version

Both point to the same interrupt ID.

---

## Complete Working Example

### helloworld.c

```c
#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"

/* GPIO Instance */
XGpio_Config *gpio_config;
XGpio gpio;

/* Interrupt Controller Instance */
XScuGic_Config *intc_config;
XScuGic intc;

/* GPIO Interrupt Handler */
void gpio_interrupt_handler(void *CallbackRef)
{
    XGpio *GpioPtr = (XGpio *)CallbackRef;

    xil_printf("GPIO Interrupt Triggered!\r\n");

    /* Read GPIO input that triggered interrupt */
    u32 data = XGpio_DiscreteRead(&gpio, 1);
    xil_printf("GPIO Input Value: 0x%02X\r\n", data);

    /* Clear the interrupt */
    XGpio_InterruptClear(GpioPtr, XGPIO_IR_CH1_MASK);
}

/* Initialize GPIO */
void gpio_init()
{
    gpio_config = XGpio_LookupConfig(XPAR_XGPIO_0_DEVICE_ID);
    int status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);

    if(status == XST_SUCCESS)
        xil_printf("GPIO INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GPIO INIT FAILED.\r\n");

    /* Set GPIO Channel 1 as input (all 8 bits) */
    XGpio_SetDataDirection(&gpio, 1, 0xFF);

    /* Enable interrupt for GPIO Channel 1 */
    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);

    /* Enable global interrupts for GPIO */
    XGpio_InterruptGlobalEnable(&gpio);
}

/* Initialize Interrupt Controller */
void intc_init()
{
    intc_config = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
    s32 status = XScuGic_CfgInitialize(&intc, intc_config, intc_config->CpuBaseAddress);

    if(status == XST_SUCCESS)
        xil_printf("GIC INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GIC INIT FAILED.\r\n");

    /* Initialize exception handler */
    Xil_ExceptionInit();

    /* Register GIC interrupt handler */
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
                                  (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                  &intc);

    /* Enable exceptions */
    Xil_ExceptionEnable();

    /* Connect GPIO interrupt handler to GIC */
    XScuGic_Connect(&intc, XPAR_FABRIC_GPIO_0_VEC_ID,
                    (Xil_ExceptionHandler)gpio_interrupt_handler,
                    &gpio);

    /* Enable GPIO interrupt in GIC */
    XScuGic_Enable(&intc, XPAR_FABRIC_GPIO_0_VEC_ID);

    xil_printf("Interrupt system initialized.\r\n");
}

int main()
{
    init_platform();

    xil_printf("=== GPIO Interrupt Example ===\r\n");

    /* Initialize GPIO */
    gpio_init();

    /* Initialize Interrupt Controller */
    intc_init();

    xil_printf("Waiting for GPIO interrupts...\r\n");
    xil_printf("(Toggle switches on ZedBoard to trigger interrupts)\r\n");

    /* Main loop - interrupts handled automatically */
    while(1) {
        /* Application code here */
    }

    cleanup_platform();
    return 0;
}
```

---

## Key Functions Explained

### 1. XScuGic_Connect()

**Purpose:** Connect an interrupt handler to a specific interrupt ID.

```c
XScuGic_Connect(&intc,                              // GIC instance
                XPAR_FABRIC_GPIO_0_VEC_ID,          // Interrupt ID (61)
                (Xil_ExceptionHandler)gpio_handler, // Handler function
                &gpio);                              // Callback reference
```

**Parameters:**
- `InstancePtr` - Pointer to GIC instance
- `Int_Id` - **This is what we needed to find! = 61**
- `Handler` - Function pointer to interrupt handler
- `CallBackRef` - Pointer passed to handler (usually device instance)

### 2. XScuGic_Enable()

**Purpose:** Enable specific interrupt in GIC.

```c
XScuGic_Enable(&intc, XPAR_FABRIC_GPIO_0_VEC_ID);
```

### 3. XGpio_InterruptEnable()

**Purpose:** Enable GPIO channel interrupt.

```c
XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);  // Enable Channel 1
```

### 4. XGpio_InterruptClear()

**Purpose:** Clear GPIO interrupt (must be done in ISR).

```c
XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);
```

---

## Common Issues and Solutions

### Issue 1: "XPAR_FABRIC_GPIO_0_VEC_ID undeclared"

**Problem:** Vitis 2023.2 SDT mode doesn't generate this macro.

**Solution:** Add manually to `xparameters.h`:
```c
#define XPAR_FABRIC_GPIO_0_VEC_ID 61
```

**Calculation:**
```c
// From xparameters.h:
#define XPAR_XGPIO_0_INTERRUPTS 0x401d

// Calculate interrupt ID:
Fabric_IRQ_Line = 0x401d & 0xFF = 0x1d = 29
GIC_Interrupt_ID = 29 + 32 = 61
```

### Issue 2: Interrupts Not Triggering

**Checklist:**
1. ✅ GPIO interrupt enabled: `XGpio_InterruptEnable()`
2. ✅ GPIO global interrupt enabled: `XGpio_InterruptGlobalEnable()`
3. ✅ Interrupt connected to GIC: `XScuGic_Connect()`
4. ✅ Interrupt enabled in GIC: `XScuGic_Enable()`
5. ✅ Exception handler registered: `Xil_ExceptionRegisterHandler()`
6. ✅ Exceptions enabled: `Xil_ExceptionEnable()`
7. ✅ Hardware connection correct in Vivado (GPIO interrupt → IRQ_F2P[0])

### Issue 3: Wrong Interrupt ID Used

**Symptoms:**
- Interrupts never trigger
- Different peripheral interrupts triggering

**Solution:**
Verify interrupt ID matches hardware connection:

```bash
# Check in xparameters.h
grep "INTERRUPTS" xparameters.h

# Should see:
XPAR_XGPIO_0_INTERRUPTS 0x401d
```

Calculate: `(0x401d & 0xFF) + 32 = 29 + 32 = 61`

### Issue 4: System Hangs in ISR

**Problem:** Interrupt not cleared in handler.

**Solution:** Always clear interrupt in ISR:
```c
void gpio_interrupt_handler(void *CallbackRef)
{
    XGpio *GpioPtr = (XGpio *)CallbackRef;

    // DO YOUR WORK HERE

    // MUST CLEAR INTERRUPT BEFORE RETURNING
    XGpio_InterruptClear(GpioPtr, XGPIO_IR_CH1_MASK);
}
```

---

## Interrupt ID Reference for ZedBoard

### Common Fabric Interrupts (IRQ_F2P):

| Fabric Line | GIC ID | Usage Example |
|-------------|--------|---------------|
| IRQ_F2P[0] | 61 | AXI GPIO 0 |
| IRQ_F2P[1] | 62 | AXI GPIO 1 / AXI Timer 0 |
| IRQ_F2P[2] | 63 | AXI Timer 1 |
| IRQ_F2P[3] | 64 | AXI DMA MM2S |
| IRQ_F2P[4] | 65 | AXI DMA S2MM |
| ... | ... | ... |
| IRQ_F2P[15] | 76 | Last fabric interrupt |

### Common PS Interrupts:

| Peripheral | GIC ID | Macro |
|------------|--------|-------|
| UART0 | 59 | XPAR_XUARTPS_0_INTR |
| UART1 | 82 | XPAR_XUARTPS_1_INTR |
| TTC0_0 | 42 | XPAR_XTTCPS_0_INTR |
| TTC0_1 | 43 | XPAR_XTTCPS_1_INTR |
| TTC0_2 | 44 | XPAR_XTTCPS_2_INTR |
| SDIO0 | 56 | XPAR_XSDPS_0_INTR |
| GPIO | 52 | XPAR_XGPIOPS_0_INTR |
| SPI0 | 58 | XPAR_XSPIPS_0_INTR |

---

## Hardware Setup (Vivado)

### 1. GPIO IP Configuration:

In Vivado IP Integrator:
- ✅ **Enable Interrupt** (check box)
- Set **GPIO Width** (e.g., 8 bits)
- Configure as **Input** or **All Input**

### 2. Interrupt Connection:

Connect GPIO `ip2intc_irpt` pin to:
```
processing_system7_0 → IRQ_F2P[0:0]
```

### 3. Address Assignment:

Ensure GPIO is assigned to M_AXI_GP0 address space:
```
Base Address: 0x41200000 (default)
Range: 64K
```

---

## Alternative: Using Device ID Instead of Base Address

### Issue in Original Code:

```c
// WRONG - using base address instead of device ID
gpio_config = XGpio_LookupConfig(XPAR_XGPIO_0_BASEADDR);
```

### Correct Method:

```c
// CORRECT - use device ID (which is 0 for first GPIO instance)
gpio_config = XGpio_LookupConfig(XPAR_GPIO_0_DEVICE_ID);

// Or even simpler:
gpio_config = XGpio_LookupConfig(0);  // Device ID = 0
```

### Why?

`XGpio_LookupConfig()` expects a **device ID** (0, 1, 2...), not a base address!

In SDT mode, the device ID macro might not exist, so you can use:
```c
#define XPAR_GPIO_0_DEVICE_ID 0
```

Or just use `0` directly if you only have one GPIO instance.

---

## Switch Debouncing for GPIO Interrupts

### The Problem: Switch Bounce

When you toggle a mechanical switch on the ZedBoard, the contacts physically bounce, creating multiple rapid transitions:

```
Physical Toggle:  ___/‾‾‾‾‾‾‾
Actual Signal:    ___/\_/\_/‾‾  (bounces for 5-50ms)
```

**Result:** Your interrupt handler fires dozens of times per switch toggle!

**Example Output Without Debouncing:**
```
SW VALUE READ : 11
SW VALUE READ : 10
SW VALUE READ : 11
SW VALUE READ : 10
SW VALUE READ : 11
SW VALUE READ : 10   <-- All from ONE physical switch toggle!
```

### Why This Happens:

1. **AXI GPIO triggers on ANY change** (both rising and falling edges)
2. **Switch bounces** create many transitions
3. **Each transition = interrupt** = flood of UART output

### Solution 1: Software Debouncing (Recommended)

**Add change detection and delay:**

```c
#include "sleep.h"

// Track last value to detect actual changes
static u32 last_value = 0;

void intr_handler(){
    XGpio_InterruptDisable(&gpio, XGPIO_IR_CH1_MASK);

    u32 dataRead = XGpio_DiscreteRead(&gpio, 1);

    // Only print if value actually changed (debounce)
    if(dataRead != last_value) {
        xil_printf("SW VALUE READ : %0d\r\n", dataRead);
        last_value = dataRead;
    }

    XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);

    // Debounce delay - ignore bounces for 50ms
    usleep(50000);

    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);
}

void gpio_init(){
    // ... GPIO configuration ...

    // Read initial value to initialize last_value
    last_value = XGpio_DiscreteRead(&gpio, 1);
    xil_printf("Initial switch value: %0d\r\n", last_value);
}
```

**How it works:**

1. **Change Detection:** Only process if `dataRead != last_value`
   - Filters out repeated reads of same bouncing value

2. **Time-Based Debounce:** `usleep(50000)` = 50ms delay
   - Interrupts disabled during this time
   - Bounces are ignored
   - After 50ms, switch has settled

3. **Update Last Value:** Store current value for next comparison

**Result:**
```
GPIO INIT SUCCESSFUL.
Initial switch value: 0
GIC INIT SUCCESSFUL.
SW VALUE READ : 1    <-- Clean, one line per toggle!
SW VALUE READ : 3
SW VALUE READ : 7
```

### Solution 2: Hardware Debouncing (Vivado)

Add RC filter or debounce logic in Vivado block design:

```verilog
// Simple debounce counter (add as custom IP or in wrapper)
reg [19:0] debounce_counter;
reg gpio_stable;

always @(posedge clk) begin
    if (gpio_raw != gpio_stable) begin
        if (debounce_counter == 20'd999999) begin  // ~10ms at 100MHz
            gpio_stable <= gpio_raw;
            debounce_counter <= 0;
        end else begin
            debounce_counter <= debounce_counter + 1;
        end
    end else begin
        debounce_counter <= 0;
    end
end
```

Connect `gpio_stable` to AXI GPIO input instead of raw switch signal.

### Solution 3: State Machine Debouncing

For critical applications, use a state machine:

```c
typedef enum {
    IDLE,
    DEBOUNCE_WAIT,
    STABLE
} debounce_state_t;

static debounce_state_t state = IDLE;
static u32 debounce_value = 0;
static u32 last_stable_value = 0;

void intr_handler(){
    XGpio_InterruptDisable(&gpio, XGPIO_IR_CH1_MASK);

    u32 dataRead = XGpio_DiscreteRead(&gpio, 1);

    switch(state) {
        case IDLE:
            if(dataRead != last_stable_value) {
                debounce_value = dataRead;
                state = DEBOUNCE_WAIT;
                // Start timer or set flag
            }
            break;

        case DEBOUNCE_WAIT:
            // Wait for 50ms (could use timer interrupt)
            usleep(50000);
            state = STABLE;
            break;

        case STABLE:
            if(XGpio_DiscreteRead(&gpio, 1) == debounce_value) {
                // Value is stable
                xil_printf("SW VALUE READ : %0d\r\n", debounce_value);
                last_stable_value = debounce_value;
            }
            state = IDLE;
            break;
    }

    XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);
    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);
}
```

### Solution 4: Hysteresis-Based Debouncing

Only trigger on significant changes:

```c
#define HYSTERESIS_THRESHOLD 2  // Ignore changes < 2 bits

static u32 last_value = 0;

void intr_handler(){
    XGpio_InterruptDisable(&gpio, XGPIO_IR_CH1_MASK);

    u32 dataRead = XGpio_DiscreteRead(&gpio, 1);

    // Count number of bit differences
    u32 diff = dataRead ^ last_value;
    u32 bit_count = 0;
    for(int i = 0; i < 8; i++) {
        if(diff & (1 << i)) bit_count++;
    }

    // Only process if significant change
    if(bit_count >= HYSTERESIS_THRESHOLD) {
        xil_printf("SW VALUE READ : %0d\r\n", dataRead);
        last_value = dataRead;
    }

    XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);
    usleep(50000);
    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);
}
```

### Choosing the Right Debounce Time

| Application | Debounce Time | Reason |
|-------------|---------------|---------|
| Mechanical Switches | 50-100ms | Covers typical bounce duration |
| Buttons | 20-50ms | Faster response acceptable |
| Rotary Encoders | 5-10ms | Need quick response |
| Slow UI | 200ms | Prevents accidental double-clicks |

**Adjust usleep() value accordingly:**
```c
usleep(20000);   // 20ms - fast button response
usleep(50000);   // 50ms - standard switches (recommended)
usleep(100000);  // 100ms - noisy/old switches
```

### Complete Working Example with Debouncing

```c
#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "sleep.h"

XGpio_Config *gpio_config;
XGpio gpio;

// Track last value to detect actual changes (debouncing)
static u32 last_value = 0;

void intr_handler(){
    XGpio_InterruptDisable(&gpio, XGPIO_IR_CH1_MASK);

    u32 dataRead = XGpio_DiscreteRead(&gpio, 1);

    // Only print if value actually changed (debounce)
    if(dataRead != last_value) {
        xil_printf("SW VALUE READ : %0d\r\n", dataRead);
        last_value = dataRead;
    }

    XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);

    // Debounce delay - ignore bounces for 50ms
    usleep(50000);

    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);
}

void gpio_init(){
    gpio_config = XGpio_LookupConfig(XPAR_XGPIO_0_BASEADDR);
    int status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);

    if(status == XST_SUCCESS)
        xil_printf("GPIO INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GPIO INIT FAILED.\r\n");

    XGpio_SetDataDirection(&gpio, 1, 0xFF);

    // Read initial value to initialize last_value
    last_value = XGpio_DiscreteRead(&gpio, 1);
    xil_printf("Initial switch value: %0d\r\n", last_value);
}

XScuGic_Config *intc_config;
XScuGic intc;

void intc_init(){
    intc_config = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
    s32 status = XScuGic_CfgInitialize(&intc, intc_config, intc_config->CpuBaseAddress);

    if(status == XST_SUCCESS)
        xil_printf("GIC INIT SUCCESSFUL.\r\n");
    else
        xil_printf("GIC INIT FAILED.\r\n");

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
                                  (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                  &intc);
    Xil_ExceptionEnable();

    XScuGic_Connect(&intc, XPAR_FABRIC_AXI_GPIO_0_VEC_ID,
                    (Xil_InterruptHandler)intr_handler, 0);
    XScuGic_Enable(&intc, XPAR_FABRIC_AXI_GPIO_0_VEC_ID);

    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);
    XGpio_InterruptGlobalEnable(&gpio);

    xil_printf("Interrupt system ready. Toggle switches to trigger.\r\n");
}

int main()
{
    init_platform();
    gpio_init();
    intc_init();

    while(1){
        // Main loop - interrupts handled automatically
    }

    cleanup_platform();
    return 0;
}
```

### Debouncing Best Practices

#### ✅ DO:
- Always implement debouncing for mechanical switches
- Use change detection (`if(value != last_value)`)
- Add appropriate delay based on switch type
- Initialize `last_value` on startup
- Clear interrupt before re-enabling

#### ❌ DON'T:
- Don't assume switches are bounce-free
- Don't process every interrupt without filtering
- Don't use very long delays (>200ms) - poor user experience
- Don't forget to update `last_value` after processing

### Troubleshooting Debouncing

#### Still Getting Multiple Interrupts?

**Check 1:** Verify debounce delay is sufficient
```c
usleep(100000);  // Try 100ms instead of 50ms
```

**Check 2:** Ensure change detection works
```c
xil_printf("Read: %d, Last: %d\r\n", dataRead, last_value);
```

**Check 3:** Check interrupt clear timing
```c
// Clear MUST happen before re-enable
XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);
usleep(50000);  // Delay AFTER clear
XGpio_InterruptEnable(&gpio, XGPIO_IR_CH1_MASK);
```

#### Interrupts Too Slow to Respond?

**Reduce debounce time:**
```c
usleep(20000);  // 20ms - faster but less filtering
```

**Use non-blocking debounce with timer:**
```c
static XTime last_time = 0;
#define DEBOUNCE_TICKS (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2 / 20)  // 50ms

void intr_handler(){
    XTime current_time;
    XTime_GetTime(&current_time);

    if((current_time - last_time) > DEBOUNCE_TICKS) {
        u32 dataRead = XGpio_DiscreteRead(&gpio, 1);
        if(dataRead != last_value) {
            xil_printf("SW VALUE READ : %0d\r\n", dataRead);
            last_value = dataRead;
        }
        last_time = current_time;
    }

    XGpio_InterruptClear(&gpio, XGPIO_IR_CH1_MASK);
}
```

---

## Debugging Tips

### 1. Print Interrupt ID:

```c
xil_printf("GPIO Interrupt ID: %d\r\n", XPAR_FABRIC_GPIO_0_VEC_ID);
xil_printf("Expected: 61\r\n");
```

### 2. Verify GPIO Interrupt Configuration:

```c
u32 intr_status = XGpio_InterruptGetStatus(&gpio);
u32 intr_enable = XGpio_InterruptGetEnabled(&gpio);
xil_printf("Interrupt Status: 0x%08X\r\n", intr_status);
xil_printf("Interrupt Enable: 0x%08X\r\n", intr_enable);
```

### 3. Check GIC Status:

```c
u32 gic_status = XScuGic_IsInterruptEnabled(&intc, XPAR_FABRIC_GPIO_0_VEC_ID);
xil_printf("GIC Interrupt Enabled: %d\r\n", gic_status);
```

### 4. Test Without Interrupts First:

```c
// Poll GPIO in main loop to verify hardware connection
while(1) {
    u32 data = XGpio_DiscreteRead(&gpio, 1);
    xil_printf("GPIO: 0x%02X\r\n", data);
    // Add delay
}
```

---

## Summary

### ❌ Problem:
Vitis 2023.2 SDT mode doesn't generate `XPAR_FABRIC_*_VEC_ID` macros.

### ✅ Solution:
Add macro manually to `xparameters.h`:

```c
#define XPAR_FABRIC_GPIO_0_VEC_ID 61
```

### 📐 Calculation:
```
Interrupt Encoding: 0x401d
Fabric Line: 0x1d = 29
GIC Interrupt ID: 29 + 32 = 61
```

### 🔧 Usage:
```c
XScuGic_Connect(&intc, XPAR_FABRIC_GPIO_0_VEC_ID, handler, &gpio);
XScuGic_Enable(&intc, XPAR_FABRIC_GPIO_0_VEC_ID);
```

---

## For Multiple GPIO Instances

If you have multiple AXI GPIO blocks:

### GPIO 0 (IRQ_F2P[0]):
```c
#define XPAR_FABRIC_GPIO_0_VEC_ID 61
```

### GPIO 1 (IRQ_F2P[1]):
```c
#define XPAR_FABRIC_GPIO_1_VEC_ID 62
```

Check `XPAR_XGPIO_1_INTERRUPTS` value and calculate accordingly.

---

**Document Version**: 1.0
**Last Updated**: 2026-05-03
**Vitis Version**: 2023.2 Unified IDE (SDT Mode)
**Project**: gpio_interrupts (ZedBoard Zynq-7000)
**Hardware**: AXI GPIO with interrupt support
