# DroneCAN Node

`modm::dronecan::Node<Can>` is a `modm::canard::Node<Can>` of the `modm:ext:libcanard`
module with the standard services of the `uavcan.protocol` namespace that every
DroneCAN node needs. This module adds their data types to the
`modm:ext:dronecan:types` option. See the `modm:ext:dronecan` module for the typed
helpers for the data types of the application.


- `NodeStatus` once per second, with `setHealth()`, `setMode()` and
  `setVendorStatus()`.
- `GetNodeInfo` with the name, versions and unique ID of `NodeInfo`.
- Dynamic node-ID allocation with `dynamic_node_id.Allocation`, if the node
  is initialized without node-ID. `nodeId()` is 0 until a server allocated one.
- `param.GetSet` and `param.ExecuteOpcode` for the parameters, which the DroneCAN
  GUI tool, Mission Planner and ArduPilot can read and write.
- `RestartNode`, if a restart function is configured.

```cpp
#include <modm/communication/dronecan.hpp>
namespace dronecan = modm::dronecan;

modm::canard::Parameter rate{"example.rate_hz", 10, 1, 100};    // integer with range
modm::canard::Parameter gain{"example.gain", 0.5f, 0.f, 10.f};  // real with range
modm::canard::Parameter enable{"example.enable", true};         // boolean
modm::canard::Parameter* const parameters[]{&rate, &gain, &enable};

dronecan::Node<Fdcan1> node;

std::array<uint8_t, 16> unique_id{};
// e.g. the UID of the device, see modm::platform::getUniqueId()
node.initialize({.name = "io.modm.example", .unique_id = unique_id, .software_major = 1},
                {.node_id = 0 /* dynamic allocation */, .parameters = parameters,
                 .restart = [] { NVIC_SystemReset(); }});

while (true)
{
    node.update();
    if (node.nodeId() and timer.execute())
        dronecan::publish(&node.canard, measurement(gain.real()), transfer_id);
}
```

The services answer in `update()`, which must be called every few
milliseconds. It needs about 1.5 kB of stack for the GetNodeInfo and GetSet
responses, which are encoded on the stack. Use the typed helpers of
`<modm/ext/dronecan/dronecan.hpp>` and the libcanard API on `node.canard` for
the data types of the application.

A DroneCAN node keeps its node-ID: libcanard would move to a free node-ID when
another node uses the same one, which DroneCAN does not allow. Instead, the
frames of the other node are dropped and counted in `node.collisions()`.

## Service Clients

`modm::dronecan::Client<Request>` calls a service of another node, e.g. the
GetSet service of a sensor, and matches the response by the server node-ID and
the transfer-ID. It handles one request at a time, and there is one client per
service and node, since libcanard only subscribes once to the responses of a
service. The response is decoded directly into the client.

```cpp
dronecan::Client<uavcan_protocol_param_GetSetRequest> getset;
getset.initialize(&node.canard);

// in a fiber, while another fiber calls node.update()
uavcan_protocol_param_GetSetRequest request{};
request.index = 0;
if (const auto* response = getset.call(sensor_id, request, 1s))
    use(response->value);

// without fibers
getset.request(sensor_id, request);
while (not getset.done() and not timeout.isExpired()) node.update();
```

The data types of the client must be added to the `modm:ext:dronecan:types`
option of the project.

## Parameters

The GetSet service reads and writes the `modm::canard::Parameter`s of the
node by name or by their index in the `parameters` span, whose order must
therefore stay the same. It refuses values outside of the range and answers
with the current value. The ExecuteOpcode service saves the parameters to the
`canard::ParameterStorage` (`OPCODE_SAVE`) or removes them from it and resets
them to their defaults (`OPCODE_ERASE`). `node.save()` and `node.erase()` do the
same from the application. See the `modm:ext:libcanard` module for the parameters
and their storage.
