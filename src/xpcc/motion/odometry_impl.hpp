#ifndef XPCC_MOTION__ODOMETRY
#error "Do not include this file. Include odometry.hpp only"
#endif

#include <math.h>

#include "odometry.hpp"

// ----------------------------------------------------------------------------

template <typename L, typename R, typename C>
int32_t xpcc::motion::Odometry<L, R, C>::stepsTemp[2] = { 0, 0 };

template <typename L, typename R, typename C>
int32_t
xpcc::motion::Odometry<L, R, C>::accumulate[2] = { 0, 0 };

template <typename L, typename R, typename C>
xpcc::Location2D<float>
xpcc::motion::Odometry<L, R, C>::robotLocation = xpcc::Location2D<float>(0, 0, 0);

template <typename L, typename R, typename C>
typename xpcc::motion::Odometry<L, R, C>::RobotSpeed
xpcc::motion::Odometry<L, R, C>::robotSpeed;

template <typename L, typename R, typename C>
float
xpcc::motion::Odometry<L, R, C>::robotForwardSpeedAccumulated = 0.0;

template <typename L, typename R, typename C>
float
xpcc::motion::Odometry<L, R, C>::robotAngularSpeedAccumulated = 0.0;

template <typename L, typename R, typename C>
uint8_t
xpcc::motion::Odometry<L, R, C>::counterLocation = 0;

template <typename L, typename R, typename C>
uint8_t
xpcc::motion::Odometry<L, R, C>::counterSpeed = 0;

// ----------------------------------------------------------------------------
template <typename L, typename R, typename C>
void
xpcc::motion::Odometry<L, R, C>::run()
{
	stepsTemp[LeftWheel]  += L::getEncoderSteps();
	stepsTemp[RightWheel] += R::getEncoderSteps();
	
	// every 2ms
	counterLocation++;
	if (counterLocation >= 2) {
		counterLocation = 0;
		
		accumulate[LeftWheel]  += stepsTemp[LeftWheel];
		accumulate[RightWheel] += stepsTemp[RightWheel];
		
		// convert to mm
		const float sl = static_cast<float>(stepsTemp[LeftWheel])  * C::BowFactorLeft;
		const float sr = static_cast<float>(stepsTemp[RightWheel]) * C::BowFactorRight;
		
		const float al = static_cast<float>(stepsTemp[LeftWheel])  * C::AngleFactorLeft;
		const float ar = static_cast<float>(stepsTemp[RightWheel]) * C::AngleFactorRight;
		
		// BOW_FACTOR_LEFT and BOW_FACTOR_RIGHT contain the divided by two
		// so it isn't needed here!
		float distance = (sr + sl);
		float angle    = (ar - al);
		
		// integrate over the distance and angle to calculate the
		// robot speed later
		robotForwardSpeedAccumulated += distance;
		robotAngularSpeedAccumulated += angle;
		
		// Convert to world coordinates
		robotLocation.move(distance, angle);
		
		stepsTemp[LeftWheel]  = 0;
		stepsTemp[RightWheel] = 0;
	}
	
	// every 10ms
	counterSpeed++;
	if (counterSpeed >= 10) {
		counterSpeed = 0;
		
		// robotForwardSpeedAccumulated is the distance driven in 10ms [mm/10ms] => convert to [mm/s]
		robotSpeed.vx    = robotForwardSpeedAccumulated * 100;
		robotSpeed.omega = robotAngularSpeedAccumulated * 100;
		
		// apply drift compensation
		// versatzX = beta*omega*v*dt*(-sinPhi)
		// versatzY = beta*omega*v*dt*(cosPhi)
		// mit beta = (-1/120.0) s
		// dt = 10ms
//		const float bwvdt(robotSpeed.omega * robotSpeed.vx * (-0.010/120.0));
//		const float phi = robotLocation.getOrientation();
//		xpcc::Vector2f offsetDirection(-std::sin(phi), std::cos(phi));
//
//		robotLocation.setPosition(robotLocation.getPosition()+(offsetDirection*bwvdt));

		robotForwardSpeedAccumulated = 0;
		robotAngularSpeedAccumulated = 0;
	}
}

// ----------------------------------------------------------------------------
template <typename L, typename R, typename C>
void
xpcc::motion::Odometry<L, R, C>::setLocation(const xpcc::Location2D<>& location)
{
	xpcc::atomic::Lock lock;
	
	robotLocation = location.convert<float>();
	
	accumulate[LeftWheel]  = 0;
	accumulate[RightWheel] = 0;
}

// ----------------------------------------------------------------------------
template <typename L, typename R, typename C>
void
xpcc::motion::Odometry<L, R, C>::getLocation(xpcc::Location2D<> *location)
{
	xpcc::atomic::Lock lock;
	
	// convert float to int16_t
	*location = robotLocation.convert<int16_t>();
}

// ----------------------------------------------------------------------------
template <typename L, typename R, typename C>
void
xpcc::motion::Odometry<L, R, C>::getSpeed(RobotSpeed * speed)
{
	xpcc::atomic::Lock lock;
	
	speed->vx = robotSpeed.vx;
	speed->omega = robotSpeed.omega;
}
