
MEMORY
{
	ROM (rx)    : ORIGIN = 0x00000000, LENGTH = 512k
	RAM (rwx)   : ORIGIN = 0x10000000, LENGTH = 32k
	SRAM0 (rwx) : ORIGIN = 0x2007C000, LENGTH = 16k
	SRAM1 (rwx) : ORIGIN = 0x20080000, LENGTH = 16k
}

INCLUDE ../../common/memory_sections.ld
