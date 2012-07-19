#ifndef XPCC_AVR32__CLOCK_HPP
#define XPCC_AVR32__CLOCK_HPP

#include <stdint.h>

#include <avr32/io.h>
//#include <avr32/pm.h>

namespace xpcc
{
	namespace avr32
	{
		/**
		 * Clock managment.
		 *
		 * \code
		 * typedef xpcc::avr32::Clock C;
		 *
		 * enableOsc();
		 * enablePll();
		 * switchToClock();
		 * \endcode
		 *
		 * \ingroup avr32
		 */
	class Clock
	{
	public:
		/**
		 * \brief	Configure the Oscillator depending on the external crystal
		 *
		 * Datasheet section 9.6.5
		 */
		enum class OscConfig : uint8_t
		{
			EXTERNAL_CLOCK 				= AVR32_PM_OSCCTRL0_MODE_EXT_CLOCK,	///< An external clock is connected.
			CRYSTAL_400_TO_900_KHZ		= AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G0,	///< A crystal with a frequency between 400 and 900 kHz is connected.
			CRYSTAL_900_KHZ_TO_3_MHZ	= AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G1,	///< A crystal with a frequency between 900 kHz and 3 MHz is connected.
			CRYSTAL_3_MHZ_TO_8_MHZ		= AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G2,	///< A crystal with a frequency between 3 MHz and 8 MHz is connected.
			CRYSTAL_ABOVE_8_MHZ			= AVR32_PM_OSCCTRL0_MODE_CRYSTAL_G3,	///< A crystal with a frequency above 8 MHz is connected.
		};

		/**
		 * \brief	Startup Time for Oscillator
		 *
		 * Datasheet section 9.6.5
		 */
		enum class StartupConfig : uint8_t
		{
			CYCLES_0			= AVR32_PM_OSCCTRL0_STARTUP_0_RCOSC,
			CYCLES_64			= AVR32_PM_OSCCTRL0_STARTUP_64_RCOSC,
			CYCLES_128			= AVR32_PM_OSCCTRL0_STARTUP_128_RCOSC,
			CYCLES_2048			= AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC,
			CYCLES_4096			= AVR32_PM_OSCCTRL0_STARTUP_4096_RCOSC,
			CYCLES_8192			= AVR32_PM_OSCCTRL0_STARTUP_8192_RCOSC,
			CYCLES_16384		= AVR32_PM_OSCCTRL0_STARTUP_16384_RCOSC,
		};

		/**
		 * \brief	Source of the main clock
		 *
		 * Datasheet section 9.6.1
		 */
		enum class ClockSource : uint8_t
		{
			SLOW_CLOCK 	= AVR32_PM_MCSEL_SLOW,	///< The slow clock is the source for the main clock.
			OSC0 		= AVR32_PM_MCSEL_OSC0,	///< Oscillator 0 is the source for the main clock.
			PLL0		= AVR32_PM_MCSEL_PLL0,	///< PLL0 is the source for the main clock.
		};

		enum class Pll : uint8_t
		{
			PLL0	= 0,
			PLL1	= 1,
		};

		enum class Osc : uint8_t
		{
			OSC0	= 0,
			OSC1	= 1,
		};

		enum class Range : uint8_t
		{
			MHZ_160_TO_240		= 0,	///< The PLL operates in the frequency range from 160 to 240 MHz
			MHZ_80_TO_180		= 1,	///< The PLL operates in the frequency range from 80 to 180 MHz
		};

		enum class OutputDivider : uint8_t
		{
			FPLL_EQUAL_FVCO 					= 0,
			FPLL_EQUAL_FCVO_DIVIDED_BY_TWO		= 1,
		};

		enum class GenericClock : uint8_t
		{
			GCLK0_PIN		= AVR32_PM_GCLK_GCLK0,
			GCLK1_PIN		= AVR32_PM_GCLK_GCLK1,
			GCLK2_PIN		= AVR32_PM_GCLK_GCLK2,
			USBB			= AVR32_PM_GCLK_USBB,
//			ABDAC			= AVR32_PM_GCLK_ABDAC,
		};

		enum class GenericClockSource : uint8_t
		{
			OSC0			= 0x00,		///< The source of the generic clock is an oscillator with number 0.
			OSC1			= 0x01,		///< The source of the generic clock is an oscillator with number 1.
			PLL0			= 0x02,		///< The source of the generic clock is a  PLL        with number 0.
			PLL1			= 0x03,		///< The source of the generic clock is a  PLL        with number 1.
		};




	public:
		static void
		enableOsc(OscConfig mode);

		static void
		enableClk(StartupConfig startup = StartupConfig::CYCLES_2048);

		static void
		switchToClock(ClockSource source);

		/**
		 * \brief	Setup up a PLL.
		 *
		 * \param	pll			Which PLL to configure
		 * \param	mul			Multiplier.
		 * \param	div			Divider.
		 * \param	osc			Which oscillator is source of PLL.
		 * \param	lockcount
		 */
		static void
		pllSetup(Pll pll, uint8_t Mul, uint8_t div, Osc osc, uint8_t lockcount, Range range, OutputDivider div2);

		/**
		 * \brief	Start PLL and wait for lock.
		 *
		 * \param	pll			Which PLL to start
		 */
		static void
		pllEnable(Pll pll);

		/**
		 * \brief	Setup a generic clock.
		 */
		static void
		gcSetup(GenericClock gc, GenericClockSource source, bool diven, uint8_t div = 0);

		static void
		gcEnable(GenericClock gc);

		/**
		 * \brief	Supply 48 MHz to the USB controller using a 12 MHz crystal at OSC0 and PLL1.
		 */
		static void
		enableUsbClock();
	}; // Clock class
	} // avr32 namespace
} // xpcc namespace

#endif // XPCC_AVR32__CLOCK_HPP
