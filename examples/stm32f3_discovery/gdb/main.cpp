#include <xpcc/architecture.hpp>
#include "../stm32f3_discovery.hpp"

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

MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedNorth::setOutput(xpcc::Gpio::Low);
	LedNorthEast::setOutput(xpcc::Gpio::High);
	LedEast::setOutput(xpcc::Gpio::High);
	LedSouthEast::setOutput(xpcc::Gpio::High);
	LedSouth::setOutput(xpcc::Gpio::High);
	LedSouthWest::setOutput(xpcc::Gpio::High);
	LedWest::setOutput(xpcc::Gpio::High);
	LedNorthWest::setOutput(xpcc::Gpio::High);

	int a = 30;
	(void)a;

	while (1)
	{
		rec_itr();
		function_chaos();
	}

	return 0;
}
