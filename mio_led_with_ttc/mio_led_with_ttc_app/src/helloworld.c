#include <stdio.h>
#include <string.h>
#include <xil_types.h>
#include "platform.h"
#include "xil_printf.h"
#include "xttcps.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "sleep.h"

 
typedef struct {
	u32 outputFreq;
	XInterval interval;
	u8 prescaler;
	u32 option;
 
} ttc_setup;
 
 
XTtcPs_Config *ttc_config;
XTtcPs ttc;
XGpioPs_Config *gpio_config;
XGpioPs gpio;

void gpio_init(u32 pin){
    u32 status;

    gpio_config = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_BASEADDR);
    status = XGpioPs_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddr);

    if(status == XST_SUCCESS)
		xil_printf("GPIO Init Successful\r\n");
    else
		xil_printf("GPIO Init Failed\r\n");

    // Configure pin as output
    XGpioPs_SetDirectionPin(&gpio, pin, 1);  // 1 = output
    XGpioPs_SetOutputEnablePin(&gpio, pin, 1);  // Enable output

    xil_printf("GPIO Pin %lu configured as output\r\n", pin);
} 

void ttc_init(){
	int status;


	ttc_config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_BASEADDR);
	status = XTtcPs_CfgInitialize(&ttc,ttc_config, ttc_config->BaseAddress);

	if(status == XST_SUCCESS)
		xil_printf("TTC Init Successful\r\n");
	else
		xil_printf("TTC Init Failed\r\n");

	status = XTtcPs_SelfTest(&ttc);

	if(status == XST_SUCCESS)
		xil_printf("Self Test Successful\r\n");
	else
		xil_printf("Self Test Failed\r\n");


}
 
 
int main()
{
    // ⭐ Zedboard MIO to LED Mapping (Confirmed):
    // MIO0  → LD11 (User LED) ✅ Working
    // MIO7  → LD9  (User LED) ✅ Working
    // MIO9  → UART LED (TX/RX indicator, not user LED)
    //
    // Try these for more user LEDs:
    // MIO10, MIO47, MIO50, MIO51, MIO52, MIO53

    #define LED_PIN  7   // ← Change to test different LEDs

    u32 led_state = 0;
    u32 iteration = 0;

	ttc_setup data = {1,0,0,0};  // 1 Hz = 1 second period

    init_platform();

    xil_printf("\r\n========================================\r\n");
    xil_printf("   MIO LED Blink with TTC\r\n");
    xil_printf("========================================\r\n\r\n");

    xil_printf("Using MIO Pin: %d\r\n\r\n", LED_PIN);

    ttc_init();
    gpio_init(LED_PIN);

    xil_printf("\r\n=== Configuring TTC ===\r\n");
    xil_printf("Target frequency: %lu Hz\r\n", data.outputFreq);

    XTtcPs_Stop(&ttc);
    data.option = data.option | XTTCPS_OPTION_DECREMENT | XTTCPS_OPTION_INTERVAL_MODE;
    XTtcPs_SetOptions(&ttc, data.option);
    XTtcPs_CalcIntervalFromFreq(&ttc, data.outputFreq, &(data.interval), &(data.prescaler));

    xil_printf("Calculated interval: %lu\r\n", data.interval);
    xil_printf("Calculated prescaler: %d\r\n", data.prescaler);

    XTtcPs_SetPrescaler(&ttc, data.prescaler);
    XTtcPs_SetInterval(&ttc, data.interval);

    xil_printf("\r\n=== Starting LED Blink Loop ===\r\n");
    xil_printf("LED will blink every 1 second\r\n\r\n");

    // Start with LED OFF
    XGpioPs_WritePin(&gpio, LED_PIN, 0);

    while(1){
 
    	 XTtcPs_Start(&ttc);
    	 while(XTtcPs_GetCounterValue(&ttc));
    	 XTtcPs_Stop(&ttc);
    	 iteration++;
         // Toggle LED
         led_state = !led_state;
         XGpioPs_WritePin(&gpio, LED_PIN, led_state);
         xil_printf("[%lu] LED %s\r\n", iteration, led_state ? "ON " : "OFF");
    	 XTtcPs_ResetCounterValue(&ttc);
         
 
    }
 
 
 
    cleanup_platform();
    return 0;
}