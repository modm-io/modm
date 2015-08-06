
MEMORY
{
	ROM (rx)      : ORIGIN = 0x08000000, LENGTH = 1m
	RAM (rwx)     : ORIGIN = 0x20010000, LENGTH = 240k	/* Main internal SRAM1 */
	AUX_RAM (rwx) : ORIGIN = 0x2004C000, LENGTH = 16k	/* Auxiliary internal SRAM2 */

	/* Data Tightly Coupled Memory */
	DTCM (rwx)    : ORIGIN = 0x20000000, LENGTH = 64k
}

INCLUDE stm32f7xx.ld
