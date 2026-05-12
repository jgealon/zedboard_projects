 #include <stdio.h>
#include <sys/unistd.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xscutimer.h"
  #include "xparameters.h"
  #include "unistd.h"
  //#include "sleep.h"

  XScuTimer_Config *tmr_config;
  XScuTimer tmr;

  void tmr_init(){
      tmr_config = XScuTimer_LookupConfig(XPAR_SCUTIMER_BASEADDR);
      s32 status = XScuTimer_CfgInitialize(&tmr, tmr_config, tmr_config->BaseAddr);

      if(status == XST_SUCCESS)
          xil_printf("TMR INIT SUCCESSFUL\n");
      else
          xil_printf("TMR INIT FAILED\n");

      status = XScuTimer_SelfTest(&tmr);

      if(status == XST_SUCCESS)
          xil_printf("TMR SELFTEST SUCCESSFUL\n");
      else
          xil_printf("TMR SELFTEST FAILED\n");
  }

  int main()
  {
      // ⚠️ With prescaler, need to adjust count!
      // Timer clock = 333.333 MHz / (2^prescaler)
      // For prescaler=0: 333.333 MHz (normal)
      // For prescaler=1: 166.666 MHz (half speed)

      u8 prescaler_value = 1;  // Change this to 0 or 1 to test
      u32 base_count = 333333333;  // 1 second at full speed
      u32 tmrCount = base_count * 5;  // 5 seconds
      u32 tmp = 0;

      init_platform();

      //Give terminal time to connect after boot
      sleep(5);

      xil_printf("\r\n========================================\r\n");
      xil_printf("   SCU TIMER PRESCALER TEST\r\n");
      xil_printf("========================================\r\n\r\n");

      tmr_init();

      // Test with different prescaler values
      xil_printf("\r\n--- Testing with Prescaler = %d ---\r\n", prescaler_value);
      xil_printf("Timer frequency will be: 333.333 MHz / (2^%d) = ", prescaler_value);
      if(prescaler_value == 0)
          xil_printf("333.333 MHz (no division)\r\n");
      else if(prescaler_value == 1)
          xil_printf("166.666 MHz (half speed)\r\n");

      xil_printf("Expected time to expire: %d seconds\r\n\r\n",
                 prescaler_value == 0 ? 5 : 10);

      XScuTimer_Stop(&tmr);
      XScuTimer_DisableAutoReload(&tmr);

      // Read prescaler before setting
      u8 prescaler_before = XScuTimer_GetPrescaler(&tmr);
      xil_printf("Prescaler BEFORE setting: %d\r\n", prescaler_before);

      XScuTimer_SetPrescaler(&tmr, prescaler_value);

      // Read prescaler after setting
      u8 prescaler_after = XScuTimer_GetPrescaler(&tmr);
      xil_printf("Prescaler AFTER setting: %d\r\n", prescaler_after);

      XScuTimer_LoadTimer(&tmr, tmrCount);

      xil_printf("Counter loaded with: %lu\r\n", tmrCount);
      u32 counter_after_load = XScuTimer_GetCounterValue(&tmr);
      xil_printf("Counter value after load: %lu\r\n", counter_after_load);

      XScuTimer_Start(&tmr);

      // Calculate effective clock frequency with prescaler
      u32 effective_clock = 333333333 / (1 << prescaler_value);

      // Show initial counter value immediately
      u32 initial_counter = XScuTimer_GetCounterValue(&tmr);
      u32 initial_seconds = initial_counter / effective_clock;

      xil_printf("\r\n⚠️ NOTE: Cannot use usleep() during timer operation!\r\n");
      xil_printf("Effective timer clock: %lu MHz (÷ by %d)\r\n",
                 effective_clock / 1000000, (1 << prescaler_value));
      xil_printf("Timer started! Initial time: %lu seconds\r\n", initial_seconds);
      xil_printf("Using tight polling loop...\r\n");

      // ⭐ Use TIGHT polling loop - no usleep!
      // This is the only way that works with SCU Timer
      while(!XScuTimer_IsExpired(&tmr)) {
          tmp++;

          // Show progress every 5 million iterations
          if(tmp % 5000000 == 0) {
              u32 current = XScuTimer_GetCounterValue(&tmr);
              u32 seconds_left = current / effective_clock;  // Use effective clock!
              xil_printf("  %lu million iterations, %lu seconds remaining\r\n",
                         tmp / 1000000, seconds_left);
          }
      }

      xil_printf("\r\n=== RESULTS ===\r\n");
      xil_printf("TIME EXPIRED\r\n");
      xil_printf("Prescaler: %d\r\n", prescaler_value);
      xil_printf("Loop iterations (tmp): %lu\r\n", tmp);
      xil_printf("Iterations in millions: %lu M\r\n\r\n", tmp / 1000000);

      xil_printf("💡 Expected behavior:\r\n");
      xil_printf("   Prescaler=0: ~65 million iterations (5 seconds)\r\n");
      xil_printf("   Prescaler=1: ~130 million iterations (10 seconds)\r\n");
      xil_printf("   Ratio should be approximately 2:1\r\n");

      cleanup_platform();
      return 0;
  }
