// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_DIFFERENTIAL_DRIVE_CONTROLLER_HPP
#	error	"Don't include this file directly, use 'differential_drive_controller.hpp' instead!"
#endif

#include <xpcc/architecture.hpp>
#include <xpcc/math/interpolation/linear.hpp>
#include <xpcc/motion/odometry.hpp>

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::Pid
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::vPid(
		Configuration::V::kp,
		Configuration::V::ki,
		Configuration::V::kd,
		Configuration::V::maxErrorSum,
		Configuration::V::maxOutput);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::Pid DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::omegaPid(
		Configuration::Omega::kp,
		Configuration::Omega::ki,
		Configuration::Omega::kd,
		Configuration::Omega::maxErrorSum,
		Configuration::Omega::maxOutput);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::Pid DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::omegaPidWeak(
		Configuration::OmegaWeak::kp,
		Configuration::OmegaWeak::ki,
		Configuration::OmegaWeak::kd,
		Configuration::OmegaWeak::maxErrorSum,
		Configuration::OmegaWeak::maxOutput);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int32_t DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::vTarget = 0;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int32_t DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::omegaTarget = 0;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int16_t DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::pwmTargetLeft = 0;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int16_t DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::pwmTargetRight = 0;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::ControlStrategy
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::controlStrategy = DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::ControlStrategy::NONE;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
bool DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::calibrationInProgress = false;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::CalibrationMode
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::calibrationMode;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::Timeout<> DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::calibrationTimer;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<> DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterLeft(
		Configuration::OvercurrentFilterLeft::maxValue,
		Configuration::OvercurrentFilterLeft::lowerBound,
		Configuration::OvercurrentFilterLeft::upperBound);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<> DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterRight(
		Configuration::OvercurrentFilterRight::maxValue,
		Configuration::OvercurrentFilterRight::lowerBound,
		Configuration::OvercurrentFilterRight::upperBound);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterLeftSlow(
		Configuration::OvercurrentFilterLeftSlow::maxValue,
		Configuration::OvercurrentFilterLeftSlow::lowerBound,
		Configuration::OvercurrentFilterLeftSlow::upperBound);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterRightSlow(
		Configuration::OvercurrentFilterRightSlow::maxValue,
		Configuration::OvercurrentFilterRightSlow::lowerBound,
		Configuration::OvercurrentFilterRightSlow::upperBound);


template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::driftWheelsFilterForward(
		Configuration::DriftWheelsFilterForward::maxValue,
		Configuration::DriftWheelsFilterForward::lowerBound,
		Configuration::DriftWheelsFilterForward::upperBound);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::driftWheelsFilterAngle(
		Configuration::DriftWheelsFilterAngle::maxValue,
		Configuration::DriftWheelsFilterAngle::lowerBound,
		Configuration::DriftWheelsFilterAngle::upperBound);

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
float DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::incForward = 0;
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
float DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::incAngle = 0;

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Ramp<float>  DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::speedTargetRamp(
		Configuration::SpeedTargetRamp::increment,
		Configuration::SpeedTargetRamp::decrement,
		Configuration::SpeedTargetRamp::initialValue);

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
LoggerOutputStreamDummy<typename DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::StreamData>
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::outputStream;

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::initialize()
{
	controlStrategy = ControlStrategy::NONE;
	calibrationInProgress = false;
}

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::setRobotSpeed(int16_t v, float omega)
{
	if (calibrationInProgress) {
		// calibration in progress => abort
		return;
	}

	int16_t omegaTicks = omega * (WheelBase / 2.0 * TicksPerMm * EngineTimestep) + 0.5;
	int16_t vTicks = v * (TicksPerMm * EngineTimestep) + 0.5;

	{
		xpcc::atomic::Lock lock;
		controlStrategy = ControlStrategy::ROBOT;
		omegaTarget = omegaTicks;
		vTarget = vTicks;
	}
}

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::setPwm(int16_t pwmLeft, int16_t pwmRight)
{
	controlStrategy = ControlStrategy::PWM;
	pwmTargetLeft = pwmLeft;
	pwmTargetRight = pwmRight;
}

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::setRobotSpeedForward(int16_t v)
{
	if (calibrationInProgress) {
		// calibration in progress => abort
		return;
	}

	int16_t vTicks = v * (TicksPerMm * EngineTimestep) + 0.5;

	{
		xpcc::atomic::Lock lock;
		controlStrategy = ControlStrategy::ROBOT_FORWARD_ONLY;
		omegaTarget = 0;
		vTarget = vTicks;
	}
}
// ----------------------------------------------------------------------------

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::run(float engineVoltage)
{
	runMotorsControl(engineVoltage);
	runDriftCalc();
}

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::runMotorsControl(float engineVoltage)
{
	int16_t pwmLeft = 0;
	int16_t pwmRight = 0;

	int16_t speedLeft  = MotorLeft::getEncoderSteps();
	int16_t speedRight = MotorRight::getEncoderSteps();

	int16_t robotSpeedV     = (speedLeft + speedRight) / 2;
	int16_t robotSpeedOmega = (speedRight - speedLeft) / 2;

	bool overcurrentLeft = MotorLeft::getOvercurrentStatus();
	bool overcurrentRight = MotorRight::getOvercurrentStatus();
	bool currentLimit = overcurrentLeft | overcurrentRight;

	//Dragons be from here

	overcurrentFilterLeft.update(overcurrentLeft);
	overcurrentFilterLeftSlow.update(overcurrentLeft);

	overcurrentFilterRight.update(overcurrentRight);
	overcurrentFilterRightSlow.update(overcurrentRight);

	speedTargetRamp.setTarget(DifferentialDriveController::vTarget);
	speedTargetRamp.update();
	int32_t vTarget = speedTargetRamp.getValue();

	switch (controlStrategy)
	{
		case ControlStrategy::ROBOT:
		{
			vPid.update(vTarget - robotSpeedV, currentLimit);

			pwmLeft  = vPid.getValue();
			pwmRight = vPid.getValue();

			omegaPid.update(omegaTarget - robotSpeedOmega, currentLimit);

			pwmLeft  += -omegaPid.getValue();
			pwmRight +=  omegaPid.getValue();

			// feed forward
			pwmLeft  += TicksPerTimeToPwm<EngineTimestepMs>::interpolate(vTarget - omegaTarget);
			pwmRight += TicksPerTimeToPwm<EngineTimestepMs>::interpolate(vTarget + omegaTarget);

			break;
		}
		case ControlStrategy::ROBOT_FORWARD_ONLY:
		{
			vPid.update(vTarget - robotSpeedV, currentLimit);

			pwmLeft  = vPid.getValue();
			pwmRight = vPid.getValue();

			omegaPidWeak.update(omegaTarget - robotSpeedOmega, currentLimit);

			pwmLeft  += -omegaPidWeak.getValue();
			pwmRight +=  omegaPidWeak.getValue();

			// feed forward
			pwmLeft  += TicksPerTimeToPwm<EngineTimestepMs>::interpolate(vTarget - omegaTarget);
			pwmRight += TicksPerTimeToPwm<EngineTimestepMs>::interpolate(vTarget + omegaTarget);

			break;
		}
		case ControlStrategy::CALIBRATION:
		{
			if (calibrationMode == CalibrationMode::PwmForwardStep) {
				pwmLeft  = 800;
				pwmRight = 800;
			}
			else if (calibrationMode == CalibrationMode::PwmRotation) {
				pwmLeft  = -500;
				pwmRight =  500;
			}
			break;
		}
		case ControlStrategy::NONE:
		{
			break;
		}
		case ControlStrategy::PWM:
		{
			pwmLeft  = pwmTargetLeft;
			pwmRight = pwmTargetRight;
			break;
		}
	}

	switch (controlStrategy)
	{
		case ControlStrategy::NONE:
			MotorLeft::disable();
			MotorRight::disable();
			break;
		case ControlStrategy::PWM:
			MotorLeft::setPwm(pwmLeft);
			MotorRight::setPwm(pwmRight);
			break;
		default:
			// our PID controllers are set for 12V input voltage.
			// ToDo may move this to a motor

			float factor = 1.0f;
			if (engineVoltage > Configuration::BatteryVoltage::minimum) // avoid factors > 1
				factor = Configuration::BatteryVoltage::minimum/engineVoltage;

			MotorLeft::setPwm(pwmLeft   * factor);
			MotorRight::setPwm(pwmRight * factor);
			break;
	}

	if (calibrationInProgress)
	{
		StreamData data;

		// omega calibration
//		data.pwm = pwmLeft;
//		data.speedTicks = robotSpeedOmega;
//		data.speedTime = omegaTarget;
//		data.current = omegaPid.getValue();
//		data.currentLimited = 0;

		// v calibration
//		data.pwm = pwmLeft;
//		data.speedTicks = robotSpeedV;
//		data.speedTime = ticksToPwm.interpolate(vTarget - omegaTarget);
//		data.current = vPid.getValue();
//		data.currentLimited = 0;

		// left Motor test
		data.pwm = pwmLeft;
		data.speedTicks = speedLeft;
		data.speedTime = 0;
		data.current = 0;
		data.currentLimited = 0;

		outputStream.write(&data);
	}
}

// ----------------------------------------------------------------------------

template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::runDriftCalc()
{
	// umrechnung von odometry steps zu motor steps
	// eigentlich braucht man nicht zwischen fL und fR zu unterscheiden da sie fast gleich sind, aber egal
	constexpr float fL = (Configuration::Odometry::BowFactorLeft  * 2) * TicksPerMm;
	constexpr float fR = (Configuration::Odometry::BowFactorRight * 2) * TicksPerMm;

//	const float odoAxisDist = constant::odometry::WHEELBASE;
//	const float driveAxisDist = constant::drive::WHEELBASE;

	// odo and drive wheels got a different axis dist: calc a scale factor (strahlensatz)
//	constexpr float scaleFactor = driveAxisDist/odoAxisDist;

	int16_t odoStepsLeft  = fL * OdometryLeft::getEncoderSteps();
	int16_t odoStepsRight = fR * OdometryRight::getEncoderSteps();

//	int16_t driveStepsLeft  = MotorLeft::getEncoderSteps();
//	int16_t driveStepsRight = MotorRight::getEncoderSteps();

	int16_t odoSum = odoStepsLeft + odoStepsRight;
//	int16_t driveSum = driveStepsLeft + driveStepsRight;

	int16_t odoDiff = odoStepsLeft - odoStepsRight;
//	int16_t driveDiff = driveStepsLeft - driveStepsRight;

	driftWheelsFilterForward.update( static_cast<float>(std::abs(odoSum)) < std::abs(vTarget)*0.04f );
	driftWheelsFilterAngle.update( static_cast<float>(std::abs(odoDiff)) < std::abs(omegaTarget)*0.04f );
}

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::disable()
{
	// disable interrupts
	xpcc::atomic::Lock lock;
	controlStrategy = ControlStrategy::NONE;
}

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::startCalibration(CalibrationMode mode)
{
	if (calibrationInProgress) {
		// Another calibration in progress => abort
		return;
	}

	// create a new Output stream
	// FIXME: generate stream
//	outputStream = component::logger.createOutputStream<StreamData>();
	if (!outputStream.isValid()) {
		// Could not create a output stream => abort
		return;
	}

	calibrationTimer.restart(700);

	{
		// disable interrupts
		xpcc::atomic::Lock lock;

		if (mode == CalibrationMode::SPEED_FORWARD_STEP) {
			controlStrategy = ControlStrategy::ROBOT;

			vTarget = 50;
			omegaTarget = 0;
		}
		else if (mode == CalibrationMode::SPEED_ROTATION_STEP) {
			controlStrategy = ControlStrategy::ROBOT;

			vTarget = 0;
			omegaTarget = 40;
		}
		else {
			controlStrategy = ControlStrategy::CALIBRATION;
		}

		calibrationInProgress = true;
		calibrationMode = mode;
	}
}

// ----------------------------------------------------------------------------
template <typename Configuration, typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<Configuration, MotorLeft, MotorRight, OdometryLeft, OdometryRight>::update()
{
	if (calibrationInProgress && calibrationTimer.isExpired()) {
		{
			xpcc::atomic::Lock lock;
			controlStrategy = ControlStrategy::NONE;
			calibrationInProgress = false;
		}
		outputStream.close();
	}
}
