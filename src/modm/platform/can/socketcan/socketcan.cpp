/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/debug/logger.hpp>

#include "socketcan.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <string.h>

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

modm::platform::SocketCan::SocketCan()
{
}

modm::platform::SocketCan::~SocketCan()
{
}

bool
modm::platform::SocketCan::open(std::string deviceName /*, modm::Can::Bitrate canBitrate */)
{
	skt = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	/* Locate the interface you wish to use */
	struct ifreq ifr;
	strcpy(ifr.ifr_name, deviceName.c_str());
	ioctl(skt, SIOCGIFINDEX, &ifr); /* ifr.ifr_ifindex gets filled with that device's index */

	/* Select that CAN interface, and bind the socket to it. */
	struct sockaddr_can addr;
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	if (bind( skt, (struct sockaddr*)&addr, sizeof(addr) ) < 0)
	{
		MODM_LOG_ERROR << MODM_FILE_INFO;
		MODM_LOG_ERROR << "Could not open SocketCAN" << modm::endl;
		return false;
	};

	fcntl(skt, F_SETFL, O_NONBLOCK);

	MODM_LOG_INFO << MODM_FILE_INFO;
	MODM_LOG_INFO << "SocketCAN opened successfully with skt = " << skt << modm::endl;

	return true;
}

void
modm::platform::SocketCan::close()
{
}

modm::Can::BusState
modm::platform::SocketCan::getBusState()
{
	return BusState::Connected;
}

bool
modm::platform::SocketCan::isMessageAvailable()
{
	struct can_frame frame;
	int nbytes = recv(skt, &frame, sizeof(struct can_frame), MSG_DONTWAIT | MSG_PEEK);

	// recv returns 'Resource temporary not available' which is wired but ignored here.
	/* if (nbytes < 0)
	{
		MODM_LOG_DEBUG << MODM_FILE_INFO;
		MODM_LOG_DEBUG << strerror(errno) << modm::endl;
	} */

	return (nbytes > 0);
}

bool
modm::platform::SocketCan::getMessage(can::Message& message)
{
	struct can_frame frame;
	int nbytes = recv(skt, &frame, sizeof(struct can_frame), MSG_DONTWAIT);

	if (nbytes > 0)
	{
		message.identifier = frame.can_id;
		message.length = frame.can_dlc;
		message.setExtended(frame.can_id & CAN_EFF_FLAG);
		message.setRemoteTransmitRequest(frame.can_id & CAN_RTR_FLAG);
		for (uint8_t ii = 0; ii < frame.can_dlc; ++ii) {
			message.data[ii] = frame.data[ii];
		}
		return true;
	}
	return false;
}

bool
modm::platform::SocketCan::sendMessage(const can::Message& message)
{
	struct can_frame frame;

	frame.can_id = message.identifier;
	if (message.isExtended()) {
		frame.can_id |= CAN_EFF_FLAG;
	}
	if (message.isRemoteTransmitRequest()) {
		frame.can_id |= CAN_RTR_FLAG;
	}

	frame.can_dlc = message.getLength();

	for (uint8_t ii = 0; ii < message.getLength(); ++ii) {
		frame.data[ii] = message.data[ii];
	}

	int bytes_sent = write( skt, &frame, sizeof(frame) );

	return (bytes_sent > 0);
}
