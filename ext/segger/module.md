# Real Time Transfer (RTT)

This module implements the [RTT protocol](https://www.segger.com/jlink-rtt.html)
compatible with JLink and OpenOCD. RTT works by placing multiple ring-buffers in
RAM which the debugger read and writes using background memory accesses via
SWD. It therefore works on any Cortex-M device without extra pins except for
the SWDCLK and SWDIO.

You can define the number of channels and their buffer size by setting the
`buffer.tx` and `buffer.rx` set options. Note that you can define *multiple*
buffer sizes indexed by channel. Here is an example of three channels:

```xml
<!-- Channel0: TX only with 256B buffer -->
<!-- Channel1: RX only with 128B buffer -->
<!-- Channel2: TX with 512B and RX with 64B buffer -->
<option name="modm:rtt:buffer.tx">256, 0, 512</option>
<option name="modm:rtt:buffer.rx">0, 128, 64</option>
```

You can set the buffer size to `0` if you don't want to use this channel
direction. This won't allocate a buffer and save a little RAM.

The [API reference is available here](https://kb.segger.com/RTT#API_functions).


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
rtt: Searching for control block 'SEGGER RTT'
rtt: Control block found at 0x20001024
Listening on port 9090 for rtt connections
(gdb) continue&
```

You can then use netcat to connect to one or multiple streams:

```sh
stty -icanon -echo
nc localhost 9090
stty sane
```

Note that this connection does not halt the target, you should therefore be able
to use this at any point during program execution.

A simple RTT client is integrated into the build system generator, however,
it can only connect to one stream at a time (disconnect with Ctrl+C).

```
 $ scons log-rtt
╭───OpenOCD───> Real Time Transfer
╰─────RTT────── stm32f103rbt
Info : rtt: Searching for control block 'SEGGER RTT'
Info : rtt: Control block found at 0x20000008
Listening on port 9090 for rtt connections
loop 51
loop 52
loop 53
^C

 $ make log-rtt channel=0
Info : rtt: Searching for control block 'SEGGER RTT'
Info : rtt: Control block found at 0x20000008
Listening on port 9090 for rtt connections
loop 58
loop 60
loop 61
```

If you want to use this as a proper communication channel with a custom protocol
you should implement the OpenOCD config yourself (with different ports).

You can also use JLink to access the RTT data, which may be significantly faster
than OpenOCD if the debug probe has hardware support for the protocol. Note the
port JLink uses is 19021 (channel 0) and 19022 (channel 1), etc.

```
 $ scons log-rtt-jlink
╭────JLink────> Real Time Transfer
╰─────RTT────── stm32l476rgt6

SEGGER J-Link - Real time terminal output
RTT Demo on Nucleo-64
loop: 0
loop: 1
loop: 2
loop: 3
loop: 4
loop: 5
loop: 6
loop: 7
```


[rtt]: http://openocd.org/doc/html/General-Commands.html#Real-Time-Transfer-_0028RTT_0029
