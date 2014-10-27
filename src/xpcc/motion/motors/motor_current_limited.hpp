// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MOTOR_CURRENT_LIMITED_HPP
#define XPCC_MOTOR_CURRENT_LIMITED_HPP

/**
 * Wraps around a Motor and limits the current in that motor.
 *
 */
template<typename Motor, typename ToPwm>
class MotorCurrentLimited
{
public:
	static void
	initialize()
	{
		// Do not initialize underlying motor.
	}

	static inline void
	setPwm(int16_t pwm)
	{
		Motor::setPwm(pwm);
		lastPwm = pwm;
	}

	static inline void
	disable()
	{
		Motor::disable();
	}

	static inline void
	setCurrentLimit(uint16_t current)
	{
		Motor::setCurrentLimit(current);
		currentLimit = current;
	}

	static inline int16_t
	getEncoderSteps()
	{
		return Motor::getEncoderSteps();
	}

	static inline bool
	getOvercurrentStatus()
	{
		if (Motor::isCurrentOverLimit())
			return true;
		const int16_t preferredPwm = ToPwm::interpolate(getEncoderSteps());
		const int16_t actualPwm = lastPwm;

		if (preferredPwm > 0){
			if (actualPwm - preferredPwm > currentLimit)
				return true;
		}
		else {
			if (actualPwm - preferredPwm < -currentLimit)
				return true;
		}
		return false;
	}

	static inline uint16_t
	getCurrent()
	{
		return Motor::getCurrent();
	}

	static inline void
	run() {
		Motor::run();
	}

private:
	/**
	 * Der Software Current Limit
	 */
	static uint16_t
	currentLimit;

	/**
	 * Fuer das software current limit gebraucht
	 */
	static
	int16_t lastPwm;
};

#include "motor_current_limited_impl.hpp"

#endif // XPCC_MOTOR_CURRENT_LIMITED_HPP
