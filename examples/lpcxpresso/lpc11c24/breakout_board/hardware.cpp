
/**
 * Hardware description file for the
 * LPCXpresso breakout board.
 *
 */

#include <xpcc/architecture.hpp>

#include "config.hpp"
#include "hardware.hpp"

// ----------------------------------------------------------------------------
void
Hardware::initialize()
{
	// Setup clock
	SystemInit();

	led::Onboard::setOutput(xpcc::gpio::HIGH);
	led::Xpresso::setOutput(xpcc::gpio::HIGH);

	for (uint8_t i = 0; i < 7; ++i)
	{
		led::Onboard::toggle();
		led::Xpresso::toggle();
		xpcc::delay_ms(50);
	}

	led::DuoRed::setOutput();
	led::DuoGreen::setOutput();
	button::Up::setInput(xpcc::lpc::InputType::PULLUP);
	button::Ok::setInput(xpcc::lpc::InputType::PULLUP);
	button::Down::setInput(xpcc::lpc::InputType::PULLUP);

	ssd::Spi::initialize();
//	ssd::Spi::configurePins(ssd::Spi::MappingSck::PIO2_11)

	ssd::Load::setOutput();
	ssd::Pwm::setOutput();

}

// ----------------------------------------------------------------------------
void
Hardware::update()
{
	// dispatcher.update();
}
