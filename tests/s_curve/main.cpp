
#include <iostream>
#include <fstream>

#include <xpcc/math/filter/s_curve_controller.hpp>
#include <xpcc/math/geometry/angle.hpp>

int
main(void)
{
	/*xpcc::SCurveController<float>::Parameter parameter(
			5,		// target area
			4,		// increment
			400,	// decrease factor
			2,		// kp
			500,	// max speed
			1);		// min speed
	float target = 2000;
	float end = 2;
	float frequency = 100;*/
	
	/*xpcc::SCurveController<float>::Parameter parameter(
			1.5 / 180.0 * 3.14,	// target area
			6.0 / 50,	// increment
			15,			// decrease factor
			4.0,		// kp
			6.0,		// max speed
			0.005);		// min speed
	float target = 4;
	float end = 0.001;
	float frequency = 100;*/
	
	xpcc::SCurveController<float>::Parameter parameter(
			xpcc::Angle::toRadian(1.5),	// target area
			0.8,	// increment
			20,		// descrease factor
			8.0,		// kp
			8.0,		// max speed
			0.005);		// min speed
	float target = 4;
	float end = 0.001;
	float frequency = 100;
	
	xpcc::SCurveController<float> curve(parameter);
	
	float speed = 0;
	float oldSpeed = 0;
	float position = 0;
	
	int index = 0;
	
	std::ofstream file("plot.dat");
	while(1)
	{
		file      << index << " " << speed << " " << position << " " << (speed - oldSpeed) << std::endl;
		std::cout << index << " " << speed << " " << position << " " << (speed - oldSpeed) << std::endl;
		
		oldSpeed = speed;
		float error = target - position;
		
		curve.update(error, speed);
		speed = curve.getValue();
		
		position += speed / frequency;
		
		if (error < end) {
			file.close();
			return 0;
		}
		
		index++;
	}
}
