SECTIONS
{
	.xpcc_assertion : AT( ADDR(.text) + SIZEOF(.text) + SIZEOF(.data))
	{
		. = ALIGN(2);
		__assertion_table_start = . + SIZEOF(.data);

		KEEP(*(.assertion))

		__assertion_table_end = . + SIZEOF(.data);
	}
}
INSERT AFTER .data
