# Fibers

This module provides a lightweight stackful fiber implementation including a
simple round-robin scheduler. Here is a minimal example that blinks an LED:

```cpp
modm::Fiber<> fiber([]()
{
	Board::LedBlue::setOutput();
	modm::this_fiber::yield();
	while(true)
	{
		Board::LedBlue::toggle();
		modm::this_fiber::sleep_for(1s);
	}
});
int main(void)
{
	modm::fiber::Scheduler::run();
	return 0;
}
```


## Construction

You can construct a fiber from any function without return type or arguments:

```cpp
modm::Fiber<> fiber([](){});
void function() {}
modm::Fiber<> fiber2(function);
```

To call objects with arguments, wrap the data into a lambda closure and
pass it to fiber. The closure will be constructed at the top of the stack and
allows the lambda wrapper to call your function with an argument:

```cpp
struct DataObject
{
	void member_function(int arg);
} object;
int number{42};
modm::Fiber<> fiber([&]()
{
	object.member_function(number);
});
```

Remember to use the right capture method for the lifetime of the objects you
want to call. You can `std::move()` already constructed objects into the
capture, or construct them in the capture directly, if they would get destroyed
after fiber construction. You may need to mark the lambda mutable:

```cpp
modm::Fiber<> fiber2([obj=std::move(object), obj2=DataObject()]() mutable
{
	obj.member_function(24);
	obj2.member_function(42);
});
```

!!! warning "Do not construct `modm::Fiber` on the stack!"
	Apart from the general lifetime issues of constructing objects on the stack,
	the allocated fiber stack size is likely too large for the caller stack
	and will lead to a stack overflow.


## Execution

Fiber are added to the scheduler automatically and start execution when the
scheduler is run. You can disable this behavior by setting `start` to `false`
during construction and manually starting the fiber when it is ready, also from
another fiber:

```cpp
// fiber does not automatically start executing
modm::Fiber<> fiber(function, false);
// fiber2 is automatically executing
modm::Fiber<> fiber2([&]()
{
	modm::this_fiber::sleep_for(1s);
	fiber.start();
});
modm::fiber::Scheduler::run();
// fiber waits 1s, then starts fiber2 and exits
```

Fibers can end by returning from their wrapper, after which they will be removed
from the scheduler. A fiber can then be restarted again by calling `start()`,
which will call the closure again from the beginning. Note, that the lambda
capture is not destructed and reconstructed, but remains unchanged between
restarts. If you need a fiber that is only callable once, you can implement this
behavior manually with a boolean in the capture:

```cpp
modm::Fiber<> fiber([ran=false]()
{
	if (ran) return;
	ran = true;
	// only called once.
});
```


## Scheduling

The scheduler `run()` function will suspend execution of the call site, usually
the main function, start each fiber and continue to execute them until they all
ended and then return execution to the call site:

```cpp
while(true)
{
	modm::fiber::Scheduler::run();
	// sleep until the next interrupt?
	__WFI();
	// then start the fibers again
	fiber.start();
}
```

Please note that neither the fiber nor scheduler is interrupt safe, so starting
threads from interrupt context is a bad idea!

!!! note "Using `yield()` outside of a fiber"
	If `yield()` is called before the scheduler started or if only one fiber is
	running, it simply returns in-place, since there is nowhere to switch to.


## Customization

The most important customization is the fiber stack size expressed in bytes:

```cpp
modm::Fiber<128> fiber1(...);
modm::Fiber<256> fiber2(...);
```

The `Fiber` class is intentionally constructed at runtime, so that it does not
increase your program size, as the `.data` section would. You may also place
the fibers into the `.faststack` section, which is not zeroed and thus saves a
bit of time on startup:

```cpp
modm_faststack modm::Fiber<>(stack, function);
```

However, it may be desirable to control the placement of the fiber task
structure and especially the stack, depending on the types of memories
available on your device. This is possible when you construct the stack and
task in combination with the `modm_section()` macros and its specializations:

```cpp
// Place a very large stack in the external memory
modm_section(".sdram_noinit") modm::fiber::Stack<1024*10> large_stack;
// But keep the task control structure in internal memory
modm_fastdata modm::fiber::Task fiber(large_stack, big_function);
```


## Stack Usage

It is difficult to measure stack usage without hardware support, however,
detecting stack overflows is simpler with watermarking. A single word at the
bottom of the stack is watermarked by default, as a cheap way to detect stack
overflows:

```cpp
// check if the bottom word of the stack was written
bool overflowed = fiber.stack_overflow();
```

Since the stack can also overflow without writing the last word, it is better to
measure maximum stack usage and size the stack with a healthy buffer. You must
watermark the stack *before* running the fiber, then you may query the stack
usage inside or outside the fiber:

```cpp
// You must watermark the stack *before* running the fiber!
fiber1.watermark_stack();
// now you can run the fibers via the scheduler
modm::fiber::Scheduler::run();
// can be called from inside or outside the fiber, before or after running!
size_t bytes = fiber.stack_usage();
```

Note that stack usage measurement through watermarking can be inaccurate if the
registers contain the watermark value.


## Platforms

Fibers are implemented by saving callee registers to the current stack, then
switching to a new stack and restoring callee registers from this stack.
The static `modm::this_fiber::yield()` function wraps this functionality in a
transparent way.

### AVR

On AVRs the fiber stack is shared with the currently active interrupt.
This requires the fiber stack size to include the worst case stack size of all
interrupts. Fortunately on AVRs interrupts cannot be nested.


### Arm Cortex-M

On Cortex-M, the main function is entered using the MSP in Handler mode. After
calling `modm::fiber::Scheduler::run()` the PSP is used as a Fiber stack
pointer in Thread mode. Therefore all interrupts are using the main stack whose
size is defined by the `modm:platform:cortex-m:main_stack_size` option and will
not increase the fiber stack size at all.


### Multi-Core Scheduling

When using this module in combination with the `modm:platform:multicore` module,
each core gets its own fiber scheduler, which will internally be selected based
on the CPU ID. Since the scheduler is not thread-safe, you cannot add fibers
from one core to the other. Instead you must construct the fiber without
starting it, and when executing on the other core, `start()` it in that context.

Here is an example for the RP2040 device, which additionally allocates the stack
and task into the core-affine memory:

```cpp
// allocate into core0 memory
modm_faststack_core0 modm::Fiber<> fiber0(function);
// allocate into core1 memory but DO NOT start yet!
modm_faststack_core1 modm::Fiber<> fiber1(function, false);

void core1_main()
{
	// start fiber1 in the core1 context!
	fiber1.start();
	modm::fiber::Scheduler::run();
}

int main()
{
	modm::platform::multicore::Core1::run(core1_main);
	// run fiber0 in core0 context
	modm::fiber::Scheduler::run();
	return 0;
}
```
