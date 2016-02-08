/*
 *                           LINKER  SCRIPT
 *
 * The Linker Script defines how the code and data emitted by the GNU C compiler
 * and assembler are  to be loaded into memory (code goes into ROM, variables
 * go into RAM).
 *
 * Any symbols defined in the Linker Script are automatically global and
 * available to the rest of the program.
 *
 * Example for STM32F427: (SRAM3 not available on F40* / F41* devices!
 *
 *                          MEMORY MAP (RAM)
 *
 * Auxiliary internal SRAM3 (64 kB):
 *   only accessible by CPU via S-Bus (less efficient than I-Bus), otherwise fully DMA capable
 *                |                                 | 0x2003 0000 <---- __ram_end, __heap_end
 *                |---------------------------------| 0x2002 FFFF
 *                |                                 |
 *                |                                 |
 *                |                                 |
 *                |---------------------------------| 0x2002 0000
 *
 * Auxiliary internal SRAM2 (16 kB):
 *   only accessible by CPU via S-Bus (less efficient than I-Bus), otherwise fully DMA capable
 *                |                                 | 0x2002 0000 <---- __ram_end, __heap_end
 *                |---------------------------------| 0x2001 FFFF       (F40* / F41* devices)
 *                |                                 |
 *                |                                 |
 *                |                                 |
 *                |---------------------------------| 0x2001 C000
 *
 *
 * Main internal SRAM1 (112 kB):
 *                |                                 | 0x2001 C000
 *                |---------------------------------| 0x2001 BFFF
 *                |                                 |
 *                |               ^                 |
 *                |               |                 |
 *                |            Heap Area            |
 *                |         (grows upwards)         |
 *                |                                 | <------- __heap_start
 *      +-------> |---------------------------------| <------- __noinit_end
 *      |         |                                 |
 *   .noinit      |     Global data not cleared     |
 *      |         |           after reset           | <------- __noinit_start
 *      +-------> |---------------------------------| <------- __bss_end
 *      |         |                                 |
 *    .bss        |     uninitialized variables     |
 *      |         |                                 | <------- __bss_start
 *      +-------> |---------------------------------| <------- __data_end
 *      |         |                                 |
 *      |         |                                 |
 *    .data       |     initialized variables       |
 *      |         |                                 |
 *      |         |                                 | <------- __data_start
 *      +-------> |---------------------------------| <------- __vector_table_ram_end
 *      |         |                                 |
 *  .vectors      |    Interrupt Vectors (in RAM)   |
 *      |         |        (if re-mapped)           | <------- __vector_table_ram_start
 *      +-------> |---------------------------------| 0x2000 0000 <--- __ram_start
 *
 *
 * CCM (Core Coupled memory) (64 kB):
 *   only accessible by the CPU (no DMA) via D-Bus, not I-Bus!
 *                |                                 | 0x1001 0000
 *                |---------------------------------| 0x1000 FFFF
 *                |           Unused RAM            |
 *                |    (perhaps usable for heap?)   |
 *      +-------> |---------------------------------| <------- __fastdata_end
 *      |         |                                 |
 *  .fastdata     |     initialized variables       |
 *      |         |                                 | <------- __fastdata_start
 *      +-------> |---------------------------------| <------- __process_stack_top, __stack_end
 *                |       Process Stack (psp)       |
 *                |       (grows downwards)         |
 *                |               |                 |
 *                |               v                 |
 *                |---------------------------------| <------- __main_stack_top
 *                |        Main Stack (msp)         |
 *                |       (grows downwards)         |
 *                |               |                 |
 *                |               v                 | <------- __stack_start
 *                |---------------------------------| 0x1000 0000
 *
 *
 *                          MEMORY MAP (Flash)
 *
 *                |                                 |
 *                |---------------------------------| 0x0802 0000
 *                |                                 | 0x0801 FFFF
 *                |                                 |
 *                |                                 |
 *                |          Unused flash           |
 *                |                                 |
 *                |                                 |
 *                |                                 |
 *      +-------> |---------------------------------|
 *      |         |                                 |
 *      |         |                                 |
 *      |         |                                 |
 *    .data       |      copy of .data area         |
 *      |         |                                 |
 *      |         |                                 |
 *      |         |                                 | <------- __data_load
 *      +-------> |---------------------------------|
 *      |         |    Static Constructors (C++)    |
 *      |         | ------------------------------- |
 *      |         |                                 |
 *    .text       |                                 |
 *      |         |          C/C++ code             |
 *      |         |     (contains .fastcode)        |
 *      |         |                                 | <------- __fastcode_load, __fastcode_start, __fastcode_end
 *      +-------> |---------------------------------|
 *      |         |                                 |
 *  .fastdata     |     Variables to be loaded      |
 *      |         |         into CCM RAM            | <------- __fastdata_load
 *      +-------> |---------------------------------|
 *      |         |                                 |
 *   .reset       |    Interrupt Vectors (in ROM)   | <------- __vector_table_ram_load
 *      |         |       (copy of .vectors)        | <------- __vector_table_rom_start
 *      +-------> |---------------------------------| 0x0800 0000
 *
 * The first two words (32-bit) in the Flash defines the initial stack pointer
 * location and the reset handler location.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* First executable instruction in an output file */
ENTRY(Reset_Handler)

/* force linker to include the syscalls from libxpcc.a */
EXTERN(_sbrk_r)
EXTERN(_init)

PROVIDE(__ram_start = ORIGIN(RAM));
PROVIDE(__ram_end   = ORIGIN(RAM) + LENGTH(RAM));
PROVIDE(__heap_end  = ORIGIN(RAM) + LENGTH(RAM));

/* the thread stack is used only for reporting hard fault conditions! It may therefore be small. */
PROCESS_STACK_SIZE  = DEFINED(__process_stack_size) ? (__process_stack_size & ~0x7) : 256;
/* the handler stack is used for main program as well as interrupts */
MAIN_STACK_SIZE     = DEFINED(__main_stack_size)    ? (__main_stack_size & ~0x7)    : 3k;
/* combined stack size */
TOTAL_STACK_SIZE = MAIN_STACK_SIZE + PROCESS_STACK_SIZE;

SECTIONS
{
	.reset :
	{
		__vector_table_rom_start = .;					/* address in ROM */
		/* Initial stack address, Reset and NMI handler */
		KEEP(*(.reset))
		. = ALIGN(4);
	} >ROM

	.vectors :  /* Vector table in RAM, only if remapped */
	{
		__vector_table_ram_load = LOADADDR (.vectors);		/* address in ROM */
		__vector_table_ram_start = .;						/* address in RAM */

		/* used for vectors remapped to RAM */
		KEEP(*(.vectors))

		. = ALIGN (4);
		__vector_table_ram_end = .;
	} >RAM AT>ROM

	.stack (NOLOAD) :
	{
		__stack_start = . ;

		. += MAIN_STACK_SIZE;
		. = ALIGN (8);
		__main_stack_top = . ;

		. += PROCESS_STACK_SIZE;
		. = ALIGN (8);
		__process_stack_top = . ;

		__stack_end = .;
	} >CCM

	.fastdata :
	{
		__fastdata_load = LOADADDR (.fastdata);		/* address in ROM */
		__fastdata_start = .;						/* address in RAM */

		KEEP(*(.fastdata))

		. = ALIGN(4);
		__fastdata_end = .;
	} >CCM AT>ROM

	.text :
	{
		/* Create a symbol for each input file in the current section, set to
		 * the address of the first byte of data written from that input file.
		 * */
		CREATE_OBJECT_SYMBOLS

		/* ABOUT .gnu.linkonce.* sections
		 *
		 * Unlike other input section types a section that is prefixed with
		 * .gnu.linkonce. is treated differently by the linker. If for example
		 * .gnu.linkonce.t.abc appears in two or more different object files then
		 * the linker will only keep one and discard the others.
		 *
		 * This is done is to support C++ vague linkage.  It is related to
		 * C++'s ODR (One Definition Rule), and can cause surprises if ODR is
		 * violated.
		 *
		 * For example if you compile with RTTI turned on, you'll see linkonce
		 * sections for all the RTTI of all the classes in the translation unit
		 * of that object file.
		 * Or if you keep generated functions for inline functions
		 * (-fkeep-inline-functions), you'll see linkonce sections for all the
		 * emitted kept inline functions.
		 *
		 * Might be replaced by ELF section groups in newer versions of ld.
		 */
		*(.fastcode .text .text.* .gnu.linkonce.t.*)
		. = ALIGN (4);

		/* From the Cortex-M3 Technical Reference Manual:
		 *
		 * """
		 * 14.5 System Interface:
		 *
		 * The system interface is a 32-bit AHB-Lite bus. Instruction and vector fetches, and data and debug accesses to
		 * the System memory space, 0x20000000 - 0xDFFFFFFF, 0xE0100000 - 0xFFFFFFFF, are performed over this bus.
		 *
		 * 14.5.6 Pipelined instruction fetches:
		 *
		 * To provide a clean timing interface on the System bus, instruction and vector fetch requests to this bus are
		 * registered. This results in an additional cycle of latency because instructions fetched from the System bus
		 * take two cycles. This also means that back-to-back instruction fetches from the System bus are not possible.
		 *
		 *  Note:
		 *    Instruction fetch requests to the ICode bus are not registered.
		 *    Performance critical code must run from the ICode interface.
		 *
		 * """
		 *
		 * So for STM32's where the CCM is _not_ connected to the I-Bus, we execute .fastcode from Flash.
		 */
		__fastcode_load = .;
		__fastcode_start = .;
		__fastcode_end = .;

		/* Position independent code will call non-static functions via the
		 * Procedure Linkage Table or PLT. This PLT does not exist in .o files.
		 * In a .o file, use of the PLT is indicated by a special relocation.
		 * When the program linker processes such a relocation, it will create
		 * an entry in the PLT
		 */
		*(.plt)
		. = ALIGN(4);

		/* .ARM.extab names a section that contains exception unwinding information */
		*(.ARM.extab* .gnu.linkonce.armextab.*)

		/* .gcc_except_table is an input section name, which gcc uses
		 * for information used to unwind the stack when an exception occurs. */
		*(.gcc_except_table)

		/* When gcc generates code that handles exceptions, it produces tables
		 * that describe how to unwind the stack. These tables are found in
		 * the .eh_frame and .eh_frame_hdr section.
		 *
		 * See http://www.airs.com/blog/archives/460 or
		 * http://www.codesourcery.com/public/cxx-abi/exceptions.pdf
		 */
		*(.eh_frame_hdr)
		*(.eh_frame)

		*(.gnu.warning)
	} >ROM

	.rodata : ALIGN (4)
	{
		*(.rodata .rodata.* .gnu.linkonce.r.*)

		. = ALIGN(4);
		KEEP(*(.init))

		. = ALIGN(4);
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		KEEP(*(.fini))

		. = ALIGN(4);
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		PROVIDE_HIDDEN (__fini_array_end = .);

		/* These are for static constructors and destructors under ELF */
		PROVIDE(__ctors_start__ = .);
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*crtend.o(.ctors))
		PROVIDE(__ctors_end__ = .);

		/*PROVIDE(__dtors_start__ = .);
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*crtend.o(.dtors))
		PROVIDE(__dtors_end__ = .);*/

		*(.init .init.*)
		*(.fini .fini.*)
	} >ROM

	/* .ARM.exidx names a section that contains index entries for
	 * section unwinding. It is sorted, so has to go in its own output section.
	 */
	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} >ROM
	_etext = .;

	/* initialized variables */
	.data : ALIGN (8)
	{
		__data_load = LOADADDR (.data);		/* address in ROM */
		__data_start = .;					/* address in RAM */

		KEEP(*(.jcr))
		*(.got.plt) *(.got)
		*(.shdata)

		*(.data .data.* .gnu.linkonce.d.*)

		. = ALIGN (4);
		__data_end = .;
	} >RAM AT>ROM

	/* uninitialized variables */
	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		__bss_start = . ;

		*(.shbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)

		. = ALIGN (4);
		__bss_end = .;
	} >RAM

	/* Global data not cleared after reset.  */
	.noinit (NOLOAD) :
	{
		. = ALIGN (4);
		PROVIDE (__noinit_start = .);

		KEEP(*(.noinit*))

		. = ALIGN (4);
		PROVIDE (__noinit_end = .);
		. = ALIGN (4);
		PROVIDE (__heap_start = .);
	} >RAM

	. = ALIGN(4);
	_end = . ;
	__end = . ;
	PROVIDE(end = .);

	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }

	/* DWARF debug sections.
	 Symbols in the DWARF debugging sections are relative to the beginning
	 of the section so we begin them at 0.  */

	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
	.debug_varnames  0 : { *(.debug_varnames) }

	.note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
	.ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
	/DISCARD/ : { *(.note.GNU-stack)  }
}
