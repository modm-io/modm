# Debugging with GDB and OpenOCD

To debug XPCC programs on your target you need OpenOCD
as well as a target specific _arm-none-eabi-_ **gdb** in your PATH.

If you only want to debug this _gdb_ example project, you can skip right to
the **Start Debugging** section.

## Prepare your Project

### Compiling with Debug Symbols

To compile code that is easily debugged, you need to specify some flags
in your local _SConstruct_ file:


	env['CXXFLAGS'].append("-O0")	# no optimizations
	env['CXXFLAGS'].append("-g")	# include debug symbols


### OpenOCD debug config

Your debug config for OpenOCD should look like this:


	source [find interface/stlink-v2.cfg]		# programmer
	source [find target/stm32f3x_stlink.cfg]	# target
	reset_config srst_only
	gdb_port 1324								# gdb port
	init
	halt	# halt device when openocd is started

## Start Debugging

### Program Target

You can load your code onto the target using _scons program_ as usual
or you can use gdb (how? _TBD....sorry_).

### Start OpenOCD as GDB server

using your openocd-debug.cfg:


	openocd -f openocd-debug.cfg


### Start GDB

In a different terminal run gdb with the path to your _.elf_ file.
For the _gdb_ example project you can use "start-gdb.sh".


	export PATH_TO_YOUR_ELF = "../../../build/stm32f3_discovery/gdb/gdb.elf" # e.g.
	arm-none-eabi-gdb $PATH_TO_YOUR_ELF


Now you need to connect GDB to you OpenOCD server:

	target extended-remote :1324

### Use GDB

Now you can set breakpoints:

	b main.cpp:133

Continue to them

	c

Inspect variables:

	p a

Look around in the source code

	l

Turn on mixed assembly/C++ view (it is best to maximize your terminal window)

	layout split

Step into functions

	s

Go to next assembly instruction

	ni

And much more. See the _Usefull GDB Commands_ section.

### Usefull GDB Commands

* **info break** (_i b_): list all breakpoints
* **break** (_b_): set breakpoint; e.g. "b main.cpp:120"
* **continue** (_c_): continue to next breakpoint
* **layout split**: display C/C++ code as well as assembler instructions in window
* **layout regs**:  display registers in window
* **list** (_l_): show 
* **next** (_n_): next line; do not step into
* **step** (_s_): steps into function
* **nexti** (_ni_): go to next machine instruction
* **stepi** (_si_): step one machine instruction
* **print** (_p_): print variable (i.e. _p a_, for reg0 _p $r0_, for reg2 in hex _p/x $r0_)
* **set** (_s_): set variable (i.e. _s a=10_, _s $reg2=16_)
* **info registers** (_i r_): show registers and content
* **info registers XX** (_i r XX_): replace XX by register name
