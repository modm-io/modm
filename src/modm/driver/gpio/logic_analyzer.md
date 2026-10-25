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

The input data registers (`GPIOx_IDR`) are captured with 32 bit DMA transfers,
because several STM32 families only allow word accesses to this register. Only
the lower 16 bits contain the port pins and are exposed by `sample()`. Unlike
the `modm:driver:gpio_sampler`, no interrupts are used, which makes the capture
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

## Timer and DMA Wiring

The timer and its N DMA channels are derived from the device data at build time.
Every STM32 family with a DMA controller supported by `modm:platform:dma` whose
GPIO registers are reachable from the DMA is supported:

| DMA controller | Families |
|---|---|
| `stm32-stream-channel` | F2, F4, F7 |
| `stm32-channel` | F0 (most devices), F1, F3, L1 |
| `stm32-channel-request` | F0 (F09x, F030xC, F098), L4 |
| `stm32-mux` | G4, L5 |
| `stm32-mux-stream` | H7 (except H7RS) |

STM32C0, STM32G0 and STM32L0 place the GPIO registers on the separate IOPORT
bus at `0x5000_0000`, which the DMA controller cannot access. A DMA read from
`GPIOx_IDR` raises a transfer error on these families, so the analyzer is not
available there either.

The GPDMA-based families (H5, U3, U5, H7RS) are not supported because
`modm:platform:dma` has no GPDMA driver and the device data does not describe
the GPDMA request mapping. STM32U0 is excluded by `modm:platform:dma` as well.
Selecting `:driver:logic_analyzer` for one of these targets is unavailable.

The analyzer picks a timer that has DMA requests for N compare channels and
whose request lines do not overlap. On targets with DMAMUX it uses the N
highest channels of the first DMA controller; on all other targets the fixed
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
           may not be accessible by every DMA controller. The analyzer buffers
           are ordinary `.bss` data and modm places them in the main SRAM.
           If the DMA detects a transfer error, `isRunning()` returns `false`
           and the capture is incomplete.
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
* `samples`: number of samples per port. Each sample uses four bytes per
  configured channel, so `channels` ports need `4 * channels * samples` bytes
  of RAM. The default is 16384 or a value that uses about a quarter of the
  internal SRAM for the maximum number of channels the target supports,
  whichever is smaller.
* `timer`: timer instance to use, or `auto` (the default) to derive it from the
  device data.
