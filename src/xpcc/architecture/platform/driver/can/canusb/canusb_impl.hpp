// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_CAN_USB_HPP
#error "Do not include this file directly. Include canusb.hpp"
#endif

#include <iostream>
#include <xpcc/debug/logger.hpp>

#include <xpcc/architecture/driver.hpp>
#include <xpcc/processing/timer.hpp>

#include <xpcc/driver/can/can_lawicel_formatter.hpp>
#include "canusb.hpp"

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::INFO

template <typename SerialPort>
xpcc::hosted::CanUsb<SerialPort>::CanUsb(SerialPort& serialPort)
:	active(false), busState(BusState::Off), serialPort(serialPort)
{
}

template <typename SerialPort>
xpcc::hosted::CanUsb<SerialPort>::~CanUsb()
{
	if (this->active)
	{
		{
			MutexGuard stateGuard(this->stateLock);
			this->active = false;
		}
		this->thread->join();
		delete this->thread;
		this->thread = 0;
	}
}

template <typename SerialPort>
bool
xpcc::hosted::CanUsb<SerialPort>::open(xpcc::Can::Bitrate canBitrate)
{
	if (this->serialPort.open())
	{
		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "SerialPort opened in canusb" << xpcc::endl;
		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "write 'C'" << xpcc::endl;
		this->serialPort.write("C\r");

		xpcc::ShortTimeout timeout;
		timeout.restart(500);
		while (not timeout.isExpired())
		{
		}

		char a;
		while( this->serialPort.read(a) );

		// Set CAN bitrate
		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "Set CAN bitrate" << xpcc::endl;
		
		switch (canBitrate)
		{
			case kBps10:
				this->serialPort.write("S0\r");
			break;
			case kBps20:
				this->serialPort.write("S1\r");
			break;
			case kBps50:
				this->serialPort.write("S2\r");
			break;
			case kBps100:
				this->serialPort.write("S3\r");
			break;
			case kBps125:
				this->serialPort.write("S4\r");
			break;
			case kBps250:
				this->serialPort.write("S5\r");
			break;
			case kBps500:
				this->serialPort.write("S6\r");
			break;
			case MBps1:
				this->serialPort.write("S8\r");
			break;
		}

		

		timeout.restart(500);
		while (not this->serialPort.read(a))
		{
			if (timeout.isExpired())
			{
				XPCC_LOG_DEBUG << XPCC_FILE_INFO << "Timer expired" << xpcc::endl;
				return false;
			}
		}
		if (a != '\r')
		{
			XPCC_LOG_ERROR << XPCC_FILE_INFO << "Wrong answer on set CAN bitrate: " << xpcc::hex << (int) a	<< xpcc::endl;
			return false;
		}
		
		// Open CAN channel
		this->serialPort.write("O\r");
		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "written 'O'" << xpcc::endl;
		timeout.restart(500);
		while (not this->serialPort.read(a))
		{
			if (timeout.isExpired())
			{
				XPCC_LOG_DEBUG << XPCC_FILE_INFO << "Timer expired" << xpcc::endl;
				return false;
			}
		}

		if (a != '\r')
		{
			XPCC_LOG_ERROR << XPCC_FILE_INFO << "Wrong answer on O: " << xpcc::hex << (int) a << xpcc::endl;
			return false;
		}

		{
			MutexGuard stateGuard(this->stateLock);
			this->active = true;
		}
		this->thread = new boost::thread(
				boost::bind(&xpcc::hosted::CanUsb<SerialPort>::update, this));

		busState = BusState::Connected;
		this->tmpRead.clear();
		return true;
	}
	else
	{
		busState = BusState::Off;
		XPCC_LOG_ERROR << XPCC_FILE_INFO << "Could not open Canusb" << xpcc::endl;
		return false;
	}
}

template <typename SerialPort>
void
xpcc::hosted::CanUsb<SerialPort>::close()
{
	this->serialPort.write("C\r");
	{
		MutexGuard stateGuard(this->stateLock);
		this->active = false;
	}

	this->thread->join();
	delete this->thread;
	this->thread = 0;
	busState = BusState::Off;
}

template <typename SerialPort>
xpcc::Can::BusState
xpcc::hosted::CanUsb<SerialPort>::getBusState()
{
	return busState;
}

template <typename SerialPort>
bool
xpcc::hosted::CanUsb<SerialPort>::getMessage(can::Message& message)
{
	if (not this->readBuffer.empty())
	{
		message = this->readBuffer.front();
		this->readBuffer.pop();
		return true;
	}
	else
	{
		return false;
	}
}

template <typename SerialPort>
bool
xpcc::hosted::CanUsb<SerialPort>::sendMessage(const can::Message& message)
{
	char str[128];
	xpcc::CanLawicelFormatter::convertToString(message, str);
	XPCC_LOG_DEBUG.printf("Sending ");
	char *p = str;
	while (*p != '\0') {
		XPCC_LOG_DEBUG.printf("%02x %c, ", *p, *p);
		++p;
	}
	this->serialPort.write(str);
	this->serialPort.write("\r");
	return true;
}

template <typename SerialPort>
void
xpcc::hosted::CanUsb<SerialPort>::update()
{
	while (true)
	{
		char a;
		if (this->serialPort.read(a))
		{
			XPCC_LOG_DEBUG.printf("Received %02x\n", a);
			if (a == 'T' || a == 't' || a == 'r' || a == 'R')
			{
				this->tmpRead.clear();
			}
			this->tmpRead += a;

			can::Message message;
			if (xpcc::CanLawicelFormatter::convertToCanMessage(
					this->tmpRead.c_str(), message))
			{
				this->readBuffer.push(message);
			}
		}
		if (not this->active) {
			break;
		}
	}
}
