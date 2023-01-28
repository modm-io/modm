# Fibers

!!! warning "Experimental and Work In Progress"
	This module will be have an unstable API until it is finalized in 2022.
	A lot of missing functionality will also be added until then.

This module provides a lightweight stackful fiber implementation including a
round-robin scheduler and several important OS primitives.

You can create a fiber like this:

```cpp
void hello()
{
	for(int ii=0; ii<10; ii++)
	{
		MODM_LOG_INFO << "Hello ";
		modm::fiber::yield();
	}
}

struct Test
{
	void world(void *arg)
	{
		for(int ii=0; ii<10; ii++)
		{
			MODM_LOG_INFO << (char*)arg << modm::endl;
			modm::fiber::yield();
		}
	}
} test;

modm::fiber::Stack<1024> stack1;
modm::fiber::Stack<1024> stack2;
// create fibers out of a void fn(void) signature
modm::Fiber fiber1(stack1, hello);

int
main(void)
{
	// more complex functions with arguments can be called by lambda closure
	const char *arg = "World";
	modm::Fiber fiber2(stack2, [arg]() { test.world(arg); });

	// The scheduler returns after all fibers finished
	modm::fiber::scheduler.run();

	return 0;
}
```

Note: If `yield()` is called outside of a fiber it returns immediately.


## AVR

On AVRs the fiber stack is shared with the currently active interrupt.
This requires the fiber stack size to include the worst case stack size of all
interrupts. Fortunately on AVRs interrupts cannot be nested.


## Arm Cortex-M

On Cortex-M, the main function is entered using the MSP in Handler mode. After
calling `modm::fiber::Scheduler::run()` the PSP is used as a Fiber stack pointer
in Thread mode. Therefore all interrupts are using the main stack whose size is
defined by the `modm:platform:cortex-m:main_stack_size` option and will not
increase the fiber stack size at all.
