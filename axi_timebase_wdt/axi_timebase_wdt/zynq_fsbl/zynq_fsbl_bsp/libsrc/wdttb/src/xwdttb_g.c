#include "xwdttb.h"

XWdtTb_Config XWdtTb_ConfigTable[] __attribute__ ((section (".drvcfg_sec"))) = {

	{
		"xlnx,axi-timebase-wdt-3.0", /* compatible */
		0x42600000, /* reg */
		0x0, /* xlnx,enable-window-wdt */
		0x0, /* xlnx,wdt-clk-freq-hz */
		{0xffff,  0xffff,  0xffff,  0xffff}, /* interrupts */
		0xffff /* interrupt-parent */
	},
	 {
		 NULL
	}
};