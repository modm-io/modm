
MEMORY
{
	ROM (rx)  : ORIGIN = 0x08000000, LENGTH = 64k
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20k
}

INCLUDE ../../common/memory_sections.ld
