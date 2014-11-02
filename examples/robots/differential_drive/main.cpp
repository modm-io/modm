#include <xpcc/architecture/platform.hpp>
#include "../../stm32f4_discovery/stm32f4_discovery.hpp"


#include <xpcc/motion/encoder_stm32.hpp>
#include <xpcc/motion/encoder_integration.hpp>
#include <xpcc/motion/odometry.hpp>
#include <xpcc/motion/motors/motor_unused.hpp>
#include <xpcc/motion/motors/motor_current_limited.hpp>
#include <xpcc/motion/motor_controllers/differential_drive_controller.hpp>

#include "configuration.hpp"


// Left Odometry Encoder
typedef xpcc::stm32::Timer3 Encoder0Timer;
typedef GpioInputC6 Encoder0A;
typedef	GpioInputC7 Encoder0B;
typedef xpcc::motion::EncoderStm32<Encoder0Timer, Encoder0A, Encoder0B> OdometryEncoderStm32Left;
typedef xpcc::motion::EncoderIntegration<OdometryEncoderStm32Left::getEncoderRaw, true> MyOdometryEncoderLeft;

// Right Odometry Encoder
typedef xpcc::stm32::Timer5 Encoder1Timer;
typedef GpioInputA0 Encoder1A;
typedef	GpioInputA1 Encoder1B;
typedef xpcc::motion::EncoderStm32<Encoder1Timer, Encoder1A, Encoder1B> OdometryEncoderStm32Right;
typedef xpcc::motion::EncoderIntegration<OdometryEncoderStm32Right::getEncoderRaw, false> MyOdometryEncoderRight;

// Connect Encoders together as one Odometry
typedef xpcc::motion::Odometry<
		MyOdometryEncoderLeft,
		MyOdometryEncoderRight,
		Configuration::DifferentialDriveController::Odometry> MyOdometry;

// Motors for Differential Drive
typedef MotorUnused MyMotorDriveLeft;	/// Placeholder for Left Driving Motor
typedef MotorUnused MyMotorDriveRight;	/// Placeholder for Right Driving Motor

// Convert encoder ticks per time to duty cycle using the correct time base.
typedef TicksPerTimeToPwm<Configuration::DifferentialDriveController::EngineTimestepMs> TicksPerTimestepToPwm;

// Limit current of MyMotorDriveLeft
typedef MotorCurrentLimited<MyMotorDriveLeft, TicksPerTimestepToPwm> MotorDriveLeftCurrentLimited;
typedef MotorCurrentLimited<MyMotorDriveRight, TicksPerTimestepToPwm> MotorDriveRightCurrentLimited;


// Build a differential drive from Motors and Odometry
typedef DifferentialDriveController <
			Configuration::DifferentialDriveController,
			MotorDriveLeftCurrentLimited,
			MotorDriveRightCurrentLimited,
			MyOdometryEncoderLeft,
			MyOdometryEncoderRight> MyMotorControl;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	while (1)
	{
		float engineVoltage = 18.0f;
		MyMotorControl::run(engineVoltage);
		LedBlue::toggle();
		LedGreen::toggle();
		xpcc::delayMilliseconds(1);
	}

	return 0;
}
