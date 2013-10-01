
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

	led::Onboard::setOutput(xpcc::Gpio::HIGH);
	led::Xpresso::setOutput(xpcc::Gpio::HIGH);

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

	/**
	 * Convert
	 *   AD1	Current
	 *   AD3	Position
	 *   AD6/7	BEMF
	 */
	uint8_t mask =
			static_cast<uint8_t>(adc::Adc::ChannelMask::CHANNEL_1)  |
			static_cast<uint8_t>(adc::Adc::ChannelMask::CHANNEL_3)  |
			static_cast<uint8_t>(adc::Adc::ChannelMask::PIO1_10) |
			static_cast<uint8_t>(adc::Adc::ChannelMask::PIO1_11);

	adc::Adc::configurePins(mask);
	adc::Adc::initialize();

	// enable ADC interrupt at end of single conversion
	uint8_t interruptMask =
			static_cast<uint8_t>(adc::Adc::ChannelMask::CHANNEL_1) |
			static_cast<uint8_t>(adc::Adc::ChannelMask::CHANNEL_3);
	LPC_ADC->INTEN = 0;
	LPC_ADC->INTEN = interruptMask;

	/* Enable interrupts */
	NVIC_EnableIRQ(ADC_IRQn);


	// PIO1_1 is a reserved pin and must be set as a GPIO first.
	ssd::Pwm::setOutput();
	// Set Alternative function after using setOutput because setOutput overwrites
	// the whole register.
	LPC_IOCON->R_PIO1_1  &= ~0x07;
	LPC_IOCON->R_PIO1_1  |= 0x01;

	can::Can::initialize(xpcc::can::BITRATE_125_KBPS);

	usb::uart::initialize(3000000);
	usb::uart::write('#'); // start char

	servo::EnA::setOutput(0);
	servo::EnB::setOutput(0);
	servo::PwmA::setOutput(0);
	servo::PwmB::setOutput(0);

	servo::pwmTimer::enable();
	servo::pwmTimer::setPrescaler(0);
	servo::pwmTimer::setModePwm(0x03, 2000);
	servo::pwmTimer::setMatchValue(0, 2000); // off
	servo::pwmTimer::setMatchValue(1, 2000); // off

	/**
	 * Adjust sampling point so that the current is measured just
	 * before the PWM is switch off.
	 *
	 */

	// Use MAT2 Interrupt for current measurement.
	// Adjust the sampling point just before PWM is switched off.
	servo::pwmTimer::setMatchValue(2, 1550);

	// Interrupt on cycle
	// MR2 to call interrupt whenever the Timer Counter is reset
	LPC_TMR16B0->MCR |= (1 << 6); // (1 << MR2I);

	/* Enable interrupts */
	NVIC_EnableIRQ(TIMER_16_0_IRQn);

}

// ----------------------------------------------------------------------------
void
Hardware::update()
{
	// dispatcher.update();
}
