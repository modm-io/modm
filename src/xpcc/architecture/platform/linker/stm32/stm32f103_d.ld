
MEMORY
{
	ROM (rx)  : ORIGIN = 0x08000000, LENGTH = 384k
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64k
}

INCLUDE ../../common/memory_sections.ld
