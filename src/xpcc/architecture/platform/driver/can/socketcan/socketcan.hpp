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

#ifndef XPCC_HOSTED_SOCKETCAN_HPP
#define XPCC_HOSTED_SOCKETCAN_HPP

#include <iostream>

#include <modm/architecture/interface/can.hpp>

namespace xpcc
{

namespace hosted
{

class SocketCan : public ::xpcc::Can
{
public:
	SocketCan();

	~SocketCan();

	bool
	open(std::string deviceName /*, xpcc::Can::Bitrate canBitrate = xpcc::Can::kBps125 */);

	void
	close();

	bool
	isMessageAvailable();

	bool
	getMessage(can::Message& message);

	inline bool
	isReadyToSend() { return true; }

	BusState
	getBusState();

	bool
	sendMessage(const can::Message& message);

private:
	int skt;
};

} // hosted namespace
} // xpcc namespace

#endif // XPCC_HOSTED_SOCKETCAN_HPP
