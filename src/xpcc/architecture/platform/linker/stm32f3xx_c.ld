
MEMORY
{
	ROM (rx)      : ORIGIN = 0x08000000, LENGTH = 256k
	RAM (rwx)     : ORIGIN = 0x20000000, LENGTH = 40k	/* Main internal SRAM */
	
	/* Core Coupled Memory, can only be accessed by the CPU */
	CCM (rwx)     : ORIGIN = 0x10000000, LENGTH = 8k
}

INCLUDE stm32f4xx.ld
