# BRAM Driver Setup Guide for Vitis Unified IDE 2023.2

## Problem Description

When creating a Vivado hardware design with an **AXI BRAM Controller** and exporting it as an XSA file to Vitis, the **BRAM driver is not automatically included** in the Board Support Package (BSP), even though the hardware contains the BRAM controller.

This causes compilation errors when trying to use the BRAM driver API:
```c
#include "xbram.h"  // Error: file not found
```

## Root Cause

The AXI BRAM Controller in the block design has the property:
```
xlnx,edk-special = "BRAM_CTRL"
```

This tells the Vitis BSP generator that the BRAM controller is a **memory controller for direct memory access**, not a **peripheral requiring a driver**. As a result:

1. The hardware device tree correctly includes the BRAM controller
2. The memory is accessible at the correct address (e.g., `0x40000000`)
3. BUT the BSP generator **does NOT include** the BRAM driver files
4. The `xbram.h` header is missing from the BSP

## Solution: Manual BRAM Driver Installation

Since the BSP auto-generation skips the BRAM driver, you must manually add it.

### Step 1: Edit `bsp.yaml`

Add the BRAM driver entry to the BSP configuration file:

**File**: `memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/bsp.yaml`

Add this entry under the `drv_info:` section (after the last driver entry):

```yaml
drv_info:
  # ... existing drivers ...
  ps7_xadc_0:
    driver: xadcps
    ip_name: ps7_xadc
    path: C:\Xilinx\Vitis\2023.2\data\embeddedsw\XilinxProcessorIPLib\drivers\xadcps_v2_7
  axi_bram_ctrl_0:
    driver: bram
    ip_name: axi_bram_ctrl
    path: C:\Xilinx\Vitis\2023.2\data\embeddedsw\XilinxProcessorIPLib\drivers\bram_v4_9
```

### Step 2: Rebuild Platform in Vitis

1. Open Vitis Unified IDE
2. In **VITIS COMPONENTS** view, right-click the platform
3. Select **Build**

**⚠️ Important**: The rebuild will NOT automatically generate the BRAM driver files because of the `edk-special` property. You must manually copy the files.

### Step 3: Manually Copy BRAM Driver Files

Run these commands from your project root:

```bash
# Copy BRAM driver source files to BSP libsrc
cp -r /c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/bram_v4_9/src \
      memory_viewer/memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/bram

# Copy BRAM header files to BSP include directory
cp memory_viewer/memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/libsrc/bram/*.h \
   memory_viewer/memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/
```

This will create:
- `libsrc/bram/xbram.c`
- `libsrc/bram/xbram.h`
- `libsrc/bram/xbram_hw.h`
- `libsrc/bram/xbram_intr.c`
- `libsrc/bram/xbram_selftest.c`
- `libsrc/bram/xbram_sinit.c`
- `include/xbram.h` ✓
- `include/xbram_hw.h` ✓

### Step 4: Add BRAM Parameters to `xparameters.h`

Edit: `memory_viewer/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xparameters.h`

Add these definitions (after the XADC definitions, before the `#endif`):

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
```

**Note**: Adjust the `BASEADDR` and `HIGHADDR` values to match your block design Address Editor settings.

### Step 5: Rebuild Application

In Vitis, rebuild your application to pick up the new headers and driver files.

## Usage Example

Once the BRAM driver is properly installed, you can use it in your application:

```c
#include "xbram.h"
#include "xparameters.h"
#include "xil_printf.h"

int main()
{
    XBram BramInstance;
    XBram_Config *ConfigPtr;

    init_platform();

    // Lookup BRAM configuration
    ConfigPtr = XBram_LookupConfig(XPAR_AXI_BRAM_CTRL_0_DEVICE_ID);
    if (ConfigPtr == NULL) {
        xil_printf("BRAM LookupConfig failed\r\n");
        return XST_FAILURE;
    }

    // Initialize BRAM controller
    int Status = XBram_CfgInitialize(&BramInstance,
                                      ConfigPtr,
                                      ConfigPtr->CtrlBaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("BRAM CfgInitialize failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("BRAM initialized successfully!\r\n");
    xil_printf("BRAM Base Address: 0x%08X\r\n",
               ConfigPtr->CtrlBaseAddress);

    // Access BRAM memory directly
    volatile uint32_t *bram_ptr = (uint32_t *)XPAR_AXI_BRAM_CTRL_0_BASEADDR;

    // Write test pattern
    for (int i = 0; i < 10; i++) {
        bram_ptr[i] = 0xDEAD0000 + i;
    }

    // Read and display (Memory Viewer)
    xil_printf("\r\nBRAM Memory Viewer:\r\n");
    xil_printf("Address    | Value\r\n");
    xil_printf("-----------+-----------\r\n");
    for (int i = 0; i < 10; i++) {
        xil_printf("0x%08X | 0x%08X\r\n",
                   XPAR_AXI_BRAM_CTRL_0_BASEADDR + (i * 4),
                   bram_ptr[i]);
    }

    cleanup_platform();
    return 0;
}
```

## Alternative: Direct Memory Access (No Driver Needed)

If you only need to read/write BRAM memory and don't need ECC or interrupt features, you can skip the driver and use direct memory access:

```c
#include "xil_printf.h"

#define BRAM_BASE 0x40000000
#define BRAM_SIZE 8192  // 8KB

int main()
{
    init_platform();

    volatile uint32_t *bram = (volatile uint32_t *)BRAM_BASE;

    // Write
    bram[0] = 0x12345678;

    // Read
    uint32_t value = bram[0];
    xil_printf("BRAM[0] = 0x%08X\r\n", value);

    cleanup_platform();
    return 0;
}
```

This approach requires **no driver setup** and works immediately after hardware export.

## Hardware Configuration Notes

### Vivado Block Design

Your block design should have:
- **AXI BRAM Controller** (`axi_bram_ctrl_0`)
- **Block Memory Generator** (`axi_bram_ctrl_0_bram`)
- Connected to **Processing System** via AXI Interconnect

### Address Editor Settings

Check **Window → Address Editor** in Vivado:
```
Master: processing_system7_0 → Data
Slave Segment: axi_bram_ctrl_0/Mem0
Offset: 0x40000000
Range: 8K (or your configured size)
```

### Key Properties

In the block design, the BRAM Controller has:
- **Protocol**: AXI4
- **BMG Instance**: EXTERNAL
- **Data Width**: 32 bits
- **Memory Depth**: 2048 words (8KB)
- **ECC**: Disabled (for simple memory access)

## Troubleshooting

### Issue: `xbram.h` still not found after rebuild

**Solution**: The BSP rebuild won't auto-generate BRAM files due to the `edk-special` property. You must manually copy the driver files (Step 3).

### Issue: Undefined reference to `XBram_LookupConfig`

**Solution**: The BRAM driver source files aren't being compiled. Ensure:
1. `libsrc/bram/` folder exists with all `.c` files
2. The platform has been rebuilt
3. Your application links against the platform BSP

### Issue: BRAM parameters missing in `xparameters.h`

**Solution**: The BSP generator skips BRAM parameters. Manually add them as shown in Step 4.

### Issue: Application can read/write but driver initialization fails

**Solution**: If direct memory access works but `XBram_CfgInitialize()` fails, verify:
- `XPAR_AXI_BRAM_CTRL_0_BASEADDR` matches your Address Editor
- `XPAR_AXI_BRAM_CTRL_0_DEVICE_ID` is defined (usually 0)

## Files Modified/Created

| File Path | Action | Purpose |
|-----------|--------|---------|
| `bsp/bsp.yaml` | Modified | Added BRAM driver configuration |
| `bsp/libsrc/bram/` | Created | BRAM driver source files |
| `bsp/include/xbram.h` | Created | BRAM driver API header |
| `bsp/include/xbram_hw.h` | Created | BRAM hardware definitions |
| `bsp/include/xparameters.h` | Modified | Added BRAM parameters |

## References

- Xilinx Driver: `/c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/bram_v4_9/`
- Driver YAML: `/c/Xilinx/Vitis/2023.2/data/embeddedsw/XilinxProcessorIPLib/drivers/bram_v4_9/data/bram.yaml`
- Compatible String: `xlnx,axi-bram-ctrl-4.1`

## Version Information

- **Vitis**: 2023.2
- **Vivado**: 2023.2
- **BRAM Driver Version**: bram_v4_9
- **Board**: Avnet ZedBoard (xc7z020clg484-1)
- **OS**: Standalone (bare-metal)

---

**Created**: 2026-05-02
**Issue**: BRAM driver not auto-generated in BSP
**Status**: Resolved with manual installation
