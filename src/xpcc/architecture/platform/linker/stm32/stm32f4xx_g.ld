
MEMORY
{
	ROM (rx)    : ORIGIN = 0x08000000, LENGTH = 1m
	RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 112k + 16k	/* Entire continuous RAM section */

	SRAM1 (rwx) : ORIGIN = 0x20000000, LENGTH = 112k		/* Main internal SRAM1 */
	SRAM2 (rwx) : ORIGIN = 0x2001C000, LENGTH = 16k			/* Auxiliary internal SRAM2 */
	CCM (rw)    : ORIGIN = 0x10000000, LENGTH = 64k			/* Core Coupled Memory */
}

INCLUDE stm32_dccm.ld
