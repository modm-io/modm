# Cross Platform Component Communication (XPCC)

!!! warning "Deprecated"
    XPCC is deprecated and will be removed in the 2027q4 release!
    Please migrate to a modern CAN protocol like DroneCAN or a similar one.

XPCC is the component-based communication protocol of the Roboterclub Aachen.
Components send actions, responses and events over a backend (usually CAN) and
an XML description of the components, actions and packets is translated into
C++ code by the `modm:communication:xpcc:generator` module.
