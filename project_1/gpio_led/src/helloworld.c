
#include <stdio.h>
#include <xstatus.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "sleep.h"  // Add this for sleep()


XGpio_Config *gpio_config;
XGpio gpio;

void gpio_init(){

    int status = XGpio_Initialize(&gpio, 0);
    if (status == XST_SUCCESS)
        xil_printf("Device INIT Successful\n.");
    else 
        xil_printf("Device INIT Failed\n.");    

} 

/* Alternatively, you can use the lookup function for safer initialization:

  void gpio_init(){
      gpio_config = XGpio_LookupConfig(0);
      XGpio_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddress);
  }
*/

int main()
{
    init_platform();
    gpio_init();
    XGpio_SetDataDirection(&gpio, 1, 0x0); // Set all 8 pins as output


while (1){
    XGpio_DiscreteWrite(&gpio, 1, 0xff); //Turn on all 8 
    sleep(1);
    XGpio_DiscreteWrite(&gpio, 1, 0x0);
    sleep(1);

}    
    
    return 0; //Never reached, but good practice
}
