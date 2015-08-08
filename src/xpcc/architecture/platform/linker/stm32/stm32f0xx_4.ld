
MEMORY
{
	ROM (rx)      : ORIGIN = 0x08000000, LENGTH = 16k
	RAM (rwx)     : ORIGIN = 0x20000000, LENGTH = 8k	/* Main internal SRAM1 */
}

INCLUDE stm32_ram.ld
