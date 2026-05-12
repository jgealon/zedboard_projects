 #include <stdio.h>
  #include "platform.h"
  #include "xil_printf.h"
  #include "xscutimer.h"
  #include "xparameters.h"
  #include "sleep.h"

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
      u32 tmrCount = 333333333*5;  // 5 seconds
      u32 iteration = 0;

      init_platform();

      // ⏱️ Give terminal time to connect after boot
      sleep(5);

      xil_printf("\r\n\r\n");
      xil_printf("========================================\r\n");
      xil_printf("   SCU TIMER REPEATING LOOP TEST\r\n");
      xil_printf("========================================\r\n\r\n");

      tmr_init();

      xil_printf("\r\n=== Starting Timer Loop ===\r\n\r\n");

      while(1){  // Outer loop - repeats forever
          iteration++;

          xil_printf("\r\n[Run %lu] Starting timer...\r\n", iteration);

          XScuTimer_DisableAutoReload(&tmr);
          XScuTimer_LoadTimer(&tmr, tmrCount);

          // ⭐ CRITICAL: Clear the expired flag from previous iteration!
          XScuTimer_ClearInterruptStatus(&tmr);

          XScuTimer_Start(&tmr);

          xil_printf("[Run %lu] Timer started. Waiting 5 seconds...\r\n", iteration);

          // Poll with countdown display
          u32 loop_count = 0;
          u32 last_seconds = 5;

          while(!XScuTimer_IsExpired(&tmr)) {
              loop_count++;

              // Display countdown every 500,000 iterations (~0.5 sec)
              if(loop_count % 500000 == 0) {
                  u32 current = XScuTimer_GetCounterValue(&tmr);
                  u32 seconds_left = current / 333333333;

                  if(seconds_left != last_seconds) {
                      xil_printf("  ... %lu seconds remaining\r\n", seconds_left);
                      last_seconds = seconds_left;
                  }
              }
          }

          xil_printf("[Run %lu] ✓ Timer expired! (took %lu loop iterations)\r\n\r\n",
                     iteration, loop_count);

          sleep(1);  // Pause before next run
      }

      cleanup_platform();
      return 0;
  }
