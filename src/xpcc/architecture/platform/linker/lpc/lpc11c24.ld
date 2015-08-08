
MEMORY
{
	/* Define each memory region */
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 32k
	RAM (rwx) : ORIGIN = 0x10000100, LENGTH = 8k
}

PROVIDE(__handler_stack_size = 124 + 4);
PROVIDE(__thread_stack_size  = 508 + 4);

INCLUDE "../stm32/stm32_ram.ld"
