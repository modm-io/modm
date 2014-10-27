// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef SINGLE_MOTOR_CONTROLLER
#	error	"Don't include this file directly, use 'single_motor_control.hpp' instead!"
#endif

template<typename Motor>
void
SingleMotorController<Motor>::setVelocityControllerParameter(const SingleMotorControllerInterface::PidControllerParameters& param)
{
	xpcc::Pid< float >::Parameter pidParam = pidControllerParameter2pid(param);
	speedController.setParameter(pidParam);
}

template<typename Motor>
void
SingleMotorController<Motor>::setPositionControllerParameter(const xpcc::SCurveController< float >::Parameter& param)
{
	positionController.setParameter(param);
}

template<typename Motor>
void
SingleMotorController<Motor>::configure(ControlMode mode, uint16_t currentLimit)
{
	setControlMode(mode);
	setCurrentLimit(currentLimit);
}

template<typename Motor>
bool
SingleMotorController<Motor>::isInitialised()
{
	return state != State::Uninitialised;
}

template<typename Motor>
bool
SingleMotorController<Motor>::isCurrentOverLimit()
{
	return Motor::isCurrentOverLimit();
}

template<typename Motor>
bool
SingleMotorController<Motor>::isMoving()
{
	return state == State::Moving;
}

template<typename Motor>
bool
SingleMotorController<Motor>::isTargetPositionReached()
{
	if (controlMode == ControlMode::Position) {
		return positionController.isTargetReached();
	}
	return false;
//	return state != State::Moving; // TODO: should this be state == State::HoldingPosition?
}

template<typename Motor>
int32_t
SingleMotorController<Motor>::getPosition()
{
	return currentPosition;
}

template<typename Motor>
int32_t
SingleMotorController<Motor>::getVelocity()
{
	return currentVelocity;
}

template<typename Motor>
int32_t
SingleMotorController<Motor>::getCurrentPwm()
{
	return currentPwm;
}

template<typename Motor>
typename SingleMotorController<Motor>::State
SingleMotorController<Motor>::getState()
{
	return state;
}

template<typename Motor>
void
SingleMotorController<Motor>::setCurrentLimit(uint16_t currentLimit)
{
	Motor::setCurrentLimit(currentLimit);
	// TODO: why do we need this stuff below?
	switch(controlMode)
	{
		case ControlMode::None:
		{
			state = State::Disabled;
			break;
		}
		case ControlMode::Velocity:
		{
			state = State::Moving;
			break;
		}
		case ControlMode::Position:
		{
			state = State::HoldingPosition;
			break;
		}
		case ControlMode::Pwm:
		{
			state = State::Moving;	// FIXME: is this correct?
			break;
		}
	}
}

template<typename Motor>
void
SingleMotorController<Motor>::setControlMode(ControlMode mode)
{
	controlMode = mode;
	if (mode == ControlMode::None)
	{
		Motor::disable();
		if (state != State::Uninitialised)
		{
			state = State::Disabled;
		}
	}
}

template<typename Motor>
void
SingleMotorController<Motor>::disable()
{
	Motor::disable();
	state = State::Disabled;
	controlMode = ControlMode::None;
}

template<typename Motor>
void
SingleMotorController<Motor>::setPwm(int16_t pwm_signed)
{
	if (controlMode == ControlMode::Pwm)
	{
		Motor::setPwm(pwm_signed);
		state = State::Moving;
	}
}

template<typename Motor>
void
SingleMotorController<Motor>::setTargetVelocity(int32_t motorVelocity)
{
	targetVelocity = motorVelocity;
	if (controlMode == ControlMode::Velocity)
	{
		state = State::Moving;
	}
}

template<typename Motor>
int32_t
SingleMotorController<Motor>::getTargetVelocity()
{
	return targetVelocity;
}

template<typename Motor>
void
SingleMotorController<Motor>::setTargetPosition(int32_t position)
{
	targetPosition = position;
	state = State::Moving;
}

template<typename Motor>
int32_t
SingleMotorController<Motor>::getTargetPosition()
{
	return targetPosition;
}

template<typename Motor>
void
SingleMotorController<Motor>::setPresentPosition(int32_t position) {
	currentPosition = position;
	state = State::HoldingPosition;
}

template<typename Motor>
void
SingleMotorController<Motor>::run()
{
	currentVelocity = Motor::getEncoderSteps();
	integratePosition(currentVelocity);

	switch(controlMode)
	{
		case ControlMode::Velocity:
		{
			runVelocityControl();
			break;
		}
		case ControlMode::Position:
		{
			runPositionControl();
			break;
		}
		case ControlMode::Pwm:
		case ControlMode::None:
		{
			//Nothing to be done here
			break;
		}
	}
}

// ------------- Private Methods

template<typename Motor>
void
SingleMotorController<Motor>::runVelocityControl()
{
	bool overcurrent = Motor::isCurrentOverLimit();
	speedController.update(targetVelocity - currentVelocity, overcurrent);
	currentPwm = speedController.getValue();
	Motor::setPwm(currentPwm);
}

template<typename Motor>
void
SingleMotorController<Motor>::runPositionControl()
{
	float signedDistance = (targetPosition - currentPosition);
	positionController.update(signedDistance, currentVelocity);
	float targetVelocity = positionController.getValue();
	speedController.update(targetVelocity - currentVelocity, Motor::isCurrentOverLimit());
	currentPwm = speedController.getValue();
	Motor::setPwm(currentPwm);
	if (positionController.isTargetReached())
	{
		state = State::HoldingPosition;
	}
	else
	{
		state = State::Moving;
	}
}

template<typename Motor>
void
SingleMotorController<Motor>::integratePosition(int32_t deltaSteps)
{
	//TODO handle over and underflow
	currentPosition += deltaSteps;
}


template<typename Motor>
typename SingleMotorController<Motor>::ControlMode
SingleMotorController<Motor>::controlMode = ControlMode::None;

template<typename Motor>
typename SingleMotorController<Motor>::State
SingleMotorController<Motor>::state = SingleMotorController<Motor>::State::Uninitialised;

template<typename Motor> int32_t SingleMotorController<Motor>::targetPosition = 0;
template<typename Motor> int32_t SingleMotorController<Motor>::currentPosition = 0;

template<typename Motor> int32_t SingleMotorController<Motor>::targetVelocity = 0;
template<typename Motor> int32_t SingleMotorController<Motor>::currentVelocity = 0;
template<typename Motor> int32_t SingleMotorController<Motor>::currentPwm = 0;

template<typename Motor>
xpcc::Pid<float> SingleMotorController<Motor>::speedController;

template<typename Motor> xpcc::SCurveController< float >::Parameter SingleMotorController<Motor>::para;
template<typename Motor> xpcc::SCurveController< float > SingleMotorController<Motor>::positionController(SingleMotorController<Motor>::para);
