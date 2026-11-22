# Undefined Behavior Sanitizer

Instruments all code compiled by modm with the compiler's Undefined Behavior
Sanitizer to find bugs like invalid shifts, divisions by zero, out-of-bounds
array accesses and invalid `bool` or `enum` values at runtime.

```xml
<module>modm:platform:ubsan</module>
<option name="modm:platform:ubsan:mode">report</option>
```

The `arm-none-eabi` toolchain does not ship the `libubsan` runtime, so there are
two modes that work without it:

- `trap`: All checks except `null` and `alignment` are enabled, since address
  zero is valid memory on some devices and Thumb function pointers are odd. The
  first failure executes a faulting instruction, which ends up in the HardFault
  handler and by default resets the device. Use a debugger or the
  `modm:platform:fault` module to find the location.
- `report`: A reduced set of checks calls a small runtime, which logs each failing
  source location once and then continues:

```
UBSAN: src/main.cpp:42:17 shift_out_of_bounds
```

!!! warning "This is a debugging tool"
    The instrumentation increases the code size considerably and changes the
    timing. The `report` mode additionally keeps its check descriptions in RAM,
    and needs `MODM_LOG_ERROR` to be connected to a logger.

Precompiled libraries, like the C library, are not instrumented. Since modm
compiles with `-fwrapv`, signed integer overflow is defined and is not reported.
