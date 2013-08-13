/**
 * Example of Siemens S75 Display connected to STM32's FSMC bus.
 *
 * Board used:	stm32f407_breakout
 *
 * See pinout of S75 in siemens_s75.hpp
 */

#include <xpcc/architecture.hpp>
#include <xpcc/driver/ui/display/tft_memory_bus.hpp>
#include <xpcc/driver/ui/display/siemens_s75.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led, A, 8);

using namespace xpcc::stm32;
using namespace xpcc::stm32::fsmc;

namespace lcd
{
	GPIO__OUTPUT(D0,   D, 14);
	GPIO__OUTPUT(D1,   D, 15);
	GPIO__OUTPUT(D2,   D,  0);
	GPIO__OUTPUT(D3,   D,  1);
	GPIO__OUTPUT(D4,   E,  7);
	GPIO__OUTPUT(D5,   E,  8);
	GPIO__OUTPUT(D6,   E,  9);
	GPIO__OUTPUT(D7,   E, 10);

	// The Command / Data Pin is mapped to an address pin of the FSMC.
	//	GPIO__OUTPUT(Cd,    D, 11);	// Command / Data,  FSMC: A16
	GPIO__OUTPUT(Cd,    E,  2);		// Command / Data,  FSMC: A23

	GPIO__OUTPUT(Cs,    D,  7);		// Chip Select,     FSMC: NE1
	GPIO__OUTPUT(Wr,    D,  5);		// Write operation, FSMC: NWE

	GPIO__OUTPUT(Reset, E,  3);     // Reset, not FSMC

	typedef xpcc::TftMemoryBus8Bit ParallelBus;

	typedef xpcc::SiemensS75LandscapeRight<lcd::ParallelBus, lcd::Reset> Display;
}

/**
 * Base Address: 0x60000000 for FSMC Bank 1 (first bank)
 * Offset for A16 : (1 << 16)
 * Offset for A23 : (1 << 23)
 */
lcd::ParallelBus
parallelBus(
		(volatile uint8_t *)  0x60000000,
		(volatile uint8_t *) (0x60000000 + (1 << 23)));

lcd::Display display(parallelBus);

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// Switch STM32F4 to 168 MHz (HSE clocked by an 25 MHz external clock)
	if (Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		Clock::enablePll(Clock::PllSource::PLL_HSE, 25, 336);
		Clock::switchToPll();
	}
	
	Led::setOutput(xpcc::gpio::LOW);

	lcd::Reset::setOutput(xpcc::gpio::LOW);

	//------------------------------------------------------

	xpcc::stm32::Fsmc::initialize();

	// A16
	lcd::Cd::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);

	// FSMC_NE1
	lcd::Cs::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);

	// FSMC_NWE
	lcd::Wr::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);

	lcd::D0::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);
	lcd::D1::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);
	lcd::D2::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);
	lcd::D3::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);
	lcd::D4::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);
	lcd::D5::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);
	lcd::D6::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);
	lcd::D7::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_100MHZ, xpcc::stm32::PULLUP);

	xpcc::stm32::fsmc::NorSram::AsynchronousTiming timing = {
		// read
		0,	// readAddressSetup
		15,	// readAddressHold
		0,	// readDataPhase

		// write
		0,	// writeAddressSetup
		0,	// writeAddressHold
		6,	// writeDataPhase

		// bus turn around
		0
	};

	xpcc::stm32::fsmc::NorSram::configureAsynchronousRegion(
			xpcc::stm32::fsmc::NorSram::CHIP_SELECT_1, /* NE1 */
			xpcc::stm32::fsmc::NorSram::NO_MULTIPLEX_8BIT,
			xpcc::stm32::fsmc::NorSram::SRAM_ROM,
			xpcc::stm32::fsmc::NorSram::MODE_A,
			timing);

	xpcc::stm32::fsmc::NorSram::enableRegion(xpcc::stm32::fsmc::NorSram::CHIP_SELECT_1);

	//------------------------------------------------------

	display.initialize();
	display.setFont(xpcc::font::Assertion);

	while (1)
	{
		static uint8_t x = 0;
		display.clear();
		display.setCursor(x, 5);
		display << "Hello";
		display.setCursor(46, 24);
		display << "World! abcdefghijk";

		// finished, copy to LCD
		// Drawingtime with FSMC: 14 msec
		Led::set();
		display.update();
		Led::reset();

		xpcc::delay_ms(20);

		if (++x > 170)
		{
			x = 0;
		}
	}
	
	return 0;
}
