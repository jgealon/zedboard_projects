/* Configuration file for llfifo driver in SDT mode */

#include "xllfifo.h"
#include "xparameters.h"

XLlFifo_Config XLlFifo_ConfigTable[] = {
	{
		"axi_fifo_mm_s_0",              /* Name */
		XPAR_AXI_FIFO_MM_S_0_BASEADDR,  /* BaseAddress */
		0x0,                             /* Axi4BaseAddress (not used) */
		0x0,                             /* Datainterface */
		0,                               /* IntId */
		0                                /* IntrParent */
	},
	{
		NULL,                            /* Terminator entry */
		0,
		0,
		0,
		0,
		0
	}
};
