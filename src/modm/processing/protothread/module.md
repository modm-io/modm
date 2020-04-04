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
