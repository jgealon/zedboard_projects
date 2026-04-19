# How to Launch UART Terminal in Vitis 2023.2

## Quick Method - Using Vitis Serial Terminal

### Step 1: Open Vitis Serial Terminal

**Option A: Via Menu**
```
Window → Show View → Vitis Serial Terminal
```

**Option B: Via Icon**
- Look for the terminal icon (looks like a monitor/screen) in the toolbar
- Click the dropdown arrow next to it
- Select "Vitis Serial Terminal"

### Step 2: Configure Serial Connection

1. Click the **"+"** (Add Connection) button in the Serial Terminal view

2. Configure settings:
   ```
   Port: COM<X>  (e.g., COM3, COM4)
   Baud Rate: 115200
   Data Bits: 8
   Stop Bits: 1
   Parity: None
   Flow Control: None
   ```

3. Click **OK**

### Step 3: Connect

- Click the **Connect** button (green plug icon)
- The terminal should show "Connected"

---

## Alternative Method - Using External Terminal (PuTTY)

### Why Use PuTTY?

- ✅ More reliable than Vitis terminal
- ✅ Better scrollback buffer
- ✅ Session saving
- ✅ Easier to clear screen

### Step 1: Find COM Port

**Method A: Windows Device Manager**
1. Press `Win + X`, select "Device Manager"
2. Expand "Ports (COM & LPT)"
3. Look for "USB Serial Port (COM<X>)" or "Silicon Labs CP210x USB to UART Bridge (COM<X>)"
4. Note the COM port number (e.g., COM3)

**Method B: Vitis**
1. In Vitis, go to: `Window → Show View → Vitis Serial Terminal`
2. Click the "+" button
3. The dropdown will show available COM ports

### Step 2: Launch PuTTY

1. Open PuTTY application
2. Configure settings:
   ```
   Connection Type: Serial
   Serial line: COM3  (your COM port)
   Speed: 115200
   ```

3. Click **Open**

### Step 3: Save Session (Optional)

1. In PuTTY configuration window
2. Under "Saved Sessions", enter name: `ZedBoard_UART`
3. Click **Save**
4. Next time, just select and click **Load**

---

## Finding the Correct COM Port

### Method 1: Check Before/After Plugging

1. Open Device Manager
2. Note current COM ports
3. Plug in your ZedBoard USB cable
4. Watch for new COM port to appear
5. That's your board!

### Method 2: Check Cable Label

- ZedBoard has **two USB ports**:
  - **USB-UART** (J14) - This one!
  - **USB-JTAG** (J17) - For programming

- Make sure you're connected to **J14 (USB-UART)**

### Method 3: Test Each Port

If unsure, try each COM port in PuTTY:
1. Connect with 115200 baud
2. Reset the board (or send data from application)
3. If you see output → correct port!
4. If nothing → try next port

---

## Vitis Serial Terminal Features

### Terminal Controls

| Button | Function |
|--------|----------|
| **+ (Add)** | Add new serial connection |
| **🔌 (Connect)** | Connect to selected port |
| **🔌❌ (Disconnect)** | Disconnect from port |
| **🗑️ (Remove)** | Remove connection from list |
| **⚙️ (Settings)** | Change port settings |
| **🧹 (Clear)** | Clear terminal screen |
| **📋 (Copy)** | Copy selected text |

### Multiple Connections

You can have multiple serial terminals open simultaneously:
1. Click **+** to add another connection
2. Configure different COM port or baud rate
3. Each connection appears in tabs

---

## Common Settings

### ZedBoard Default Settings

```
Baud Rate: 115200
Data Bits: 8
Stop Bits: 1
Parity: None
Flow Control: None
```

### Different Baud Rates for Different Applications

| Application | Typical Baud Rate |
|------------|------------------|
| System boot messages | 115200 |
| Custom UART app | 9600 (or as configured) |
| High-speed data | 921600 |

**Important:** Your application and terminal **must match** baud rates!

---

## Verifying UART Connection

### Test 1: Boot Messages

1. Connect terminal
2. Reset ZedBoard (press RESET button)
3. You should see:
   ```
   Xilinx Zynq MP First Stage Boot Loader
   Release 2023.2
   ...
   ```

If you see garbage characters → **wrong baud rate**

### Test 2: Simple Echo Test

Add to your code:
```c
#include "xil_printf.h"

int main() {
    init_platform();

    xil_printf("Hello from ZedBoard!\n\r");
    xil_printf("UART is working!\n\r");

    cleanup_platform();
    return 0;
}
```

Run the application and check terminal for output.

---

## Troubleshooting

### Problem 1: No Output in Terminal

**Possible Causes:**
1. ❌ Wrong COM port selected
2. ❌ Wrong baud rate
3. ❌ USB cable not connected
4. ❌ Board not powered on
5. ❌ Application not running

**Solutions:**
```c
// In your code, add diagnostic:
xil_printf("=== UART TEST ===\n\r");
xil_printf("If you see this, UART works!\n\r");
```

### Problem 2: Garbage Characters

**Cause:** Baud rate mismatch

**Solutions:**
1. Check your code for `XUartPs_SetBaudRate()`
2. Verify terminal is set to 115200 (default)
3. Try other common rates: 9600, 38400, 57600, 115200

### Problem 3: "Port in Use" Error

**Cause:** Another program is using the COM port

**Solutions:**
1. Close other terminals (PuTTY, TeraTerm, etc.)
2. In Vitis, disconnect from port
3. Unplug/replug USB cable
4. Restart Vitis

### Problem 4: COM Port Not Listed

**Cause:** Driver not installed

**Solutions:**
1. Install Silicon Labs CP210x driver
2. Download from: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
3. Restart computer
4. Check Device Manager

### Problem 5: Terminal Stops Responding

**Solutions:**
1. Disconnect and reconnect
2. Click the **Clear** button
3. Close and reopen terminal view
4. Restart Vitis if persistent

---

## Using Multiple UART Instances

If your design has multiple UARTs (UART0, UART1):

### In Vivado
- Assign different MIO pins to each UART
- Export hardware

### In Vitis
- Each UART will appear as a different COM port
- Connect separate terminals to each

### Example: Two UARTs
```c
XUartPs uart0, uart1;

// Initialize UART0 (default debug UART)
XUartPs_Initialize(&uart0, XPAR_XUARTPS_0_BASEADDR);
XUartPs_SetBaudRate(&uart0, 115200);  // Terminal on COM3

// Initialize UART1 (custom data UART)
XUartPs_Initialize(&uart1, XPAR_XUARTPS_1_BASEADDR);
XUartPs_SetBaudRate(&uart1, 9600);    // Terminal on COM4
```

---

## Sending/Receiving Data

### Sending Data (printf style)

```c
#include "xil_printf.h"

xil_printf("Hello World\n\r");
xil_printf("Counter: %d\n\r", count);
xil_printf("Hex: 0x%08X\n\r", value);
```

**Note:** Use `\n\r` (newline + carriage return) for proper line breaks!

### Sending Data (direct UART)

```c
#include "xuartps.h"

XUartPs uart;
char *msg = "Hello UART!\n\r";

// Send string
XUartPs_Send(&uart, (u8*)msg, strlen(msg));
```

### Receiving Data

```c
u8 recv_buffer[32];
u32 bytes_received;

// Check if data available
bytes_received = XUartPs_Recv(&uart, recv_buffer, 32);

if (bytes_received > 0) {
    xil_printf("Received: %s\n\r", recv_buffer);
}
```

### Echo Example

```c
int main() {
    u8 recv_byte;

    init_platform();

    xil_printf("Echo Test - Type something:\n\r");

    while(1) {
        // Wait for character
        if (XUartPs_Recv(&uart, &recv_byte, 1) == 1) {
            // Echo it back
            XUartPs_Send(&uart, &recv_byte, 1);
        }
    }

    cleanup_platform();
    return 0;
}
```

---

## Keyboard Shortcuts (Vitis Terminal)

| Shortcut | Action |
|----------|--------|
| `Ctrl + C` | Copy selected text |
| `Ctrl + V` | Paste text (to send to board) |
| `Ctrl + L` | Clear terminal |
| `Ctrl + F` | Find text |

---

## Best Practices

### 1. Always Use `\n\r` for Line Breaks

❌ **WRONG:**
```c
xil_printf("Hello\n");  // Only newline
```

✅ **CORRECT:**
```c
xil_printf("Hello\n\r");  // Newline + carriage return
```

### 2. Add Diagnostic Messages

```c
void gpio_init() {
    xil_printf("\n=== GPIO Init Start ===\n\r");

    int status = XGpio_Initialize(&gpio, XPAR_XGPIO_0_BASEADDR);

    if (status == XST_SUCCESS)
        xil_printf("[OK] GPIO initialized\n\r");
    else
        xil_printf("[FAIL] GPIO init error: %d\n\r", status);
}
```

### 3. Use Formatted Output

```c
xil_printf("Switch Value: 0x%02X (%d)\n\r", sw_val, sw_val);
//                        ^hex    ^decimal
```

### 4. Flush Output (if needed)

```c
#include "xuartps.h"

xil_printf("Critical message\n\r");
// Make sure it's sent before continuing:
// (Usually not needed with xil_printf)
```

### 5. Set Baud Rate Early

```c
int main() {
    init_platform();

    // Set baud rate FIRST
    XUartPs_SetBaudRate(&uart, 9600);

    // Then send messages
    xil_printf("UART configured to 9600 baud\n\r");

    // Rest of code...
}
```

---

## Quick Start Checklist

- [ ] Connect USB cable to J14 (USB-UART) on ZedBoard
- [ ] Open Vitis Serial Terminal (Window → Show View → Vitis Serial Terminal)
- [ ] Add connection with settings:
  - Baud: 115200
  - Data: 8
  - Stop: 1
  - Parity: None
- [ ] Find correct COM port in Device Manager
- [ ] Connect terminal
- [ ] Reset board or run application
- [ ] Verify output appears

---

## Additional Resources

### COM Port Drivers
- Silicon Labs CP210x: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- FTDI Drivers: https://ftdichip.com/drivers/vcp-drivers/

### Terminal Alternatives
- **PuTTY**: https://www.putty.org/
- **TeraTerm**: https://ttssh2.osdn.jp/
- **RealTerm**: https://realterm.sourceforge.io/
- **CoolTerm**: https://freeware.the-meiers.org/

### ZedBoard Documentation
- ZedBoard User Guide: https://digilent.com/reference/programmable-logic/zedboard/start
- Zynq-7000 TRM: https://www.xilinx.com/support/documentation/user_guides/ug585-Zynq-7000-TRM.pdf

---

**Document Version:** 1.0
**Created:** 2026-04-19
**Author:** Claude AI Assistant
**Target:** Vitis 2023.2 / ZedBoard
