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

// from big/little
#include <configuration/drive.hpp>
#include <configuration/engine.hpp>
#include <configuration/odometry.hpp>

// from common
#include <xpcc/motion/odometry.hpp>

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::Pid
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::vPid(
		Configuration::Drive::DifferentialDriveControllerV::kp,
		Configuration::Drive::DifferentialDriveControllerV::ki,
		Configuration::Drive::DifferentialDriveControllerV::kd,
		Configuration::Drive::DifferentialDriveControllerV::maxErrorSum,
		Configuration::Drive::DifferentialDriveControllerV::maxOutput);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::Pid DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::omegaPid(
		Configuration::Drive::DifferentialDriveControllerOmega::kp,
		Configuration::Drive::DifferentialDriveControllerOmega::ki,
		Configuration::Drive::DifferentialDriveControllerOmega::kd,
		Configuration::Drive::DifferentialDriveControllerOmega::maxErrorSum,
		Configuration::Drive::DifferentialDriveControllerOmega::maxOutput);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::Pid DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::omegaPidWeak(
		Configuration::Drive::DifferentialDriveControllerOmegaWeak::kp,
		Configuration::Drive::DifferentialDriveControllerOmegaWeak::ki,
		Configuration::Drive::DifferentialDriveControllerOmegaWeak::kd,
		Configuration::Drive::DifferentialDriveControllerOmegaWeak::maxErrorSum,
		Configuration::Drive::DifferentialDriveControllerOmegaWeak::maxOutput);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int32_t DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::vTarget = 0;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int32_t DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::omegaTarget = 0;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int16_t DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::pwmTargetLeft = 0;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
int16_t DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::pwmTargetRight = 0;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
typename DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::ControlStrategy
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::controlStrategy = DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::ControlStrategy::NONE;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
bool DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::calibrationInProgress = false;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
robot::packet::EngineCalibrationMode DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::calibrationMode;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::Timeout<> DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::calibrationTimer;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<> DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterLeft(
		Configuration::Drive::DifferentialDriveControllerOvercurrentFilterLeft::maxValue,
		Configuration::Drive::DifferentialDriveControllerOvercurrentFilterLeft::lowerBound,
		Configuration::Drive::DifferentialDriveControllerOvercurrentFilterLeft::upperBound);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<> DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterRight(
		Configuration::Drive::DifferentialDriveControllerOvercurrentFilterRight::maxValue,
		Configuration::Drive::DifferentialDriveControllerOvercurrentFilterRight::lowerBound,
		Configuration::Drive::DifferentialDriveControllerOvercurrentFilterRight::upperBound);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterLeftSlow(
		Configuration::Drive::OvercurrentFilterLeftSlow::maxValue,
		Configuration::Drive::OvercurrentFilterLeftSlow::lowerBound,
		Configuration::Drive::OvercurrentFilterLeftSlow::upperBound);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::overcurrentFilterRightSlow(
		Configuration::Drive::OvercurrentFilterRightSlow::maxValue,
		Configuration::Drive::OvercurrentFilterRightSlow::lowerBound,
		Configuration::Drive::OvercurrentFilterRightSlow::upperBound);


template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::driftWheelsFilterForward(
		Configuration::Drive::DriftWheelsFilterForward::maxValue,
		Configuration::Drive::DriftWheelsFilterForward::lowerBound,
		Configuration::Drive::DriftWheelsFilterForward::upperBound);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Debounce<uint16_t> DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::driftWheelsFilterAngle(
		Configuration::Drive::DriftWheelsFilterAngle::maxValue,
		Configuration::Drive::DriftWheelsFilterAngle::lowerBound,
		Configuration::Drive::DriftWheelsFilterAngle::upperBound);

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
float DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::incForward = 0;
template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
float DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::incAngle = 0;

template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
xpcc::filter::Ramp<float>  DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::speedTargetRamp(
		Configuration::Drive::SpeedTargetRamp::increment,
		Configuration::Drive::SpeedTargetRamp::decrement,
		Configuration::Drive::SpeedTargetRamp::initialValue);

// ----------------------------------------------------------------------------
template <typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
component::Logger::OutputStream<typename DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::StreamData>
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::outputStream;

namespace component
{
	extern Logger logger;
}

using namespace constant::drive;

// ----------------------------------------------------------------------------
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::initialize()
{
	controlStrategy = ControlStrategy::NONE;
	calibrationInProgress = false;
}

// ----------------------------------------------------------------------------
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::setRobotSpeed(int16_t v, float omega)
{
	if (calibrationInProgress) {
		// calibration in progress => abort
		return;
	}

	int16_t omegaTicks = omega * (WHEELBASE / 2.0 * TICK_PER_MM * TIMESTEP_ENGINE) + 0.5;
	int16_t vTicks = v * (TICK_PER_MM * TIMESTEP_ENGINE) + 0.5;

	{
		xpcc::atomic::Lock lock;
		controlStrategy = ControlStrategy::ROBOT;
		omegaTarget = omegaTicks;
		vTarget = vTicks;
	}
}

// ----------------------------------------------------------------------------
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::setPwm(int16_t pwmLeft, int16_t pwmRight)
{
	controlStrategy = ControlStrategy::PWM;
	pwmTargetLeft = pwmLeft;
	pwmTargetRight = pwmRight;
}

// ----------------------------------------------------------------------------
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::setRobotSpeedForward(int16_t v)
{
	if (calibrationInProgress) {
		// calibration in progress => abort
		return;
	}

	int16_t vTicks = v * (TICK_PER_MM * TIMESTEP_ENGINE) + 0.5;

	{
		xpcc::atomic::Lock lock;
		controlStrategy = ControlStrategy::ROBOT_FORWARD_ONLY;
		omegaTarget = 0;
		vTarget = vTicks;
	}
}
// ----------------------------------------------------------------------------

template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::run()
{
	runMotorsControl();
	runDriftCalc();
}

// ----------------------------------------------------------------------------
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::runMotorsControl()
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
			pwmLeft  += constant::engine::ticksToPwm.interpolate(vTarget - omegaTarget);
			pwmRight += constant::engine::ticksToPwm.interpolate(vTarget + omegaTarget);

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
			pwmLeft  += constant::engine::ticksToPwm.interpolate(vTarget - omegaTarget);
			pwmRight += constant::engine::ticksToPwm.interpolate(vTarget + omegaTarget);

			break;
		}
		case ControlStrategy::CALIBRATION:
		{
			if (calibrationMode == robot::packet::EngineCalibrationMode::PWM_FORWARD_STEP) {
				pwmLeft  = 800;
				pwmRight = 800;
			}
			else if (calibrationMode == robot::packet::EngineCalibrationMode::PWM_ROTATION_STEP) {
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
			if (global::engineVoltage > Configuration::Drive::BatteryVoltage::minimum) // avoid factors > 1
				factor = Configuration::Drive::BatteryVoltage::minimum/global::engineVoltage;

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

template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::runDriftCalc()
{
	// umrechnung von odometry steps zu motor steps
	// eigentlich braucht man nicht zwischen fL und fR zu unterscheiden da sie fast gleich sind, aber egal
	constexpr float fL = (Configuration::Odometry::BOW_FACTOR_LEFT  * 2) * constant::drive::TICK_PER_MM;
	constexpr float fR = (Configuration::Odometry::BOW_FACTOR_RIGHT * 2) * constant::drive::TICK_PER_MM;

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
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::disable()
{
	// disable interrupts
	xpcc::atomic::Lock lock;
	controlStrategy = ControlStrategy::NONE;
}

// ----------------------------------------------------------------------------
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::startCalibration(robot::packet::EngineCalibrationMode mode)
{
	if (calibrationInProgress) {
		// Another calibration in progress => abort
		return;
	}

	// create a new Output stream
	outputStream = component::logger.createOutputStream<StreamData>();
	if (!outputStream.isValid()) {
		// Could not create a output stream => abort
		return;
	}

	calibrationTimer.restart(700);

	{
		// disable interrupts
		xpcc::atomic::Lock lock;

		if (mode == robot::packet::EngineCalibrationMode::SPEED_FORWARD_STEP) {
			controlStrategy = ControlStrategy::ROBOT;

			vTarget = 50;
			omegaTarget = 0;
		}
		else if (mode == robot::packet::EngineCalibrationMode::SPEED_ROTATION_STEP) {
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
template<typename MotorLeft, typename MotorRight, typename OdometryLeft, typename OdometryRight>
void
DifferentialDriveController<MotorLeft, MotorRight, OdometryLeft, OdometryRight>::update()
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
