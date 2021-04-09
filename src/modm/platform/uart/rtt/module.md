# Real Time Transfer (RTT)

This module implements the RTT protocol compatible with OpenOCD. RTT works by
placing multiple ring-buffers in RAM which the debugger read and writes using
background memory accesses via SWD. It therefore works on any Cortex-M devices
without extra pins except for the SWDCLK and SWDIO.

See https://www.segger.com/jlink-rtt.html

The RTT channels are exposed as a UART interface which you can use like so:

```cpp
modm::platform::Rtt rtt(/* channel= */0);
// Access data directly via UART interface: eg. loop back
if (uint8_t data; rtt.read(data)) { rtt.write(data); }
// Or wrap into an IOStream for printing
modm::IODeviceObjectWrapper<modm::platform::Rtt,
                            modm::IOBuffer::DiscardIfFull> rtt_device(rtt);
modm::IOStream stream(rtt_device);
stream << "Hello World" << modm::endl;
// Reading is more annoying
char data;
stream.get(data);
if (data != modm::IOStream::eof) { /* process new data */ }
```

You can define the number of channels and their buffer size by setting the
`buffer.tx` and `buffer.rx` set options. Note that you can define *multiple*
buffer sizes indexed by channel. Here is an example of three channels:

```xml
<!-- Channel0: TX only with 256B buffer -->
<!-- Channel1: RX only with 128B buffer -->
<!-- Channel2: TX with 512B and RX with 64B buffer -->
<option name="modm:platform:rtt:buffer.tx">256, 0, 512</option>
<option name="modm:platform:rtt:buffer.rx">0, 128, 64</option>
```

You can set the buffer size to `0` if you don't want to use this channel
direction. This won't allocate a buffer and save a little RAM.


## Accessing Data

[OpenOCD has built-in support for RTT][rtt] and modm generates a config that
opens each RTT channel as a TCP port starting at 9090 (ie. 9090=channel 0,
9091=channel 1, etc).

```sh
openocd -f modm/openocd.cfg -c modm_rtt
```

You can also call this from inside GDB via the `monitor` command:

```
(gdb) monitor modm_rtt
rtt: Searching for control block 'modm.rtt.modm'
rtt: Control block found at 0x20001024
Listening on port 9090 for rtt connections
(gdb)
```

You can then use for example `telnet 127.0.0.1 9090` to connect to the stream.

Note that this connection does not halt the target, you should therefore be able
to use this at any point during program execution.

A simple telnet client is integrated into the build system generator, however,
it can only connect to one stream at a time (disconnect with Ctrl+D).

```
 $ scons log-rtt
╭───OpenOCD───> Real Time Transfer
╰─────RTT────── stm32f103rbt
Info : rtt: Searching for control block 'modm.rtt.modm'
Info : rtt: Control block found at 0x20000008
Listening on port 9090 for rtt connections
loop 51
loop 52
loop 53
^D

 $ make log-rtt channel=0
Info : rtt: Searching for control block 'modm.rtt.modm'
Info : rtt: Control block found at 0x20000008
Listening on port 9090 for rtt connections
loop 58
loop 60
loop 61
```

If you want to use this as a proper communication channel with a custom protocol
you should implement the OpenOCD config yourself (with different ports).


[rtt]: http://openocd.org/doc/html/General-Commands.html#Real-Time-Transfer-_0028RTT_0029
