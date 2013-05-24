
MEMORY
{
	ROM (rx)  : ORIGIN = 0x08000000, LENGTH = 32k
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 10k
}

INCLUDE ../../common/memory_sections.ld
