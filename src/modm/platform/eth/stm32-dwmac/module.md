# STM32H5/H7 Ethernet

This backend supports the Synopsys DWMAC Ethernet v3.0 block used by classic
STM32H7 devices and by STM32H563/H573. It provides the MAC, MTL, one channel-DMA
ring, Clause-22 MDIO access, and deferred interrupt processing. Ethernet PHYs
remain separate drivers.

The backend is available for STM32H7 device metadata marked `eth:stm32-v3.0`
and for STM32H563/H573 packages with that driver. The different v3.1 block in
STM32H7R/S and the v3.2 block in STM32H5E/F are deliberately excluded. Devices
without Ethernet metadata, including STM32H562, are also excluded. The
dual-core STM32H745/H747/H755/H757 targets are globally disabled by the current
modm target catalog and have not been added to this driver's supported set.


## Storage and ownership

Descriptor rings and packet buffers are caller-owned and statically allocated:

```cpp
using Storage = modm::platform::EthernetStorage<4, 4, 1536>;
Storage storage;
using Ethernet = modm::platform::Eth<
    Storage, modm::ethernet::MediaInterface::Rmii>;
```

The storage template parameters are the RX descriptor count, TX descriptor
count, and packet-buffer size. The buffer size controls DMA allocation and may
be larger than a frame; frames are limited to 1518 bytes including one VLAN tag
and excluding the hardware-generated FCS. `tryTransmit()` copies one frame into
a DMA buffer and never waits for a descriptor. It returns `NotStarted` while the
MAC is stopped. `receive()` returns a move-only, zero-copy lease holding a
`std::span<const uint8_t>`. Destroying or explicitly releasing the lease returns
its descriptor exactly once. Only one RX lease may be outstanding, so the
application must consume frames promptly and call `poll()` regularly.

The driver does not allocate memory and does not use stack-specific packet
types. It checks at initialization that the complete storage object is aligned
and located in an Ethernet-DMA-accessible SRAM region.


## H7 memory and cache

Classic H7 Ethernet DMA can access D1, D2, and D3 SRAM, but not ITCM or DTCM.
The generated hardware trait rejects storage in the TCM regions. The default H7
policy performs Cortex-M7 D-cache clean/invalidate operations and isolates every
descriptor and packet buffer on 32-byte cache-line boundaries.

The normal `.bss` section on STM32H723 is in D1 SRAM and is suitable. Storage
can be placed explicitly, for example:

```cpp
modm_section(".bss_d2_sram1")
Storage storage;
```

An application that already maps the entire storage region as non-cacheable may
select `modm::platform::stm32::dwmac::Stm32NonCacheablePolicy` as the third
`Eth` template argument, after the media interface. The Ethernet driver neither
changes the MPU nor disables the D-cache.

STM32H563/H573 use the no-maintenance policy for the on-chip SRAM regions
accepted by the generated hardware trait. The H5 DCACHE is on the slow path to
external FMC/OCTOSPI memory, not on the path to this internal SRAM. External
memory is not accepted as Ethernet storage. A future target that makes a
DMA-accessible storage region cacheable must provide an appropriate cache
policy or non-cacheable storage; the policy is part of the `Eth` type for this
reason.

On H5, the storage region and Ethernet peripheral must also be accessible from
the application's TrustZone security domain. The driver does not change SRAM or
peripheral security attribution.


## Board integration and raw MAC use

The application supplies every pin. This RMII example uses one valid H723 pin
mapping; other packages and boards may select any combination accepted by the
generated GPIO connector:

```cpp
Ethernet::connect<GpioC1::Mdc, GpioA2::Mdio,
    GpioA1::Refclk, GpioA7::Rcccrsdv,
    GpioC4::Rxd0, GpioC5::Rxd1, GpioB11::Txen, GpioB12::Txd0,
    GpioB13::Txd1>();

constexpr modm::platform::EthernetMacAddress mac{0x02, 0, 0, 0, 0, 1};
auto result = Ethernet::initialize<Board::SystemClock>(storage, mac);
```

The `Eth` media-interface template argument is the single source of truth for
GPIO validation, peripheral selection, recovery, and interface-specific
workarounds. `connect()` checks its complete RMII or MII signal set at compile
time. `initialize()` configures RCC, the interface selector, MAC, MTL, DMA
rings, MAC address, MDIO clock, and interrupt. It returns a bounded reset
failure instead of waiting indefinitely. Ownership is shared across all `Eth`
template specializations, so a second specialization cannot reconfigure an
already-owned peripheral. The board remains responsible for:

- generating or receiving the required 50 MHz RMII reference clock;
- PHY address straps and all RMII/MII electrical connections;
- an optional PHY reset GPIO and its timing;
- an optional PHY interrupt GPIO; polling does not require one.

After initialization, apply the first PHY state with `setLinkState()`, then call
`start()` for a valid link. A speed, duplex, or link-state change must use the
explicit `stop()` -> `setLinkState()` -> conditional `start()` sequence;
configuration returns `MacRunning` instead of modifying a running MAC.
`poll()` reclaims TX descriptors, advances returned RX descriptors, handles
recoverable DMA conditions, and reports fatal errors as `RecoveryRequired`. A
stopped MAC is never restarted by deferred events. Call `recover()` from
foreground context; it refuses to reset while an RX lease is outstanding.

`setInternalLoopback(true)` enables the MAC loopback bit for datapath testing
and, like link configuration, requires a stopped MAC. It is not a promise of
PHY-independent board operation: the STM32 Ethernet software reset and RMII
logic may still require a valid external reference clock. This path has compile
coverage but still needs measurement on the target board.


## LAN8720A and LAN8742A

The MAC type is also its static Clause-22 MDIO transport. PHY address is a
compile-time argument and is never assumed by the MAC:

```cpp
using Lan8720 = modm::Lan8720aPhy<Ethernet, 0>;
using Lan8742 = modm::Lan8742aPhy<Ethernet, 3>;

Lan8742 phy;
modm::ethernet::PhyIdentifier identifier;
auto idResult = phy.readIdentifier(identifier);
auto resetResult = phy.startSoftwareReset(nowMilliseconds);
```

Call `pollSoftwareReset()` until it returns `Ok` or `Timeout`, configure the
advertisement with `configureAdvertisement()`, then call
`startAutoNegotiation()` and periodically `pollAutoNegotiation()`. These state
machines perform one bounded MDIO step per call and do not hide a multi-second
wait. `readLinkState()` handles the PHY's latched-low link bit and reports
speed/duplex. Forced speed and duplex are available through
`configureForcedLink()`.

Once the PHY reports a stable state, stop the MAC before passing a changed
`phy.linkState()` to `Ethernet::setLinkState()`, then restart only when the new
state is up. Do not apply the cached PHY state after an MDIO error. LAN8720A and
LAN8742A register details remain in the PHY layer; the MAC backend contains no
PHY-specific register numbers.


## Hardware validation status

Register layout, code generation, hosted descriptor/PHY behavior, and target
compilation are validated without attached PHY hardware. External clocking,
signal integrity, strap values, PHY reset timing, negotiation, traffic under
load, cache behavior on silicon, and recovery after injected bus faults still
require tests on the actual board and PHY.
