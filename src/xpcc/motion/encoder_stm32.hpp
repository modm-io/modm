#ifndef XPCC_MOTION__ENCODER_STM32_HPP
#define XPCC_MOTION__ENCODER_STM32_HPP

#include <stdint.h>

/**
 * Use a Timer of STM32 in slave mode to read a quadrature encoder.
 *
 * Template parameter TIMER must be one of the Advanced Timers.
 * Template parameter GPIO_A and GPIO_B must be channel 1 and 2 pins
 * of the corresponding timer connected to signal A and B, respectively, 
 * of the quadrature encoder.
 *
 * run() must be called on a regular basis and the getEncoderRaw() returns
 * the current counter value.
 *
 * This encoder can then be used in encoder integration.
 *
 * Example:
 *   typedef EncoderStm32< xpcc::stm32::Timer3, Carrier::Encoder0A, Carrier::Encoder0B> Stm32EncoderLeft;
 *
 * Run one:
 *   initialize();
 *
 * Run on regular basis (e.g. 1 ms in interrupt handler)
 * 	run()
 *
 */
template < typename TIMER, typename GPIO_A, typename GPIO_B >
class EncoderStm32
{
public:
	typedef uint16_t EncoderT;

	static inline void
	initialize()
	{
		GPIO_A::connect(TIMER::Channel1, GPIO_A::InputType::PullUp);
		GPIO_B::connect(TIMER::Channel2, GPIO_B::InputType::PullUp);

		TIMER::enable();
		TIMER::setMode(TIMER::Mode::UpCounter, TIMER::SlaveMode::Encoder3);
		TIMER::setOverflow(0xffff);
		TIMER::start();
	}

	// Sample the counter value to member variable
	static inline void
	run()
	{
		encoderRaw = TIMER::getValue();
	}

	// Get the raw counter value from last sample point
	static inline EncoderT
	getEncoderRaw()
	{
		return encoderRaw;
	}

private:
	static
	EncoderT encoderRaw;
};

template < typename TIMER, typename GPIO_A, typename GPIO_B >
EncoderT
EncoderStm32<TIMER, GPIO_A, GPIO_B>::encoderRaw;

#endif /* XPCC_MOTION__ENCODER_STM32_HPP */
