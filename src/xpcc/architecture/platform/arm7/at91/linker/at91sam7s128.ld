
/* Memory Definitions */
/* Flash is mapped to 0x0 at startup, may be remapped by the Software */
MEMORY
{
	ROM (rx)  : ORIGIN = 0x00100000, LENGTH = 128k
	RAM (rwx) : ORIGIN = 0x00200000, LENGTH = 32k
}

INCLUDE ../../common/memory_sections.ld
