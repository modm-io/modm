# Clock Driver  Interface


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
