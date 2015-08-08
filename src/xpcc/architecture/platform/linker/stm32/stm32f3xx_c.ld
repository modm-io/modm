
MEMORY
{
	ROM (rx)  : ORIGIN = 0x08000000, LENGTH = 256k  /* Main internal FLASH */
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 40k	/* Main internal SRAM */
	CCM (rwx) : ORIGIN = 0x10000000, LENGTH = 8k    /* Core Coupled Memory */
}

INCLUDE stm32_iccm.ld
