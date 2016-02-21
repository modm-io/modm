
MEMORY
{
	ROM (rx)    : ORIGIN = 0x08000000, LENGTH = 2m
	RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 160k + 32k + 128k	/* Entire continuous RAM section */

	SRAM1 (rwx) : ORIGIN = 0x20000000, LENGTH = 160k		/* Main internal SRAM1 */
	SRAM2 (rwx) : ORIGIN = 0x20028000, LENGTH = 32k			/* Auxiliary internal SRAM2 */
	SRAM3 (rwx) : ORIGIN = 0x20030000, LENGTH = 128k		/* Auxiliary internal SRAM3 */
	CCM (rw)    : ORIGIN = 0x10000000, LENGTH = 64k			/* Core Coupled Memory */
}

INCLUDE stm32_dccm.ld
