# Software Timers

An implementation of lightweight software timeouts and periodic timers.

- `modm::Timestamp` containing a time in millisecond resolution up to 49 days.
- `modm::Timeout` for timeouts up to 24 days.
- `modm::PeriodicTimer` for periodic timeouts up to 24 days periods.

There are also 16-bit versions of these, in case you need to preserve memory:

- `modm::ShortTimestamp` containing a time in millisecond resolution up to 65 seconds.
- `modm::ShortTimeout` for timeouts up to 32 seconds.
- `modm::ShortPeriodicTimer` for periodic timeouts up to 32 second periods.

These classes default to using `modm::Clock`, which must be implemented on your
system to return a time with millisecond resolution.

You may also create your own timers with custom time bases and resolutions
using these classes:

- `modm::GenericTimestamp`.
- `modm::GenericTimeout`.
- `modm::GenericPeriodicTimer`.

!!! warning
	Never use these classes when a precise timebase is needed!

## Timeouts

The `modm::GenericTimeout` classes allow for a signal to be generated after a
period of time, which can also be used to execute code once after timeout
expiration.

Its behavior can be described by the following annotated waveform:

- C: Default Constructor
- S: (Re-)Start timeout
- E: Timeout Expired
- H: Code handler (`execute()` returned `true`)
- P: Stop timeout

```
Event:    C      S      E     H       P            S          E H
                         _____________                         ______________
Expired:  ______________/             \_______________________/              ...
                  ______                            __________
Armed:    _______/      \__________________________/          \______________...
          _______                      ____________
Stopped:         \____________________/            \_________________________...

                              _                                 _
Handle:   ___________________/ \_______________________________/ \___________...

Remaining:   0  |   +   |      -      |     0      |     +     |   -

     time ——————>
```

The default constructor initializes the timeout in the `Stopped` state,
in which `isExpired()` and `execute()` never return `true`.
If you need a timeout to expire immidiately after construction, you need
to explicitly initialize the constructor with time `0`, which has the
same behavior as `restart(0)`.

If you want to execute code once after the timeout expired, poll the
`execute()` method, which returns `true` exactly once after expiration.

```cpp
if (timeout.execute())
{
    // called once after timeout
    Led::toggle();
}
```

Be aware, however, that since this method is polled, it cannot execute
exactly at the time of expiration, but some time after expiration, as
indicated in the above waveform graph.

The `remaining()` time until expiration is signed positive before, and
negative after expiration. This means `Clock::now() + Timeout::remaining()`
will yield the timestamp of the expiration.
If the timeout is stopped, `remaining()` returns zero.


## Periodic Timers

The `modm::GenericPeriodicTimer` class allows for periodic code execution.

Its behavior can be described by the following annotated waveform:

- C: Constructor
- S: (Re-)Start timer
- I: Period interval
- H: Code handler (`execute()` returned `true`)
- P: Stop timer

```
Event:    C         IH        I         I  H      I  S    IH   I    IH  P
                     _         _____________       __      _    ______
Expired:  __________/ \_______/             \_____/  \____/ \__/      \____...
          __________   _______               _____    ____   __        _
Armed:              \_/       \_____________/     \__/    \_/  \______/ \__...
                                                                         __
Stopped:  ______________________________________________________________/  ...

                     _                     _               _         _
Handle:   __________/ \___________________/ \_____________/ \_______/ \____...

Remaining:     +    |0|   +   |     -     |0|  +  | -|  + |0| +| -  |0|+| 0

     time ——————>
```

If you want to execute code once per period interval, poll the
`execute()` method, which returns `true` exactly once after expiration.

```cpp
if (timer.execute())
{
    // periodically called once
    Led::toggle();
}
```

Be aware, however, that since this method is polled, it cannot execute
exactly at the time of expiration, but some time after expiration, as
indicated in the above waveform graph.
If one or several periods are missed when polling `execute()`, these
code executions are discarded and will not be caught up.
Instead, `execute()` returns `true` once and then reschedules itself
for the next period, without any period skewing.

Notice, that the `PeriodicTimerState::Expired` is reset to
`PeriodicTimerState::Armed` only after `execute()` has returned `true`.
This is different to the behavior of GenericTimeout, where calls to
`GenericTimeout::execute()` have no impact on class state.

The `remaining()` time until period expiration is signed positive before,
and negative after period expiration until `execute()` is called.
If the timer is stopped, `remaining()` returns zero.
