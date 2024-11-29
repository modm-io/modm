# Fibers

This module provides a lightweight stackful fiber implementation including a
simple round-robin scheduler. Here is a minimal example that blinks an LED:

```cpp
modm::Fiber<> fiber([]
{
	Board::LedBlue::setOutput();
	while(true)
	{
		Board::LedBlue::toggle();
		modm::this_fiber::sleep_for(1s);
	}
});
int main()
{
	modm::fiber::Scheduler::run();
	return 0;
}
```

## Construction

You can construct a fiber from any function without return type or arguments:

```cpp
modm::Fiber<> fiber([]{});
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
modm::Fiber<> fiber([&]
{
	object.member_function(number);
});
```

Remember to use the right capture method for the lifetime of the objects you
want to call. You can `std::move()` already constructed objects into the
capture, or construct them in the capture directly, if they would get destroyed
after fiber construction. You may need to mark the lambda mutable:

```cpp
modm::Fiber<> fiber2([obj=std::move(object), obj2=DataObject()] mutable
{
	obj.member_function(24);
	obj2.member_function(42);
});
```

!!! warning "Do not construct `modm::Fiber` on the stack!"
	Apart from the general lifetime issues of constructing objects on the stack,
	the allocated fiber stack size is likely too large for the caller stack
	and will lead to a stack overflow.

A fiber can be passed a `modm::fiber::stop_token` to allow the fiber to be
stopped cooperatively.

```cpp
modm::Fiber<> fiber([](modm::fiber::stop_token stoken)
{
	// set up
	while(not stoken.stop_requested())
	{
		// run your task
	}
	// clean up
});
// externally request the fiber to stop
fiber.request_stop();
// wait until fiber has stopped
fiber.join();
```

Note that the fiber destructor requests to stop and joins automatically.
The interface and behavior is similar to the C++20 `std::jthread`.


## Delayed Start

Fiber are added to the scheduler automatically and start execution when the
scheduler is run. You can disable this behavior by setting `start` to
`modm::fiber::Start::Later` during construction and manually starting the fiber
when it is ready, also from another fiber:

```cpp
// fiber does not automatically start executing
modm::Fiber<> fiber2(function, modm::fiber::Start::Later);
// fiber2 is automatically executing
modm::Fiber<> fiber1([&]
{
	modm::this_fiber::sleep_for(1s);
	fiber2.start();
});
modm::fiber::Scheduler::run();
// fiber1 waits 1s, then starts fiber2 and exits
```

Fibers can end by returning from their wrapper, after which they will be removed
from the scheduler. A fiber can then be restarted again by calling `start()`,
which will call the closure again from the beginning. Note, that the lambda
capture is not destructed and reconstructed, but remains unchanged between
restarts. If you need a fiber that is only callable once, you can implement this
behavior manually with a boolean in the capture:

```cpp
modm::Fiber<> fiber([ran=false]
{
	if (ran) return;
	ran = true;
	// only called once.
});
```


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


### Subclassing

You can inherit from the `modm::Fiber` class to extend a fiber with a proper
object interface. This is useful for providing a more expressive interface to a
fiber by means of shared memory.

```cpp
class CustomFiber : public modm::Fiber<>
{
public:
	CustomFiber() : Fiber([this]{ run(); }) {}

	// called from another fiber
	void trigger() { trigger_ = true; }

private:
	bool trigger_{false}; // shared memory between fibers

	void inline
	run()
	{
		while(true)
		{
			modm::this_fiber::poll([&]{ return trigger_; });
			trigger_ = false;
			// long running response to the trigger
		}
	}
};
```


## Concurrency Support

The `modm::fiber` namespace provides several standard concurrency primitives to
synchronize fibers based on the [`std::thread` interface behavior][std_thread].
Most primitives are implemented on top of `<atomic>`, therefore can be called
from within (nested) interrupts. The API docs explicitly mention if a function
is safe to call from an interrupt.


### Threads

- `Task` implements most of the `std::jthread` interface.

In particular, `Task` only implements functionality that does not require
dynamic memory allocations. The stack memory needs to be allocated externally
and fibers are not movable or copyable and therefore cannot be detached or
swapped.


### Thread Cancellation

- `stop_token` and `stop_source` with simplified implementations.
- `stop_callback` **not implemented**.

To avoid dynamic memory allocations, a `stop_state` object provides the actual
memory required for the limited functionality:

```cpp
modm::fiber::stop_state state;
// only valid as long as state is valid!
auto source = state.get_source();
auto token = state.get_token();
// use token in a condition variable
cv.wait(lock, token, predicate);
// request a stop somewhere else
source.request_stop();
```

Implemented using interrupt-safe atomics.


### Mutual Exclusion

- `mutex` and `timed_mutex`.
- `recursive_mutex` and `recursive_timed_mutex`.
- `shared_mutex` and `shared_timed_mutex`.

Implemented using interrupt-safe atomics.

#### Generic Mutex Management

- `lock_guard`, `scoped_lock`, `unique_lock` and `shared_lock`.
- `defer_lock_t`, `try_to_lock_t` and `adopt_lock_t`.
- `defer_lock`, `try_to_lock` and `adopt_lock`.

#### Generic Locking Algorithms

- `try_lock` and `lock`.

#### Call Once

- `once_flag` and `call_once`.

Implemented using interrupt-safe atomic flag.


### Condition Variables

- `condition_variable` and `condition_variable_any`.
- `cv_status`.
- `notify_all_at_thread_exit` **not implemented**.

Notification is implemented as a interrupt-safe 16-bit atomic counter.


### Semaphores

- `counting_semaphore` and `binary_semaphore`.

Counts are implemented as interrupt-safe 16-bits atomics.


### Latches and Barriers

- `latch`: implemented as interrupt-safe atomics.
- `barrier`: **not** interrupt-safe!

Counts are implemented as 16-bits.


## Stack Usage

To measure the stack usage of a fiber, you need to explicitly watermark the
stack *before* running the fiber, then you may query the stack usage inside or
outside the fiber:

```cpp
// You must watermark the stack *before* running the fiber!
fiber1.stack_watermark();
// now you can run the fibers via the scheduler
modm::fiber::Scheduler::run();
// can be called from inside or outside the fiber, before or after running!
size_t bytes = fiber.stack_usage();
```

Note that stack usage measurement through watermarking can be inaccurate if the
registers contain the watermark value.


## Stack Overflow

Each context switch checks if the stack overflowed, in which case the scheduler
will abandon execution and trigger an assertion on the main stack with the
identifier `fbr.stkof` and the fiber pointer as context. Note that the assertion
is executed on the main stack and not on the fiber stack that overflowed!

On ARMv8-M devices, the stack overflow is checked in hardware via the PSPLIM
register, therefore the context switch is a little faster.


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


## Platforms

Fibers are implemented by saving callee registers to the current stack, then
switching to a new stack and restoring callee registers from this stack.
The static `modm::this_fiber::yield()` function wraps this functionality in a
transparent way.


### AVR

On AVRs the fiber stack is shared with the currently active interrupt.
This requires the fiber stack size to include the worst case stack size of all
interrupts. Fortunately on AVRs interrupts cannot be nested.

Therefore the default stack size is a fairly large **512B**.


### Arm Cortex-M

On Cortex-M, the main function is entered using the MSP in Handler mode. After
calling `modm::fiber::Scheduler::run()` the PSP is used as a Fiber stack
pointer in Thread mode. Therefore all interrupts are using the main stack whose
size is defined by the `modm:platform:cortex-m:main_stack_size` option and will
not increase the fiber stack size at all.

The default stack size is **1KiB**.


### Hosted

Two implementations for x86_64 and ARM64 are provided.

The default stack size is **1MiB**.


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
modm_faststack_core1 modm::Fiber<> fiber1(function, modm::fiber::Start::Later);

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

[std_thread]: https://en.cppreference.com/w/cpp/thread
