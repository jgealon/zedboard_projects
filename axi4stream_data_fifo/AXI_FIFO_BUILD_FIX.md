# AXI FIFO Application Build Fix

## Problem
The axi4stream_data_fifo_app application was failing to link with undefined references to:
- `XLlFifo_iRxOccupancy`
- `XLlFifo_RxGetWord`

## Root Cause
The llfifo driver configuration file `xllfifo_g.c` was missing from the application build.

## Files Modified

### 1. Added xllfifo_g.c to Application Source
**Location:** `axi4stream_data_fifo_app/src/xllfifo_g.c`

Copied from BSP location to application src folder.

### 2. Updated CMakeLists.txt
**File:** `axi4stream_data_fifo_app/src/CMakeLists.txt`

Added `xllfifo_g.c` to the build:
```cmake
collect (PROJECT_LIB_SOURCES helloworld.c)
collect (PROJECT_LIB_SOURCES platform.c)
collect (PROJECT_LIB_SOURCES xllfifo.c)
collect (PROJECT_LIB_SOURCES xllfifo_sinit.c)
collect (PROJECT_LIB_SOURCES xllfifo_g.c)      # <-- ADDED
collect (PROJECT_LIB_SOURCES xstreamer.c)
```

### 3. Fixed Logic Error in helloworld.c
**File:** `axi4stream_data_fifo_app/src/helloworld.c` (lines 144-151)

**Original Code (INCORRECT):**
```c
for (int i = 0; i < RecvLen; i++)
{
    RecvWord = XLlFifo_RxGetWord(InstancePtr);      // Read first word
    if (XLlFifo_iRxOccupancy(InstancePtr))
        RecvWord = XLlFifo_RxGetWord(InstancePtr);  // Read second word, overwriting first!
    DstAddr[i] = RecvWord;
}
```

**Problem:** This code reads two words per loop iteration but only stores the second one, effectively skipping every other word in the FIFO.

**Fixed Code (CORRECT):**
```c
for (int i = 0; i < RecvLen; i++)
{
    if (XLlFifo_iRxOccupancy(InstancePtr))          // Check if data available
    {
        RecvWord = XLlFifo_RxGetWord(InstancePtr);  // Read one word
        DstAddr[i] = RecvWord;                      // Store it
    }
}
```

## Files Already in Place

The following llfifo driver files were already copied to the application in previous steps:
- `axi4stream_data_fifo_app/src/xllfifo.c`
- `axi4stream_data_fifo_app/src/xllfifo_sinit.c`
- `axi4stream_data_fifo_app/src/xstreamer.c`

Headers in BSP:
- `axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xllfifo.h`
- `axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xllfifo_hw.h`
- `axi4stream_data_fifo_pfrm/ps7_cortexa9_0/standalone_ps7_cortexa9_0/bsp/include/xstreamer.h`

## Next Steps

### To Rebuild in Vitis IDE:
1. Open Vitis 2023.2
2. Right-click on `axi4stream_data_fifo_app` project
3. Select **Build Project** (do NOT clean first - just build directly)

The application should now compile and link successfully.

**Note:** The build directory has been reset. A fresh CMake configuration will be generated during the first build.

### Expected Output
When the application runs on the Zedboard, it should:
1. Initialize the AXI FIFO (axi_fifo_mm_s_0)
2. Write 48 words (8 packets × 6 words) to the FIFO transmit buffer
3. Read back the same 48 words from the FIFO receive buffer
4. Compare transmitted and received data
5. Print success if data matches

### Test Flow
```
Source Buffer → AXI FIFO MM-S (Tx) → AXI Stream →
AXI Stream Data FIFO → AXI Stream → AXI FIFO MM-S (Rx) → Destination Buffer
```

## Verification Checklist
- [x] xllfifo_g.c copied to src folder
- [x] CMakeLists.txt updated to include xllfifo_g.c
- [x] FifoRecv function logic error fixed
- [ ] Clean build directory (do in Vitis)
- [ ] Rebuild application (do in Vitis)
- [ ] Test on hardware

## Additional Notes

### Device ID Configuration
The `xparameters.h` file already has the correct device ID definitions:
```c
#define XPAR_AXI_FIFO_0_DEVICE_ID XPAR_AXI_FIFO_MM_S_0_DEVICE_ID
```

### Xilinx Driver Typo
The code correctly uses `XLlFfio_LookupConfig` (with double 'f') which matches the typo in the Xilinx driver header file.

### Block Design Components
- **axi_fifo_mm_s_0**: Memory-Mapped to AXI-Stream FIFO (controlled by PS via llfifo driver)
- **axis_data_fifo_0**: AXI-Stream Data FIFO (passthrough, in Standard FIFO mode)

The loopback connects the Tx and Rx sides of the axi_fifo_mm_s_0 through the axis_data_fifo_0.
