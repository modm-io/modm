// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef SINGLE_MOTOR_CONTROLLER
#define SINGLE_MOTOR_CONTROLLER

#include <xpcc/math/filter/s_curve_controller.hpp>
#include <xpcc/math/filter/pid.hpp>

#include "single_motor_controller_interface.hpp"

/**	\brief	Control a single motor by speed or by position and speed.
 *
 * This controller has two controllers:
 *   - a speed controller
 *   - a position controller.
 * The user can select which of these controllers is currently active.
 *
 * The class supplied by the template parameter 'Motor' must provide an
 * interface described in ../motors/README.md
 */
template< typename Motor >
class SingleMotorController : public SingleMotorControllerInterface {

public:
	static inline void
	setVelocityControllerParameter(const SingleMotorControllerInterface::PidControllerParameters& param);

	static inline void
	setPositionControllerParameter(const xpcc::SCurveController< float >::Parameter& param);

	static inline void
	configure(ControlMode mode, uint16_t currentLimit);

	static inline bool
	isInitialised();

	static inline bool
	isCurrentOverLimit();

	static inline bool
	isMoving();

	static inline bool
	isTargetPositionReached();

	static inline int32_t
	getPosition();

	static inline int32_t
	getVelocity();

	static inline int32_t
	getCurrentPwm();

	static inline State
	getState();

	static inline void
	setCurrentLimit(uint16_t currentLimit);

	static inline void
	setControlMode(ControlMode mode);

	static inline void
	disable();

	static inline void
	setPwm(int16_t signed_pwm);

	static inline void
	setTargetVelocity(int32_t motorVelocity);

	static inline int32_t
	getTargetVelocity();

	static inline void
	setTargetPosition(int32_t position);

	static inline int32_t
	getTargetPosition();

	static inline void
	setPresentPosition(int32_t position);

	static void
	run();

private:

	static void
	runVelocityControl();

	static void
	runPositionControl();

	static void
	integratePosition(int32_t deltaSteps);

	static ControlMode controlMode;
	static State state;

	static int32_t currentPwm;

	static int32_t targetPosition;
	static int32_t currentPosition;

	static int32_t targetVelocity;
	static int32_t currentVelocity;

	static xpcc::Pid<float> speedController;
	static xpcc::SCurveController<float>::Parameter para;
	static xpcc::SCurveController<float> positionController;

};

#include "single_motor_controller_impl.hpp"

// make sure interface is implemented
//#include "../motors/motor_unused.hpp"
//template class IsSingleMotorController<SingleMotorController<MotorUnused> >;

#endif /*SINGLE_MOTOR_CONTROLLER */
