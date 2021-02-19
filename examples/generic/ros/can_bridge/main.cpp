/*
 * Copyright (c) 2018, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing/timer.hpp>

#include <can_msgs/Frame.h>
#include <ros/node_handle.h>
#include <modm/communication/ros.hpp>

namespace modm {
namespace can {
bool
messageRosToModm(const can_msgs::Frame& source, modm::can::Message& destination) {
	if (source.dlc > 8) {
		return false;
	}

	destination.setIdentifier(source.id);
	destination.setLength(source.dlc);
	destination.setExtended(source.is_extended);
	destination.setRemoteTransmitRequest(source.is_rtr);
	memcpy(destination.data, source.data, source.dlc);

	return true;
}

bool
messageModmToRos(const modm::can::Message& source, can_msgs::Frame& destination) {
	destination.id = source.getIdentifier();
	destination.dlc = source.getLength();
	destination.is_rtr = source.isRemoteTransmitRequest();
	destination.is_extended = source.isExtended();
	memcpy(destination.data, source.data, source.getLength());
	return true;
}
} // can namespace
} // modm namespace

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DISABLED

using RosSerialUart = Usart2;

namespace ros
{
	using modmHardware = ModmHardware<RosSerialUart>;
	using ModmNodeHandle = NodeHandle_<modmHardware>;
}

ros::ModmNodeHandle nh;

// For publisher
can_msgs::Frame can_msg;
ros::Publisher pub_can("/can/received_messages", &can_msg);

static void
publishMessage(const modm::can::Message& message) {
	can_msgs::Frame can_msg;
	if (modm::can::messageModmToRos(message, can_msg)) {
		can_msg.header.stamp = nh.now();
		pub_can.publish(&can_msg);
		Board::LedGreen::toggle();
	}
}

// For subscriber
static void
message_cb_can(const can_msgs::Frame& ros_can_msg) {
	modm::can::Message modm_can_msg;
	if (messageRosToModm(ros_can_msg, modm_can_msg)) {
		Can::sendMessage(modm_can_msg);
		Board::LedGreen::toggle();
	}
}


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Initialize Usart
	RosSerialUart::connect<GpioOutputA2::Tx>();
	RosSerialUart::connect<GpioInputA3::Rx>();

	// Reinit onboard UART to 1 Mbps
	// Do not use it for logging because this will destroy ROS serial messages.
	RosSerialUart::initialize<Board::SystemClock, 250_kBd>();

	CanFilter::setStartFilterBankForCan2(14);

	// Initialize Can1
	if (false) {
		Can::connect<GpioInputA11::Rx, GpioOutputA12::Tx>(Gpio::InputType::PullUp);
	} else {
		Can::connect<GpioInputB8::Rx, GpioOutputB9::Tx>(Gpio::InputType::PullUp);
	}
	Can::initialize<Board::SystemClock, 250_kbps>(9);

	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	CanFilter::setFilter(1, CanFilter::FIFO0,
			CanFilter::StandardIdentifier(0),
			CanFilter::StandardFilterMask(0));

	nh.initNode();

	ros::Subscriber<can_msgs::Frame> sub_can ("/can/sent_messages", &message_cb_can);
	nh.subscribe(sub_can);
	nh.advertise(pub_can);

	modm::ShortPeriodicTimer heartbeat(1s);
	while (true)
	{
		if (Can::isMessageAvailable())
		{
			modm::can::Message message;
			Can::getMessage(message);
			publishMessage(message);
		}

		nh.spinOnce();
		if (heartbeat.execute()) {
			Board::LedGreen::toggle();
		}
	}

	return 0;
}
