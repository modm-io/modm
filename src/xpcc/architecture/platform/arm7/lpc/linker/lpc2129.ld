
/* Memory Definitions */
MEMORY
{
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 256k - 8k
	RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 16k
}

INCLUDE ../../common/memory_sections.ld
