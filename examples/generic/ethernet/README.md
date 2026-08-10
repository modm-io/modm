# STM32 Ethernet MAC

This example exercises the STM32H5/H7 Ethernet MAC and LAN8742A PHY directly,
without a network stack. It sends one broadcast Ethernet frame per second and
alternates between the copied transmit API and the acquire/fill/commit API.
The MAC configuration uses its default locally administered address derived
from the STM32 unique identifier. The lwIP example uses the same driver
helper. The frames use
`0x88b5`, which the
[IEEE Registration Authority EtherType registry](https://standards-oui.ieee.org/ethertype/eth.txt)
assigns as Local Experimental EtherType 1 for public prototype and
vendor-specific protocol development. They are not IP packets.
The typed MAC configuration selects `ChecksumMode::Software`, leaving hardware
checksum offload disabled because these frames contain no IP or transport
checksum.
Initialization succeeds with the cable unplugged; periodic link service reports
`down`, `negotiating`, and `up` transitions, and transmission begins only when
the negotiated link is up.

Applications that need IP networking should use the
[lwIP Ethernet example](../ethernet_lwip_raw) instead.

Build from this directory:

```sh
lbuild build
scons -Q build=release
scons -Q build=release program
```

Connect the board Ethernet port to the capture interface and start the capture
before running the `program` command. Monitor the ST-LINK virtual COM port at
115200 baud, 8 data bits, no parity, and 1 stop bit. The serial log reports
initialization, link transitions, the alternating transmit APIs used, and the
sequence number.

Use this Wireshark display filter:

```text
eth.type == 0x88b5
```

Or capture from a command prompt, replacing `<interface>` with the capture
interface reported by `tshark -D`:

```text
tshark -i <interface> -f "ether proto 0x88b5" -V
```

Both transmit APIs construct the same frame layout. Each 60-byte frame contains
the ASCII marker `modm-stm32h7-eth` followed by a little-endian 32-bit sequence
number. The destination is broadcast so the frames remain visible when the
board and capture host are connected through a switch.

Validation is manual: confirm that capture lengths are 60 bytes, source MAC
addresses match the serial log, destination addresses are broadcast, EtherType
is `0x88b5`, markers are intact, and sequence numbers increase without gaps.
