/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**
 * Example to demonstrate periodically publishing environment data
 * (temperature, pressure, humidity) to ROS as stamped messages.
 *
 * Connect a Bosch BME280 sensor and a SSD1306 (128x64) display to I2C.
 */

#include <modm/board/board.hpp>
#include <modm/processing/processing.hpp>

#include <sensor_msgs/Temperature.h>
#include <sensor_msgs/FluidPressure.h>
#include <sensor_msgs/RelativeHumidity.h>

#include "thread_bme280.hpp"
#include "thread_display.hpp"

#include "hardware.hpp"

#include <ros/node_handle.h>
#include <modm/communication/ros.hpp>

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DISABLED

// Define which UART is used for communication to and from ROS serial
// When using the STlink Uart on Nucleo boards logging must be disabled.
using RosSerialUart = Board::stlink::Uart;

namespace ros
{
	using modmHardware = ModmHardware<RosSerialUart>;
	using ModmNodeHandle = NodeHandle_<modmHardware>;
}

ros::ModmNodeHandle nh;

sensor_msgs::Temperature      temperature_msg;
sensor_msgs::FluidPressure    pressure_msg;
sensor_msgs::RelativeHumidity humidity_msg;

ros::Publisher pub_temperature("/environment/temperature",       &temperature_msg);
ros::Publisher pub_pressure   ("/environment/pressure",          &pressure_msg);
ros::Publisher pub_humidity   ("/environment/relative_humidity", &humidity_msg);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Reinit onboard UART to 1 Mbps
	// Do not use it for logging because this will destroy ROS serial messages.
	Board::stlink::Uart::initialize<Board::systemClock, modm::Uart::Baudrate::MBps1>(12);

    MyI2cMaster::connect<Board::D14::Sda, Board::D15::Scl>();
    MyI2cMaster::initialize<Board::systemClock, MyI2cMaster::Baudrate::Standard>();

	Board::LedGreen::set();

	nh.initNode();

	nh.advertise(pub_temperature);
	nh.advertise(pub_pressure);
	nh.advertise(pub_humidity);

	Bme280Thread bme280thread;
	DisplayThread display_thread;

	modm::ShortPeriodicTimer timer(1000);
	bool bme_sampling(false);

	while (true)
	{
		bme280thread.update();
		display_thread.update();

		if (timer.execute())
		{
			bme280thread.startMeasurement();
			bme_sampling = true;
		}

		if (bme_sampling and bme280thread.isNewDataAvailable())
		{
			bme_sampling = false;
			Board::LedGreen::toggle();
			int32_t temp = bme280thread.getTemperature();
			int32_t pres = bme280thread.getPressure();
			int32_t humi = bme280thread.getHumidity();

			// Convert to standard ROS units.
			temperature_msg.temperature = temp / 100.0;
			temperature_msg.variance = 1.0;
			temperature_msg.header.stamp = nh.now();

			pressure_msg.fluid_pressure = pres / 1000.0;
			pressure_msg.variance = 100;
			pressure_msg.header.stamp = nh.now();

			humidity_msg.relative_humidity = humi / 100000.0;
			humidity_msg.variance = 0.03;
			humidity_msg.header.stamp = nh.now();

			pub_temperature.publish(&temperature_msg);
			pub_pressure.publish(&pressure_msg);
			pub_humidity.publish(&humidity_msg);

			display_thread.setSeq(temperature_msg.header.stamp.sec);
			display_thread.setTemp(temp);
			display_thread.setPres(pres);
			display_thread.setHumi(humi);

			// Do not enable when STlink UART is used for rosserial
			MODM_LOG_DEBUG.printf("Temp = %6.2f\n", temp / 100.0);
		}

		nh.spinOnce();
	}

	return 0;
}
