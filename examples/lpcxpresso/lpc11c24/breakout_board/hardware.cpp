
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

	ssd::Ssel::setOutput();
	ssd::Load::setOutput();

	adc::Adc::initialize(static_cast<uint8_t>(adc::Adc::ChannelMask::PIO1_4));
	adc::Adc::startConverstion(adc::Adc::ChannelMask::PIO1_4);

	// PIO1_1 is a reserved pin and must be set as a GPIO first.
	ssd::Pwm::setOutput();
	// Set Alternative function after using setOutput because setOutput overwrites
	// the whole register.
	LPC_IOCON->R_PIO1_1  &= ~0x07;
	LPC_IOCON->R_PIO1_1  |= 0x01;

}

// ----------------------------------------------------------------------------
void
Hardware::update()
{
	// dispatcher.update();
}
