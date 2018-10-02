/*
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <invensense/eMD.hpp>

using namespace Board;

namespace imu
{
using Scl = Board::D15;
using Sda = Board::D14;
using Master = I2cMaster1;
}
int result;

int main()
{
	Board::initialize();
	LedD13::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	imu::Master::connect<imu::Sda::Sda, imu::Scl::Scl>();
	imu::Sda::configure(Gpio::InputType::PullUp);
	imu::Scl::configure(Gpio::InputType::PullUp);
	imu::Master::initialize<Board::systemClock, imu::Master::Baudrate::Fast>();
	result = modm::emd::set_i2c_start_function([](modm::I2cTransaction *t)
	{
		return imu::Master::start(t);
	});
	MODM_LOG_INFO << "set_i2c_start_function=" << result << modm::endl;


	struct int_param_s int_param;

	result = mpu_init(&int_param);
	MODM_LOG_INFO << "mpu_init=" << result << modm::endl;

	result = inv_init_mpl();
	MODM_LOG_INFO << "inv_init_mpl=" << result << modm::endl;

	inv_enable_quaternion();
	inv_enable_9x_sensor_fusion();
	inv_enable_fast_nomot();
	inv_enable_gyro_tc();
	inv_enable_vector_compass_cal();
	inv_enable_magnetic_disturbance();
	inv_enable_eMPL_outputs();

	result = inv_start_mpl();
	MODM_LOG_INFO << "inv_start_mpl=" << result << modm::endl;

	while (1)
	{
		LedD13::toggle();
		modm::delayMilliseconds(Button::read() ? 100 : 500);

		static uint32_t counter(0);
		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
