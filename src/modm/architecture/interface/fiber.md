# Fiber Interface

This module provides an interface to yield control back to a scheduler. The
basic functionality is provided by the `yield()` function which transparently
gives control back to the scheduler and returns afterwards. It is particularly
important to yield in long running loops to prevent the system from locking up
by preventing other fibers from making progress:

```cpp
while(true)
{
    // run your code here
    // but always yield to other fibers whenever possible
    modm::this_fiber::yield();
}
```

For convenience a `poll()` function is provided that can be used to yield until
a condition is met:

```cpp
modm::this_fiber::poll([&]{ return condition; });
```

An extension of this concept is provided by the `poll_for()` and `poll_until()`
functions, which yield until the condition is met or until a timeout occurs:

```cpp
bool condition_met = modm::this_fiber::poll_for(1s, [&]{ return condition; });
// if (not condition_met) condition did not return true for 1s.
```

If microseconds are passed for the duration, the functions use the
`modm::chrono::micro_clock` (=`modm::PreciseClock`), otherwise they use
`modm::chrono::milli_clock` (=`modm::Clock`). This requires that these clocks
are already initialized and running.

These basic building blocks are then used to implement the `sleep_for()` and
`sleep_until()` convenience functions:

```cpp
modm::this_fiber::sleep_for(1s);
```


## Implementation

The `yield()` function is implemented by the `modm:processing:fiber` module
which provides a cooperative multitasking scheduler that is able to switch
between multiple fiber contexts.

If `yield()` is called outside of a fiber context, for example, in the `main()`
function when the scheduler is not yet running, `yield()` will return in-place.
This mechanism allows for a graceful fallback to a blocking API without changes
to the code using `yield()`.

```cpp
modm::Fiber fiber_nonblocking([]
{
    modm::Timeout timeout(100ms);
    timeout.wait(); // non-blocking call!
});
int main()
{
    modm::Timeout timeout(100ms);
    timeout.wait(); // blocking call!

    modm::fiber::Scheduler::run()
    return 0;
}
```

This mechanism also supports running modm on devices with very small memories
where a stackful scheduler may be to resource intensive:
The `modm:processing:fiber` module is strictly opt-in and if not selected the
scheduler is not included and the `yield()` function is implemented as an empty
stub while still allowing for the whole API to be used without changes:

```cpp
modm::Lis3mdl<I2cMaster1> sensor{};
int main()
{
    sensor.configure(); // blocking but works just fine
    while(true)
    {
        modm::Vector3f magVector;
        sensor.readMagnetometer(magVector); // another blocking call
        modm::this_fiber::sleep_for(10ms); // this will wait in place
    }
}
```

Therefore, if you use these functions in your code, only depend on
`modm:architecture:fiber` and let the user decide on the implementation by
including `modm:processing:fiber` or not. This compromise allows for a seamless
transition between different devices and scheduling strategies.


## Identifier

You can check what fiber your code is executed in by calling the `get_id()`
function:

```cpp
auto id = modm::this_fiber::get_id();
// if (id == 0) called outside a fiber
// else called from inside a fiber
```

The returned ID is the address of the currently running fiber object. If called
outside of a fiber, for example, in the main function before the scheduler is
running, the function returns `0`. The implementation ensures that all returned
values are unique and thus allow the ID to be used for tracking ownership of
various recursive locks, for example.
