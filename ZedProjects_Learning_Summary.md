# ZedProjects Learning Summary

Xilinx Zynq SoC's are know to provide maximum performance per watt along with maximum reconfiguration flexibility. Zynq family features Dual-Core ARM Cortex A9 processors tightly coupled with the 7-series FPGA to enable faster communication interfaces development with ARM Design flow and hardware acceleration. Zynq devices are available in two categories viz. Zynq-7000s family FPGA for the cost-effective application such as IoT related applications while Zynq 7000 family FPGA are best for high-performance applications such as Embedded Vision etc. The Zynq 7000s comes with Single core ARM while Zynq 7000 comes with Dual-Core ARM.

These ZedProjects cover fundamentals of Popular Xilinx drivers viz. UART, AXI Timers, UART16550, AXI GPIO, AXI BRAM, etc. The projects also illustrate the usage of the AXI interrupt controller for handling Interrupts. Also to felicitate incorporation of Hardware accelerators with Zynq based design few examples on building Custom AXI Peripherals are also included. Software and Hardware Debugging, Profiling fundamentals are demonstrated with Zynq to felicitate performance measurement.

These projects build the foundation necessary to quickly start building applications on Zynq FPGA devices without prior experience in this domain.  The entire collection is Lab-based with a major focus on building skills necessary to handle simple peripherals such as GPIO, Intermediate Peripherals such as UART PS, AXI BRAM, and complex Peripherals such as AXI Interrupt Controller,  AXI Timers, GIC etc.

## 1. Zynq-7000 Embedded Design Fundamentals
- Built and used Xilinx Vivado/Vitis workflows for Zynq-7000 boards.
- Learned hardware/software co-design: export hardware from Vivado, then build bare-metal apps in Vitis.
- Worked with a standalone PS7 Cortex-A9 system and PS peripherals.

## 2. AXI GPIO and Interactive Embedded Apps
- `dual_channel_axi_gpio` is the main practical platform.
- Learned AXI GPIO usage:
  - read switches from channel 1
  - control LEDs on channel 2
- Created interactive applications:
  - direct switch-to-LED mirror
  - binary counter with speed, direction, and pattern control
  - Knight Rider / Cylon LED animations
  - reaction-time game
  - binary trainer quiz game

## 3. Timer Peripherals
- Learned about Zynq timer options:
  - `ttc/` project shows TTC (Triple Timer Counter)
  - `mio_led_with_ttc/` uses TTC + PS MIO GPIO for LED blink
- Key timer lessons:
  - frequency calculation with `XTtcPs_CalcIntervalFromFreq`
  - interval mode + auto-reload
  - 16-bit counter limits and prescaler selection
  - polling-based timer loops

## 4. Watchdog and UART Integration
- `pvt_wdt` documentation taught PS watchdog usage and UART selection.
- Important fix: use `XUartPs` (PS UART) rather than `XUartLite` for the current hardware design.
- Learned how to:
  - initialize PS UART
  - print console feedback
  - combine PS peripherals with watchdog logic

## 5. Debugging and System Tools
- The workspace includes debug-focused projects like `debugging_techniques/`, `ila/`, and `memory_viewer/`.
- This means you learned about:
  - on-chip logic analyzer usage
  - memory inspection techniques
  - debug workflows for FPGA/embedded development

## Practical Skills Gained
- Designing, building, and running FPGA applications
- Mapping board I/O (switches, LEDs, MIO pins)
- Writing robust bare-metal C code with Xilinx drivers
- Understanding AXI vs PS peripheral tradeoffs
- Documenting hardware/software behavior clearly

> Overall, this folder shows a solid learning path from basic GPIO control to more advanced timer, watchdog, and debug peripherals on Zynq-7000 systems.
