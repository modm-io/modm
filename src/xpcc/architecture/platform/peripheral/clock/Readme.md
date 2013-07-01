# Clock Driver Interface

In addition to platform specific functions, every _clock_ driver needs to
implement two standardized interfaces:
* a dynamic interface that supports changing configuration and frequency during
  runtime
* a static interface located in the _S_ subnamespace that should be used for
  static configurations because it supports some compile time checks

## Clock Interface Basics

Both, the dynamic as well as the static interface, build on the same design:
The goal is to represent different parts of a clock tree as separate classes
that can be connected to each other.
In addiditon to that the user needs to be able to diable as well as enable
these building blocks where possible.

### Clock Sources

On almost every controller supported by XPCC there are three different clock
sources:

1. **InternalClock**: the clock the controller runs on at startup
2. **ExternalClock**: an active external clock source
3. **ExternalOscillator**: an external crystal oscillator that need to be
  driven by the controller

Please note that for example on the STM32 platform the _ExternalClock_ and the
_ExternalOscillator_ both use the same hardware. Nonetheless the two separate
classes need to implemented in orde to provide a common interface.

### PLL

An important part of every clock tree is a Pll. These are used to multiply
an input frequency. Thus the need to be connected to a Clock Source and to
be enabled with a target frequency as an argument. If there is more than one
Pll on yout system, they should be called: Pll, Pll2, Pll3 etc.

### The System Clock

The **SystemClock** basically represents the multiplexer that is used to
change the main clock source of the controller. Most of the time it can be
connected to a Pll or to a Clock Source directly.

## The Dynamic Interface

**TODO**

## The Static Interface

**TODO**

## TODO

We need to find a way to fit the F_CPU define into our clock architecture.
If someone uses the static clock setup only, F_CPU can be specified as a macro,
but if someone was going to dynamically change clock speeds, this would
probably break some things (_which??_).
For dynamic clock control one could maybe use a value in ram that contains
the CPU speed and is updated by the dynamic clock classes. However, this
will mean worse performance and changing a lot of code.

## Current Interfaces


### atXMega

from _platform/avr/atxmega/clock.hpp_:

	enableExternalClock(
		OSC_FRQRANGE_t frequency = OSC_FRQRANGE_2TO9_gc)
	enableExternalOscillator(
		OSC_FRQRANGE_t frequency = OSC_FRQRANGE_12TO16_gc,
		OSC_XOSCSEL_t startupTime = OSC_XOSCSEL_XTAL_256CLK_gc);
	enablePll(OSC_PLLSRC_t source, uint8_t factor);
	selectSystemClockSource(CLK_SCLKSEL_t source);
	setSystemClockPrescaler(
		CLK_PSADIV_t prescalerA = CLK_PSADIV_1_gc,
		CLK_PSBCDIV_t prescalerBC = CLK_PSBCDIV_1_1_gc);


### AVR32

from _platform/avr32/uc3b/clock.hpp_

	enableOsc(OscConfig mode);
	startOsc(StartupConfig startup = StartupConfig::CYCLES_2048);
	switchToClock(ClockSource source);
	pllSetup(Pll pll, Mul mul, Div div, Osc osc,
		uint8_t lockcount, Range range, OutputDivider div2);
	pllEnable(Pll pll);
	selectClock(bool pbadiv, uint8_t pbasel, bool pbbdiv,
		uint8_t pbbsel, bool cpudiv, uint8_t cpusel);
	gcSetup(GenericClock gc, GenericClockSource source,
		bool diven, uint8_t div = 0);
	gcEnable(GenericClock gc);
	enableUsbClock(void);


### LPC2xxx

from _platform/arm7/lpc/clock.hpp_

	initialize()
	getPllMultiplier()


### STM32F1

from _platform/cortex_m3/stm32/stm32f1/clock.hpp_


	bool enableHse(HseConfig config, uint32_t waitCycles = 1500)
	enablePll2(uint32_t div, Pll2Mul mul)
	enablePll(PllSource source, PllMul pllMul, PreDiv1Source preDivSource)
	enablePll(PllSource source, PllMul pllMul)
	bool switchToPll(uint32_t waitCycles = 1500)


### STM32F2/F4

from _platform/cortex_m3/stm32/stm32f2_4/clock.hpp_

	bool enableHse(HseConfig config, uint32_t waitCycles = 1500)
	enablePll(PllSource source, uint8_t pllM, uint16_t pllN)
	bool switchToPll(uint32_t waitCycles = 1500)


### STM32F3

from _platform/cortex_m3/stm32/stm32f3/clock.hpp_

	bool enableHse(HseConfig config, uint32_t waitCycles = 1500)
	enablePll(PllSource source, PllMul pllM)
	bool switchToPll(int32_t waitCycles = 1500)
	bool switchToHse(uint32_t waitCycles = 1500)
