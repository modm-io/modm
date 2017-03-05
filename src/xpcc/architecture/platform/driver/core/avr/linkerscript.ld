SECTIONS
{
	.xpcc_assertion : ALIGN(2)
	{
		__assertion_table_start = .;
		KEEP(*(.assertion))
		__assertion_table_end = .;
	}
}
INSERT AFTER .data
