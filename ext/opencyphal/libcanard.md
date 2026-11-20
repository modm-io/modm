# libcanard: Cyphal/CAN and DroneCAN

Compact implementation of the Cyphal/CAN protocol stack in C for
high-integrity real-time embedded systems. Supports Cyphal v1.1 and v1.0 on
Classic CAN and CAN FD, and the legacy UAVCAN v0/DroneCAN on Classic CAN.

See https://github.com/OpenCyphal/libcanard.


## modm::canard::Node

`modm::canard::Node<Can>` connects libcanard to a modm CAN driver: it owns a
`canard_t`, a private o1heap arena for the memory of libcanard, the receive loop
and the acceptance filters. Use the libcanard C API on `node.canard` with the
data types of the `modm:nunavut` module.

```cpp
#include <modm/ext/canard/canard.hpp>

modm::canard::Node<Fdcan1> node;
node.initialize(42, /* unique seed */ 0x1234);

while (true)
{
    node.update();  // receives, transmits and updates the filters
    // libcanard API on node.canard
}
```

Subscription callbacks receive the payload and must free it with
`modm::canard::release(subscription, payload)`. They must not call libcanard.

### Cyphal

A Cyphal node publishes and subscribes with the `canard_*()` functions of
libcanard and the serialization code that the `modm:nunavut` module generates:

```cpp
#include <modm/ext/canard/canard.hpp>
#include <uavcan/node/Heartbeat_1_0.h>

modm::canard::Node<Fdcan1> node;

Fdcan1::connect<GpioB8::Rx, GpioB9::Tx>();
Fdcan1::initialize<Board::SystemClock, 500_kbps, 1_pct, 2_Mbps>(9);
node.initialize(42, /* unique seed */ 0x1234, {.fd = true});

while (true)
{
    node.update();
    if (timer.execute())
    {
        uavcan_node_Heartbeat_1_0 heartbeat{.uptime = uptime++};
        uint8_t buffer[uavcan_node_Heartbeat_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
        size_t size = sizeof(buffer);
        uavcan_node_Heartbeat_1_0_serialize_(&heartbeat, buffer, &size);
        canard_publish_13b(&node.canard, modm::canard::now() + 1'000'000,
                           CANARD_IFACE_BITMAP_ALL, canard_prio_nominal,
                           uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_, transfer_id++,
                           {{size, buffer}, nullptr}, nullptr);
    }
}
```

For CAN FD, set `modm:architecture:can:message.buffer` to 64 and initialize the
CAN driver with a data bitrate.
