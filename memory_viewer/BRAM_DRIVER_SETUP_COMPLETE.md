# Complete BRAM Driver Manual Installation Guide
## Vitis Unified IDE 2023.2

**Last Updated**: 2026-05-02
**Status**: Verified Working Procedure

---

## Problem Summary

The AXI BRAM Controller in Vivado is marked with `xlnx,edk-special = "BRAM_CTRL"`, which tells Vitis BSP generator to treat it as **memory** rather than a **peripheral**. This results in:

- ✗ No BRAM driver files generated
- ✗ No `xbram.h` header available
- ✗ No BRAM parameters in `xparameters.h`
- ✗ Compilation errors: `undefined reference to XBram_LookupConfig`

---

## Complete Installation Procedure

### Step 1: Add BRAM to `bsp.yaml`

**File**: `memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/bsp.yaml`

Add this entry under `drv_info:` (after the last driver):

```yaml
  axi_bram_ctrl_0:
    driver: bram
    ip_name: axi_bram_ctrl
    path: C:\Xilinx\Vitis\2023.2\data\embeddedsw\XilinxProcessorIPLib\drivers\bram_v4_9
```

---

### Step 2: Copy BRAM Driver Files

**CRITICAL**: Files must be in `libsrc/bram/src/` (note the `src` subdirectory).

```bash
# Create directory structure
mkdir -p memory_viewer/memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/bram/src

# Copy all driver source files
cp /c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/bram_v4_9/src/* \
   memory_viewer/memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/bram/src/

# Copy headers to include directory
cp memory_viewer/memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/bram/src/*.h \
   memory_viewer/memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/
```

This creates:
- `libsrc/bram/src/xbram.c`
- `libsrc/bram/src/xbram.h`
- `libsrc/bram/src/xbram_hw.h`
- `libsrc/bram/src/xbram_g.c` ← auto-generated config
- `libsrc/bram/src/xbram_intr.c`
- `libsrc/bram/src/xbram_selftest.c`
- `libsrc/bram/src/xbram_sinit.c`
- `libsrc/bram/src/CMakeLists.txt`
- `include/xbram.h` ✓
- `include/xbram_hw.h` ✓

---

### Step 3: Add BRAM to CMake Driver List

**File**: `memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/DRVLISTConfig.cmake`

Add `bram;` to the **beginning** of DRIVER_LIST:

```cmake
set(DRIVER_LIST bram;common;coresightps_dcc;devcfg;dmaps;emacps;gpiops;qspips;scugic;scutimer;scuwdt;sdps;ttcps;uartps;usbps;xadcps)
```

This tells CMake to compile and link the BRAM driver into `libxil.a`.

---

### Step 4: Add ALL BRAM Parameters to `xparameters.h`

**File**: `memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xparameters.h`

Add these definitions **before the final `#endif`** (typically after XADC definitions):

**IMPORTANT**: The driver requires BOTH `XPAR_XBRAM_0_*` AND `XPAR_BRAM_0_*` naming conventions!

```c
/* Definitions for BRAM */
#define XPAR_XBRAM_NUM_INSTANCES 1

/* Definitions for peripheral AXI_BRAM_CTRL_0 */
#define XPAR_AXI_BRAM_CTRL_0_DEVICE_ID 0
#define XPAR_AXI_BRAM_CTRL_0_DATA_WIDTH 32
#define XPAR_AXI_BRAM_CTRL_0_ECC_PRESENT 0
#define XPAR_AXI_BRAM_CTRL_0_FAULT_INJECT_PRESENT 0
#define XPAR_AXI_BRAM_CTRL_0_CE_FAILING_REGISTERS 0
#define XPAR_AXI_BRAM_CTRL_0_UE_FAILING_REGISTERS 0
#define XPAR_AXI_BRAM_CTRL_0_ECC_STATUS_REGISTERS 0
#define XPAR_AXI_BRAM_CTRL_0_CE_COUNTER_WIDTH 0
#define XPAR_AXI_BRAM_CTRL_0_ECC_ONOFF_REGISTER 0
#define XPAR_AXI_BRAM_CTRL_0_ECC_ONOFF_RESET_VALUE 0
#define XPAR_AXI_BRAM_CTRL_0_WRITE_ACCESS 0
#define XPAR_AXI_BRAM_CTRL_0_BASEADDR 0x40000000
#define XPAR_AXI_BRAM_CTRL_0_HIGHADDR 0x40001FFF
#define XPAR_AXI_BRAM_CTRL_0_S_AXI_CTRL_BASEADDR 0xFFFFFFFF
#define XPAR_AXI_BRAM_CTRL_0_S_AXI_CTRL_HIGHADDR 0xFFFFFFFF

/* Canonical definitions for peripheral AXI_BRAM_CTRL_0 */
#define XPAR_XBRAM_0_DEVICE_ID XPAR_AXI_BRAM_CTRL_0_DEVICE_ID
#define XPAR_XBRAM_0_DATA_WIDTH XPAR_AXI_BRAM_CTRL_0_DATA_WIDTH
#define XPAR_XBRAM_0_ECC_PRESENT XPAR_AXI_BRAM_CTRL_0_ECC_PRESENT
#define XPAR_XBRAM_0_FAULT_INJECT_PRESENT XPAR_AXI_BRAM_CTRL_0_FAULT_INJECT_PRESENT
#define XPAR_XBRAM_0_CE_FAILING_REGISTERS XPAR_AXI_BRAM_CTRL_0_CE_FAILING_REGISTERS
#define XPAR_XBRAM_0_UE_FAILING_REGISTERS XPAR_AXI_BRAM_CTRL_0_UE_FAILING_REGISTERS
#define XPAR_XBRAM_0_ECC_STATUS_REGISTERS XPAR_AXI_BRAM_CTRL_0_ECC_STATUS_REGISTERS
#define XPAR_XBRAM_0_CE_COUNTER_WIDTH XPAR_AXI_BRAM_CTRL_0_CE_COUNTER_WIDTH
#define XPAR_XBRAM_0_ECC_ONOFF_REGISTER XPAR_AXI_BRAM_CTRL_0_ECC_ONOFF_REGISTER
#define XPAR_XBRAM_0_ECC_ONOFF_RESET_VALUE XPAR_AXI_BRAM_CTRL_0_ECC_ONOFF_RESET_VALUE
#define XPAR_XBRAM_0_WRITE_ACCESS XPAR_AXI_BRAM_CTRL_0_WRITE_ACCESS
#define XPAR_XBRAM_0_BASEADDR XPAR_AXI_BRAM_CTRL_0_BASEADDR
#define XPAR_XBRAM_0_HIGHADDR XPAR_AXI_BRAM_CTRL_0_HIGHADDR

/* Driver xbram_g.c definitions - REQUIRED for compilation */
#define XPAR_BRAM_0_DEVICE_ID XPAR_AXI_BRAM_CTRL_0_DEVICE_ID
#define XPAR_BRAM_0_DATA_WIDTH XPAR_AXI_BRAM_CTRL_0_DATA_WIDTH
#define XPAR_BRAM_0_ECC XPAR_AXI_BRAM_CTRL_0_ECC_PRESENT
#define XPAR_BRAM_0_ECC_PRESENT XPAR_AXI_BRAM_CTRL_0_ECC_PRESENT
#define XPAR_BRAM_0_FAULT_INJECT XPAR_AXI_BRAM_CTRL_0_FAULT_INJECT_PRESENT
#define XPAR_BRAM_0_FAULT_INJECT_PRESENT XPAR_AXI_BRAM_CTRL_0_FAULT_INJECT_PRESENT
#define XPAR_BRAM_0_CE_FAILING_REGISTERS XPAR_AXI_BRAM_CTRL_0_CE_FAILING_REGISTERS
#define XPAR_BRAM_0_CE_FAILING_DATA_REGISTERS XPAR_AXI_BRAM_CTRL_0_CE_FAILING_REGISTERS
#define XPAR_BRAM_0_UE_FAILING_REGISTERS XPAR_AXI_BRAM_CTRL_0_UE_FAILING_REGISTERS
#define XPAR_BRAM_0_UE_FAILING_DATA_REGISTERS XPAR_AXI_BRAM_CTRL_0_UE_FAILING_REGISTERS
#define XPAR_BRAM_0_ECC_STATUS_REGISTERS XPAR_AXI_BRAM_CTRL_0_ECC_STATUS_REGISTERS
#define XPAR_BRAM_0_CE_COUNTER_WIDTH XPAR_AXI_BRAM_CTRL_0_CE_COUNTER_WIDTH
#define XPAR_BRAM_0_ECC_ONOFF_REGISTER XPAR_AXI_BRAM_CTRL_0_ECC_ONOFF_REGISTER
#define XPAR_BRAM_0_ECC_ONOFF_RESET_VALUE XPAR_AXI_BRAM_CTRL_0_ECC_ONOFF_RESET_VALUE
#define XPAR_BRAM_0_WRITE_ACCESS XPAR_AXI_BRAM_CTRL_0_WRITE_ACCESS
#define XPAR_BRAM_0_BASEADDR XPAR_AXI_BRAM_CTRL_0_BASEADDR
#define XPAR_BRAM_0_HIGHADDR XPAR_AXI_BRAM_CTRL_0_HIGHADDR
#define XPAR_BRAM_0_CTRL_BASEADDR XPAR_AXI_BRAM_CTRL_0_S_AXI_CTRL_BASEADDR
#define XPAR_BRAM_0_CTRL_HIGHADDR XPAR_AXI_BRAM_CTRL_0_S_AXI_CTRL_HIGHADDR
```

**Notes**:
- Adjust `BASEADDR` and `HIGHADDR` to match your Vivado Address Editor settings
- The `XPAR_BRAM_0_*` definitions are CRITICAL - `xbram_g.c` will fail without them
- Both `ECC` and `ECC_PRESENT` variants are needed
- Both `FAULT_INJECT` and `FAULT_INJECT_PRESENT` variants are needed
- Both `CE/UE_FAILING_REGISTERS` and `CE/UE_FAILING_DATA_REGISTERS` variants are needed

---

### Step 5: Rebuild Platform in Vitis

**In Vitis IDE:**

1. Right-click **platform** (e.g., `memory_viewer`) → **Clean**
2. Right-click **platform** → **Build**

**What happens during build:**
1. CMake reads `DRVLISTConfig.cmake` and finds `bram` in the driver list
2. CMake compiles all files in `libsrc/bram/src/`:
   - `xbram.c` → `xbram.c.obj`
   - `xbram_g.c` → `xbram_g.c.obj` (uses XPAR_BRAM_0_* parameters)
   - `xbram_intr.c` → `xbram_intr.c.obj`
   - `xbram_selftest.c` → `xbram_selftest.c.obj`
   - `xbram_sinit.c` → `xbram_sinit.c.obj`
3. Creates `libbram.a` archive
4. Links all driver archives into `libxil.a`
5. Exports to `export/memory_viewer/sw/standalone_ps7_cortexa9_0/lib/libxil.a`

**Verify success:**
```bash
# Check if BRAM symbols are in the library
nm memory_viewer/export/memory_viewer/sw/standalone_ps7_cortexa9_0/lib/libxil.a | grep XBram
```

You should see symbols like:
- `XBram_CfgInitialize`
- `XBram_LookupConfig`
- `XBram_SelfTest`

---

### Step 6: Rebuild Application

After platform builds successfully:

1. Right-click **application** (e.g., `memory_viewr_app`) → **Clean**
2. Right-click **application** → **Build**

The application will now successfully link against BRAM driver functions.

---

## Common Build Errors & Solutions

### ❌ Error: `add_subdirectory given source "bram/src" which is not an existing directory`

**Cause**: Files copied to wrong location.

**Fix**: Ensure files are in `libsrc/bram/src/` (with `src` subdirectory), not `libsrc/bram/`.

---

### ❌ Error: `'XPAR_BRAM_0_ECC' undeclared`

**Cause**: Missing short-form parameter names.

**Fix**: Add `#define XPAR_BRAM_0_ECC XPAR_AXI_BRAM_CTRL_0_ECC_PRESENT` (note: `ECC` not `ECC_PRESENT`).

---

### ❌ Error: `'XPAR_BRAM_0_FAULT_INJECT' undeclared`

**Cause**: Missing short-form parameter name.

**Fix**: Add `#define XPAR_BRAM_0_FAULT_INJECT XPAR_AXI_BRAM_CTRL_0_FAULT_INJECT_PRESENT`.

---

### ❌ Error: `'XPAR_BRAM_0_CE_FAILING_DATA_REGISTERS' undeclared`

**Cause**: Driver expects different parameter name than what's defined.

**Fix**: Add both variants:
```c
#define XPAR_BRAM_0_CE_FAILING_REGISTERS ...
#define XPAR_BRAM_0_CE_FAILING_DATA_REGISTERS ...
```

---

### ❌ Error: `undefined reference to 'XBram_LookupConfig'`

**Cause**: BRAM driver not linked into `libxil.a`.

**Fix**:
1. Verify `bram` is in `DRVLISTConfig.cmake`
2. Rebuild **platform** (not just application)
3. Check `libxil.a` contains BRAM symbols: `nm libxil.a | grep XBram`

---

### ❌ Warning: `missing initializer for field 'CtrlBaseAddress'`

**Cause**: Missing `CTRL_BASEADDR` definitions.

**Fix**: Add:
```c
#define XPAR_BRAM_0_CTRL_BASEADDR XPAR_AXI_BRAM_CTRL_0_S_AXI_CTRL_BASEADDR
#define XPAR_BRAM_0_CTRL_HIGHADDR XPAR_AXI_BRAM_CTRL_0_S_AXI_CTRL_HIGHADDR
```

---

## Application Usage Example

Once installed, use the BRAM driver in your application:

```c
#include "xbram.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xil_printf.h"

XBram BramInstance;

int main()
{
    init_platform();

    // Lookup BRAM configuration
    XBram_Config *ConfigPtr = XBram_LookupConfig(XPAR_AXI_BRAM_CTRL_0_DEVICE_ID);
    if (ConfigPtr == NULL) {
        xil_printf("BRAM LookupConfig FAILED\r\n");
        return XST_FAILURE;
    }

    // Initialize BRAM controller
    int Status = XBram_CfgInitialize(&BramInstance,
                                      ConfigPtr,
                                      ConfigPtr->CtrlBaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("BRAM CfgInitialize FAILED\r\n");
        return XST_FAILURE;
    }

    xil_printf("BRAM initialized successfully!\r\n");

    // Access BRAM memory directly
    volatile uint32_t *bram = (uint32_t *)XPAR_AXI_BRAM_CTRL_0_BASEADDR;

    // Write test pattern
    for (int i = 0; i < 10; i++) {
        bram[i] = 0xDEAD0000 + i;
    }

    // Read and verify
    for (int i = 0; i < 10; i++) {
        xil_printf("BRAM[%d] = 0x%08X\r\n", i, bram[i]);
    }

    cleanup_platform();
    return XST_SUCCESS;
}
```

---

## Files Modified/Created Summary

| File | Action | Purpose |
|------|--------|---------|
| `bsp/bsp.yaml` | Modified | Added BRAM driver metadata |
| `bsp/libsrc/DRVLISTConfig.cmake` | Modified | Added `bram` to CMake build |
| `bsp/libsrc/bram/src/*` | Created | Driver source files (9 files) |
| `bsp/include/xbram.h` | Created | Driver API header |
| `bsp/include/xbram_hw.h` | Created | Hardware register definitions |
| `bsp/include/xparameters.h` | Modified | Added 30+ BRAM parameters |
| `export/.../lib/libxil.a` | Updated | BRAM driver linked into BSP |

---

## Why Manual Installation?

**Design Rationale**: Xilinx BSP generator treats BRAM controllers differently because:

1. **Most applications don't need the driver** - direct memory access is simpler
2. **Hardware marks it specially**: `xlnx,edk-special = "BRAM_CTRL"`
3. **Memory vs Peripheral distinction**: Memory-mapped peripherals are accessed differently

**When you NEED the driver**:
- ECC (Error Correction Code) features
- Interrupt-driven access
- Standard Xilinx driver API compatibility
- Tutorial/example code that uses driver functions

---

## Alternative: Direct Memory Access (No Driver)

If you don't need ECC or interrupts, skip the driver entirely:

```c
#include "xparameters.h"
#include "xil_printf.h"

#define BRAM_BASE 0x40000000
#define BRAM_SIZE 8192  // 8KB

int main()
{
    init_platform();

    volatile uint32_t *bram = (uint32_t *)BRAM_BASE;

    // Write
    bram[0] = 0x12345678;

    // Read
    uint32_t value = bram[0];
    xil_printf("BRAM[0] = 0x%08X\r\n", value);

    cleanup_platform();
    return 0;
}
```

**No driver installation needed!**

---

## Version Information

- **Vitis**: 2023.2
- **Vivado**: 2023.2
- **BRAM Driver**: bram_v4_9
- **BSP Build System**: CMake 3.24.2
- **Board**: Avnet ZedBoard (xc7z020clg484-1)
- **Target**: ARM Cortex-A9 (ps7_cortexa9_0)
- **OS**: Standalone (bare-metal)

---

**Document Version**: 2.0
**Created**: 2026-05-02
**Last Verified**: 2026-05-02
**Status**: ✅ Complete working procedure with all build errors resolved

---

**Tested Steps**:
1. ✅ bsp.yaml modification
2. ✅ Driver file copying to src/ subdirectory
3. ✅ DRVLISTConfig.cmake update
4. ✅ xparameters.h with ALL required parameters
5. ✅ Platform build succeeds
6. ✅ Application links successfully
7. ✅ Driver functions work at runtime
