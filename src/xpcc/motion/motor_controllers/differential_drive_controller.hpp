// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_DIFFERENTIAL_DRIVE_CONTROLLER_HPP
#define XPCC_DIFFERENTIAL_DRIVE_CONTROLLER_HPP

#include <stdint.h>
#include <xpcc/architecture/driver/atomic.hpp>
#include <xpcc/processing/timeout.hpp>
#include <xpcc/math/filter.hpp>

#include <xpcc/motion/utils.hpp> // TicksPerTimeToPwm




template <typename T>
class LoggerOutputStreamDummy
{
public:
	inline bool
	isValid()
	{
		return false;
	}

	inline void
	write(const T * /*data*/)
	{
	}

	inline void
	close()
	{
	}
};



/**
 * Closed loop velocity control of two motors which form a differential drive of a robot.
 *
 * This class controls two motors with cross-coupled PID controller for linear and angular
 * velocity.
 * All access to hardware is done by the Motor template parameter, so that
 * this is independent of processor and hardware.
 * 
 * The template parameters MotorLeft and MotorRight should implement the Motor interface.
 * The template parameters OdometryLeft and OdometryRight should implement the Odometry interface.
 *
 * \c OdometryLeft and \c OdometryRight are only used for drift calculation.
 *
 */
template<
	typename Configuration,
	typename MotorLeft,    typename MotorRight,
	typename OdometryLeft, typename OdometryRight> class DifferentialDriveController
{
public:
	enum class
	CalibrationMode
	{
		SpeedForwardStep,
		SpeedRotationStep,
		PwmForwardStep,
		PwmRotationStep,
	};

public:
	static void
	initialize();
	
	static void
	disable();
	
	/**
	 * Set pwm for the motors.
	 *
	 * Control will be deactivated.
	 */
	static void
	setPwm(int16_t pwmLeft, int16_t pwmRight);

	/**
	 * Set a new speed for the robot.
	 * 
	 * Controls forward speed and orientation.
	 */
	static void
	setRobotSpeed(int16_t v, float omega);

	/**
	 * Control forward speed but not the orientation.
	 * 
	 * Speed in mm per second.
	 */
	static void
	setRobotSpeedForward(int16_t v);
	
	static void
	startCalibration(CalibrationMode mode);
	
	static void
	setVelocityControllerParameter(const xpcc::Pid< float, 10 >::Parameter& param)
	{
		vPid.setParameter(param);
	}

	static void
	setVelocityWeakControllerParameter(const xpcc::Pid< float, 10 >::Parameter& /*param*/)
	{
		// FIXME: we don't have that yet
		// vPidWeak.setParameter(param);
	}

	static void
	setOmegaControllerParameter(const xpcc::Pid< float, 10 >::Parameter& param)
	{
		omegaPid.setParameter(param);
	}

	static void
	setOmegaWeakControllerParameter(const xpcc::Pid< float, 10 >::Parameter& param)
	{
		omegaPidWeak.setParameter(param);
	}

	static inline void
	setCurrentLimit(uint16_t current)
	{
		MotorLeft::setCurrentLimit(current);
		MotorRight::setCurrentLimit(current);
	}

	// ----------------------------------------------------------------------------

	static inline bool
	isCurrentLeftLimited()
	{
		xpcc::atomic::Lock lock;
		return overcurrentFilterLeft.getValue();
		//return slidingWheelsFilterLeft.getValue();
	}
	
	static inline bool
	isCurrentRightLimited()
	{
		xpcc::atomic::Lock lock;
		return overcurrentFilterRight.getValue();
		//return slidingWheelsFilterRight.getValue();
	}
	
	static inline bool
	isCurrentLeftLimitedSlow()
	{
		xpcc::atomic::Lock lock;
		//return false;
		//return overcurrentFilterLeftSlow.getValue();
		//return overcurrentFilterLeftSlow.getValue() || driftWheelsFilterForward.getValue();
		return overcurrentFilterLeftSlow.getValue() || driftWheelsFilterAngle.getValue() || driftWheelsFilterForward.getValue();
		//return slidingWheelsFilterLeft.getValue();
	}

	static inline bool
	isCurrentRightLimitedSlow()
	{
		xpcc::atomic::Lock lock;
		//return false;
		//return overcurrentFilterRightSlow.getValue();
		//return overcurrentFilterRightSlow.getValue() || driftWheelsFilterForward.getValue();
		return overcurrentFilterRightSlow.getValue() || driftWheelsFilterAngle.getValue() || driftWheelsFilterForward.getValue();
		//return slidingWheelsFilterRight.getValue();
	}

	static inline void
	currentLeftLimitedSlowReset()
	{
		xpcc::atomic::Lock lock;
		overcurrentFilterLeftSlow.reset(false);
		overcurrentFilterRightSlow.reset(false);
		driftWheelsFilterForward.reset(false);
		driftWheelsFilterAngle.reset(false);

		incForward = 0;
		incAngle = 0;
	}
	
	static inline uint16_t
	getCurrentLeft()
	{
		return MotorLeft::getCurrent();
	}

	static inline uint16_t
	getCurrentRight()
	{
		return MotorRight::getCurrent();
	}

	// ----------------------------------------------------------------------------

	// called from the main-loop
	static void
	update();
	
	/**
	 * must be called from 1ms Interrupt (by Hardware)
	 * uses motors uses odometry, so their internal values have to be updated before
	 */
	static void
	run(float engineVoltage);
	
private:
	static void
	runMotorsControl(float engineVoltage);
	
	static void
	runDriftCalc();
	
	enum class
	ControlStrategy
	{
		None,				/// Disable both motors.
		Robot,				/// Control strategy that controls linear and angular velocity.
		RobotForwardOnly,	/// Control strategy that focuses on driving forward.
		Calibration,		/// Fixed PWM values for calibration at beginning of game.
		Pwm,				/// No control, motors get specified target pwm.
	};
	
	typedef xpcc::Pid<float, 10> Pid;
	
	static ControlStrategy controlStrategy;
	
	static Pid vPid;
	static Pid omegaPid;
	static Pid omegaPidWeak;
	
	static int32_t vTarget;				///< ticks/timestep = (ticksr + ticksl) / 2
	
	// increasing drifting counter

	/*
	 * Rotation rate in ticks per timestep. The ticks equals the number ticks
	 * that one wheel does in one timestep!
	 * => [ticks/timestep] = (ticksr - ticksl) / 2
	 */
	static int32_t omegaTarget;
	
	// used in control strategy pwm
	static int16_t pwmTargetLeft;
	static int16_t pwmTargetRight;

	static float incForward;
	static float incAngle;

	struct StreamData
	{
		int16_t pwm;
		int16_t speedTicks;
		int16_t speedTime;
		int16_t current;
		uint8_t currentLimited;
	} __attribute__((packed));

	static bool calibrationInProgress;
	static CalibrationMode calibrationMode;
	static xpcc::Timeout<> calibrationTimer;
	//static component::Logger::OutputStream<StreamData> outputStream;
	static LoggerOutputStreamDummy<StreamData> outputStream;
	
	static xpcc::filter::Debounce<> overcurrentFilterLeft;
	static xpcc::filter::Debounce<> overcurrentFilterRight;
	static xpcc::filter::Debounce<uint16_t> overcurrentFilterLeftSlow;
	static xpcc::filter::Debounce<uint16_t> overcurrentFilterRightSlow;
	static xpcc::filter::Debounce<uint16_t> driftWheelsFilterForward;
	static xpcc::filter::Debounce<uint16_t> driftWheelsFilterAngle;
	static xpcc::filter::Ramp<float> speedTargetRamp;

private:
	// import constants into this classe's scope to make methods more readable
	/// contains the encoder ticks per mm of motion
	static constexpr float TicksPerMm          = Configuration::TicksPerMm;
	/// track width in mm
	static constexpr float WheelBase           = Configuration::WheelBase;
	/// time between calls to the engine code in seconds
	static constexpr float EngineTimestep      = Configuration::EngineTimestep;
	/// time between calls to the engine code in milliseconds
	static constexpr uint16_t EngineTimestepMs = Configuration::EngineTimestepMs;
};

#include "differential_drive_controller_impl.hpp"

#endif // XPCC_DIFFERENTIAL_DRIVE_CONTROLLER_HPP
