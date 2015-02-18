// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <iostream>
#include <xpcc/debug/logger.hpp>

#include <xpcc/architecture/driver.hpp>
#include <xpcc/processing/timer.hpp>

#include <xpcc/driver/can/can_lawicel_formatter.hpp>
#include "canusb.hpp"

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::INFO

xpcc::hosted::CanUsb::CanUsb()
:	active(false), busState(BusState::Off)
{
}

xpcc::hosted::CanUsb::~CanUsb()
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
	this->serialPort.close();
	while (this->serialPort.isOpen())
	{
		//wait for port to close;
	}
}

bool
xpcc::hosted::CanUsb::open(std::string deviceName, unsigned int baudRate)
{
	if (this->serialPort.isOpen())
		this->serialPort.close();

	while (this->serialPort.isOpen())
	{
		// wait for port to close;
	}

	this->serialPort.setDeviceName(deviceName);
	this->serialPort.setBaudRate(baudRate);

	if (this->serialPort.open())
	{
		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "SerialPort opened in canusb" << xpcc::endl;
		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "write 'C'" << xpcc::endl;
		this->serialPort.write("C\r");

		xpcc::ShortTimeout timeout;
		timeout.restart(500);
		while (!timeout.isExpired())
		{
		}

		char a;
		while( this->serialPort.read(a) );

		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "write 'S4'" << xpcc::endl;
		this->serialPort.write("S4\r");

		timeout.restart(500);
		while (!this->serialPort.read(a))
		{
			if (timeout.isExpired())
			{
				XPCC_LOG_DEBUG << XPCC_FILE_INFO << "Timer expired" << xpcc::endl;
				this->serialPort.close();
				while (this->serialPort.isOpen())
				{
					// wait for port to close;
				}
				return false;
			}
		}
		if (a != '\r')
		{
			XPCC_LOG_ERROR << XPCC_FILE_INFO << "Wrong answer on S4: " << xpcc::hex << (int) a	<< xpcc::endl;
			this->serialPort.close();
			while (this->serialPort.isOpen())
			{
				// wait for port to close;
			}
			return false;
		}
		this->serialPort.write("O\r");
		XPCC_LOG_DEBUG << XPCC_FILE_INFO << "written 'O'" << xpcc::endl;
		timeout.restart(500);
		while (!this->serialPort.read(a))
		{
			if (timeout.isExpired())
			{
				XPCC_LOG_DEBUG << XPCC_FILE_INFO << "Timer expired" << xpcc::endl;
				this->serialPort.close();
				while (this->serialPort.isOpen())
				{
					// wait for port to close;
				}
				return false;
			}
		}

		if (a != '\r')
		{
			XPCC_LOG_ERROR << XPCC_FILE_INFO << "Wrong answer on O: " << xpcc::hex << (int) a << xpcc::endl;
			this->serialPort.close();
			while (this->serialPort.isOpen())
			{
				// wait for port to close;
			}
			return false;
		}

		{
			MutexGuard stateGuard(this->stateLock);
			this->active = true;
		}
		this->thread = new boost::thread(
				boost::bind(&xpcc::hosted::CanUsb::update, this));

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

void
xpcc::hosted::CanUsb::close()
{
	this->serialPort.write("C\r");
	{
		MutexGuard stateGuard(this->stateLock);
		this->active = false;
	}

	this->thread->join();
	delete this->thread;
	this->thread = 0;
	this->serialPort.close();
	busState = BusState::Off;
}

xpcc::Can::BusState
xpcc::hosted::CanUsb::getBusState()
{
	return busState;
}

bool
xpcc::hosted::CanUsb::getMessage(can::Message& message)
{
	if (!this->readBuffer.empty())
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

bool
xpcc::hosted::CanUsb::sendMessage(const can::Message& message)
{
	char str[128];
	xpcc::CanLawicelFormatter::convertToString(message, str);
	this->serialPort.write(str);
	return true;
}

void
xpcc::hosted::CanUsb::update()
{
	while (1)
	{
		char a;
		if (this->serialPort.read(a))
		{
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
		if (!this->active)
			break;
	}
}
