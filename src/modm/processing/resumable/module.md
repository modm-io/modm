# Resumable Functions

An implementation of lightweight resumable functions which allow for nested calling.

This base class and its macros allows you to implement and use several
resumable functions in one class.
This allows you to modularize your code by placing it into its own resumable functions
instead of the placing everything into one big method.
It also allows you to call and run resumable functions within your resumables,
so you can reuse their functionality.

Note that you should call resumable functions within a protothreads, with the
`PT_CALL(group.resumable())` macro, which will return the result of the
resumable function. To call a resumable function inside another resumable
function, use the `RF_CALL(group.resumable())`.

You may use the `RF_CALL_BLOCKING(group.resumable())` macro to execute
a resumable function outside of a protothread, however, this which will
force the CPU to busy-wait until the resumable function ended.

!!! warning "Resumable functions are not thread-safe!"
    Use an external mutex to arbitrate access if two Protothreads access the
    same resumable function.

You can either run your group of resumable functions independently from each
other, or only run one function at a time, but it may nest calls to its own
group's functions.
So you need to inherit your group from `modm::Resumable<#Functions>`, and from
`modm::NestedResumable<#Functions>` respectively.


## Independent operation

You must begin each resumable function using `RF_BEGIN(index)` where `index` is
the unique index of your resumable function starting at zero.
You may exit and return a value by using `RF_RETURN(value)` or
return the result of another resumable function using `RF_RETURN_CALL(resumable())`.
This return value is wrapped in a `modm::ResumableResult<Type>` struct
and transparently returned by the `RF_CALL` macro so it can be used
to influence your program flow.
If the resumable function reaches `RF_END()` it will exit automatically,
with the result of `0` cast to the return type.
Should you wish to return a value at the end, you may use
`RF_END_RETURN(value)`.
You may also return the result of another resumable function using
`RF_END_RETURN_CALL(resumable())`.

Be aware that this class keeps a separate state for each of your resumable functions.
This allows each resumable function to be run at the same time.
This might require the use of an internal semaphore or mutex if such
dependencies exist in your use case.
Take a look at the `NestedResumable` class for mutually exclusive resumable functions,
which also require a little less memory.


## Nested operation

You are responsible to choosing the right nesting depth!
This class will guard itself against calling another resumable function at too
deep a nesting level and fail the `resumable.begin.nesting` assertion!
It is then up to you to recognize this in your program design
and increase the nesting depth or rethink your code.

The resumable functions of this class are mutually exclusive, so only one
resumable function of the same object can run at the same time. Even if you
call another resumable function, it will simply return `modm::rf::WrongState`.
Using the `RF_CALL(resumable())` macro, you can wait for these
resumable functions to become available and then run them, so you usually do
not need to worry about those cases.

You must begin each resumable function using `RF_BEGIN()`.
You may exit and return a value by using `RF_RETURN(value)` or
return the result of another resumable function using `RF_RETURN_CALL(resumable())`.
This return value is wrapped in a `modm::ResumableResult<Type>` struct
and transparently returned by the `RF_CALL` macro so it can be used
to influence your program flow.
If the resumable function reaches `RF_END()` it will exit automatically,
with the result of `0` cast to the return type.
Should you wish to return a value at the end, you may use
`RF_END_RETURN(value)`.
You may also return the result of another resumable function using
`RF_END_RETURN_CALL(resumable())`.


## Example

Here is a (slightly over-engineered) example:

```cpp
#include <modm/platform/platform.hpp>
#include <modm/processing/processing.hpp>

using Led = GpioOutputB0;

class BlinkingLight : public modm::pt::Protothread, private modm::NestedResumable<2>
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
            Led::set();
            PT_CALL(waitForTimer());

            Led::reset();
            PT_CALL(setTimer(200));

            PT_WAIT_UNTIL(timeout.isExpired());
        }

        PT_END();
    }

    modm::ResumableResult<bool>
    waitForTimer()
    {
        RF_BEGIN();

        // nested calling is allowed
        if (RF_CALL(setTimer(100)))
        {
            RF_WAIT_UNTIL(timeout.isExpired());
            RF_RETURN(true);
        }

        RF_END_RETURN(false);
    }

    modm::ResumableResult<bool>
    setTimer(uint16_t new_timeout)
    {
        RF_BEGIN();

        timeout.restart(std::chrono::milliseconds(new_timeout));

        if(timeout.isArmed()) {
            RF_RETURN(true);
        }

        // clean up code goes here

        RF_END_RETURN(false);
    }

private:
    modm::ShortTimeout timeout;
};

BlinkingLight light;

while (true) {
    light.run();
}
```

For other examples take a look in the `examples` folder in the modm
root folder. The given example is in `modm/examples/generic/resumable`.