/*
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2013, Martin Rosekeit
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HOSTED_CAN_USB_HPP
#define XPCC_HOSTED_CAN_USB_HPP

#include <queue>
#include <string>

// FIXME: remove this dependency!
#include "../../uart/hosted/serial_interface.hpp"
#include <xpcc/architecture/interface/can.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

namespace xpcc
{

namespace hosted
{

/**
 * Driver for a CAN232 or CANUSB adapter
 *
 * @see		http://www.canusb.com/
 * @see		http://www.can232.com/
 * @ingroup	hosted
 */
class CanUsb : public ::xpcc::Can
{
public:
	CanUsb();

	~CanUsb();

	bool
	open(std::string deviceName, unsigned int serialBaudRate, xpcc::Can::Bitrate canBitrate = xpcc::Can::kBps125);

	void
	close();

	inline bool
	isMessageAvailable()
	{
		return (!this->readBuffer.empty());
	}

	bool
	getMessage(can::Message& message);

	inline bool
	isReadyToSend()
	{
		// TODO
		return true;
	}

	BusState
	getBusState();

	/*
	 * Send a message over the CAN.
	 *
	 * @return `true` if the message was send, `false` otherwise
	 */
	bool
	sendMessage(const can::Message& message);

	bool
	isOpen()
	{
		return this->serialPort.isOpen();
	}

private:
	// updated by boost thread
	void
	update();

private:
	typedef boost::mutex				Mutex;
	typedef boost::mutex::scoped_lock	MutexGuard;

	Mutex stateLock;
	Mutex readBufferLock;
	bool active;

	BusState busState;

	xpcc::hosted::SerialInterface serialPort;

	std::string tmpRead;
	std::queue<can::Message> readBuffer;

	boost::thread* thread;
};

}	// namespace hosted

}	// namespace xpcc

#endif // XPCC_HOSTED_CAN_USB_HPP
