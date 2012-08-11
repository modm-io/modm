
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

	ssd::Spi::configurePins(ssd::Spi::MappingSck::PIO2_1);
//	ssd::Spi::configurePins(ssd::Spi::MappingSck::PIO0_6);
	ssd::Spi::initialize();

	led::DuoGreen::set();

	ssd::Load::setOutput();
	ssd::Pwm::setOutput();

}

// ----------------------------------------------------------------------------
void
Hardware::update()
{
	// dispatcher.update();
}
