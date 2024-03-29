/*
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2013, 2015, Kevin Läufer
 * Copyright (c) 2013, 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
Make sure to compile and upload the *debug* version of the firmware:

	scons program profile=debug

Then you can start GDB in TUI mode by default:

	scons debug profile=debug

Alternatively if you installed gdbgui you can use:

	scons debug profile=debug ui=gdbgui


Useful GDB Commands:

- info break (i b)           list all breakpoints
- break (b)                  set breakpoint; e.g. "b main.cpp:120"
- continue (c)               continue to next breakpoint
- layout split               display C/C++ code as well as assembler instructions in window
- layout regs                display registers in window
- list (l)                   show
- next (n)                   next line; do not step into
- step (s)                   steps into function
- nexti (ni)                 go to next machine instruction
- stepi (si)                 step one machine instruction
- print (p)                  print variable (i.e. p a, for reg0 p $r0, for reg2 in hex p/x $r0)
- set (s)                    set variable (i.e. s a=10, s $reg2=16)
- info registers (i r)       show registers and content
- info registers XX (i r XX) replace XX by register name

*/

#include <modm/board.hpp>

int global_a = 1;
int global_b = 2;

/* Can you debug this function chaos */
/* Do you dare to???                 */

void foo2(int &x, int &y)
{
	x += 10;
	y -= 10;
}

void foo1(int x, int y)
{
	x += 100;
	y += 30;
	foo2(x, y);
}

void foo3(int *x, int *y)
{
	(*x)++;
	(*y) += 200;
}

/* infinite loop ... or maybe not */
int foo_foo(int z)
{
	if(z == 42) return 42;
	z += 3;
	return foo_foo(z);
}

void function_chaos()
{
	int x = 4;
	int y = 6;

	foo1(x, y);
	foo2(x, y);
	foo3(&x, &y);

	foo_foo(0);
	foo_foo(1);
}

/* Come fun iterative / recoursive functions for you to debug */
int fib_itr(int a)
{
	if(a == 0) return 0;
	if(a == 1) return 1;
	int f0 = 0, f1 = 1, res = 2;

	for (int i = 2; i <= a; i++) {	// way better runtime (linear!!)
		res = f0 + f1;
		f0 = f1;
		f1 = res;
	}

	return res;
}

int fib_rec(int a)
{
	if(a == 0) return 0;
	if(a == 1) return 1;
	return fib_rec(a-1) + fib_rec(a-2);	// this has exponential runtime...
}

// x to the power of y
int pow(int x, int y)
{
	if(y <= 1) return x;
	return x * pow(x, y-1);
}

void rec_itr()
{
	int i = 0;

	i = fib_itr(10);
	i = fib_rec(10);
	i = pow(2, 30);	// = (1<<30)
	(void)i;
}

int
main()
{
	Board::initialize();

	int a = 30;
	(void)a;

	while (true)
	{
		rec_itr();
		function_chaos();
	}

	return 0;
}
