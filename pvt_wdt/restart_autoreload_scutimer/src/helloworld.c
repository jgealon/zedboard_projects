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

      //Give terminal time to connect after boot
      sleep(5);

      xil_printf("\r\n\r\n");
      xil_printf("========================================\r\n");
      xil_printf("   SCU TIMER REPEATING LOOP TEST\r\n");
      xil_printf("========================================\r\n\r\n");

      tmr_init();

      xil_printf("\r\n=== Starting Auto-Reload Timer Loop ===\r\n\r\n");

      // Setup auto-reload timer ONCE
      XScuTimer_Stop(&tmr);
      XScuTimer_DisableAutoReload(&tmr);
      XScuTimer_LoadTimer(&tmr, tmrCount);

      xil_printf("Initial setup:\n");
      xil_printf("Load value: %lu (= %lu seconds)\n", tmrCount, tmrCount / 333333333);
      xil_printf("Counter after load: %lu\n", XScuTimer_GetCounterValue(&tmr));
      xil_printf("Auto-reload enabled: YES\n\n");

      XScuTimer_Start(&tmr);

      while(1){  // Outer loop - repeats forever
          iteration++;

          xil_printf("\r\n[Run %lu] Waiting for timer expiry...\r\n", iteration);

          // Check counter BEFORE clearing interrupt
          u32 counter_before_clear = XScuTimer_GetCounterValue(&tmr);
          xil_printf("  Counter BEFORE clear: %lu (= %lu seconds)\r\n",
                     counter_before_clear, counter_before_clear / 333333333);

          
          // Check counter AFTER clearing interrupt
          u32 counter_after_clear = XScuTimer_GetCounterValue(&tmr);
          xil_printf("  Counter AFTER clear: %lu (= %lu seconds)\r\n",
                     counter_after_clear, counter_after_clear / 333333333);

          // Let's check if we need to reload the Load Register
          xil_printf("  Reloading Load Register to: %lu\r\n", tmrCount);
          XScuTimer_LoadTimer(&tmr, tmrCount);

          u32 counter_after_load = XScuTimer_GetCounterValue(&tmr);
          xil_printf("  Counter AFTER manual load: %lu (= %lu seconds)\r\n",
                     counter_after_load, counter_after_load / 333333333);

          xil_printf("[Run %lu] Timer counting. Waiting 5 seconds...\r\n", iteration);

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
                      xil_printf("  Current: %lu | ", current);
                      xil_printf("%lu seconds remaining\r\n", seconds_left);
                      last_seconds = seconds_left;
                  }
              }
              // REMOVED: Don't print status every iteration - breaks timing!
          }

          xil_printf("[Run %lu] ✓ Timer expired!\r\n", iteration);

          // Check what happened immediately after expiry
          u32 counter_after_expire = XScuTimer_GetCounterValue(&tmr);
          xil_printf("  Counter immediately after expire: %lu (= %lu seconds)\r\n",
                     counter_after_expire, counter_after_expire / 333333333);
          xil_printf("  Timer should have auto-reloaded to: %lu (= 5 seconds)\r\n", tmrCount);

          // With auto-reload, timer automatically reloaded and is counting again
          // No need to Stop/Start - just clear the flag on next iteration
        XScuTimer_Stop(&tmr);
        // CRITICAL: Clear expired flag from previous iteration
        XScuTimer_ClearInterruptStatus(&tmr);
        xil_printf("TIME EXPIRED : Delay of 5 Seconds.\n");
        XScuTimer_RestartTimer(&tmr);
        XScuTimer_Start(&tmr);
        sleep(1);  // Pause before next run
      }

      cleanup_platform();
      return 0;
  }
