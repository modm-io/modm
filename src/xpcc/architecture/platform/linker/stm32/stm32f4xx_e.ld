
MEMORY
{
	ROM (rx)      : ORIGIN = 0x08000000, LENGTH = 512k
	RAM (rwx)     : ORIGIN = 0x20000000, LENGTH = 112k	/* Main internal SRAM1 */
	RAM2 (rwx)    : ORIGIN = 0x2001C000, LENGTH = 16k	/* Auxiliary internal SRAM2 */

	/* Core Coupled Memory, can only be accessed by the CPU */
	CCMRAM (rwx)      : ORIGIN = 0x10000000, LENGTH = 64k
}

INCLUDE stm32f4xx.ld
