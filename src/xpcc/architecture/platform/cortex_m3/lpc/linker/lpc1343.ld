
MEMORY
{
	ROM (rx)    : ORIGIN = 0x00000000, LENGTH = 32k
	RAM (rwx)   : ORIGIN = 0x10000000, LENGTH =  8k
}

INCLUDE ../../common/memory_sections.ld
