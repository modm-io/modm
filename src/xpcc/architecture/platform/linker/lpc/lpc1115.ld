
MEMORY
{
	/* Define each memory region */
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 64k
	RAM (rwx) : ORIGIN = 0x10000100, LENGTH = 8k-256
}

PROVIDE(__main_stack_size = 1k);

INCLUDE "../stm32/stm32_ram.ld"
