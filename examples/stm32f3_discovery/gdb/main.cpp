
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedNorth,     E,  9); // LD3
GPIO__OUTPUT(LedNorthEast, E, 10); // LD5
GPIO__OUTPUT(LedEast,      E, 11); // LD7
GPIO__OUTPUT(LedSouthEast, E, 12); // LD9
GPIO__OUTPUT(LedSouth,     E, 13); // LD10
GPIO__OUTPUT(LedSouthWest, E, 14); // LD8
GPIO__OUTPUT(LedWest,      E, 15); // LD6
GPIO__OUTPUT(LedNorthWest, E,  8); // LD4

GPIO__INPUT(Button, A, 0);

using namespace xpcc::stm32;

static bool
initClock()
{
	// use external 8MHz clock from ST-LINK
	if (!Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, Clock::PllMul::MUL_9);
	return Clock::switchToPll();
}

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
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	initClock();

	LedNorth::setOutput(xpcc::Gpio::LOW);
	LedNorthEast::setOutput(xpcc::Gpio::HIGH);
	LedEast::setOutput(xpcc::Gpio::HIGH);
	LedSouthEast::setOutput(xpcc::Gpio::HIGH);
	LedSouth::setOutput(xpcc::Gpio::HIGH);
	LedSouthWest::setOutput(xpcc::Gpio::HIGH);
	LedWest::setOutput(xpcc::Gpio::HIGH);
	LedNorthWest::setOutput(xpcc::Gpio::HIGH);

	int a = 30;
	
	while (1)
	{
		rec_itr();
		function_chaos();
	}

	return 0;
}
