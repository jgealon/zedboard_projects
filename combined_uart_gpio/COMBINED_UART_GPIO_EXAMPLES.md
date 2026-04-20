# Combined UART, UARTLite Loopback, GPIO Examples

## Overview

This document presents **3 advanced applications** that combine:
- **PS UART** (Control & monitoring)
- **Dual UARTLite** (PL - Loopback communication)
- **GPIO LEDs** (Visual output)
- **GPIO Switches** (User input)

All examples follow the **uart_axi_uartlite** implementation structure with proper dual UARTLite loopback verification.

---

## Hardware Requirements

### Required Components

| Component | Quantity | Description |
|-----------|----------|-------------|
| **PS UART** | 1 | MIO UART for command & control |
| **UARTLite 1** | 1 | AXI UARTLite transmitter |
| **UARTLite 2** | 1 | AXI UARTLite receiver (loopback) |
| **GPIO LED** | 1 | AXI GPIO for 8 LEDs (output) |
| **GPIO Switch** | 1 | AXI GPIO for 8 switches (input) |

### Hardware Connections

```
┌─────────────────────────────────────────┐
│           ZedBoard Zynq-7000            │
│                                         │
│  ┌─────────┐         ┌──────────────┐  │
│  │ PS UART │◄────────┤ USB Terminal │  │  (115200 baud)
│  │ (MIO)   │         └──────────────┘  │
│  └─────────┘                           │
│                                         │
│  ┌───────────┐       ┌───────────┐    │
│  │ UARTLite1 ├───────► UARTLite2 │    │  (Loopback)
│  │   (TX)    │       │   (RX)    │    │
│  └───────────┘       └───────────┘    │
│                                         │
│  ┌──────────┐        ┌──────────┐     │
│  │ GPIO LEDs├────────►  8 LEDs   │     │
│  └──────────┘        └──────────┘     │
│                                         │
│  ┌────────────┐      ┌──────────┐     │
│  │ GPIO Switch│◄─────┤ 8 Switches│     │
│  └────────────┘      └──────────┘     │
└─────────────────────────────────────────┘
```

### Address Map Example

Based on uart_axi_uartlite project structure:

```c
// PS UART
#define XPAR_XUARTPS_0_BASEADDR    0xE0001000

// UARTLite instances
#define XPAR_XUARTLITE_0_BASEADDR  0x42C00000  // TX
#define XPAR_XUARTLITE_1_BASEADDR  0x42C10000  // RX

// GPIO instances
#define XPAR_XGPIO_0_BASEADDR      0x41200000  // LEDs
#define XPAR_XGPIO_1_BASEADDR      0x41210000  // Switches
```

---

## Example 1: LED Controller with UARTLite Loopback Monitor

### Description

An interactive LED controller with real-time switch monitoring and data verification via UARTLite loopback.

### Features

- **Command-based LED control** via PS UART
- **Real-time switch monitoring**
- **UARTLite loopback testing** for data integrity
- **Statistics tracking** (bytes sent/received)
- **Streaming mode** for continuous data transmission

### How to Use

#### Step 1: Hardware Setup

1. Connect USB cable to J14 (PS UART)
2. Ensure UARTLite TX/RX are looped in Vivado block design
3. Verify LEDs connected to GPIO 0
4. Verify switches connected to GPIO 1

#### Step 2: Build & Run

1. Copy `uart_led_loopback.c` to your project `src` folder
2. Build in Vitis
3. Program FPGA and run application

#### Step 3: Open Terminal

```
PuTTY Settings:
- Connection Type: Serial
- COM Port: COM3 (or your port)
- Speed: 115200
- Data bits: 8, Stop bits: 1, Parity: None
```

#### Step 4: Commands

| Command | Description | Example |
|---------|-------------|---------|
| `led <hex>` | Set LED pattern | `led FF` (all on) |
| `test <text>` | Test loopback | `test hello` |
| `stream` | Stream switch data | `stream` |
| `stats` | Show statistics | `stats` |
| `mirror` | Mirror switches to LEDs | `mirror` |
| `status` | Show system status | `status` |
| `help` | Show command menu | `help` |

### Expected Output

```
======================================================
   LED Controller with Dual UARTLite Loopback
======================================================
Three UART interfaces:
  1. PS UART   - Command & Control (this terminal)
  2. UARTLite1 - Loopback transmitter
  3. UARTLite2 - Loopback receiver

Commands:
  led <0-255>   - Set LED pattern (hex)
  test <text>   - Send text to UARTLite loopback
  stream        - Stream switch data to loopback
  stats         - Show loopback statistics
  mirror        - Mirror switches to LEDs
  status        - Show system status
  help          - Show this menu
======================================================

Type 'test hello' to test loopback!
> test hello

--- Loopback Test ---
Sending: "hello" (5 bytes)
Waiting for data...
Received: "hello" (5 bytes)
Result: ✓ PASS - Loopback successful!
---------------------

> led AA

LED set to 0xAA

> stats

--- UARTLite Statistics ---
Bytes Sent:     5
Bytes Received: 5
Success Rate:   100.0%
---------------------------

> stream

--- Streaming Switch Data ---
Press any key to stop...

Packets: 10
Packets: 20
Packets: 30

Streaming stopped.

>
```

### LED Behavior

- **Test success**: All LEDs flash 3 times
- **Test failure**: LEDs flash error pattern (0x81)
- **Stream mode**: LEDs display current switch state
- **Mirror mode**: LEDs continuously mirror switches

### Technical Details

**Initialization:**
```c
// PS UART for commands
uart_ps_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
XUartPs_CfgInitialize(&uart_ps, uart_ps_config, uart_ps_config->BaseAddress);
XUartPs_SetBaudRate(&uart_ps, 115200);

// UARTLite1 (TX)
uart_config1 = XUartLite_LookupConfig(XPAR_XUARTLITE_0_BASEADDR);
XUartLite_CfgInitialize(&uart_lite1, uart_config1, uart_config1->RegBaseAddr);

// UARTLite2 (RX)
uart_config2 = XUartLite_LookupConfig(XPAR_XUARTLITE_1_BASEADDR);
XUartLite_CfgInitialize(&uart_lite2, uart_config2, uart_config2->RegBaseAddr);
```

**Loopback Test:**
```c
// Send via UARTLite1
XUartLite_Send(&uart_lite1, (u8*)text, len);
while (XUartLite_IsSending(&uart_lite1));

// Receive via UARTLite2
u32 byteRcvd = 0;
while (byteRcvd != len) {
    byteRcvd += XUartLite_Recv(&uart_lite2, &recv_buffer[byteRcvd], len);
}

// Verify data integrity
if (memcmp(text, recv_buffer, len) == 0) {
    // SUCCESS
}
```

---

## Example 2: Binary Quiz Game with UARTLite Score Transmission

### Description

An educational binary-to-decimal conversion game with score tracking transmitted via UARTLite loopback.

### Features

- **3 difficulty levels** (4-bit, 6-bit, 8-bit)
- **Progressive difficulty** based on performance
- **Streak counter** for consecutive correct answers
- **Score transmission** via UARTLite with verification
- **LED visual feedback** for questions and results

### How to Use

#### Step 1: Start Game

```
Type 'start' to begin!
> start
```

#### Step 2: Watch LEDs

The LEDs will display a binary number for 3 seconds.

Example: LEDs show `00010110` (binary)

#### Step 3: Calculate

Convert binary to decimal:
- `00010110` = 22 (decimal)

#### Step 4: Set Switches

Set switches to the binary representation:
- SW0 = 0, SW1 = 1, SW2 = 1, SW3 = 0, SW4 = 1, SW5 = 0, SW6 = 0, SW7 = 0
- This represents `00010110` = 22

#### Step 5: Check Answer

```
> check
```

### Expected Output

```
========================================================
  Binary Quiz Game with Dual UARTLite Score Loopback
========================================================
HOW TO PLAY:
1. Watch the LEDs show a BINARY number
2. Calculate the DECIMAL value
3. Set switches to that BINARY value
4. Type 'check' to verify

NEW: Scores transmitted & verified via UARTLite loopback!

Commands: start, check, hint, score, quit
========================================================

Type 'start' to begin!
> start

--- NEW QUESTION ---
Level 1 - Question #1 (Streak: 0)
Watch the LEDs!
Set switches and type 'check'
> check

===========================================
      ✓ CORRECT! Great job!
===========================================
Current Streak: 1

[UARTLite] Score transmitted & verified: SCORE:1/1,LVL:1,STREAK:1

Score: 1/1 (100.0%)

--- NEW QUESTION ---
Level 1 - Question #2 (Streak: 1)
Watch the LEDs!
Set switches and type 'check'
> check

===========================================
      ✓ CORRECT! Great job!
===========================================
Current Streak: 2

[UARTLite] Score transmitted & verified: SCORE:2/2,LVL:1,STREAK:2

Score: 2/2 (100.0%)

[Continue playing...]

> score

Score: 5/6 (83.3%) - Level 2 - Streak: 3

[UARTLite] Score transmitted & verified: SCORE:5/6,LVL:2,STREAK:3

> quit

Thanks for playing!
Final Score: 5/6

[UARTLite] Score transmitted & verified: SCORE:5/6,LVL:2,STREAK:3

Type 'start' to play again!
>
```

### LED Behavior

| Event | LED Pattern | Duration |
|-------|-------------|----------|
| **Question** | Binary value | 3 seconds |
| **Correct** | All ON (0xFF) | Flash 3× |
| **Incorrect** | 0x81 pattern | Flash 3× |
| **Level Up** | 0xAA pattern | Flash 5× |

### Game Levels

| Level | Range | Bits Used |
|-------|-------|-----------|
| **1** | 1-15 | 4 bits (SW0-SW3) |
| **2** | 1-63 | 6 bits (SW0-SW5) |
| **3** | 1-255 | 8 bits (SW0-SW7) |

**Level progression**: Every 5 correct answers advances to next level

### UARTLite Data Format

```
SCORE:5/6,LVL:2,STREAK:3
│     │ │ │   │      │
│     │ │ │   │      └─ Current streak
│     │ │ │   └──────── Current level
│     │ │ └──────────── Total questions
│     │ └────────────── Score
│     └──────────────── Data tag
```

### Commands

| Command | Function |
|---------|----------|
| `start` | Start new game |
| `check` | Verify answer |
| `hint` | Get range hint |
| `score` | Show current score |
| `quit` | End game |

---

## Example 3: Pattern Sequencer with UARTLite Data Export

### Description

Create and play custom LED animation sequences with real-time export via UARTLite loopback.

### Features

- **16-pattern sequence capacity**
- **Adjustable playback speed** (50-2000ms)
- **Loop mode** for continuous playback
- **4 preset patterns** included
- **Record mode** from switches
- **Real-time export** via UARTLite with verification

### How to Use

#### Method 1: Manual Pattern Entry

```
> add FF
Added: 0xFF (Length: 1)

> add 00
Added: 0x00 (Length: 2)

> add AA
Added: 0xAA (Length: 3)

> list

--- Current Sequence ---
Length: 3/16
Speed: 200 ms
Loop: ON
Export: OFF
Status: STOPPED

Patterns:
   1: 0xFF  ████████
   2: 0x00  ░░░░░░░░
   3: 0xAA  █░█░█░█░
------------------------

> play
Playing...
```

#### Method 2: Load Preset

```
> preset 1
Loading: Knight Rider

--- Current Sequence ---
Length: 14/16
Speed: 100 ms
Loop: ON
Export: OFF
Status: STOPPED

Patterns:
   1: 0x01  ░░░░░░░█
   2: 0x02  ░░░░░░█░
   3: 0x04  ░░░░░█░░
   [... continues ...]
  14: 0x02  ░░░░░░█░
------------------------

> play
Playing...
```

#### Method 3: Record from Switches

```
> record

=== RECORD MODE ===
Set switches, press SW0 to add pattern
Press SW7 to finish

Added #1: 0x18
Added #2: 0x24
Added #3: 0x42
Added #4: 0x81

Recording finished!

--- Current Sequence ---
Length: 4/16
[... sequence details ...]
```

### Expected Output

```
=========================================================
  Pattern Sequencer with Dual UARTLite Data Export
=========================================================
Create LED animations and export via UARTLite loopback!

Commands:
  add <hex>     - Add pattern to sequence
  list          - Show current sequence
  play          - Start playing sequence
  stop          - Stop playing
  clear         - Clear sequence
  speed <ms>    - Set speed (50-2000ms)
  loop          - Toggle loop mode
  export        - Toggle UARTLite export
  record        - Record from switches
  preset <1-4>  - Load preset pattern
  help          - Show this menu
=========================================================

> export
UARTLite Export: ON

> preset 1
Loading: Knight Rider
Sequence exported & verified via UARTLite loopback!

--- Current Sequence ---
Length: 14/16
Speed: 100 ms
Loop: ON
Export: ON
Status: STOPPED

[Pattern list...]

> play
Playing...

> stop
Stopped.

> speed 50
Speed: 50 ms

> play
Playing...
[Patterns play at 50ms intervals - much faster!]

> stop
Stopped.

>
```

### Available Presets

#### Preset 1: Knight Rider
```
14 patterns, 100ms speed
Effect: Single LED bouncing left-right
Patterns: 0x01→0x02→0x04→0x08→0x10→0x20→0x40→0x80→0x40→...→0x02
```

#### Preset 2: Binary Counter
```
16 patterns, 300ms speed
Effect: Counts from 0 to 15 in binary
Patterns: 0x00, 0x01, 0x02, 0x03, ... 0x0F
```

#### Preset 3: Alternating
```
2 patterns, 200ms speed
Effect: Alternating checkerboard
Patterns: 0xAA ↔ 0x55
```

#### Preset 4: Expanding Center
```
8 patterns, 150ms speed
Effect: Expands from center outward
Patterns: 0x18→0x3C→0x7E→0xFF→0x7E→0x3C→0x18→0x00
```

### UARTLite Export Format

When export is enabled, data is transmitted via UARTLite:

**Sequence Export:**
```
SEQ_START,LEN=14,SPEED=100
PATTERN[0]=0x01
PATTERN[1]=0x02
PATTERN[2]=0x04
...
PATTERN[13]=0x02
SEQ_END
```

**Playback Events:**
```
PLAY_START
NOW:0x01
NOW:0x02
NOW:0x04
...
PLAY_STOP
```

**Recording Events:**
```
REC[0]=0x18
REC[1]=0x24
REC[2]=0x42
REC[3]=0x81
```

### Commands Reference

| Command | Parameters | Description |
|---------|-----------|-------------|
| `add` | `<hex>` | Add pattern (0x00-0xFF) |
| `list` | none | Show current sequence |
| `play` | none | Start playback |
| `stop` | none | Stop playback |
| `clear` | none | Clear all patterns |
| `speed` | `<ms>` | Set speed (50-2000) |
| `loop` | none | Toggle loop mode |
| `export` | none | Toggle UARTLite export |
| `record` | none | Enter recording mode |
| `preset` | `<1-4>` | Load preset pattern |
| `help` | none | Show command menu |

### Recording Mode Controls

- **SW0**: Add current switch pattern
- **SW7**: Finish recording
- **SW1-SW6**: Pattern bits

---

## Common Code Patterns

### Dual UARTLite Initialization

```c
void uart_lite_init() {
    // Initialize UARTLite 1 (TX)
    uart_config1 = XUartLite_LookupConfig(XPAR_XUARTLITE_0_BASEADDR);
    int status1 = XUartLite_CfgInitialize(&uart_lite1, uart_config1,
                                          uart_config1->RegBaseAddr);

    // Initialize UARTLite 2 (RX)
    uart_config2 = XUartLite_LookupConfig(XPAR_XUARTLITE_1_BASEADDR);
    int status2 = XUartLite_CfgInitialize(&uart_lite2, uart_config2,
                                          uart_config2->RegBaseAddr);

    if ((status1 == XST_SUCCESS) && (status2 == XST_SUCCESS)) {
        xil_printf("[OK] UARTLite loopback initialized\n\r");
    }
}
```

### Loopback Send & Receive

```c
// Send data
u32 len = strlen(data);
XUartLite_Send(&uart_lite1, (u8*)data, len);
while (XUartLite_IsSending(&uart_lite1));  // Wait for completion

// Receive data
u8 recv_buffer[128];
u32 byteRcvd = 0;
while (byteRcvd != len) {
    byteRcvd += XUartLite_Recv(&uart_lite2, &recv_buffer[byteRcvd], len);
}

// Verify
if (memcmp(data, recv_buffer, len) == 0) {
    // Data verified successfully
}
```

### GPIO Operations

```c
// Initialize
XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR);
XGpio_Initialize(&sw, XPAR_XGPIO_1_BASEADDR);

// Set directions
XGpio_SetDataDirection(&led, 1, 0x00);  // All outputs
XGpio_SetDataDirection(&sw, 1, 0xFF);   // All inputs

// Read/Write
u8 sw_val = XGpio_DiscreteRead(&sw, 1);
XGpio_DiscreteWrite(&led, 1, sw_val);
```

---

## Troubleshooting

### Problem: "UARTLite init failed"

**Causes:**
1. UARTLite not present in hardware design
2. Wrong base addresses
3. BSP not regenerated

**Solutions:**
```bash
# Check xparameters.h
grep XUARTLITE xparameters.h

# Should show:
# XPAR_XUARTLITE_0_BASEADDR
# XPAR_XUARTLITE_1_BASEADDR

# If not found, regenerate platform in Vitis
```

### Problem: Loopback test fails

**Causes:**
1. UARTLite TX/RX not connected in Vivado
2. Baud rate mismatch
3. Data corruption

**Solutions:**
1. Verify Vivado block design connections
2. Regenerate bitstream
3. Check statistics: `stats` command

### Problem: LEDs don't respond

**Causes:**
1. Wrong GPIO base address
2. Direction not set correctly
3. Hardware not programmed

**Solutions:**
```c
// Verify GPIO initialization
if (XGpio_Initialize(&led, XPAR_XGPIO_0_BASEADDR) == XST_SUCCESS) {
    xil_printf("GPIO LED init OK\n\r");
}

// Force LED test
XGpio_DiscreteWrite(&led, 1, 0xFF);  // All ON
sleep(1);
XGpio_DiscreteWrite(&led, 1, 0x00);  // All OFF
```

### Problem: Switches read incorrectly

**Causes:**
1. Active-HIGH vs Active-LOW configuration
2. Pull-up/pull-down settings
3. Wrong GPIO instance

**Solutions:**
```c
// Test switch reading
u8 sw_val = XGpio_DiscreteRead(&sw, 1);
xil_printf("Switches: 0x%02X\n\r", sw_val);

// If inverted, use:
sw_val = ~sw_val;  // Invert all bits
```

### Problem: PS UART garbled output

**Causes:**
1. Wrong baud rate
2. Wrong COM port
3. Cable issue

**Solutions:**
```
Terminal Settings:
- Baud: 115200
- Data: 8
- Stop: 1
- Parity: None
- Flow Control: None
```

---

## Performance Notes

### UARTLite Throughput

- **Baud Rate**: Configured in Vivado (typically 9600 or 115200)
- **Latency**: ~1-10ms for small packets
- **Reliability**: 100% in loopback configuration

### Timing Considerations

```c
// Good: Non-blocking check
if (XUartPs_Recv(&uart_ps, &byte, 1) == 1) {
    // Process byte
}

// Bad: Blocking wait (hangs program)
XUartPs_Recv(&uart_ps, &byte, 1);  // Waits forever if no data!
```

### Memory Usage

| Example | Flash | RAM |
|---------|-------|-----|
| LED Controller | ~15 KB | ~2 KB |
| Binary Game | ~18 KB | ~3 KB |
| Pattern Sequencer | ~20 KB | ~4 KB |

---

## Building Projects

### Method 1: Replace Main File

```bash
# Navigate to project
cd /path/to/project/src/

# Backup original
mv helloworld.c helloworld.c.bak

# Copy example
cp uart_led_loopback.c helloworld.c

# Build in Vitis
```

### Method 2: New Application

1. In Vitis: **New → Application Component**
2. Name: `led_controller` (or other name)
3. Platform: Use existing platform with UARTLite
4. Copy example code to `src` folder
5. Build and run

---

## Hardware Setup Guide

### Vivado Block Design Requirements

**Required IP Blocks:**
1. **Zynq PS** with UART enabled
2. **AXI UARTLite (×2)** - for loopback
3. **AXI GPIO (×2)** - for LEDs and switches

**Connections:**
```
Zynq PS:
  M_AXI_GP0 → AXI Interconnect

AXI Interconnect:
  M_AXI[0] → UARTLite_0
  M_AXI[1] → UARTLite_1
  M_AXI[2] → GPIO_0 (LEDs)
  M_AXI[3] → GPIO_1 (Switches)

External Connections:
  GPIO_0 → 8 LEDs (LD0-LD7)
  GPIO_1 → 8 Switches (SW0-SW7)
  UARTLite_0.TX → UARTLite_1.RX (loopback)
```

### Address Assignment Example

```
0x41200000 - GPIO LEDs
0x41210000 - GPIO Switches
0x42C00000 - UARTLite_0 (TX)
0x42C10000 - UARTLite_1 (RX)
0xE0001000 - PS UART1
```

---

## Advanced Features

### Adding Pattern Animation

```c
void animate_pattern(u8 *patterns, u32 count, u32 delay_ms) {
    for (u32 i = 0; i < count; i++) {
        XGpio_DiscreteWrite(&led, 1, patterns[i]);
        usleep(delay_ms * 1000);
    }
    XGpio_DiscreteWrite(&led, 1, 0x00);
}

// Usage
u8 startup[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
animate_pattern(startup, 8, 100);
```

### Adding Sound Effects

If you have a buzzer on GPIO:

```c
void beep(u32 duration_ms) {
    XGpio_DiscreteSet(&gpio_buzzer, 1, 0x01);
    usleep(duration_ms * 1000);
    XGpio_DiscreteClear(&gpio_buzzer, 1, 0x01);
}

// Usage
beep(100);  // Short beep
```

### Data Logging

```c
typedef struct {
    u32 timestamp;
    u8 pattern;
    u8 switches;
} LogEntry;

LogEntry log[100];
u32 log_index = 0;

void log_state(u8 pattern, u8 switches) {
    if (log_index < 100) {
        log[log_index].timestamp = get_time();
        log[log_index].pattern = pattern;
        log[log_index].switches = switches;
        log_index++;
    }
}
```

---

## Testing Checklist

### Example 1: LED Controller

- [ ] PS UART connects successfully
- [ ] UARTLite loopback initializes
- [ ] GPIOs initialize successfully
- [ ] `test hello` passes
- [ ] LEDs respond to `led` command
- [ ] Switches detected in `status`
- [ ] Stream mode works
- [ ] Statistics accurate
- [ ] Mirror mode functional

### Example 2: Binary Game

- [ ] Game starts with `start`
- [ ] LEDs show binary numbers
- [ ] Correct answers detected
- [ ] Incorrect answers handled
- [ ] Score transmitted via UARTLite
- [ ] Loopback verification works
- [ ] Level progression works
- [ ] Streak counter updates
- [ ] LED animations display

### Example 3: Pattern Sequencer

- [ ] Patterns can be added
- [ ] Sequence displays correctly
- [ ] Playback works
- [ ] Speed adjustment works
- [ ] Loop mode toggles
- [ ] Export transmits data
- [ ] Loopback verification passes
- [ ] Presets load correctly
- [ ] Recording mode captures switches

---

## Summary Comparison

| Feature | Example 1 | Example 2 | Example 3 |
|---------|-----------|-----------|-----------|
| **Primary Function** | LED Control | Quiz Game | Sequencer |
| **Interaction** | Command-based | Question/Answer | Pattern Creation |
| **UARTLite Use** | Data streaming | Score logging | Sequence export |
| **Switch Use** | Monitoring | Answer input | Recording |
| **LED Use** | Display/Status | Question/Feedback | Pattern playback |
| **Complexity** | Medium | Medium | High |
| **Educational Value** | Data comms | Binary math | Timing/Animation |
| **Best For** | Testing | Learning | Creative |

---

## Additional Resources

### Related Documentation

- `MULTIPLE_GPIO_PITFALLS.md` - GPIO SDT mode issues
- `XPAR_NAMING_GUIDE.md` - Understanding base addresses
- `VITIS_UART_TERMINAL_GUIDE.md` - Terminal setup
- `UART_EXAMPLES_GUIDE.md` - Basic UART examples

### Reference Projects

- `uart_axi_uartlite` - Base loopback implementation
- `dual_channel_axi_gpio` - GPIO dual-channel example
- `multiple_instance_axi_gpio` - Multi-GPIO example

### Xilinx Documentation

- **UG585**: Zynq-7000 Technical Reference Manual
- **PG142**: AXI UARTLite Product Guide
- **PG144**: AXI GPIO Product Guide
- **UG1400**: Vitis Unified Software Platform

---

## Quick Start Checklist

- [ ] Hardware design has 2× UARTLite instances
- [ ] UARTLite TX/RX connected in block design
- [ ] Platform regenerated with correct addresses
- [ ] BSP includes GPIO and UARTLite drivers
- [ ] Example code copied to `src` folder
- [ ] Project builds without errors
- [ ] FPGA programmed with bitstream
- [ ] PS UART terminal connected (115200 baud)
- [ ] Application running on hardware

---

**Document Version:** 1.0
**Created:** 2026-04-20
**Author:** Claude AI Assistant
**Target:** ZedBoard / Vitis 2023.2
**Based on:** uart_axi_uartlite reference implementation
