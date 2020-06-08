# Asynchronous Multi-Node Bus (AMNB)

The AMNB (**A**synchronous **M**ulti-**N**ode **B**us) is a multi-master bus
system, using p-persistent CSMA/CD to send messages.

One bus can be populated with up to 256 (logical) nodes. The nodes can be
queried for data and they will respond like an SAB Slave, and can query data
from other nodes like an SAB Master, or they can just broadcast a message. Each
node can listen to all the responses and broadcasts and store that information
for its purpose.

Action callbacks to query requests can be defined as well as universal callbacks
to any transmitted messaged (Listener callbacks). As an optional advanced
feature, error handling callbacks can also be defined, which fire if messages
have not been able to be sent, or requests timed out or misbehaved in other
manners, or other nodes query unavailable information.


## Protocol Structure

Message without data:

- `SYNC`: Synchronization sequence.
- `CRC8`: checksum for the header
- `ADDR`: Node address.
- `TYPE`: Message type .
- `COMMAND`: Command code.

```
  1  2    3      4        5       6
+------+------+------+---------+------+
| SYNC | CRC8 | ADDR | COMMAND | TYPE |
+------+------+------+---------+------+
```

Message with <=28B data:

- `CRC8`: checksum for the header and data.
- `TYPE/LENGTH`: Message type and length.

```
  1  2    3      4        5           6         7      ...
+------+------+------+---------+-------------+--------------------+
| SYNC | CRC8 | ADDR | COMMAND | TYPE/LENGTH | ... <=28B DATA ... |
+------+------+------+---------+-------------+--------------------+
```

Message with <8kB data:

- `CRC16`: checksum for the data.
- `DATA`: Up to 8kB payload (nodes may support *much* less though!)

```
  1  2    3      4        5       6      7   8   9  10    11    ...
+------+------+------+---------+------+--------+-------+-------------------+
| SYNC | CRC8 | ADDR | COMMAND | TYPE | LENGTH | CRC16 | ... <8kB DATA ... |
+------+------+------+---------+------+--------+-------+-------------------+
```


## Message Types

| Type |                Meaning                 |
|:----:|:---------------------------------------|
| 000  | Data broadcast by a node               |
| 010  | Data request by a node                 |
| 100  | Negative response from the node (NACK) |
| 101  | Positive response from the node (ACK)  |

When transmitting, the *second bit* determines, whether or not to expect an
answer from the addressed node. To just send information without any need for
acknowledgment, use a broadcast.

When a node is responding, the *second bit* has the following meaning:

- `true`: Message is an positive response and may contain a payload
- `false`: Message signals an error condition and carries only one byte of
           payload. This byte is an error code.


## Electrical characteristics

Between different boards CAN transceivers are used. Compared to RS485 the CAN
transceivers have the advantage to work without a separate direction input. You
can just connected the transceiver directly to the UART of your microcontroller.
These are identical to the SAB CAN electrical characteristics. You have to use
the CAN transceivers, otherwise it cannot be determined, if the bus is busy or
available for transmission.
