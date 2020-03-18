# Assertions and Abandonment

This module provides a way to define and report assertions, that act as a
low-cost replacement for C++ exceptions and as a low-cost customization point
for errors raised in asynchronous code.

Assertions are called with or without a context:

- `modm_assert(condition, name, descr);`
- `modm_assert(condition, name, descr, context);`

They have the following arguments:

1. `bool condition`: The assertion fails when this condition evaluated to false.
2. `const char *name`: A short and unique assertion name.
3. `const char *description`: A detailed description of the failure.
4. `uintptr_t context = -1`: Optional context.

The condition is evaluated at most once by a (C-style) cast to bool.

The name format is not enforced, however, it is recommended to either use `what`
for top-level failures, like `malloc` for heap troubles, or `scope.what` for
failures that may not be unique, like `can.rx` vs. `uart.rx` for when their
respective receive buffers overflow.

The description can be as detailed as necessary, since it is only included in
the firmware if the `with_description` option is set to true, which also defines
`MODM_ASSERTION_INFO_HAS_DESCRIPTION` to 1 or 0. You can either find the
detailed description in your code via its name, or if you prefer a stand-alone
solution and your target has enough memory, include all strings in your binary.

The context is of pointer size, and anything passed to it is cast to
`uintptr_t`. Otherwise all bits are set via `uintptr_t(-1)`.

Assertions are implemented as macros and expand to this pseudo-code equivalent:

```cpp
void modm_assert(bool condition, const char *name, const char *description,
                 uintptr_t context=uintptr_t(-1))
{
    if (not condition)
    {
        modm::AssertionInfo info{name, description, context};
        modm_assert_report(info);
        // Unreachable code
    }
}
```

!!! note "Assertions can be used in both C and C++ code."

If you like to know the technical details, you can [read here about the original
assertions in xpcc](https://blog.salkinium.com/xpccs-error-model).


## Assertion Handlers

Assertions may also be recoverable if the call site allows for it. For example
if the CAN receive buffer overflows, you may want to simply discard the input.
If malloc fails to allocate it just returns NULL and the caller is responsible
to deal with that. But maybe you want to enable an additional assertion in debug
mode just to double-check.

When an assertion fails, the runtime calls any number of user-defined handlers,
registered using `MODM_ASSERTION_HANDLER(handler)`. The handlers must return a
`modm::Abandonment` value, specifing whether they want to continue with the
execution with `Abandonment::Ignore`, or abandon execution with
`Abandonment::Fail` leading to a call to `modm_abandon(info)`, or delegate the
decision with `Abandonment::DontCare`.

For example, this neutral handler logs the failed assertion's name, but
delegates all further decisions to others:

```cpp
static modm::Abandonment
log_assertion(const modm::AssertionInfo &info)
{
    MODM_LOG_ERROR.printf("Assertion '%s' raised!\n", info.name);
    return Abandonment::DontCare;
}
// Register handler for all builds
MODM_ASSERTION_HANDLER(log_assertion);
// Or register only for debug builds
MODM_ASSERTION_HANDLER_DEBUG(log_assertion);
```

You may register specialized handlers anywhere in your code, for example for
ignoring the mentioned CAN receive buffer overflow in your connectivity code:

```cpp
static modm::Abandonment
ignore_can_rx_handler(const modm::AssertionInfo &info)
{
    if (info.name == "can.rx"s) {
        // Only silently ignore this specific assertion!
        return Abandonment::Ignore;
    }
    return Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(ignore_can_rx_handler);
```

You may define any number of handlers, which are called in random order by the
runtime and their results are accumulated as follows:

1. If at least one handler returns `Abandonment::Fail` execution abandons.
2. Otherwise if any handler returns `Abandonment::Ignore` execution resumes.
3. Otherwise if no handlers are registered or all handlers return
   `Abandonment::DontCare`, the assertion type determines the outcome.

!!! note "Handler execution order is undefined."
    The order of handler execution is undefined and must not be relied upon for
    any functionality!

!!! warning "Assertions and handlers are not reentrant!"
    Assertions may fail in an interrupt context, thus calling handlers in that
    context too. Since handlers may make use of interrupts themselves (for
    logging via UART) assertions are not atomically scoped by default. You may
    however use a `modm::atomic::Lock` inside your assertion handler.

!!! warning "Handlers may be called inside high-priority interrupts!"
    This is problematic when relying on interrupts still working inside handlers
    for example for logging the failure via UART. Be aware of this and make sure
    you do not inadvertantly block inside handlers.


## Assertion Types

The call site of the assertion decides whether an assertion can be recovered
from or not. For example, if the CAN receive buffer has overflowed, but
execution continues, then code to discard the message must be in place.

In case no handlers are registered or they all delegate the abandoment decision
away, the call site must decide what the default behavior is. For this purpose
the following assertions are available:

1. `void modm_assert()`: Always abandons execution when failed.
2. `bool modm_assert_continue_fail()`: Abandons execution unless overwritten.
3. `bool modm_assert_continue_ignore()`: Resumes execution unless overwritten.

Assertions that can resume execution return the evaluated boolean condition to
be used to branch to cleanup code:

```cpp
// Can be done inline in any flow control statement
if (not modm_assert_continue_fail(condition, ...)) {
    // cleanup code
}

// Or saved in a variable and queried multiple times.
const bool cleanup = not modm_assert_continue_ignore(condition, ...);
if (cleanup) {
    // cleanup code part 1
}
// other code
if (cleanup) {
    // cleanup code part 1
}

// or if cleanup is require *before* assertion is called
const bool is_ok = (condition);
if (not is_ok) {
    // pre-assert cleanup
}
modm_assert_continue_fail(is_ok, ...);
if (not is_ok) {
    // post-assert cleanup
}
```

Additionally, these assertions are only active in debug mode. Of course they
still evaluate and return the condition in release mode, so you can use them
just as above:

4. `bool modm_assert_continue_fail_debug()`
5. `bool modm_assert_continue_ignore_debug()`


### When to use what?

Here are some guidelines for choosing the best assertion type:

1. Prefer to report errors via return types whenever possible!
2. If no sane recovery is possible, use `modm_assert()`.
3. If there is a (sensible) fallback for the failure, use
   `modm_assert_continue_{fail|ignore}()`:
   a. Abort by default, if the failure runs contrary to expected behavior.
   b. Resume by default, if the failure is expected and its behavior is well
      documented.
4. If the failure is expected *and* communicated via the normal API, or it
   only occurs rarely or through a clear misuse of the API, use
   `modm_assert_continue_{fail|ignore}_debug()`.

Let's illustrate these with a few examples:

1. libc `exit()` is called. There is no sensible fallback, since there is no
   operating system to return control back to, so use `modm_assert()`.
2. An interrupt without a user-defined handler is triggered. The developer most
   likely enabled the wrong interrupt or provided the wrong handler. A sensible
   fallback is to disable the interrupt from triggering again and to alert the
   developer with `modm_assert_continue_fail(..., irq_number)`.
3. The CAN receive buffer overflows. A sensible fallback is to discard the
   message, which is documented as the expected behavior. Since this occurs
   asynchronously inside the CAN RX interrupt, there is no way to return an
   error code, so call `modm_assert_continue_ignore(..., &message)` with a
   pointer to the message to inform the developer.
4. malloc fails to due to heap exhaustion and returns NULL, delegating the
   fallback implementation to the caller. Since the typical callers of malloc
   are known for not checking for NULL, using
   `modm_assert_continue_fail_debug()` here is warranted, helping the
   developer find potential issues faster, and then ignoring this assert for
   debug builds by registering a handler via `MODM_ASSERTION_HANDLER_DEBUG()`.
5. An I2C transfer failure is detected inside an interrupt. Such failures are
   expected on busses and typically the transfers are simply retried. You can
   use `modm_assert_continue_ignore_debug()` to give the developer a way to
   log the failure frequency without having to provide a special API. This can
   help diagnose a problem perhaps with the bus connection faster.


## Abandoning execution

If execution is abandoned `modm_abandon(const AssertionInfo &info)` is called,
which is a weak and empty function by default.

The function is meant to be overwritten by the application on embedded targets
for example to disable relevant hardware for safety, log the failure via UART
and perhaps blink some LEDs wildly to get the user's attention.

After returning from that function, the runtime resets the chip on Cortex-M, or
loops forever on AVR, or calls `abort()` on hosted. You may of course, implement
your own abandoning behavior instead of returning from `modm_abandon()`.

!!! warning "`modm_abandon()` may be called inside high-priority interrupts!"
    You can try to lower the active IRQ priority to let UART work, however, in
    the worst case you're called from within the HardFault or even NMI handlers
    which have the highest fixed priority.
