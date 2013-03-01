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
 * Example for AT91SAM7S256:
 * 
 *                          MEMORY MAP (RAM)
 *                |                                 | 0x00210000 <---- __ram_end, __head_end
 *                |---------------------------------| 
 *                |                                 | 
 *                |               ^                 | 
 *                |               |                 | 
 *                |            Heap Area            | 
 *                |         (grows upwards)         | 
 *                |                                 | <------- __head_start
 *                |---------------------------------| <------- __abort_stack_top, __stack_end
 *                |     Abort (ABT, UDF) Stack      | 
 *                |---------------------------------| <------- __svc_stack_top
 *                |      Supervisor (SVC) Stack     | 
 *                |---------------------------------| <------- __irq_stack_top
 *                |            IRQ Stack            | 
 *                |---------------------------------| <------- __fiq_stack_top
 *                |            FIQ Stack            | 
 *                |---------------------------------| <------- __usr_stack_top
 *                |                                 | 
 *                |   Stack area for user program   | 
 *                |       (grows downwards)         | 
 *                |               |                 | 
 *                |               v                 | 
 *                |                                 | <------- __stack_start
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
 *      +-------> |---------------------------------| <------- __fastcode_end
 *      |         |                                 | 
 *  .fastcode     |       C-Functions in RAM        | 
 *      |         |    (e.g. Interrupt Handler)     | <------- __fastcode_start
 *      +-------> |---------------------------------| 0x00200040
 *      |         |                                 | 0x0020003F
 *  .vectors      |  Interrupt Vectors (re-mapped)  |
 *      |         |            64 bytes             | 
 *      +-------> |---------------------------------| 0x00200000 <--- __ram_start
 * 
 * 
 *                         MEMORY MAP (Flash)
 *                |                                 |
 *                |---------------------------------| 0x00140000
 *                |                                 | 0x0013FFFF
 *                |                                 |
 *                |                                 |
 *                |          Unused flash           |
 *                |                                 |
 *                |                                 |
 *                |                                 |
 *     +--------> |---------------------------------|
 *     |          |                                 |
 *     |          |                                 |
 *     |          |                                 |
 *   .data        |      copy of .data area         |
 *     |          |                                 |
 *     |          |                                 |
 *     |          |                                 | <------- __data_load
 *     +--------> |---------------------------------| 
 *     |          |    Static Constructors (C++)    |
 *     |          | ------------------------------- |
 *     |          |                                 |
 *   .text        |                                 |
 *     |          |          C/C++ code             |
 *     |          |                                 |
 *     |          |                                 |
 *     +--------> |---------------------------------| 
 *     |          |                                 | 
 * .fastcode      |     C-Functions to be loaded    | 
 *     |          |           into RAM              | <------- __fastcode_load
 *     +--------> |---------------------------------| 
 *     |          |                                 | 
 *     |          |         Startup Code            |
 *     |          |         (Assembler)             |
 *     |          |                                 |
 *  .reset        |---------------------------------| 0x00100040 _reset
 *     |          |                                 | 0x0010003F
 *     |          | Interrupt Vector Table (unused) | 
 *     |          |            64 bytes             | 
 *     +--------> |---------------------------------| 0x00100000 _startup (mapped to 0 after Reset)
 * 
 * 
 * Usage:
 *   Use __attribute__ ((long_call, section (".fastcode"))) to move functions
 *   into the RAM.
 */

/* Note for LPC2000 devices:
 * 
 * While setting the stack pointer please note that the Flash programming
 * routines use a section of the on-chip SRAM. In-System Programming (ISP) uses
 * the top 256 bytes and In-Application Programming (IAP) uses the top
 * 128 bytes of the on-chip SRAM. The application stack should not overlap
 * this area.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* First executable instruction in an output file */
ENTRY(_startup)

/* force linker to include the syscalls from libxpcc.a */
EXTERN(_sbrk_r)

PROVIDE(__ram_start = ORIGIN(RAM));
PROVIDE(__ram_end   = ORIGIN(RAM) + LENGTH(RAM));
PROVIDE(__head_end  = ORIGIN(RAM) + LENGTH(RAM));

/* Sizes of the stacks used by the application. Every Stack consumes four bytes
   by default for a Stack Boundary Marker (0xDEADBEEF).
   
   NOTE: you need to adjust these */
USR_STACK_SIZE = 508 + 4;

/* The IRQ_STACK needs 4 words * 4 byte = 16 byte per priority level. The
   AT91SAM7S has 8 level, if all levels are used than 128 bytes are needed.
   
   An additional 4 words (16 byte) are stored in the SVC_STACK per priority
   level. */
IRQ_STACK_SIZE = 128 + 4;

/* The FIQ stack is not used at all by the default FIQ wrapper. The wrapper
   only saves two words (8 bytes) on the SVC_STACK.
   
   If the size is set to zero the Boundary Marker overlaps with the marker
   from the IRQ stack and therefore won't be visible. Apart from that there
   will be no problem as long as the stack is not used while the Core is
   in FIQ mode. */
FIQ_STACK_SIZE = 0;

SVC_STACK_SIZE = 128 + 4;
ABORT_STACK_SIZE = 252 + 4;

SECTIONS
{
	.reset :
	{
		/* startup code (ARM vectors and reset handler) */
		*startup.o (.text)
		. = ALIGN(4);
	} >ROM
	
	/* used for vectors remapped to RAM */
	.vectors :
	{
		/* reserve 64 Bytes of RAM for the vector table */
		. = 0x40;
	} >RAM
	
	.fastcode :
	{
		__fastcode_load = LOADADDR (.fastcode);		/* address in ROM */
		__fastcode_start = .;						/* address in RAM */
		
		*(.fastcode)
		
		*(.glue_7t)		/* TODO: are these useful here or better only in .text? */
		*(.glue_7)
		
		. = ALIGN (4);
		__fastcode_end = .;
	} >RAM AT>ROM

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
		*(.text .text.* .gnu.linkonce.t.*)
		. = ALIGN(4);
		
		/* Position independent code will call non-static functions via the
		 * Procedure Linkage Table or PLT. This PLT does not exist in .o files.
		 * In a .o file, use of the PLT is indicated by a special relocation.
		 * When the program linker processes such a relocation, it will create
		 * an entry in the PLT
		 */
		*(.plt)
		. = ALIGN(4);
		
		/* These sections glue together ARM and Thumb code. 
		 * .glue_7 is used for ARM code calling Thumb code, and .glue_7t is
		 * used for Thumb code calling ARM code.
		 */
		*(.glue_7t)
		*(.glue_7)		 /* NOTE: placed already in .fastcode */

		. = ALIGN (4);
		
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
		
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		
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
		
		*(.init)
		/**(.fini)*/
		
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
		/*PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		PROVIDE_HIDDEN (__fini_array_end = .);*/
		
		*(.gnu.warning)		/* TODO: remove this? */
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
	.data :
	{
		. = ALIGN (4);
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
	.bss :
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
	.noinit :
	{
		. = ALIGN (4);
		PROVIDE (__noinit_start = .);
		
		*(.noinit*)
		
		. = ALIGN (4);
		PROVIDE (__noinit_end = .);
	} >RAM
	
	.stack :
	{
		. = ALIGN(4);
		__stack_start = . ;
		
		. += USR_STACK_SIZE;
		. = ALIGN (4);
		__usr_stack_top = . ;
		
		. += FIQ_STACK_SIZE;
		. = ALIGN (4);
		__fiq_stack_top = . ;
		
		. += IRQ_STACK_SIZE;
		. = ALIGN (4);
		__irq_stack_top = . ;

		. += SVC_STACK_SIZE;
		. = ALIGN (4);
		__svc_stack_top = . ;

		. += ABORT_STACK_SIZE;
		. = ALIGN (4);
		__abort_stack_top = . ;
		
		__stack_end = .;
		
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
