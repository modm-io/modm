# libcanard: Cyphal/CAN and DroneCAN

Compact implementation of the Cyphal/CAN protocol stack in C for
high-integrity real-time embedded systems. Supports Cyphal v1.1 and v1.0 on
Classic CAN and CAN FD, and the legacy UAVCAN v0/DroneCAN on Classic CAN.

See https://github.com/OpenCyphal/libcanard.


## modm::canard::Node

`modm::canard::Node<Can>` connects libcanard to a modm CAN driver: it owns a
`canard_t`, a private o1heap arena for the memory of libcanard, the receive loop
and the acceptance filters. Use the libcanard C API on `node.canard` with the
data types of the `modm:ext:nunavut` (Cyphal) or `modm:ext:dronecan` (DroneCAN) modules.

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
libcanard and the serialization code that the `modm:ext:nunavut` module generates:

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

### DroneCAN

libcanard sends and receives DroneCAN (UAVCAN v0) transfers with the
`canard_v0_*()` functions, always as Classic CAN frames. The `modm:ext:dronecan`
module generates the data types and provides typed helpers for them.

### Memory

libcanard allocates memory for every queued frame, received transfer and
remote node, so the bus traffic decides how much it needs. By default each
node has a private o1heap arena of 4096 bytes (`Node<Can, ArenaSize>`), so that
a busy bus cannot exhaust the application heap. The allocations take constant
time, and an arena of twice the peak usage never fails due to fragmentation.
Use `node.heapDiagnostics()` to find the peak usage and size the arena.

With `Node<Can, 0>` libcanard uses `malloc()` and `free()` of the application
heap instead, which needs the `modm:platform:heap` module.


### Hosted

On hosted targets, the CAN drivers are objects, which `CanInstance` adapts to
the static interface of the node:

```cpp
modm::platform::SocketCan can;
modm::canard::Node<modm::canard::CanInstance<can>> node;

can.open("vcan0");
node.initialize(42, seed);
```


### CAN Driver Configuration

libcanard keeps its transmit queue sorted by priority and drops frames whose
deadline has passed. Frames that are already in the queues of the CAN driver
cannot be dropped anymore, so keep the driver queues short: for the STM32
FDCAN disable the software buffer (`buffer.tx = 0`) and use the priority mode
of the hardware queue (`tx_hw_queue_mode = Priority`). The bxCAN always has a
software buffer, which is sorted by priority as well.

libcanard configures the extended acceptance filters of the STM32 FDCAN and
bxCAN drivers, which needs one CAN controller per node: for several nodes on
one controller initialize them with `{.filters = false}` and configure a filter
that accepts all extended frames. On the bxCAN, the extended filters use all
filter banks of the instance, so they replace the filters set with `CanFilter`.

libcanard treats a received frame with its own node-ID as a node-ID collision,
so a node with a CAN controller in loopback mode must drop the echoes of its
own frames with `{.loopback = true}`. Several nodes on one controller read
the driver once and pass each frame to all nodes with `ingest()`.


## Parameters

`modm::canard::Parameter` in `<modm/ext/canard/parameter.hpp>` holds an
integer, real or boolean value with a default, and a range for integers and
reals, for the configuration services of the DroneCAN and Cyphal nodes. The
application reads it with `integer()`, `real()` or `boolean()` and may change
it with `set()`, which refuses values out of range.

```cpp
modm::canard::Parameter node_id{"uavcan.node.id", 0, 0, 127}; // integer with range
modm::canard::Parameter gain{"example.gain", 0.5f, 0.f, 10.f}; // real with range
modm::canard::Parameter enable{"example.enable", true};        // boolean
modm::canard::Parameter* const parameters[]{&node_id, &gain, &enable};
```

`canard::loadParameters()`, `saveParameters()` and `eraseParameters()` read
and write them in a `canard::ParameterStorage`. Each parameter is stored under
a 32-bit hash of its name and type, so the order may change between firmware
versions, and parameters with the default value are removed from the storage,
so that a new default takes effect. Load them before the node is initialized,
so that e.g. its node-ID can be a parameter.

With the `modm:ext:zms` module, `modm::canard::ZmsStorage` stores the
parameters in a ZMS file system on the internal or an external flash:

```cpp
#include <modm/ext/canard/parameter_zms.hpp>

zms_fs fs;
modm::zms::setup(fs, Flash::Size - 4 * Flash::MaxPageSize, 4, Flash::MaxPageSize);
zms_mount(&fs);
modm::canard::ZmsStorage storage{fs};
modm::canard::loadParameters(parameters, storage);
```

Saving blocks the node while the flash is programmed or erased. On devices
with a single flash bank, all interrupts are masked during these operations,
and the CAN receive FIFO overflows if frames arrive meanwhile. Put the storage
into the second bank of dual-bank devices instead, where the CPU keeps running
and the CAN driver keeps receiving into its software queue.
