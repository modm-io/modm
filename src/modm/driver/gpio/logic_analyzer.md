# Logic Analyzer

Samples one to four GPIO ports at a fixed rate into static RAM buffers, which a
debugger can read out over SWD. A timer compare event triggers one DMA channel
per port, which copies the input data register into its buffer, so the CPU does
not have to take part in the capture.

The number of ports is set at build time with the `channels` option (1 to 4,
default 2). Each channel samples one complete GPIO port, so four ports capture
64 pins. This is useful for buses such as SPI with a separate chip-select port:

```cpp
#include <modm/driver/logic_analyzer.hpp>

// Sample PA8, PB8, PC8 and PD8 at 1 MHz until the buffers are full.
// Requires `modm:driver:logic_analyzer:channels = 4` in the project.
const uint32_t rate =
    modm::LogicAnalyzer::start<Board::SystemClock, 1'000'000, GpioA8, GpioB8, GpioC8, GpioD8>();
// ... do something that is worth looking at ...
modm::LogicAnalyzer::stop();

for (uint8_t channel = 0; channel < modm::LogicAnalyzer::ports(); channel++)
{
    for (size_t ii = 0; ii < modm::LogicAnalyzer::count(channel); ii++)
    {
        const uint16_t idr = modm::LogicAnalyzer::sample(channel, ii);
        (void) idr;
    }
}
```

The sample rate is a template argument, so the timer division and the unique
port map are computed by the compiler. For multiple rates, switch over the
requested value and call the corresponding instantiation; this is smaller than
computing the timer division at runtime.

The host tool reads the descriptor and all buffers from the target, writes a
VCD file for PulseView, GWire or GTKWave, and can decode I2C:

```sh
PYTHONPATH=modm python3 -m modm_tools.logic_analyzer --elf build/scons-release/project.elf \
    --config modm/openocd.cfg --channels A8,B8,C8,D8 --vcd capture.vcd --i2c A8,B8
```

The tool is copied into `modm/modm_tools` when this driver module is selected.
In a modm checkout use `PYTHONPATH=tools` instead; `--channels all` (or simply
omitting `--channels` with `--vcd`) exports every pin of the captured ports.

## Sampling

Only the lower 16 bits of the input data registers (`GPIOx_IDR`) contain the
port pins, so the DMA reads and stores 16 bit values. Unlike the
`modm:driver:gpio_sampler`, no interrupts are used, which makes the capture
timing exact and independent of the CPU load. Because whole ports are sampled,
multiple pins with the same number on different ports can be captured at the
same time.

All ports are sampled on the **same timer event**: compare channels 1 to N of
the timer are programmed with the same compare value, so all compare events are
generated on the same timer tick. The N DMA requests are then serviced
back-to-back by the DMA arbiter. The samples are therefore separated only by a
few bus cycles, not by half a timer period as in earlier versions of this
driver.

The capture window is `Samples / rate`. In circular mode the DMA overwrites the
buffers, `stop()` records which part is valid, and the buffers contain the last
samples before the stop. `sample()` accounts for the buffer head; `data()` and
`head()` give access to the raw buffer.

## Sample Rate and Capture Time

Every sample costs one DMA transfer per port. All transfers of one timer event
must finish before the next event, otherwise the DMA loses requests. A single
DMA transfer from a GPIO register into SRAM takes about 5 to 6 bus cycles on the
channel DMA (arbitration, peripheral read, SRAM write, see AN2548) and longer on
the stream DMA, which reaches the GPIO through the bus matrix. The maximum rate
below assumes half of the DMA bandwidth to leave room for the CPU and other bus
masters:

```
max rate ≈ bus clock / (2 * cycles per transfer * ports)
```

The capture time is `samples / rate` and therefore limited by the buffer RAM.
At the maximum rate it does not depend on the number of ports: four ports have
a quarter of the samples each, but at a quarter of the rate. Each buffer holds
at most 65535 samples (the DMA transfer counter is 16 bits), or 32767 samples on
the GPDMA, which counts bytes instead of samples, so large devices can capture
longer with more ports.

| Family | DMA | Bus clock | Max rate, 1 port | Max rate, 4 ports | Free RAM for buffers | Capture time at max rate | Tested in hardware |
|---|---|---:|---:|---:|---:|---:|---|
| F0 | channel | 48 MHz | ~5 MHz | ~1.2 MHz | 2–24 kB | 0.2–2.5 ms | F091RC, 4 ports at 500 kHz |
| F1 | channel | 72 MHz | ~6 MHz | ~1.5 MHz | 10–64 kB | 0.8–5 ms | F103RB, 4 ports at 500 kHz |
| F2 | stream | 120 MHz | ~7 MHz | ~1.8 MHz | 48–96 kB | 3–7 ms | F207ZG, 4 ports at 500 kHz |
| F3 | channel | 72 MHz | ~7 MHz | ~1.8 MHz | 6–48 kB | 0.4–3.5 ms | F303VC, 4 ports at 500 kHz |
| F4 | stream | 84–180 MHz | ~5–11 MHz | ~1.3–2.8 MHz | 48–192 kB | 2–19 ms | F429ZI, 4 ports at 500 kHz |
| F7 | stream | 216 MHz | ~13 MHz | ~3.4 MHz | 192–384 kB | 5–15 ms | F722ZE, 4 ports at 500 kHz |
| G4 | channel | 170 MHz | ~17 MHz | ~4 MHz | 24–96 kB | 0.7–3 ms | G474RE, 4 ports at 500 kHz |
| H5 | GPDMA | 250 MHz | ~15 MHz | ~3.9 MHz | 16–320 kB | 1–8 ms | H563ZI, 4 ports at 500 kHz |
| H7 | stream | 200–275 MHz | ~7–9 MHz | ~1.7–2.3 MHz | 256–512 kB | 7–40 ms | H743ZI, 4 ports at 500 kHz |
| L1 | channel | 32 MHz | ~2.7 MHz | ~0.7 MHz | 8–64 kB | 1.5–12 ms | L152RE, 4 ports at 500 kHz |
| L4 | channel | 80–120 MHz | ~8–12 MHz | ~2–3 MHz | 32–256 kB | 1.4–16 ms | L476RG, 4 ports at 500 kHz |
| L5 | channel | 110 MHz | ~11 MHz | ~2.8 MHz | 128–192 kB | 6–9 ms | L552ZE-Q, 4 ports at 500 kHz |
| U0 | channel | 56 MHz | ~5 MHz | ~1.2 MHz | 6–20 kB | 0.6–2 ms | U083RC, 4 ports at 500 kHz |
| U3 | GPDMA | 96 MHz | ~6 MHz | ~1.5 MHz | 40–320 kB | 3–22 ms | U385RG-Q, 4 ports at 500 kHz |
| U5 | GPDMA | 160 MHz | ~10 MHz | ~2.5 MHz | 96–1500 kB | 3–13 ms | U575ZI-Q, 4 ports at 500 kHz |

The bus clock is the AHB clock of the usual modm `SystemClock` configurations.
The free RAM assumes that about half of the internal SRAM that the DMA can
access is left for the buffers. The cycle counts for the stream DMA (about 8 on
F2/F4/F7, about 15 on H7, where the DMA in the D2 domain reads the GPIO in the
D3 domain) and for the GPDMA (about 8) are estimates and not measured. The maximum rates are therefore
approximate; check that `isRunning()` reports no transfer error and that the
samples look plausible when getting close to them.

At lower rates the capture time grows proportionally: 16384 samples at 1 MHz
cover 16 ms, at 100 kHz 164 ms.

## Buffer Placement

The bus matrix arbitrates between the CPU and the DMA with round-robin, so on a
shared memory the CPU keeps at least half of the bandwidth, but every CPU access
can delay a DMA transfer. The buffers are therefore placed into the
`.noinit_sram1` section (`.noinit_d2_sram1` on STM32H7, `.noinit_sram1_ns` on
STM32H5/U3/U5). modm places this
section into the continuous RAM region after the static data, so large buffers
extend from SRAM1 into the following SRAM2 and SRAM3, if they are contiguous.
These SRAMs are separate bus matrix slaves and usually only used by the heap,
so only the part of the buffers in SRAM1 competes with the stack and static
data of the CPU. On STM32H7 the CPU uses DTCM and AXI SRAM for its data, so the
D2 SRAMs are not shared at all. The buffers reduce the heap by their size and
are not zeroed at startup. Targets without SRAM1 (F0, F3, L1 and others with a
single SRAM) place the buffers into `.bss`.

The GPIO read is still shared with the CPU, since both use the same AHB bus
to the GPIO.

## Timer and DMA Wiring

The timer and its N DMA channels are derived from the device data at build time.
Every STM32 family with a DMA controller supported by `modm:platform:dma` whose
GPIO registers are reachable from the DMA is supported:

| DMA controller | Families |
|---|---|
| `stm32-stream-channel` | F2, F4, F7 |
| `stm32-channel` | F0 (most devices), F1, F3, L1 |
| `stm32-channel-request` | F0 (F09x, F030xC, F098), L4 |
| `stm32-mux` | G4, L5, U0 |
| `stm32-mux-stream` | H7 (except H7RS) |
| `stm32-dma3` (GPDMA) | H5, U3, U5 |

STM32C0, STM32G0 and STM32L0 place the GPIO registers on the separate IOPORT
bus at `0x5000_0000`, which the DMA controller cannot access. A DMA read from
`GPIOx_IDR` raises a transfer error on these families, so the analyzer is not
available there either. STM32U0 uses the same address, but its reference
manual (RM0503) lists the GPIO ports on the AHB instead of the IOPORT, and its
DMA can read them.

STM32H7RS has a GPDMA and an HPDMA, but it is not supported yet, since it is
untested whether they can read the GPIO registers.

The analyzer picks a timer that has DMA requests for N compare channels and
whose request lines do not overlap. On targets with DMAMUX it uses the N
highest channels of the first DMA controller, on targets with GPDMA the N
highest channels of GPDMA1; on all other targets the fixed
timer request mapping determines the DMA channels. The chosen timer can be
overridden with the `timer` option, for example to move the analyzer out of the
way of another peripheral.

On STM32F2/F4/F7 the DMA1 peripheral port is not connected to the AHB bus matrix
and cannot read the GPIO input registers, only DMA2 can. The analyzer therefore
selects a timer whose compare channels are routed to DMA2 on these families
(usually TIM1 or TIM8). Some timer compare channels share a DMA request line with
other compare channels of the same timer; using both would lose requests, so
such combinations are not selected either.

@warning   The timer used by the analyzer must not be used by
           `modm:platform:timer` at the same time.
@note      The DMA channels are not allocated dynamically. Check that they do
           not collide with other DMA users of your application.
@note      The buffers must be placed in memory that the DMA controller can
           access. On STM32H7, DTCM is not DMA capable; on other families, CCM
           may not be accessible by every DMA controller. The analyzer never
           places its buffers there. If the DMA detects a transfer error,
           `isRunning()` returns `false` and the capture is incomplete.
@note      If the data cache is enabled, invalidate the buffers before reading
           them after the capture.
@note      On STM32H7 all DMA channels are allocated on DMA1; on all other
           supported families the fixed request mapping of the timer determines
           which DMA channels are used.
@note      At most four ports can be sampled with one timer. Sampling more than
           four ports would require multiple synchronized timers and is not
           implemented yet.

## Options

* `channels`: maximum number of GPIO ports to sample, from 1 to 4. The default
  is 2. Each channel captures one complete port (16 pins); unused channels cost
  no RAM and no DMA channels.
* `samples`: number of samples per port, at most 65535 (32767 on the GPDMA).
  Each sample uses two bytes per configured channel, so `channels` ports need
  `2 * channels * samples` bytes of RAM. The default is 16384 or a value that
  uses about a quarter of the internal SRAM for the maximum number of channels
  the target supports, whichever is smaller.
* `timer`: timer instance to use, or `auto` (the default) to derive it from the
  device data.
