// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef SINGLE_MOTOR_CONTROLLER_BASE
#define SINGLE_MOTOR_CONTROLLER_BASE

#include <stdint.h>
#include <xpcc/math/filter/s_curve_controller.hpp> // for SCurveController::Parameter

/// This class serves as a template for a single motor controller
/// as well as it provides the State/ControlMode Constants that can
/// thus be used throughout different template instances of motor controllers.
class SingleMotorControllerInterface
{
public:
	enum class
	State : uint8_t {
		Uninitialised   = 0,
		HoldingPosition = 1,
		Moving          = 2,
		Disabled        = 3,
		Error           = 4,
	};

	enum class
	ControlMode : uint8_t {
		None     = 0,
		Pwm      = 1,
		Velocity = 2,
		Position = 3,
	};

public:
	class PidControllerParameters
	{
	public:
		PidControllerParameters(
				const float kp,
				const float ki,
				const float kd,
				const int32_t maxErrorSum,
				const int32_t maxOutput) :
		kp(kp), ki(ki), kd(kd), maxErrorSum(maxErrorSum), maxOutput(maxOutput)
	{
	}

	public:
		float kp;
		float ki;
		float kd;
		int32_t maxErrorSum;
		int32_t maxOutput;
	};
	
	static inline const xpcc::Pid< float >::Parameter
	pidControllerParameter2pid(const SingleMotorControllerInterface::PidControllerParameters& p)
	{
		return xpcc::Pid< float >::Parameter(p.kp, p.ki, p.kd, p.maxErrorSum, p.maxOutput);
	}

public:
	static inline const char*
	enumToString(State state)
	{
		switch(state) {
		case State::Uninitialised: return "Uninitialised";
		case State::HoldingPosition: return "HoldingPosition";
		case State::Moving: return "Moving";
		case State::Disabled: return "Disable";
		case State::Error: return "Error";
		default: return "__UNKNOWN__";
		}
	}

	static inline const char*
	enumToString(ControlMode controlMode)
	{
		switch(controlMode) {
		case ControlMode::None: return "None";
		case ControlMode::Pwm: return "Pwm";
		case ControlMode::Velocity: return "Velocity";
		case ControlMode::Position: return "Position";
		default: return "__UNKNOWN__";
		}
	}

#ifdef __DOXYGEN__
public:
	/// @name Setup Functions
	///@{

	/// set constants for the velocity/position controller
	/// you **must** call this function **before** calling
	/// `configure(ControlMode::Velocity, ...)` or `configure(ControlMode::Position, ...)`
	static void
	setVelocityControllerParameter(const SingleMotorControllerInterface::PidControllerParameters& param);

	/// set constants for the position controller
	/// you **must** call this function **before** calling
	/// `configure(ControlMode::Position, ...)`
	static void
	setPositionControllerParameter(const xpcc::SCurveController< float >::Parameter& param);

	/// setup controller with specific control mode and current limit
	/// this function may also setup the hardware and thus **must** be called
	/// before you will be able to make use of the motor controller
	static void
	configure(ControlMode mode, uint16_t currentLimit);

	///@}


	/// @name Status Functions
	///@{

	/// returns `true` if internal state is not equal to Uninitialised
	static bool
	isInitialised();

	/// returns `true` if the current is higher than the current limit
	static bool
	isCurrentOverLimit();

	/// returns `true` if internal state is _equal_ to Moving
	/// you can use this function to check whether
	/// the target position has been reached
	static bool
	isMoving();

	/// returns `true` if internal state is _not equal_ to Moving
	/// you can use this function to check whether
	/// the target position has been reached
	static bool
	isTargetPositionReached();

	/// returns the current position
	static int32_t
	getPosition();

	/// returns the current velocity (normally encoder ticks per millisecond)
	static int32_t
	getVelocity();

	/// returns the motor current as measured
	/// this might not be available on an iMotor connected via CAN!!
	static int32_t
	getCurrentPwm();

	/// returns the internal state
	static State
	getState();

	///@}


	/// @name General Control Functions
	/// can be called regardless of the ControlMode
	///@{

	/// update the current limit
	/// to set the initial current limit, please use the configure function
	static void
	setCurrentLimit(uint16_t currentLimit);

	/// update the ControlMode
	/// WARNING: this does **not** initialize the hardware, use configure in
	///          the beginning and setControlMode only to change the mode
	///          during operation
	static void
	setControlMode(ControlMode mode);

	/// disable the motor
	/// TODO: what state should this put the motor in?
	static void
	disable();

	///@}


	/// @name Pwm Control Functions
	///@{

	static void
	setPwm(int16_t pwm_signed);

	///@}


	/// @name Velocity Control Functions
	///@{

	static void
	setTargetVelocity(int32_t motorVelocity);

	static int32_t
	getTargetVelocity();

	///@}


	/// @name Position Control Functions
	///@{

	static void
	setTargetPosition(int32_t position);

	static int32_t
	getTargetPosition();

	/// Adjust the present position.
	/// If the position is known (e.g. by limit switches)
	/// the internal position can be set.
	static void
	setPresentPosition(int32_t position);

	///@}


	/// @name Run Function
	///@{

	/// call this every 1ms for this control algorithm to be calculated
	/// in regular intervals
	static void
	run();

	///@}
#endif // __DOXYGEN__

};

/// use this template class to make sure that
/// a static class implements the SingleMotorController interface
/// e.g. `IsSingleMotorController<IMotor>;`
template<class PotentialSingleMotorController>
class
IsSingleMotorController
{
private:
	void
	test() const
	{
		// TODO: add macros to make it easier to add new methods
		const SingleMotorControllerInterface::PidControllerParameters param(0,0,0,0,0);
		PotentialSingleMotorController::setVelocityControllerParameter(param);

		const xpcc::SCurveController<float>::Parameter param2(0,0,0,0,0,0,0);
		PotentialSingleMotorController::setPositionControllerParameter(param2);

		SingleMotorControllerInterface::ControlMode mode = static_cast<SingleMotorControllerInterface::ControlMode>(0);
		uint16_t currentLimit = 0;
		PotentialSingleMotorController::configure(mode, currentLimit);

		bool r0 = PotentialSingleMotorController::isInitialised(); (void)r0;

		bool r1 = PotentialSingleMotorController::isCurrentOverLimit(); (void)r1;

		bool r2 = PotentialSingleMotorController::isMoving(); (void)r2;

		bool r3 = PotentialSingleMotorController::isTargetPositionReached(); (void)r3;

		int32_t r4 = PotentialSingleMotorController::getPosition(); (void)r4;

		int32_t r5 = PotentialSingleMotorController::getVelocity(); (void)r5;

		int32_t r6 = PotentialSingleMotorController::getCurrentPwm(); (void)r6;

		SingleMotorControllerInterface::State r7 = PotentialSingleMotorController::getState(); (void)r7;

		PotentialSingleMotorController::setCurrentLimit(currentLimit);

		PotentialSingleMotorController::setControlMode(mode);

		PotentialSingleMotorController::disable();

		int16_t pwm = 0;
		PotentialSingleMotorController::setPwm(pwm);

		int32_t motorVelocity = 0;
		PotentialSingleMotorController::setTargetVelocity(motorVelocity);

		int32_t r8 = PotentialSingleMotorController::getTargetVelocity(); (void)r8;

		int32_t position = 0;
		PotentialSingleMotorController::setTargetPosition(position);

		int32_t r9 = PotentialSingleMotorController::getTargetPosition(); (void)r9;

		PotentialSingleMotorController::setPresentPosition(position);

		PotentialSingleMotorController::run();
	}
};

#endif // SINGLE_MOTOR_CONTROLLER_BASE
