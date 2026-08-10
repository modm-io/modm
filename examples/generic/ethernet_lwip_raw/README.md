# STM32 lwIP Ethernet

The default configuration uses STM32H7 hardware checksum offload:

```sh
lbuild build
scons -Q build=release
scons -Q build=release program
```

Build the same example with lwIP software checksums and a separate output
directory using:

```sh
lbuild -D modm:lwip:checksum=software \
  -D modm:build:build.path=../../../build/generic/ethernet_lwip_raw_software \
  build
scons -Q build=release
scons -Q build=release program
```

After programming, monitor the ST-LINK virtual COM port at 115200 baud, 8 data
bits, no parity, and 1 stop bit. Run the repository host checks from this
directory:

```sh
python ethernet_echo.py 10.66.0.42
python ethernet_echo.py 10.66.0.42 --stress
python ethernet_echo.py 10.66.0.42 --throughput
```

The base checks validate ping plus UDP and TCP echo integrity. Stress mode
validates delayed reads, concurrent clients, reset recovery, and repeated
connections. Throughput mode validates the length and SHA-256 digest of a
32 MiB TCP echo before reporting goodput; no minimum throughput is specified.
Review the serial diagnostics for adapter, MAC, and DMA errors after each run.

These host checks do not inject malformed packets, force PHY speed or duplex,
or change the physical link while traffic is active. The STM32H7 Ethernet
[hardware suite](../../../test/modm/platform/eth/stm32h7/README.md) covers
malformed loopback frames and forced PHY modes as documented there. Active link
transitions require a separate external-link-partner procedure.
