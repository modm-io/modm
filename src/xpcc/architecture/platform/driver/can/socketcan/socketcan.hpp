// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HOSTED_SOCKETCAN_HPP
#define XPCC_HOSTED_SOCKETCAN_HPP

#include <iostream>

#include <xpcc/architecture/interface/can.hpp>

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
