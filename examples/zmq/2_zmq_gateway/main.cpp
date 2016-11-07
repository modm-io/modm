/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>

#include <xpcc/architecture/interface/can.hpp>
#include <xpcc/architecture/platform/driver/can/canusb/canusb.hpp>
#include <xpcc/architecture/platform/driver/can/socketcan/socketcan.hpp>

#include <xpcc/communication.hpp>
#include <xpcc/communication/xpcc/backend/can.hpp>

#include <xpcc/communication/xpcc/backend/can/connector.hpp>
#include <xpcc/communication/xpcc/backend/zeromq/connector.hpp>

/**
 * Listens to a CAN bus connected by a CAN2USB and publishes xpcc messages with zeromq.
 *
 * How to use:
 * - Connect a CAN2USB to a CAN bus with traffic.
 * - Connect this CAN2USB by USB to your computer.
 * - Make sure the user can access the /dev/ttyUSB0 device.
 *   Brute force method is: #sudo chmod 777 /dev/ttyUSB0
 *   A wiser method is to add the user the appropriate group
 * - Adjust the baud rate of the CAN bus in this example.
 * - Do
 *   scons run
 * - All xpcc messages will be published on port 8211 by zeromq
 */

// Default baud rate
static constexpr uint32_t canBusBaudRate = 125000;

/* Either use an USB CAN2USB adaptor with xpcc Lawicel interpreter
   or use a CAN controller supported by Linux' SocketCAN.

   With SocketCAN the baudrate must be set with the operating system.
   $ ip link set can0 type can bitrate
*/
// static xpcc::hosted::CanUsb canUsb;
static xpcc::hosted::SocketCan canSocket;

// static xpcc::CanConnector< xpcc::hosted::CanUsb > canConnector(&canUsb);
static xpcc::CanConnector< xpcc::hosted::SocketCan > canConnector(&canSocket);

#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

int
main()
{
	XPCC_LOG_DEBUG << "ZeroMQ SocketCAN XPCC bridge" << xpcc::endl;

	// if (not canUsb.open("/dev/ttyUSB0", canBusBaudRate)) {
	if (not canSocket.open("can0" /*, canBusBaudRate */)) {
		XPCC_LOG_ERROR << "Could not open port" << xpcc::endl;
		exit(EXIT_FAILURE);
	}

	const std::string endpointOut = "tcp://*:8211";
	const std::string endpointIn  = "tcp://*:8212";

	xpcc::ZeroMQConnector zmqConnector(endpointIn, endpointOut, xpcc::ZeroMQConnector::Mode::PubPull);

	XPCC_LOG_DEBUG << "Entering main loop" << xpcc::endl;

	while(true)
	{
		canConnector.update();
		zmqConnector.update();

		while (canConnector.isPacketAvailable())
		{
			xpcc::Header header = canConnector.getPacketHeader();
			xpcc::SmartPointer payload = canConnector.getPacketPayload();

			XPCC_LOG_DEBUG << "C->Z " << header << " " << payload.getSize() << " " << payload << xpcc::endl;

			zmqConnector.sendPacket(header, payload);

			canConnector.dropPacket();
		}

		while (zmqConnector.isPacketAvailable())
		{
			xpcc::Header header = zmqConnector.getPacketHeader();
			xpcc::SmartPointer payload = zmqConnector.getPacketPayload();

			XPCC_LOG_DEBUG << "Z->C " << header << " " << payload.getSize() << " " << payload << xpcc::endl;

			canConnector.sendPacket(header, payload);

			zmqConnector.dropPacket();
		}

		xpcc::delayMilliseconds(10);
	}

	// canUsb.close();
	canSocket.close();
}
