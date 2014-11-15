#include <xpcc/architecture/platform.hpp>
#include "../../stm32f4_discovery.hpp"

#include <xpcc/motion/encoder_stm32.hpp>
#include <xpcc/motion/encoder_integration.hpp>
#include <xpcc/motion/odometry.hpp>

// ----------------------------------------------------------------------------
struct
OdometryConfiguration
{
	static constexpr float WheelBase = 285.2;
	static constexpr float AngleFactorLeft =   1.2971385268961444E-4;
	static constexpr float AngleFactorRight =  1.2975322755503696E-4;
	static constexpr float BowFactorLeft  = (AngleFactorLeft  * WheelBase / 2);
	static constexpr float BowFactorRight = (AngleFactorRight * WheelBase / 2);
};

MAIN_FUNCTION
{
	/**
	 * Encoders are connected to STM hardware Timers
	 */
	class Carrier {
	public:
		typedef xpcc::stm32::Timer3 Encoder0Timer;
		typedef xpcc::stm32::GpioInputC6 Encoder0A;
		typedef	xpcc::stm32::GpioInputC7 Encoder0B;

		typedef xpcc::stm32::Timer5 Encoder1Timer;
		typedef xpcc::stm32::GpioInputA0 Encoder1A;
		typedef	xpcc::stm32::GpioInputA1 Encoder1B;
	};

	/**
	 * Configure a STM timer with the help of this template.
	 * Call run() on a regular basis.
	 */
	typedef xpcc::motion::EncoderStm32<Carrier::Encoder0Timer, Carrier::Encoder0A, Carrier::Encoder0B> EncoderStm32Left;
	typedef xpcc::motion::EncoderStm32<Carrier::Encoder1Timer, Carrier::Encoder1A, Carrier::Encoder1B> EncoderStm32Right;

	EncoderStm32Left::initialize();
	EncoderStm32Right::initialize();

	/**
	 * To integrate the encoders and get the steps the encoder moved, use EncoderIntegration.
	 * EncoderIntegration takes any method that returns the current counter value from a timer.
	 * If the encoder counts downwards when moving forward, add template parameter REVERSED = true.
	 * Call run() on a regular basis. Make sure it is called after EncoderStm32::run()
	 */
	typedef xpcc::motion::EncoderIntegration<EncoderStm32Left::getEncoderRaw       > EncoderLeft;
	typedef xpcc::motion::EncoderIntegration<EncoderStm32Right::getEncoderRaw, true> EncoderRight;

	EncoderLeft::initialize();
	EncoderRight::initialize();

	/**
	 * From these two encoders a Odometry can be build. The Odometry template needs configuration parameters.
	 */
	typedef xpcc::motion::Odometry<EncoderLeft, EncoderRight, OdometryConfiguration> MyOdometry;

	// ------------------------------------------------------------------------

	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();

		// Run all the components
		EncoderStm32Left::run();
		EncoderStm32Right::run();
		EncoderLeft::run();
		EncoderRight::run();
		MyOdometry::run();

		// Now the position was calculated
		xpcc::Location2D<> *location;
		MyOdometry::getLocation(location);

		xpcc::delayMilliseconds(500);
	}

	return 0;
}
