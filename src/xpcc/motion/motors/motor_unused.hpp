// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MOTOR_UNUSED_HPP
#define XPCC_MOTOR_UNUSED_HPP

/**
 * This is a dummy motor implementing the Motor Interface without functionality.
 */
class MotorUnused
{
public:
	static void
	initialize()
	{
	}

	/**
	 * Set the PWM value of a motor. Positive PWM means driving forward.
	 */
	static inline void
	setPwm(int16_t /*pwm*/)
	{
	}

	/**
	 * Disables a motor, no current flows through the windings.
	 */
	static inline void
	disable()
	{
	}

	/**
	 * Get the steps of an encoder.
	 *
	 * Returns the number of steps the encoder counted since last run().
	 * Call readEncoders on a regular basis.
	 */
	static inline int16_t
	getEncoderSteps()
	{
		return 0;
	}

	/**
	 * Set the maximum current.
	 *
	 * When the maximum current is reached the overcurrent flag is set.
	 */
	static inline void
	setCurrentLimit(uint16_t /*current*/)
	{
	}

	static inline uint16_t
	getCurrent()
	{
		return 0;
	}

	/**
	 * Get the overcurrent status of the motor.
	 */
	static inline bool
	isCurrentOverLimit()
	{
		return false;
	}

	/**
	 * must be called from 1ms Interrupt (by Hardware)
	 * Read raw encoder value and calculate number of steps
	 * and update the overcurrent
	 */
	static inline void
	run() {
	}
};

#endif // XPCC_MOTOR_UNUSED_HPP
