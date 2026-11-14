# Cross Platform Component Communication (XPCC)

!!! warning "Deprecated"
    XPCC moved into the [modm-io/xpcc](https://github.com/modm-io/xpcc)
    repository and this module will be removed in the 2027q4 release!
    Add the xpcc repository to your project and use the `xpcc:communication` module
    instead, see its README for how to migrate. For new projects please use a
    modern CAN protocol like DroneCAN.

XPCC is the component-based communication protocol of the Roboterclub Aachen.
Components send actions, responses and events over a backend (usually CAN) and
an XML description of the components, actions and packets is translated into
C++ code by the `modm:communication:xpcc:generator` module.
