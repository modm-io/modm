
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

	xpcc::lpc::SysTickTimer::enable();

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
	ssd::Spi::initialize();

	led::DuoGreen::set();

	ssd::Ssel::setOutput();
	ssd::Load::setOutput();

	uint8_t mask = static_cast<uint8_t>(adc::Adc::ChannelMask::PIO1_2);
	uint8_t interruptMask = static_cast<uint8_t>(adc::Adc::ChannelMask::PIO1_2);

	adc::Adc::configurePins(mask);
	adc::Adc::initialize();
	adc::Adc::startConversion(mask, interruptMask);

	// PIO1_1 is a reserved pin and must be set as a GPIO first.
	ssd::Pwm::setOutput();
	// Set Alternative function after using setOutput because setOutput overwrites
	// the whole register.
	LPC_IOCON->R_PIO1_1  &= ~0x07;
	LPC_IOCON->R_PIO1_1  |= 0x01;

	can::Can::initialize(xpcc::can::BITRATE_125_KBPS);

	usb::uart uart(1000000);
	usb::uart::setBaudrate(1500000);
	uint8_t str[] = "Hello!";
	usb::uart::write(str, 12);

	servo::EnA::setOutput(0);
	servo::EnB::setOutput(0);
	servo::PwmA::setOutput(0);
	servo::PwmB::setOutput(0);

	servo::pwmTimer::enable();
	servo::pwmTimer::setPrescaler(0);
	servo::pwmTimer::setModePwm(0x03, 2000);
	servo::pwmTimer::setMatchValue(0, 2000); // off
	servo::pwmTimer::setMatchValue(1, 2000); // off
}

// ----------------------------------------------------------------------------
void
Hardware::update()
{
	// dispatcher.update();
}
