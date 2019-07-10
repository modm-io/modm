# ARM CMSIS Support

The [*Cortex Microcontroller Software Interface Standard*][cmsis] is a software
standard by ARM that defines a number of foundational interfaces for Cortex-M
microcontrollers.

<center>
![][overview]
</center>

We use only use these parts of CMSIS:

- The `modm:cmsis:core` module provides access to the ARM Cortex-M peripherals
  via CMSIS-CORE.
- The `modm:cmsis:device` module provides access to the peripherals of the
  `modm:target` devices via the vendor's CMSIS implementation.
- The `modm:cmsis:dsp` module provides a basic digital signal processing library
  via CMSIS-DSP.

[cmsis]: https://developer.arm.com/embedded/cmsis
[overview]: https://developer.arm.com/-/media/Arm%20Developer%20Community/Images/Block%20Diagrams/Cortex%20Microcontroller%20Software%20Interface%20Standard%20-%20CMSIS/CMSIS%20Diagram%20v2.png
