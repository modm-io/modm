# DroneCAN DSDL Code Generator

Generates header-only C serialization code for the DroneCAN (UAVCAN v0) data
types. Select the namespaces of the [DroneCAN DSDL](https://github.com/dronecan/DSDL)
with the `modm:ext:dronecan:namespaces` option and add your own root namespace
folders with `modm:ext:dronecan:sources`. The `modm:ext:dronecan:types` option limits
the code to some types and their dependencies.

The API of the generated code follows the dronecan_dsdlc generator used by
ArduPilot. For `uavcan.protocol.NodeStatus`:

- `#include <uavcan.protocol.NodeStatus.h>`
- `struct uavcan_protocol_NodeStatus` with the fields, dynamic arrays as
  `struct { uint8_t len; T data[N]; }`, unions with a `union_tag`.
- `UAVCAN_PROTOCOL_NODESTATUS_ID`, `_SIGNATURE`, `_MAX_SIZE` and the constants.
- `uint32_t uavcan_protocol_NodeStatus_encode(const struct uavcan_protocol_NodeStatus*, uint8_t* buffer)`
  returns the payload length.
- `bool uavcan_protocol_NodeStatus_decode(const CanardRxTransfer*, struct uavcan_protocol_NodeStatus*)`
  returns true if the payload is invalid.
- Services have `...Request` and `...Response` structs in `_req.h` and `_res.h`.
- In C++, a `modm::dronecan::Type<uavcan_protocol_NodeStatus>` specialization
  with `id`, `signature`, `max_size`, `encode()` and `decode()`, and the
  `Request` or `Response` type of a service, for the typed helpers below.

The code packs the bits with the serialization functions of the DroneCAN fork
of libcanard, whose header is renamed to `<dronecan/canard.h>`. The transport
is implemented by libcanard v5 with a `modm::canard::Node` of the
`modm:ext:libcanard` module.

The generator is the `modm_tools.dronecan_dsdl` command line tool, which
parses the DSDL with pydronecan and renders the code with jinja2:
`pip install dronecan jinja2`. The `modm:build:scons` module calls it during
the build. Other build systems can call it with the DSDL that this module
copies to `modm/ext/dronecan/dsdl`:

```sh
python3 -m modm_tools.dronecan_dsdl -O generated/dronecan \
    --build uavcan.protocol.NodeStatus modm/ext/dronecan/dsdl/uavcan
```

`--list` prints the files it would write, so that a build system can declare
them as outputs.


## Typed Helpers

The typed helpers take the data type ID, signature and size from the
`modm::dronecan::Type<T>` specialization in every generated header:

```cpp
#include <modm/ext/dronecan/dronecan.hpp>
#include <uavcan.protocol.NodeStatus.h>
#include <uavcan.protocol.GetNodeInfo.h>
namespace dronecan = modm::dronecan;

modm::canard::Node<Can1> node;

// publishing increments the transfer-ID
uavcan_protocol_NodeStatus status{.uptime_sec = uptime};
dronecan::publish(&node.canard, status, transfer_id);
// services
dronecan::request(&node.canard, server_id, uavcan_protocol_GetNodeInfoRequest{}, transfer_id);
dronecan::respond(&node.canard, client_id, request_transfer_id, info);

// subscribing to messages, requests (as server) and responses (as client)
canard_subscription_t subscription;
dronecan::subscribe<uavcan_protocol_NodeStatus, onStatus>(&node.canard, &subscription);
// the callback gets the decoded valid transfers, the payload is already released
void onStatus(const dronecan::Transfer& transfer, const uavcan_protocol_NodeStatus& status)
{
    table[transfer.source] = status;
}
```

Like all libcanard callbacks, the callback must not call libcanard, so store
what it needs and respond or publish later. The value is decoded on the stack,
so the callback of a large type needs some stack. `subscribe()` with a `context`
passes it to the callback in `transfer.context`.

For full control, subscribe with a `canard_subscription_vtable_t`, decode the
payload yourself and release it:

```cpp
dronecan::subscribe<uavcan_protocol_NodeStatus>(&node.canard, &subscription, &vtable);
// in the subscription callback
uavcan_protocol_NodeStatus status;
if (dronecan::decode(payload, status)) { /* valid */ }
modm::canard::release(subscription, payload);
```

The `canard_v0_*()` functions can also be used directly with the generated C
functions and `modm::dronecan::crcSeed()` and `modm::dronecan::transfer()`.

libcanard cannot send anonymous frames, since Cyphal assigns node-IDs without
them. A DroneCAN node without node-ID sends the anonymous requests of the
dynamic node-ID allocation with `dronecan::publishAnonymous(node, allocation,
transfer_id)` instead, and sets the allocated node-ID with
`canard_set_node_id()`. The DroneCAN example implements a client and server.
