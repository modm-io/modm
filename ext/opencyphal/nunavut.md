# Nunavut: DSDL Code Generator

Generates C serialization code for Cyphal DSDL data types with
[Nunavut](https://github.com/OpenCyphal/nunavut). The standard `uavcan` and
`reg` namespaces of the public regulated data types are included.

Add your own root namespace folders (e.g. `dsdl/mycompany`) with the
`modm:ext:nunavut:sources` option. The generated headers are written into
`modm:ext:nunavut:path` and included as `<uavcan/node/Heartbeat_1_0.h>`.

Needs the `nnvg` tool: `pip install nunavut`.

!!! bug "Currently only with SCons support"
    Only the `modm:build:scons` module currently supports this feature.
