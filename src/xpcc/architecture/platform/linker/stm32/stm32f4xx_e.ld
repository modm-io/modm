
MEMORY
{
	ROM (rx)    : ORIGIN = 0x08000000, LENGTH = 512k
	RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 96k

	SRAM1 (rwx) : ORIGIN = 0x20000000, LENGTH = 96k		/* Main internal SRAM1 */
}

INCLUDE stm32_ram.ld
