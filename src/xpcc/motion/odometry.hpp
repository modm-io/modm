
#ifndef	XPCC_MOTION__ODOMETRY
#define XPCC_MOTION__ODOMETRY

#include <cmath>
#include <xpcc/math/geometry/location_2d.hpp>
#include <xpcc/architecture/driver/atomic.hpp>

namespace xpcc {
namespace motion {

/**
 * Position calculation with the external odometry wheels
 * 
 */
template <typename OdometryEncoderLeft,
	typename OdometryEncoderRight,
	typename OdometryCalibrationParameters>
class Odometry
{
public:
	/** Geschwindigkeit des Roboters, x ist geradeaus */
	struct RobotSpeed
	{
		RobotSpeed() : vx(0), vy(0), omega(0) {};

		RobotSpeed(int16_t vx, int16_t vy, float omega):
			vx(vx),
			vy(vy),
			omega(omega)
		{};

		int16_t vx;
		int16_t vy;
		float omega;
	} __attribute__((packed));
	
public:
	enum Wheel
	{
		LEFT = 0,
		RIGHT = 1,
	};
	
public:
	static void
	setLocation(const xpcc::Location2D<>& location);
	
	// Get robot location in mm
	static void
	getLocation(xpcc::Location2D<> *location);
	
	// Get robot speed in mm/s and rad/s
	static void
	getSpeed(RobotSpeed* speed);
	
	static inline int32_t
	getAccumulatedSteps(Wheel wheel)
	{
		xpcc::atomic::Lock lock;
		return accumulate[wheel];
	}
	
	/**
	 * must be called from 1ms Interrupt (by Hardware)
	 * calculates the robot Location and Speed
	 */
	static void
	run();
	
private:
	static int32_t stepsTemp[2];
	static int32_t accumulate[2];
	
	/// In world-coordinates [mm]
	static xpcc::Location2D<float> robotLocation;
	static RobotSpeed robotSpeed;	// in [mm/s] and [rad/s]
	
	// used to integrate over a given time period
	static float robotForwardSpeedAccumulated;
	static float robotAngularSpeedAccumulated;
	
	static uint8_t counterLocation;
	static uint8_t counterSpeed;
};

} // motion namespace
} // xpcc namespace

#include "odometry_impl.hpp"

#endif	// XPCC__MOTION_ODOMETRY_HPP
