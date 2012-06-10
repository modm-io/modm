
/* Memory Definitions */
MEMORY
{
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 512k - 8k
	RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 32k
}

INCLUDE ../../common/memory_sections.ld
