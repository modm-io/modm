# Protothreads

Protothreads are extremely lightweight stackless threads designed for
severely memory constrained systems, such as small embedded systems or
wireless sensor network nodes. Protothreads provide linear code execution
for event-driven systems implemented in C. Protothreads can be used with or
without an underlying operating system to provide blocking event-handlers.

Protothreads provide sequential flow of control without complex state
machines or full multi-threading.

Since they implement some kind of cooperative multi-threading, Protothreads
are non-preemptable. Therefore, a context switch can only take place on
blocking operations, which means you don't need complex synchronization.

Protothreads are also stackless, so local variables are not preserved across
context switches, and must instead become member variables of the
`modm::Protothread` subclass

A protothread runs within a single function (`modm::Protothread::run()`) and
cannot span over other functions. A protothread may call normal functions,
but cannot block inside a called function. Blocking inside nested function
calls is instead made by spawning a separate protothread for each
potentially blocking function.

The protothread concept was developed by Adam Dunkels and Oliver Schmidt:
http://dunkels.com/adam/pt

Originally ported to C++ for use by Hamilton Jet (www.hamiltonjet.co.nz) by
Ben Hoyt, but stripped down for public release.


## Example

```cpp
#include <modm/processing/protothread.hpp>

using Led = GpioB0;

class BlinkingLight : public modm::pt::Protothread
{
public:
    bool
    run()
    {
        PT_BEGIN();

        // set everything up
        Led::setOutput();
        Led::set();

        while (true)
        {
            timeout.restart(100ms);
            Led::set();
            PT_WAIT_UNTIL(timeout.isExpired());

            timeout.restart(200ms);
            Led::reset();
            PT_WAIT_UNTIL(timeout.isExpired());
        }

        PT_END();
    }

private:
    modm::ShortTimeout timeout;
};
// ...

BlinkingLight light;

while (true) {
    light.run();
}
```


## Using Fibers

Protothreads can be implemented using stackful fibers by setting the `use_fiber`
option, which replaces the preprocessor macros and C++ implementations of this
and the `modm:processing:resumable` module with a fiber version.

Specifically, the `PT_*` and `RF_*` macros are now forwarding their arguments
unmodified and instead relying on `modm::this_fiber::yield()` for context switching:

```cpp
#define PT_YIELD() modm::this_fiber::yield()
#define PT_WAIT_WHILE(cond) while(cond) { modm::this_fiber::yield(); }
#define PT_CALL(func) func
```

The `modm::pt::Protothread` class is implemented using `modm::Fiber<>` with the
default stack size `MODM_PROTOTHREAD_STACK_SIZE`. It automatically runs the two
virtual methods `bool run()` and `bool update()` if they are defined in the
protothread class.

There should be no modification of the existing code necessary with the
exception that you must replace the main loop calling all protothreads with the
fiber scheduler:

```cpp
int main()
{
    /*
    while(true)
    {
        protothread1.update();
        protothread2.update();
    }
    */
    modm::fiber::Scheduler::run();
    return 0;
}
```


### Restrictions

If the default stack size is too low, you can set `MODM_PROTOTHREAD_STACK_SIZE`
to a higher value, however, this will apply to *all* protothreads, consuming a
lot more memory. Instead, we recommend refactoring the protothread into a fiber
function.

All functions and macros work as expected, except for the  `Protothread::stop()`
function, which not implementable using fibers. It is left unimplemented,
so that you may define it with the behavior most suitable to your use case.

See the `modm:processing:resumable` module for additional restrictions when
calling resumable functions from a protothread.
