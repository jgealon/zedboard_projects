# Vitis Memory Debugging Guide
## Complete Guide to Memory Monitoring in Debug Sessions
**Vitis Unified IDE 2023.2**

**Last Updated**: 2026-05-02

---

## Table of Contents

1. [Overview](#overview)
2. [Memory View (Primary Method)](#memory-view-primary-method)
3. [Variables View with Array Inspection](#variables-view-with-array-inspection)
4. [Expressions View](#expressions-view)
5. [Memory Browser (Advanced)](#memory-browser-advanced)
6. [Memory Watchpoints](#memory-watchpoints)
7. [Practical Examples](#practical-examples)
8. [Export/Import Memory](#exportimport-memory)
9. [Tips & Best Practices](#tips--best-practices)
10. [Keyboard Shortcuts Reference](#keyboard-shortcuts-reference)
11. [Troubleshooting](#troubleshooting)

---

## Overview

Vitis provides multiple ways to monitor and inspect memory during debug sessions:

| Feature | Use Case | Real-time | Best For |
|---------|----------|-----------|----------|
| **Memory View** | View raw memory contents | Yes | General memory inspection |
| **Variables View** | Inspect variables & pointers | Yes | Variable-based debugging |
| **Expressions View** | Custom expressions & casts | Yes | Complex data structures |
| **Memory Browser** | Multiple regions, export/import | Yes | Advanced analysis |
| **Watchpoints** | Break on memory access | Yes | Finding memory corruption |

---

## Memory View (Primary Method)

The Memory View is the most versatile tool for memory debugging.

### Opening Memory View

**Method 1: From Menu**
1. **Window** → **Show View** → **Memory**

**Method 2: From Debug Perspective**
- If in Debug perspective, Memory tab is usually visible in bottom panel
- If not visible: **Window** → **Show View** → **Memory**

**Method 3: Quick Access**
- Press **Ctrl+3**
- Type "Memory"
- Select **Memory** from list

**Method 4: From Code Editor (During Debug)**
1. Right-click on a variable in your source code
2. Select **Inspect Memory** or **View Memory**
3. Memory View opens automatically at that variable's address
4. No need to manually type the address!

**Method 5: From View Menu (During Debug)**
1. **View** → **Memory Inspector**
2. OR **Window** → **Memory Inspector**
3. Opens Memory View with default configuration

### Adding a Memory Monitor

**Step 1: Click Add Monitor Button**
- Look for **+** (Add Memory Monitor) button in Memory View toolbar
- OR right-click in Memory View → **Add Memory Monitor**

**Step 2: Enter Address**

You can specify address in multiple formats:

```
Hex with 0x prefix:   0x40000000
Hex without prefix:   40000000
Decimal:              1073741824
Expression:           baseAddr
Variable:             &myArray[0]
Register:             $PC
```

**Step 3: Set Length (Optional)**
- Enter number of bytes to monitor
- Default: Shows entire memory region
- Example: `200` for 50 integers (50 × 4 bytes)

### Configuring Display Format

**Right-click in Memory View to access options:**

#### Format Options

| Format | Display | Best For |
|--------|---------|----------|
| **Hex** | `0x00 0x05 0x0A` | General purpose (default) |
| **Decimal** | `0 5 10` | Numeric data, counters |
| **Binary** | `00000000 00000101` | Bit manipulation |
| **ASCII** | `...Hello` | String data |
| **Signed Decimal** | `-10 -5 0 5 10` | Signed integers |
| **Unsigned Decimal** | `0 5 10 15` | Unsigned integers |
| **Float** | `3.14 2.71` | Floating point data |

#### Cell Size (Word Width)

Right-click → **Cell Size**:

| Size | Use For |
|------|---------|
| **1 Byte** | `char`, `uint8_t`, byte arrays |
| **2 Bytes** | `short`, `uint16_t` |
| **4 Bytes** | `int`, `uint32_t`, `float` (recommended) |
| **8 Bytes** | `long long`, `uint64_t`, `double` |

#### Row Size (Columns)

Right-click → **Row Size**:
- **4 columns**: Good for viewing 4-byte words
- **8 columns**: Balanced view
- **16 columns**: Hex dump style (traditional)
- **Custom**: Enter any number

### Example: Monitoring BRAM Memory

```c
// Your code
#define BRAM_BASE 0x40000000
volatile int *baseAddr = (int *)BRAM_BASE;

for(int i = 0; i < 50; i++) {
    baseAddr[i] = 5 * i;
}
```

**Debug Session:**

1. **Start Debug** (Right-click app → Debug As → Launch Hardware)

2. **Open Memory View**

3. **Add Monitor:**
   - Click **+**
   - Enter: `0x40000000` or `baseAddr`
   - Click **OK**

4. **Configure Display:**
   - Right-click → **Format** → **Decimal**
   - Right-click → **Cell Size** → **4 Bytes**
   - Right-click → **Row Size** → **8 columns**

5. **Step Through Code:**
   - Set breakpoint in loop
   - Press **F6** (Step Over) repeatedly
   - Watch memory update in real-time

**Expected Memory View:**

```
Address    | +0  +4  +8   +12  +16  +20  +24  +28
-----------+----------------------------------------
0x40000000 |  0   5  10   15   20   25   30   35
0x40000020 | 40  45  50   55   60   65   70   75
0x40000040 | 80  85  90   95  100  105  110  115
...
```

### Auto-Refresh Feature

**Enable Auto-Refresh:**
1. Right-click in Memory View
2. Check **☑ Auto Refresh**
3. Memory updates automatically when:
   - Stepping through code (F5, F6, F7)
   - Hitting breakpoints
   - Pausing execution

**Manual Refresh:**
- Click **Refresh** button (circular arrow icon)
- OR press **F5** while focused on Memory View

### Multiple Memory Monitors

**Monitor Multiple Regions Simultaneously:**

1. Click **+** to add first monitor: `0x40000000` (BRAM)
2. Click **+** to add second monitor: `0x00100000` (DDR)
3. Click **+** to add third monitor: `0xFFFF0000` (OCM)

**Each monitor appears as a separate tab:**
- Tab name shows address
- Click tab to switch views
- Right-click tab → **Rename** for descriptive names

**Organizing Monitors:**
- Drag tabs to reorder
- Right-click tab → **Close** to remove
- **Close All** removes all monitors

---

## Variables View with Array Inspection

The Variables View provides context-aware memory inspection.

### Basic Variable Inspection

**Automatic Display:**
- When stopped at breakpoint, Variables View shows:
  - Local variables
  - Function parameters
  - Global variables (if accessed)
  - Register values

**Expanding Structures:**
- Click **▶** to expand:
  - Structs
  - Arrays
  - Pointers (shows dereferenced value)

### Viewing Memory from Variables

**Method 1: View Memory (from Variables View)**

1. In Variables View, right-click on a pointer variable (e.g., `baseAddr`)
2. Select **View Memory** or **Inspect Memory**
3. Memory View opens at that address automatically
4. No need to manually type address!

**Method 1B: View Memory (from Code Editor)**

1. In source code editor, right-click directly on a variable name
2. Select **Inspect Memory**
3. Works for:
   - Pointers: `baseAddr`
   - Arrays: `buffer[0]`
   - Struct members: `BramInstance.ConfigPtr`
4. Memory View opens at variable's address
5. **Most convenient method!**

**Method 2: Display as Array**

For pointer variables:

1. Right-click pointer variable (e.g., `baseAddr`)
2. Select **Display As Array**
3. Enter array size: `50`
4. Click **OK**

Variables View now shows:
```
▼ baseAddr = 0x40000000
  ▶ [0] = 0
  ▶ [1] = 5
  ▶ [2] = 10
  ...
  ▶ [49] = 245
```

**Method 3: Change Value Format**

Right-click on variable → **Format**:
- **Natural** - Default format
- **Decimal** - Show as decimal
- **Hex** - Show as hexadecimal
- **Binary** - Show as binary
- **Octal** - Show as octal

### Inspecting Complex Data Structures

**Example: Monitoring BRAM Instance**

```c
XBram BramInstance;

// After initialization
XBram_CfgInitialize(&BramInstance, ConfigPtr, ConfigPtr->CtrlBaseAddress);
```

**In Variables View:**
```
▼ BramInstance
  ▶ ConfigPtr = 0x...
    ▶ BaseAddress = 0x40000000
    ▶ MemoryLength = 8192
    ▶ DataWidth = 32
    ▶ EccPresent = 0
    ...
  ▶ IsReady = 1
```

**To View Underlying Memory:**
- Right-click `BramInstance` → **View Memory**
- Shows raw memory representation of struct

---

## Expressions View

The Expressions View allows custom expressions and type casting.

### Opening Expressions View

1. **Window** → **Show View** → **Expressions**
2. OR in Debug perspective, look for **Expressions** tab

### Adding Expressions

**Click green + icon** or right-click → **Add New Expression**

#### Useful Expression Types

**1. View Memory as Array:**
```c
// Cast address to array pointer
*(int(*)[50])0x40000000

// View as char array
*(char(*)[200])0x40000000

// View as structure array
*(MyStruct(*)[10])0x40000000
```

**2. Pointer Arithmetic:**
```c
// Access specific element
*(baseAddr + 10)

// Range of elements
baseAddr[0] @ 50   // GDB syntax for array of 50 elements
```

**3. Structure Field Access:**
```c
// Access specific field
BramInstance.ConfigPtr->BaseAddress

// Multiple fields
{BramInstance.IsReady, BramInstance.ConfigPtr}
```

**4. Mathematical Expressions:**
```c
// Compare values
baseAddr[0] == 0

// Compute offset
baseAddr + (i * sizeof(int))

// Check range
(baseAddr[10] >= 0 && baseAddr[10] <= 100)
```

### Expression Features

**Persistent Across Debug Sessions:**
- Expressions are saved
- Automatically evaluated when stopped at breakpoint

**Real-Time Updates:**
- Values update when stepping
- Shows **<optimized out>** if variable optimized away
- Shows **<unavailable>** if out of scope

**Format Options:**
- Right-click expression → **Format**
- Same format options as Variables View

### Practical Example: Memory Viewer Application

```c
volatile int *baseAddr = (int *)0x40000000;
int i = 0;

for(i = 0; i < 50; i++) {
    *(baseAddr + i) = 5*i;
}
```

**Add These Expressions:**

| Expression | Shows |
|------------|-------|
| `*(int(*)[50])baseAddr` | All 50 values as array |
| `*(baseAddr + i)` | Current element being written |
| `i` | Loop counter |
| `baseAddr[0]` | First element |
| `baseAddr[49]` | Last element |

**During Debug:**
- All expressions update as you step
- Easy to verify write operations
- Compare expected vs actual values

---

## Memory Browser (Advanced)

Memory Browser provides enhanced features for complex debugging.

### Opening Memory Browser

1. **Window** → **Show View** → **Other...**
2. Expand **Debug**
3. Select **Memory Browser**
4. Click **OK**

### Memory Browser Features

#### 1. Multiple Panes

- Split view with multiple memory regions
- Compare different memory areas side-by-side
- Synchronize scrolling across panes

#### 2. Search Functionality

**Search for Values:**
1. Click **Search** button (magnifying glass icon)
2. Enter search value: `0x0000000A` (decimal 10)
3. Select:
   - **Search Forward** or **Search Backward**
   - **Case Sensitive** (for ASCII)
   - **Wrap Around**
4. Click **Find Next**

**Useful Searches:**
- Find specific pattern in memory
- Locate string in BRAM
- Find all occurrences of value

#### 3. Go to Address

**Quick Navigation:**
1. Press **Ctrl+G** in Memory Browser
2. Enter address: `0x40000000`
3. Press **Enter**

**Bookmark Addresses:**
- Right-click address → **Add Bookmark**
- Access from Bookmarks View
- Jump back to important addresses quickly

#### 4. Memory Comparison

**Compare Two Regions:**

1. Open two memory panes
2. Pane 1: Address `0x40000000` (BRAM)
3. Pane 2: Address `0x00100000` (DDR backup)
4. **View** → **Compare Memory**
5. Differences highlighted in color

**Use Case:** Verify memory copy operations

#### 5. Memory Fill

**Fill Memory with Pattern:**

1. Right-click in Memory Browser
2. **Fill Memory**
3. Enter:
   - **Start Address**: `0x40000000`
   - **Length**: `200` (bytes)
   - **Pattern**: `0xAA` or `0xDEADBEEF`
4. Click **OK**

**Warning:** This modifies memory! Use carefully.

---

## Memory Watchpoints

Watchpoints break execution when memory is accessed.

### Hardware vs Software Watchpoints

| Type | Count | Speed | Scope |
|------|-------|-------|-------|
| **Hardware** | 2-4 on Zynq | Fast | Specific address |
| **Software** | Unlimited | Slow | Page-level |

### Adding a Watchpoint

**Method 1: From Memory View**

1. Right-click on specific address in Memory View
2. **Add Watchpoint**
3. Configure:
   - **Access Type**:
     - **Read** - Break when memory is read
     - **Write** - Break when memory is written
     - **Access** - Break on read OR write
   - **Range**: 1 byte, 4 bytes, etc.
4. Click **OK**

**Method 2: From Breakpoints View**

1. **Window** → **Show View** → **Breakpoints**
2. Click **Add Watchpoint** button
3. Enter expression: `*(int*)0x40000000`
4. Configure access type
5. Click **OK**

### Watchpoint Example

**Detect Corruption of BRAM:**

```c
volatile int *baseAddr = (int *)0x40000000;

// Initialize
baseAddr[0] = 100;

// ... lots of code ...

// Somewhere, accidental write:
someFunction();  // Bug: overwrites baseAddr[0]
```

**Debug Strategy:**

1. Set watchpoint on `0x40000000`
2. Type: **Write**
3. Run program
4. Execution stops at line that modifies `baseAddr[0]`
5. Check call stack to find culprit

### Managing Watchpoints

**Breakpoints View:**
- Shows all watchpoints with ⓦ icon
- Enable/disable with checkbox
- Right-click → **Properties** to modify
- Double-click to jump to code

**Watchpoint Properties:**
- Hit count (break after N hits)
- Condition (break only if condition true)
- Action (run script when hit)

---

## Practical Examples

### Example 1: BRAM Memory Viewer Application

**Application Code:**

```c
#include "xil_printf.h"
#include "xparameters.h"
#include "xbram.h"
#include "xstatus.h"
#include "platform.h"

XBram BramInstance;

void bram_init() {
    XBram_Config *ConfigPtr = XBram_LookupConfig(XPAR_AXI_BRAM_CTRL_0_DEVICE_ID);
    if (ConfigPtr == NULL) {
        xil_printf("ERROR: BRAM LookupConfig failed\r\n");
        return;
    }

    int Status = XBram_CfgInitialize(&BramInstance, ConfigPtr,
                                      ConfigPtr->CtrlBaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: BRAM CfgInitialize failed\r\n");
        return;
    }

    xil_printf("BRAM initialized successfully\r\n");
}

int main() {
    init_platform();

    bram_init();

    volatile int *baseAddr = (int *)XPAR_AXI_BRAM_CTRL_0_BASEADDR;

    // ← SET BREAKPOINT HERE

    // Write test pattern
    for (int i = 0; i < 50; i++) {
        *(baseAddr + i) = 5 * i;
    }

    // ← SET BREAKPOINT HERE

    // Verify and display
    xil_printf("\r\nBRAM Memory Contents:\r\n");
    for (int i = 0; i < 10; i++) {
        xil_printf("BRAM[%d] = %d\r\n", i, baseAddr[i]);
    }

    cleanup_platform();
    return 0;
}
```

**Debug Session Steps:**

**1. Setup Debug Configuration:**
- Right-click app → **Debug As** → **Launch on Hardware**
- OR use existing debug configuration

**2. Set Breakpoints:**
- Line before `for` loop (line marked above)
- Line after `for` loop

**3. Start Debug:**
- Click **Debug** button
- Program loads and stops at `main()`

**4. Open Views:**
- **Window** → **Show View** → **Memory**
- **Window** → **Show View** → **Variables**
- **Window** → **Show View** → **Expressions**

**5. Configure Memory View:**
- Click **+** to add monitor
- Enter address: `0x40000000` (or type `baseAddr`)
- Right-click → **Format** → **Decimal**
- Right-click → **Cell Size** → **4 Bytes**
- Right-click → **Row Size** → **8**

**6. Add Expressions:**
- In Expressions View, click **+**
- Add: `*(int(*)[50])baseAddr`
- Add: `i`
- Add: `baseAddr[0]`

**7. Run to First Breakpoint:**
- Press **F8** (Resume)
- Stops before loop

**8. Step Through Loop:**
- Press **F6** (Step Over) repeatedly
- Watch in Memory View:
  - Values appear incrementally
  - Address 0x40000000: 0, 5, 10, 15...
- Watch in Expressions:
  - Array expands showing all values
  - `i` increments
  - `baseAddr[0]` stays 0

**9. Run to Second Breakpoint:**
- Press **F8** (Resume)
- Stops after loop complete

**10. Verify Memory:**
- Memory View shows all 50 values written
- Scroll through to verify pattern: 0, 5, 10, 15, ..., 245

**11. Check Console:**
- Serial Terminal shows printed values
- Compare with Memory View values

---

### Example 2: Detecting Buffer Overflow

**Code with Bug:**

```c
#define BUFFER_SIZE 10
volatile int buffer[BUFFER_SIZE];

void write_data() {
    for (int i = 0; i <= BUFFER_SIZE; i++) {  // BUG: <= instead of <
        buffer[i] = i * 10;  // Writes to buffer[10], which is out of bounds!
    }
}

int main() {
    init_platform();

    volatile int guardValue = 0xDEADBEEF;  // Canary value

    write_data();  // Overflow corrupts guardValue

    if (guardValue != 0xDEADBEEF) {
        xil_printf("ERROR: Buffer overflow detected!\r\n");
    }

    cleanup_platform();
    return 0;
}
```

**Debug Strategy:**

**1. Set Watchpoint on Guard:**
- In Variables View, right-click `guardValue`
- **Add Watchpoint** → **Write**
- Click **OK**

**2. Run Program:**
- Press **F8** (Resume)
- Execution stops when `guardValue` is overwritten

**3. Check Call Stack:**
- Call Stack View shows: `write_data()` at line `buffer[i] = i * 10;`
- Variables View shows: `i = 10` (out of bounds!)

**4. Inspect Memory:**
- Memory View shows buffer layout:
  - `buffer[0..9]`: Valid data
  - `buffer[10]`: **Overwrites guardValue!**

**5. Fix the Bug:**
- Change `i <= BUFFER_SIZE` to `i < BUFFER_SIZE`

---

### Example 3: Memory Copy Verification

**Code:**

```c
#define SIZE 100
volatile int source[SIZE];
volatile int dest[SIZE];

void copy_memory() {
    for (int i = 0; i < SIZE; i++) {
        dest[i] = source[i];
    }
}

int main() {
    init_platform();

    // Initialize source
    for (int i = 0; i < SIZE; i++) {
        source[i] = i * 2;
    }

    copy_memory();

    // Verify
    int errors = 0;
    for (int i = 0; i < SIZE; i++) {
        if (dest[i] != source[i]) {
            errors++;
        }
    }

    xil_printf("Copy complete. Errors: %d\r\n", errors);

    cleanup_platform();
    return 0;
}
```

**Debug Verification:**

**1. Open Memory Browser:**
- **Window** → **Show View** → **Memory Browser**

**2. Add Two Memory Panes:**
- Pane 1: Address of `source`
- Pane 2: Address of `dest`

**3. Set Breakpoint:**
- After `copy_memory()` call

**4. Run and Compare:**
- Press **F8** to run
- Stops at breakpoint
- Visually compare both panes
- **View** → **Compare Memory** to highlight differences

**5. Verify No Differences:**
- If copy is correct, no differences highlighted
- If bug exists, different values shown in color

---

## Export/Import Memory

### Exporting Memory Contents

**Save Memory Dump to File:**

**Step 1: Right-click in Memory View**

**Step 2: Select Export** → **Export Memory**

**Step 3: Configure Export:**
- **Start Address**: `0x40000000`
- **Length**: `200` (in bytes)
- **File**: `C:\debug\bram_dump.bin`
- **Format Options**:
  - **Binary** - Raw binary file
  - **Hex** - Intel Hex format
  - **S-Record** - Motorola S-Record format
  - **Plain Text** - Human-readable

**Step 4: Click OK**

**Exported File Uses:**
- Archive memory state for later analysis
- Compare memory between debug sessions
- Share memory dumps with team
- Process with external tools

### Importing Memory Contents

**Load Memory from File:**

**Step 1: Right-click in Memory View**

**Step 2: Select Import** → **Import Memory**

**Step 3: Configure Import:**
- **File**: Select saved memory dump
- **Start Address**: `0x40000000` (where to load)
- **Format**: Must match export format

**Step 4: Click OK**

**Import Use Cases:**
- Restore known-good memory state
- Load test data from file
- Replicate issue from another session

### Export Formats Comparison

| Format | Size | Use Case |
|--------|------|----------|
| **Binary** | Smallest | Exact memory copy, fast |
| **Intel Hex** | Medium | Standard format, portable |
| **S-Record** | Medium | Embedded systems, bootloaders |
| **Plain Text** | Largest | Human-readable, documentation |

---

## Tips & Best Practices

### Memory View Tips

**1. Use Descriptive Names:**
- Right-click memory tab → **Rename**
- Change "0x40000000" to "BRAM_Data"
- Makes navigation easier with multiple monitors

**2. Link Memory to Variables:**
- Don't type addresses manually
- Drag variable from Variables View into Memory View
- Automatically tracks pointer changes

**3. Set Appropriate Cell Size:**
- Match cell size to data type:
  - `char`: 1 byte
  - `short`: 2 bytes
  - `int`/`float`: 4 bytes
  - `long long`/`double`: 8 bytes

**4. Use Row Size for Structure Alignment:**
- If struct is 16 bytes, set row size to 16
- Each row shows one struct instance
- Easier to visualize array of structures

**5. Preserve Monitor Setup:**
- Memory monitors persist across debug sessions
- Set up once, reuse every time
- No need to re-add monitors

### Performance Optimization

**1. Disable Auto-Refresh for Large Regions:**
- Monitoring large memory regions slows stepping
- Disable auto-refresh when not needed
- Manually refresh (F5) when necessary

**2. Limit Memory Range:**
- Don't monitor entire address space
- Set specific length when adding monitor
- Example: 200 bytes instead of entire BRAM

**3. Use Hardware Watchpoints:**
- Faster than software watchpoints
- Limited to 2-4 simultaneously
- Reserve for critical addresses

### Debugging Strategies

**1. Canary Values:**
```c
#define CANARY 0xDEADBEEF
volatile int start_canary = CANARY;
volatile int buffer[100];
volatile int end_canary = CANARY;

// Check canaries to detect overflow
if (start_canary != CANARY || end_canary != CANARY) {
    xil_printf("ERROR: Corruption detected!\r\n");
}
```

**2. Memory Patterns:**
```c
// Initialize memory with known pattern
for (int i = 0; i < SIZE; i++) {
    buffer[i] = 0xA5A5A5A5;  // Alternating bit pattern
}

// After operations, check for unexpected values
```

**3. Incremental Verification:**
- Don't wait until end of program
- Set breakpoints at key points
- Verify memory state incrementally
- Catches bugs closer to source

**4. Compare Against Expected:**
- Maintain expected values in separate array
- Use memory comparison feature
- Automates verification

---

## Keyboard Shortcuts Reference

### Debug Control

| Action | Shortcut | Description |
|--------|----------|-------------|
| **Resume** | F8 | Continue execution |
| **Suspend** | Ctrl+F8 | Pause execution |
| **Step Into** | F5 | Step into function |
| **Step Over** | F6 | Step over function |
| **Step Return** | F7 | Step out of function |
| **Terminate** | Ctrl+F2 | Stop debug session |

### Memory View

| Action | Shortcut | Description |
|--------|----------|-------------|
| **Go to Address** | Ctrl+G | Jump to address |
| **Refresh** | F5 | Refresh memory |
| **Find** | Ctrl+F | Search memory |
| **Copy** | Ctrl+C | Copy selection |
| **Select All** | Ctrl+A | Select all memory |

### View Management

| Action | Shortcut | Description |
|--------|----------|-------------|
| **Quick Access** | Ctrl+3 | Open any view |
| **Maximize View** | Ctrl+M | Maximize active view |
| **Previous View** | Ctrl+F7 | Switch to previous view |
| **Next View** | Ctrl+F8 | Switch to next view |

### Breakpoints

| Action | Shortcut | Description |
|--------|----------|-------------|
| **Toggle Breakpoint** | Ctrl+Shift+B | Add/remove breakpoint |
| **Disable Breakpoint** | Ctrl+B | Enable/disable breakpoint |
| **Skip All Breakpoints** | Alt+Shift+B | Skip all breakpoints |

---

## Troubleshooting

### Issue: Memory View Shows "??" or "N/A"

**Causes:**
- Target not running
- Address out of range
- Memory access violation
- Bus error

**Solutions:**
1. Ensure program is stopped at breakpoint (not running)
2. Verify address is valid for your hardware
3. Check address is within accessible memory range
4. BRAM: 0x40000000-0x40001FFF (8KB example)
5. DDR: Typically starts at 0x00100000

### Issue: Memory View Not Updating

**Causes:**
- Auto-refresh disabled
- Optimized variable
- Cached memory

**Solutions:**
1. Check **Auto Refresh** is enabled (right-click menu)
2. Manually refresh: Press F5 or click refresh button
3. Disable compiler optimization: `-O0` flag
4. Declare variables as `volatile`

### Issue: Cannot Add Watchpoint

**Error:** "Hardware watchpoints limit reached"

**Cause:** Zynq ARM cores support 2-4 hardware watchpoints

**Solutions:**
1. Remove existing watchpoints you don't need
2. **Breakpoints View** → Delete unused watchpoints
3. Use software watchpoints (slower but unlimited)
4. Use conditional breakpoints instead

### Issue: Variables Show "<optimized out>"

**Cause:** Compiler optimization removed variable

**Solutions:**
1. Reduce optimization: Change from `-O2` to `-O0`
2. Declare variable as `volatile`
3. Use variable in way compiler can't optimize out
4. Add `-fno-inline` flag to prevent function inlining

### Issue: Memory View Very Slow

**Causes:**
- Monitoring huge memory region
- Auto-refresh on large area
- Debug interface bandwidth

**Solutions:**
1. Reduce monitored memory length
2. Disable auto-refresh for large regions
3. Increase cell size (fewer cells to display)
4. Close unused memory monitors

### Issue: Address Expression Not Working

**Error:** "Cannot evaluate expression"

**Causes:**
- Variable out of scope
- Invalid pointer
- Syntax error

**Solutions:**
1. Ensure stopped in function where variable exists
2. Check pointer is initialized (not NULL)
3. Use explicit cast: `(int*)0x40000000`
4. For arrays: `&myArray[0]` instead of just `myArray`

### Issue: Exported Memory File is Empty

**Causes:**
- Length set to 0
- Invalid address range
- Access violation during export

**Solutions:**
1. Verify length is non-zero
2. Check address range is accessible
3. Export smaller region to test
4. Check file permissions on export path

---

## Memory Map Reference (Zynq-7000)

For reference when setting up memory monitors:

| Region | Address Range | Size | Description |
|--------|---------------|------|-------------|
| **OCM** | 0x00000000 - 0x0003FFFF | 256 KB | On-Chip Memory (fast) |
| **DDR** | 0x00100000 - 0x3FFFFFFF | ~1 GB | External SDRAM |
| **PL BRAM** | 0x40000000 - 0x7FFFFFFF | Variable | FPGA Block RAM (your design) |
| **IOP** | 0xE0000000 - 0xE02FFFFF | 3 MB | I/O Peripherals |
| **SMC** | 0xE1000000 - 0xE3FFFFFF | 48 MB | Static Memory (NOR/NAND) |
| **SLCR** | 0xF8000000 - 0xF8000FFF | 4 KB | System Level Control |
| **PS Peripherals** | 0xF8001000 - 0xF8FFFFFF | ~16 MB | PS7 Peripherals |

**Your BRAM Location:**
- Base: 0x40000000
- Size: 8 KB (0x2000)
- End: 0x40001FFF

---

## Additional Resources

### Xilinx Documentation

- **UG1400**: Vitis Unified Software Platform Documentation
- **UG1085**: Zynq-7000 SoC Technical Reference Manual
- **UG1144**: Vivado Design Suite Tutorial - Embedded Processor Hardware Design

### Debug Features Documentation

- Memory View: UG1400 Chapter 11
- Watchpoints: UG1400 Chapter 12
- Expression Evaluation: GDB Documentation

### Useful Links

- Xilinx Forums: https://forums.xilinx.com
- Vitis GitHub Examples: https://github.com/Xilinx/Vitis-Tutorials

---

## Summary Checklist

**Before Starting Debug Session:**
- ☐ Code compiled with debug symbols (`-g` flag)
- ☐ Optimization level appropriate (`-O0` for debugging)
- ☐ Critical variables declared as `volatile`
- ☐ Breakpoints set at key locations

**During Debug Session:**
- ☐ Memory View configured with correct format
- ☐ Cell size matches data type
- ☐ Auto-refresh enabled (if monitoring live updates)
- ☐ Expressions added for key variables
- ☐ Watchpoints set on critical memory

**After Debug Session:**
- ☐ Export memory dumps if needed
- ☐ Document findings
- ☐ Remove temporary watchpoints
- ☐ Save working debug configuration

---

**Document Version**: 1.0
**Created**: 2026-05-02
**For**: Vitis Unified IDE 2023.2
**Target**: Zynq-7000 (ARM Cortex-A9)
**Status**: ✅ Complete Reference Guide

---

## Quick Start Summary

### Fastest Way to Inspect Memory

**During Active Debug Session:**

1. **Right-click on variable in code editor**
2. Select **Inspect Memory**
3. Done! Memory View opens at variable's address

**Example:**
```c
volatile int *baseAddr = (int *)0x40000000;
//           ^^^^^^^^^ Right-click here → Inspect Memory
```

### Standard Method

**1. Open Views:**
- Memory: Window → Show View → Memory
- OR Right-click in editor → Inspect Memory (fastest!)
- Variables: Already visible in Debug perspective
- Expressions: Window → Show View → Expressions

**2. Add Memory Monitor (if not auto-opened):**
- Click + in Memory View
- Enter: 0x40000000 (or variable name)
- OR right-click variable → View Memory

**3. Configure Display:**
- Right-click → Format → Decimal
- Right-click → Cell Size → 4 Bytes
- Right-click → Row Size → 8

**4. Add Expressions:**
- Click + in Expressions View
- Enter: *(int(*)[50])baseAddr

**5. Debug:**
- Set breakpoint (Ctrl+Shift+B)
- Start debug (F11)
- Step through (F6)
- Watch memory update in real-time!
