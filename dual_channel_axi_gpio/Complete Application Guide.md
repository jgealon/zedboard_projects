 Complete Application Guide                                                                                                                                                                                      
  Application 1: Binary Counter with Controls                                                                                                                                                                     
  // binary_counter.c
  #include <stdio.h>
  #include <xstatus.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xgpio.h"
  #include "xparameters.h"
  #include "sleep.h"

  XGpio_Config *gpio_config;
  XGpio gpio;

  void gpio_init(){
      int status;
      gpio_config = XGpio_LookupConfig(0); // Device ID is 0 (first GPIO instance)
      status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);
      if (status == XST_SUCCESS)
          xil_printf("Device INIT Successful.\n");
      else
          xil_printf("Device INIT Failed.\n");
  }

  int main() {
      u8 counter = 0;
      u8 prevSwValue = 0;
      u8 swValue = 0;
      int speed = 500; // Default speed in ms
      int direction = 1; // 1 = up, -1 = down
      int paused = 0;

      init_platform();
      gpio_init();

      XGpio_SetDataDirection(&gpio, 1, 0xff); // Channel 1 = input (switches)
      XGpio_SetDataDirection(&gpio, 2, 0x00); // Channel 2 = output (LEDs)

      xil_printf("\n\r=== Binary Counter Application ===\n\r");
      xil_printf("SW0: Pause/Resume\n\r");
      xil_printf("SW1: Change Direction (Up/Down)\n\r");
      xil_printf("SW2-SW3: Speed Control (00=Fast, 01=Normal, 10=Slow, 11=Very Slow)\n\r");
      xil_printf("SW4: Reset Counter\n\r");
      xil_printf("SW5-SW7: Pattern Mode\n\r\n\r");

      while (1) {
          swValue = XGpio_DiscreteRead(&gpio, 1);

          // SW0: Pause/Resume (toggle on change)
          if ((swValue & 0x01) && !(prevSwValue & 0x01)) {
              paused = !paused;
              xil_printf("Counter %s\n\r", paused ? "PAUSED" : "RESUMED");
          }

          // SW1: Direction (toggle on change)
          if ((swValue & 0x02) && !(prevSwValue & 0x02)) {
              direction = -direction;
              xil_printf("Direction: %s\n\r", direction == 1 ? "UP" : "DOWN");
          }

          // SW2-SW3: Speed control
          int speedMode = (swValue >> 2) & 0x03;
          switch(speedMode) {
              case 0: speed = 100; break;   // Fast
              case 1: speed = 500; break;   // Normal
              case 2: speed = 1000; break;  // Slow
              case 3: speed = 2000; break;  // Very Slow
          }

          // SW4: Reset counter
          if (swValue & 0x10) {
              counter = 0;
              xil_printf("Counter RESET\n\r");
          }

          // SW5-SW7: Pattern modes
          int patternMode = (swValue >> 5) & 0x07;

          if (!paused) {
              switch(patternMode) {
                  case 0: // Normal binary count
                      counter = (counter + direction) & 0xFF;
                      break;
                  case 1: // Only odd numbers
                      counter = (counter + (direction * 2)) & 0xFF;
                      counter |= 0x01;
                      break;
                  case 2: // Only even numbers
                      counter = (counter + (direction * 2)) & 0xFE;
                      break;
                  case 3: // Powers of 2
                      if (direction == 1) {
                          if (counter == 0 || counter == 0x80) counter = 1;
                          else counter = counter << 1;
                      } else {
                          if (counter == 0 || counter == 1) counter = 0x80;
                          else counter = counter >> 1;
                      }
                      break;
                  case 4: // Fibonacci-like pattern (mod 256)
                      {
                          static u8 prev1 = 0, prev2 = 1;
                          u8 next = (prev1 + prev2) & 0xFF;
                          prev1 = prev2;
                          prev2 = next;
                          counter = next;
                      }
                      break;
                  default: // Normal count for other modes
                      counter = (counter + direction) & 0xFF;
                      break;
              }

              XGpio_DiscreteWrite(&gpio, 2, counter);
              xil_printf("Counter: %3d (0x%02X) | Mode: %d | Speed: %dms\n\r",
                         counter, counter, patternMode, speed);
          }

          prevSwValue = swValue;
          usleep(speed * 1000);
      }

      cleanup_platform();
      return 0;
  }

  How to Use - Binary Counter:

  Initial Setup:

  1. All switches DOWN (OFF position)
  2. Power on the board
  3. Connect UART terminal (115200 baud, 8N1)
  4. Program the FPGA and run the application

  Switch Functions:

  ┌─────────┬───────────────┬─────────────────────────────────────────────────────────────────────┐
  │ Switch  │   Function    │                          Expected Behavior                          │
  ├─────────┼───────────────┼─────────────────────────────────────────────────────────────────────┤
  │ SW0     │ Pause/Resume  │ Toggle UP to pause counter, toggle DOWN to resume                   │
  ├─────────┼───────────────┼─────────────────────────────────────────────────────────────────────┤
  │ SW1     │ Direction     │ Toggle UP to count down, toggle DOWN to count up                    │
  ├─────────┼───────────────┼─────────────────────────────────────────────────────────────────────┤
  │ SW2-SW3 │ Speed Control │ 00=100ms (fast), 01=500ms (normal), 10=1s (slow), 11=2s (very slow) │
  ├─────────┼───────────────┼─────────────────────────────────────────────────────────────────────┤
  │ SW4     │ Reset         │ Keep UP to hold counter at 0, release to continue                   │
  ├─────────┼───────────────┼─────────────────────────────────────────────────────────────────────┤
  │ SW5-SW7 │ Pattern Mode  │ Select counting pattern (see table below)                           │
  └─────────┴───────────────┴─────────────────────────────────────────────────────────────────────┘

  Pattern Modes (SW5-SW7):

  ┌──────────────────────┬───────────────┬────────────────────────────────────────────────┐
  │ Binary (SW7-SW6-SW5) │     Mode      │            What You'll See on LEDs             │
  ├──────────────────────┼───────────────┼────────────────────────────────────────────────┤
  │ 000                  │ Normal Binary │ Counts 0→255 (or 255→0) showing binary numbers │
  ├──────────────────────┼───────────────┼────────────────────────────────────────────────┤
  │ 001                  │ Odd Numbers   │ Only odd: 1,3,5,7...255                        │
  ├──────────────────────┼───────────────┼────────────────────────────────────────────────┤
  │ 010                  │ Even Numbers  │ Only even: 0,2,4,6...254                       │
  ├──────────────────────┼───────────────┼────────────────────────────────────────────────┤
  │ 011                  │ Powers of 2   │ 1→2→4→8→16→32→64→128 (single LED moving)       │
  ├──────────────────────┼───────────────┼────────────────────────────────────────────────┤
  │ 100                  │ Fibonacci     │ 1,1,2,3,5,8,13,21... (wraps at 255)            │
  ├──────────────────────┼───────────────┼────────────────────────────────────────────────┤
  │ 101-111              │ Normal        │ Falls back to normal counting                  │
  └──────────────────────┴───────────────┴────────────────────────────────────────────────┘

  Expected LED Behavior Examples:

  Example 1: Basic Counting
  - All switches DOWN → LEDs count up from 0 at normal speed
  - LED0 (rightmost) blinks fastest (LSB)
  - LED7 (leftmost) changes slowest (MSB)

  Example 2: Powers of 2
  - Set SW5=UP, SW6=DOWN, SW7=DOWN (Pattern Mode 1)
  - See single LED sweep from right to left: 00000001 → 00000010 → 00000100 → etc.

  Example 3: Fast Countdown
  - SW1=UP (count down)
  - SW2=DOWN, SW3=DOWN (fast speed)
  - LEDs rapidly count down: 255→254→253...

  Example 4: Paused Reset
  - SW0=UP (pause) → LEDs freeze at current value
  - SW4=UP (reset) → All LEDs turn OFF (counter = 0)
  - SW0=DOWN (resume) → Counting continues from 0

  UART Console Output:

  === Binary Counter Application ===
  SW0: Pause/Resume
  SW1: Change Direction (Up/Down)
  SW2-SW3: Speed Control (00=Fast, 01=Normal, 10=Slow, 11=Very Slow)
  SW4: Reset Counter
  SW5-SW7: Pattern Mode

  Counter:   0 (0x00) | Mode: 0 | Speed: 500ms
  Counter:   1 (0x01) | Mode: 0 | Speed: 500ms
  Counter:   2 (0x02) | Mode: 0 | Speed: 500ms
  ...

  ---
  Application 2: Knight Rider / Cylon Scanner

  // knight_rider.c
  #include <stdio.h>
  #include <xstatus.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xgpio.h"
  #include "xparameters.h"
  #include "sleep.h"

  XGpio_Config *gpio_config;
  XGpio gpio;

  void gpio_init(){
      int status;
      gpio_config = XGpio_LookupConfig(0); // Device ID is 0 (first GPIO instance)
      status = XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);
      if (status == XST_SUCCESS)
          xil_printf("Device INIT Successful.\n");
      else
          xil_printf("Device INIT Failed.\n");
  }

  // LED patterns for different effects
  u8 getKnightRiderPattern(int position, int width) {
      u8 pattern = 0;
      for (int i = 0; i < width && (position + i) < 8; i++) {
          pattern |= (1 << (position + i));
      }
      return pattern;
  }

  u8 getCylonPattern(int position, int width) {
      // Center-weighted pattern
      u8 pattern = (1 << position);
      if (width > 1 && position > 0) pattern |= (1 << (position - 1));
      if (width > 2 && position < 7) pattern |= (1 << (position + 1));
      return pattern;
  }

  u8 getBouncingPattern(int position) {
      return (1 << position) | (1 << (7 - position));
  }

  int main() {
      int position = 0;
      int direction = 1; // 1 = right, -1 = left
      u8 swValue = 0;
      int speed = 100;
      int width = 1;
      int effect = 0;

      init_platform();
      gpio_init();

      XGpio_SetDataDirection(&gpio, 1, 0xff); // Channel 1 = input (switches)
      XGpio_SetDataDirection(&gpio, 2, 0x00); // Channel 2 = output (LEDs)

      xil_printf("\n\r=== Knight Rider / Cylon Scanner ===\n\r");
      xil_printf("SW0-SW2: Speed Control (000=Fastest ... 111=Slowest)\n\r");
      xil_printf("SW3-SW4: LED Width (00=1 LED, 01=2 LEDs, 10=3 LEDs, 11=4 LEDs)\n\r");
      xil_printf("SW5-SW7: Effect Mode\n\r");
      xil_printf("  000 = Classic Knight Rider\n\r");
      xil_printf("  001 = Cylon (center-weighted)\n\r");
      xil_printf("  010 = Bouncing Dual\n\r");
      xil_printf("  011 = Chase Left\n\r");
      xil_printf("  100 = Chase Right\n\r");
      xil_printf("  101 = Expand/Contract\n\r");
      xil_printf("  110 = Random Sparkle\n\r");
      xil_printf("  111 = All Patterns Cycle\n\r\n\r");

      while (1) {
          swValue = XGpio_DiscreteRead(&gpio, 1);

          // SW0-SW2: Speed control (0=fastest, 7=slowest)
          int speedMode = swValue & 0x07;
          speed = 50 + (speedMode * 50); // 50ms to 400ms

          // SW3-SW4: Width control
          width = ((swValue >> 3) & 0x03) + 1; // 1 to 4 LEDs

          // SW5-SW7: Effect selection
          effect = (swValue >> 5) & 0x07;

          u8 ledPattern = 0;

          switch(effect) {
              case 0: // Classic Knight Rider
                  ledPattern = getKnightRiderPattern(position, width);
                  break;

              case 1: // Cylon (center-weighted)
                  ledPattern = getCylonPattern(position, width);
                  break;

              case 2: // Bouncing Dual
                  ledPattern = getBouncingPattern(position);
                  break;

              case 3: // Chase Left
                  ledPattern = (0xFF >> (8 - width)) << position;
                  direction = -1;
                  break;

              case 4: // Chase Right
                  ledPattern = (0xFF >> (8 - width)) << position;
                  direction = 1;
                  break;

              case 5: // Expand/Contract
                  {
                      static int expanding = 1;
                      int center = 3;
                      ledPattern = 0;
                      for (int i = 0; i < position; i++) {
                          if (center - i >= 0) ledPattern |= (1 << (center - i));
                          if (center + i <= 7) ledPattern |= (1 << (center + i));
                      }
                      if (position >= 4) expanding = 0;
                      if (position <= 0) expanding = 1;
                      direction = expanding ? 1 : -1;
                  }
                  break;

              case 6: // Random Sparkle
                  ledPattern = (rand() % 256);
                  break;

              case 7: // Cycle through all effects
                  {
                      static int cycleEffect = 0;
                      static int cycleCount = 0;
                      cycleCount++;
                      if (cycleCount > 20) {
                          cycleEffect = (cycleEffect + 1) % 6;
                          cycleCount = 0;
                          xil_printf("Effect: %d\n\r", cycleEffect);
                      }
                      // Recursive-like pattern selection
                      switch(cycleEffect) {
                          case 0: ledPattern = getKnightRiderPattern(position, 2); break;
                          case 1: ledPattern = getCylonPattern(position, 3); break;
                          case 2: ledPattern = getBouncingPattern(position); break;
                          case 3: ledPattern = (1 << position); break;
                          case 4: ledPattern = (0x03 << position); break;
                          case 5: ledPattern = (0xFF << position) & 0xFF; break;
                      }
                  }
                  break;
          }

          // Write pattern to LEDs
          XGpio_DiscreteWrite(&gpio, 2, ledPattern);

          // Update position
          position += direction;

          // Bounce at edges
          if (position >= (8 - width + 1)) {
              position = 8 - width;
              direction = -1;
          } else if (position < 0) {
              position = 0;
              direction = 1;
          }

          usleep(speed * 1000);
      }

      cleanup_platform();
      return 0;
  }

  How to Use - Knight Rider:

  Initial Setup:

  1. All switches DOWN (OFF position)
  2. Power on the board
  3. Connect UART terminal (115200 baud, 8N1)
  4. Program and run
  5. You'll immediately see classic Knight Rider effect (single LED sweeping left-right)

  Switch Functions:

  ┌──────────────┬─────────────────┬───────────────────────────────────────────┐
  │ Switch Group │    Function     │                  Values                   │
  ├──────────────┼─────────────────┼───────────────────────────────────────────┤
  │ SW0-SW2      │ Animation Speed │ 000=50ms (fastest) to 111=400ms (slowest) │
  ├──────────────┼─────────────────┼───────────────────────────────────────────┤
  │ SW3-SW4      │ LED Width       │ 00=1 LED, 01=2 LEDs, 10=3 LEDs, 11=4 LEDs │
  ├──────────────┼─────────────────┼───────────────────────────────────────────┤
  │ SW5-SW7      │ Effect Select   │ 8 different animation patterns            │
  └──────────────┴─────────────────┴───────────────────────────────────────────┘

  Effect Modes (SW5-SW7):

  ┌──────────────────────┬──────────────────────┬──────────────────────────────────────────┬──────────────────────────────────────────────────────┐
  │ Binary (SW7-SW6-SW5) │         Mode         │            Visual Description            │                 LED Pattern Example                  │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 000                  │ Classic Knight Rider │ Single LED sweeps back and forth         │ 00010000 → 00100000 → 01000000 → back                │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 001                  │ Cylon Scanner        │ LED with fade effect (center-weighted)   │ 00101000 sweeping                                    │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 010                  │ Bouncing Dual        │ Two LEDs mirror each other               │ 10000001 → 01000010 → 00100100                       │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 011                  │ Chase Left           │ Bar of LEDs chasing left                 │ 00011110 sliding left                                │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 100                  │ Chase Right          │ Bar of LEDs chasing right                │ 01111000 sliding right                               │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 101                  │ Expand/Contract      │ LEDs expand from center then contract    │ 00011000 → 00111100 → 01111110 → 11111111 → contract │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 110                  │ Random Sparkle       │ Random LED patterns                      │ Chaotic twinkling                                    │
  ├──────────────────────┼──────────────────────┼──────────────────────────────────────────┼──────────────────────────────────────────────────────┤
  │ 111                  │ Auto Cycle           │ Automatically cycles through all effects │ Changes every ~2 seconds                             │
  └──────────────────────┴──────────────────────┴──────────────────────────────────────────┴──────────────────────────────────────────────────────┘

  Expected LED Behavior Examples:

  Example 1: Classic Knight Rider (Default)
  - All switches DOWN
  - Single LED sweeps: → → → → ← ← ← ←
  - Like KITT from Knight Rider TV show

  Example 2: Cylon with 3-LED Width
  - SW3=UP, SW4=DOWN (width = 3)
  - SW5-SW7=DOWN (Knight Rider mode)
  - Three LEDs sweep together: ●●●○○○○○ → ○●●●○○○○ → ○○●●●○○○

  Example 3: Fast Bouncing Dual
  - SW0-SW2=DOWN (fastest speed)
  - SW5=UP, SW6=DOWN, SW7=DOWN (Bouncing Dual mode)
  - Two LEDs bounce from edges toward center:
  ●○○○○○○●  →
  ○●○○○○●○  →
  ○○●○○●○○  →
  ○○○●●○○○  →
  ○○●○○●○○  ← bounces back

  Example 4: Expand/Contract (Breathing Effect)
  - SW5=UP, SW6=DOWN, SW7=UP (mode 101)
  - LEDs grow from center then shrink:
  ○○○●●○○○  → expand
  ○○●●●●○○  →
  ○●●●●●●○  →
  ●●●●●●●●  → all ON
  ○●●●●●●○  ← contract
  ○○●●●●○○  ←
  ○○○●●○○○  ← repeat

  Example 5: Slow Wide Chase
  - SW0-SW2=UP (slowest = 400ms)
  - SW3-SW4=UP (width = 4 LEDs)
  - SW5-SW7=DOWN (Knight Rider/chase mode)
  - Four LEDs sweep slowly: ●●●●○○○○ sliding back and forth

  Example 6: Random Sparkle Show
  - SW5=UP, SW6=UP, SW7=DOWN (mode 110)
  - Random LED patterns rapidly change
  - Like a twinkling starfield

  Example 7: Auto Demonstration
  - SW5-SW7=ALL UP (mode 111)
  - Automatically demonstrates all effects
  - Changes pattern every ~2 seconds
  - Perfect for showing off the board!

  UART Console Output:

  === Knight Rider / Cylon Scanner ===
  SW0-SW2: Speed Control (000=Fastest ... 111=Slowest)
  SW3-SW4: LED Width (00=1 LED, 01=2 LEDs, 10=3 LEDs, 11=4 LEDs)
  SW5-SW7: Effect Mode
    000 = Classic Knight Rider
    001 = Cylon (center-weighted)
    010 = Bouncing Dual
    011 = Chase Left
    100 = Chase Right
    101 = Expand/Contract
    110 = Random Sparkle
    111 = All Patterns Cycle

  Effect: 0
  Effect: 1
  Effect: 2
  ...

  ---
  How to Build and Deploy:

  For Binary Counter:

  1. Copy binary_counter.c to 8leds_8sws_gpio/src/ (replacing or renaming helloworld.c)
  2. In Vitis: Right-click project → Build Project
  3. Program FPGA with hardware design
  4. Run application on processor

  For Knight Rider:

  1. Create new application project in Vitis (or duplicate existing)
  2. Copy knight_rider.c to the src/ folder
  3. Build and run

  Common Issues & Fixes:

  ┌────────────────────────────────┬──────────────────────┬────────────────────────────────────────────────────────────┐
  │             Issue              │        Cause         │                          Solution                          │
  ├────────────────────────────────┼──────────────────────┼────────────────────────────────────────────────────────────┤
  │ LEDs don't change              │ GPIO not initialized │ Check UART for "Device INIT Failed"                        │
  ├────────────────────────────────┼──────────────────────┼────────────────────────────────────────────────────────────┤
  │ Random LED behavior            │ Switches floating    │ Ensure switches have pull-up/down resistors                │
  ├────────────────────────────────┼──────────────────────┼────────────────────────────────────────────────────────────┤
  │ No UART output                 │ Wrong COM port/baud  │ Use 115200 baud, 8N1, no flow control                      │
  ├────────────────────────────────┼──────────────────────┼────────────────────────────────────────────────────────────┤
  │ Compilation error on device ID │ Wrong xparameters.h  │ Ensure using 0 as device ID, not XPAR_AXI_GPIO_0_DEVICE_ID │
  └────────────────────────────────┴──────────────────────┴────────────────────────────────────────────────────────────┘

  ---
  Both applications are fully self-contained and ready to run! 🚀
