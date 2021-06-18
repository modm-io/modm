# Asynchronous Multi-Node Bus (AMNB)

The AMNB (Asynchronous Multi-Node Bus) is a RPC communication system over a
shared half-duplex UART bus. It is intended as a low-cost replacement for higher
level protocols like CAN or Ethernet, which are not available in every device
unlike UART.

A node can provide two communication services, each with an 8-bit ID:

- publish/subscribe: sent to everyone and received by anyone.
- request/response: sent to one node which then sends a response.

Messages support up to 28 bytes of in-place data, and up to 8kB of heap data and
are protected with 8-bit and 16-bit CCIT CRC sums. The receiving node can refuse
to allocate any data, which will discard messages with >28B payload, or define a
dynamic allocation threshold (for example <=1kB) above which the message will
also be refused. This allows devices with very little RAM resources to share the
same bus and same protocol as larger devices.

Nodes are identified with a unique 8-bit address. The protocol does not define a
central node, so any node can initiate communication as arbitration is provided
via a CSMA/CD mechanism, with the retransmission count and backoff time
controlled by the service ID, with higher IDs having higher priority.

Data serialization is done by copying their memory representation and are
described as C/C++ types which must be available for both the sender and
receiver, for example, in the form of shared header files. The types must be
trivially constructable, since no constructor (or destructor) is called.


## Electrical Configuration

The protocol requires an open-drain + pullup config on the UART TX pin and an
input + pullup config on the RX pin. Additionally, a loopback from TX to RX is
required so that collisions can be detected.

There are three bus configurations:

1. Some UART peripherals have a built-in half-duplex mode, where the TX/RX pins
   are connected internally. The bus is just one pin now with an external pullup
   or if you use internal pullups with a slow baudrate, zero external
   components.

   ```
    ┌────────┐  ┌────────┐  ┌────────┐
    │  Node  │  │  Node  │  │  Node  │
    │  TxRx  │  │  TxRx  │  │  TxRx  │
    └───┬────┘  └───┬────┘  └───┬────┘
        └───────────┴───────────┘
   ```

2. Alternately you can connect all TX pins and RX pins together in two separate
   lines and [use a diode from RX ->- TX to create the loopback][loopback] with
   internal or external pullups:

   ```
    ┌────────┐  ┌────────┐  ┌────────┐
    │  Node  │  │  Node  │  │  Node  │
    │ Tx  Rx │  │ Tx  Rx │  │ Tx  Rx │
    └─┬───┬──┘  └─┬───┬──┘  └─┬───┬──┘
      │   └───────┼───┴───────┼───┘
      └───────────┴───────────┘─◄─┘  <- diode
   ```


3. And lastly you can use a differential transceiver with automatic direction
   control (like a CAN transceiver) to connect nodes that are further away.
   Note that you can still connect local nodes before the transceiver (but
   without the diode now):

   ```
    ┌────────┐  ┌────────┐                               ┌────────┐
    │  Node  │  │  Node  │                               │  Node  │
    │ Tx  Rx │  │ Tx  Rx │                               │ Tx  Rx │
    └─┬───┬──┘  └─┬───┬──┘  ┌────────┐       ┌────────┐  └─┬───┬──┘
      │   └───────┼───┴─────┤ Trans- ╞═╦═╦═╦═╡ Trans- ├────┼───┘
      └───────────┴─────────┤ ceiver ╞═╩═╩═╩═╡ ceiver ├────┘
                            └────────┘       └────────┘
   ```

Note that small glitches on the bus can be interpreted as a UART start condition
and will trigger a byte reception. In order to recognize this, the UART must be
configured as 8 data bits and 1 **even** parity bit (9E1)!

```cpp
Uart::initialize<SystemClock, Baudrate>(Uart::Parity::Even, Uart::WordLength::Bit9);
Uart::connect<GpioRx::Rx, GpioTx::Tx>(Gpio::InputType::PullUp, Gpio::OutputType::OpenDrain);
GpioTx::configure(Gpio::InputType::PullUp); // if using internal pullups
```


## Subscription and Response Handlers

A list of the types `modm::amnb::Listener` and `modm::amnb::Action` store the
callbacks for the IDs you want to subscribe to. The callbacks store a closure up
to size `MODM_AMNB_HANDLER_STORAGE`, which can be overwritten to increase
storage. The argument type is deduced from the callback's signature and it must
match the message size otherwise the message is discarded.

!!! warning "Data lifetime ends after callback"
    The received message is deallocated immediately after all callback have been
    executed. You must therefore copy any data you want to keep inside the
    callback.

All listeners are given the sender ID as first argument and return nothing.
The second argument is optional and may be a type passed as const reference or
a const data buffer:

```cpp
using namespace modm::amnb;

Listener listeners[] =
{
    {0, [](uint8_t sender)
        {
            // a broadcast without payload
        }
    },
    {1, [](uint8_t sender, const ArgumentType& arg)
        {
            // a broadcast with an argument
        }
    },
    {2, [obj_ptr](uint8_t sender, const uint8_t *data, size_t length)
        {
            // a broadcast with a variable length memory buffer
            // forward to member function of a specific object
            obj_ptr->member_function(sender, data, length);
        }
    },
    // You can have multiple listeners on the same ID *with a closure*
    {0, [count = uint32_t(0)](uint8_t sender) mutable
        {
            // keeps track how many times it has been published
            MODM_LOG_DEBUG "ID0 count=" << count++ << modm::endl;
        }
    },
};
```

Actions don't know the sender, and are passed either nothing, a const argument
reference or a const data buffer. They can return a `modm::amnb::Response` with
an optional return type, or a `modm::amnb::ErrorResponse` with an optional user
error type. Not returning anything still sends an implicit (good) Response.


```cpp
using namespace modm::amnb;

Action actions[] =
{
    {0, []()
        {
            // request without payload, automatic good reponse
        }
    },
    {1, []() -> Response
        {
            // return user error or response without payloads
            return failure ? ErrorReponse() : Response();
        }
    },
    {2, [](const ArgumentType& arg) -> Response
        {
            // return user return type
            ReturnType return_arg = ...;
            return return_arg;
        }
    },
    {3, [](const uint8_t *data, size_t length) -> Response
        {
            // return user error code
            return ErrorResponse(UserErrorType());
        }
    },
};
```


## Publish and Request Node

The node class manages the whole stack via its `update()` function which must
be called in the main loop continuously.

To broadcast, the `node.broadcast(id, args)` function places the message in the
transmit queue, the size of which you can control. It returns false if the queue
is full.

Requests `node.request<ReturnType=void>(id, args)` must be made from within a
protothread, since they can take some time. The response is a
`modm::amnb::Result` object, which contains the result, system error and user
error.

Note that if a request returns a user error type, you must define it as
`node.request<ReturnType, UserErrorType>(id, args)` and then explicitly check
for it via `response.hasUserError()`. Note that the request itself may fail at a
system level, for example sending a request with a payload too large for the
receiver will return with a `modm::amnb::Error::RequestAllocationFailed` error.

!!! warning "Result lifetime only valid until node.update()"
    The result is only valid until the next call to `node.update()`, therefore
    you must copy the result into your own storage!

!!! warning "Do not use `RF_CALL_BLOCKING` for requests!"
    `node.update()` must be called during requests to service the protocol and
    `RF_CALL_BLOCKING(node.request(...))` won't call it, creating a deadlock!


```cpp
using namespace modm::amnb;

DeviceWrapper<Usart1> device;
Node</* TX msg queue size =*/10, /* max heap allocation = */1024>
    node(device, /*address=*/0x10, actions, listeners);


// Broadcasts are buffered in a transmit queue
node.broadcast(0);                     // no payload
node.broadcast(1, ArgumentType());     // with argument
node.broadcast(2, data, sizeof(data)); // with memory buffer


// Responses must be called from within a protothread
response = PT_CALL(node.request<>(0));
// The request may fail, in that case check the error
if (not response) Error error = response.error();

// Responses can also be called from within a resumable function
response = RF_CALL(node.request<ReturnType>(1, ArgumentType()));
// The response is temporary, copy it before the next call to node.update()
if (response) ReturnType result = *response;

// User Error types must be declared
response = RF_CALL(node.request<ReturnType, UserErrorType>(2, data, sizeof(data)));
// The user error must be checked for explicitly now
if (response) ReturnType result = *response;
else if (response.hasUserError())
    UserErrorType error = *response.userError();
else Error error = response.error();


/// node.update() must be called as often as possible!
while(true)
{
    node.update();
}
```


## Wire Format

There are three message formats:

1. Messages without payload
2. Messages with payloads up to 28B
3. Messages with payloads up to 8kB

The header contains these common fields:

- `SYNC`: Synchronization sequence
- `CRC8`: checksum for the header
- `ADDR`: Node address (sender or receiver)
- `COMMAND`: Command code (listener/action ID)
- `TYPE`: Message type (upper 3 bits)

  | Type |    Meaning   |
  |:----:|:-------------|
  | 000  | Broadcast    |
  | 010  | Request      |
  | 011  | Response     |
  | 100  | System Error |
  | 101  | User Error   |

Note that there are two sync bytes prefixed to every messages, since the UART
hardware in most devices does not signal when it is receiving, therefore
collision avoidance it not possible for the first byte, only collision detection
and only *after* the first byte has been sent. The second byte acts as a
"detection buffer" in case another node starts sending during the first byte.

```
  1  2    3      4        5       6
┌──────┬──────┬──────┬─────────┬──────┐
│ SYNC │ CRC8 │ ADDR │ COMMAND │ TYPE │
└──────┴──────┴──────┴─────────┴──────┘
```

Messages with <=28B data:

- `CRC8`: checksum for the header *and* data
- `TYPE/LENGTH`: Message type (upper 3 bits) and length (lower 5 bits) <= 28

```
  1  2    3      4        5           6         7      ...
┌──────┬──────┬──────┬─────────┬─────────────┬────────────────────┐
│ SYNC │ CRC8 │ ADDR │ COMMAND │ TYPE/LENGTH │ ... <=28B DATA ... │
└──────┴──────┴──────┴─────────┴─────────────┴────────────────────┘
```

Messages with <8kB data:

- `TYPE`: lower 5 bits >28
- `LENGTH`: >28 and <8k
- `CRC16`: checksum for the data
- `DATA`: Up to 8kB payload (nodes may support *much* less though!)

```
  1  2    3      4        5       6      7   8   9  10    11    ...
┌──────┬──────┬──────┬─────────┬──────┬────────┬───────┬───────────────────┐
│ SYNC │ CRC8 │ ADDR │ COMMAND │ TYPE │ LENGTH │ CRC16 │ ... <8kB DATA ... │
└──────┴──────┴──────┴─────────┴──────┴────────┴───────┴───────────────────┘
```

[loopback]: https://www.mikrocontroller.net/attachment/28831/siemens_AP2921.pdf
