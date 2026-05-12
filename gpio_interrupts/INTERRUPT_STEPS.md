# GPIO Interrupts Workflow

This document describes the common series of steps for working with GPIO interrupts on a Zynq-based bare-metal design.

The attached image illustrates the interrupt flow:
- `INTC` initialization and self-test
- enabling the exception handler
- mapping the GPIO interrupt to the ISR
- enabling interrupts in software

## 1. Hardware design in Vivado
1. Create or open a Vivado project for your Zynq target board.
2. Add the Zynq Processing System (PS) IP and run block automation.
3. Instantiate an AXI GPIO IP core for the interrupt-enabled GPIO channel.
4. Configure the AXI GPIO core:
   - Enable the `Interrupt` option.
   - Choose `GPIO` channel direction (input, output) as needed.
   - Connect the port pins to board switches, buttons, or LEDs.
5. Connect the AXI GPIO interrupt output to the Zynq PS interrupt input (e.g. `IRQ_F2P[0]`).
6. Generate IP and validate the design.
7. Create or update the block design Tcl so the interrupt connection is preserved.
8. Run synthesis and implementation, then generate the bitstream.

## 2. Export hardware and create a software platform
1. Export the hardware design including the bitstream from Vivado.
2. Open Vitis and create a new platform project or import the exported hardware description.
3. Create a new application project targeting the platform.
4. Choose a standalone BSP or bare-metal template.
5. If required, enable the interrupt controller support in the BSP settings.

## 3. Software setup in Vitis
1. Add the AXI GPIO driver to the application.
2. Add the interrupt controller driver (`XScuGic`) if using the Zynq GIC.
3. Configure the peripheral base addresses and interrupt IDs in the `xparameters.h` file.
4. Create an interrupt handler function for the GPIO interrupt.
5. Write initialization code that does the following:
   - Initialize the GPIO driver (`XGpio_Initialize`).
   - Initialize the interrupt controller (`XScuGic_CfgInitialize`).
   - Register the GPIO interrupt handler with `XScuGic_Connect`.
   - Enable the GPIO interrupt in the interrupt controller with `XScuGic_Enable`.
   - Enable the GPIO interrupt source in the AXI GPIO core with `XGpio_InterruptEnable`.
   - Clear any pending interrupts in the AXI GPIO core with `XGpio_InterruptClear`.
   - Enable exceptions globally with `Xil_ExceptionEnable`.

## 4. Typical software sequence
1. Initialize the PS UART for logging and debug output.
2. Initialize the interrupt controller.
3. Connect the ISR and enable the interrupt source.
4. Enable the GPIO interrupt output at the peripheral level.
5. Start the main loop and wait for asynchronous events.
6. In the ISR:
   - Read the GPIO status if needed.
   - Clear the interrupt by calling `XGpio_InterruptClear`.
   - Perform any application-specific action (toggle LEDs, increment counters, signal events).

## 5. Enable / clear interrupt handling
1. Enable interrupt generation in the peripheral:
   - `XGpio_InterruptEnable(&GpioInstance, GPIO_IR_MASK);`
2. Clear pending interrupts before enabling:
   - `XGpio_InterruptClear(&GpioInstance, GPIO_IR_MASK);`
3. Enable the interrupt vector in the GIC:
   - `XScuGic_Enable(&IntcInstance, GPIO_INTERRUPT_ID);`
4. Enable CPU exceptions:
   - `Xil_ExceptionEnable();`

## 6. Debugging tips
- Confirm the interrupt connection in the Vivado block design and address map.
- Verify the correct interrupt ID and device ID from `xparameters.h`.
- Use UART print statements to trace initialization and ISR entry.
- Check that the GPIO input pin is correctly routed to the board switch or button.
- Ensure any active-low signals are handled correctly in software.
- If the ISR does not fire, verify that the interrupt enable bits are set in both the GPIO peripheral and the GIC.

## 7. Validation and testing
1. Program the FPGA with the generated bitstream.
2. Run the application from Vitis or from the target.
3. Toggle the input source connected to the GPIO interrupt.
4. Confirm the ISR executes and the expected response occurs.
5. Use the onboard LEDs or UART output to verify behavior.

## 8. Common pitfalls
- Forgetting to connect the AXI GPIO interrupt line to the PS interrupt input.
- Using the wrong interrupt ID or device ID in software.
- Not clearing the peripheral interrupt before re-enabling it.
- Enabling the GPIO input but leaving the interrupt mask disabled.
- Forgetting to enable global exceptions in the CPU.

---

File saved in `gpio_interrupts/INTERRUPT_STEPS.md`.
