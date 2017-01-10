/*
 * Copyright (c) 2010-2011, Thorsten Lajewski
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012, Dave Webb
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <iostream>
#include <modm/debug/logger.hpp>

#include <modm/architecture/driver.hpp>
#include <modm/processing/timer.hpp>

#include <modm/driver/can/can_lawicel_formatter.hpp>
#include "canusb.hpp"

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

modm::hosted::CanUsb::CanUsb()
:	active(false), busState(BusState::Off)
{
}

modm::hosted::CanUsb::~CanUsb()
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
modm::hosted::CanUsb::open(std::string deviceName, unsigned int serialBaudRate, modm::Can::Bitrate canBitrate)
{
	if (this->serialPort.isOpen())
		this->serialPort.close();

	while (this->serialPort.isOpen())
	{
		// wait for port to close;
	}

	this->serialPort.setDeviceName(deviceName);
	this->serialPort.setBaudRate(serialBaudRate);

	if (this->serialPort.open())
	{
		MODM_LOG_DEBUG << MODM_FILE_INFO << "SerialPort opened in canusb" << modm::endl;
		MODM_LOG_DEBUG << MODM_FILE_INFO << "write 'C'" << modm::endl;
		this->serialPort.write("C\r");

		modm::ShortTimeout timeout;
		timeout.restart(500);
		while (!timeout.isExpired())
		{
		}

		char a;
		while( this->serialPort.read(a) );

		// Set CAN bitrate
		MODM_LOG_DEBUG << MODM_FILE_INFO << "Set CAN bitrate" << modm::endl;
		
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
		while (!this->serialPort.read(a))
		{
			if (timeout.isExpired())
			{
				MODM_LOG_DEBUG << MODM_FILE_INFO << "Timer expired" << modm::endl;
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
			MODM_LOG_ERROR << MODM_FILE_INFO << "Wrong answer on set CAN bitrate: " << modm::hex << (int) a	<< modm::endl;
			this->serialPort.close();
			while (this->serialPort.isOpen())
			{
				// wait for port to close;
			}
			return false;
		}
		
		// Open CAN channel
		this->serialPort.write("O\r");
		MODM_LOG_DEBUG << MODM_FILE_INFO << "written 'O'" << modm::endl;
		timeout.restart(500);
		while (!this->serialPort.read(a))
		{
			if (timeout.isExpired())
			{
				MODM_LOG_DEBUG << MODM_FILE_INFO << "Timer expired" << modm::endl;
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
			MODM_LOG_ERROR << MODM_FILE_INFO << "Wrong answer on O: " << modm::hex << (int) a << modm::endl;
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
				boost::bind(&modm::hosted::CanUsb::update, this));

		busState = BusState::Connected;
		this->tmpRead.clear();
		return true;
	}
	else
	{
		busState = BusState::Off;
		MODM_LOG_ERROR << MODM_FILE_INFO << "Could not open Canusb" << modm::endl;
		return false;
	}
}

void
modm::hosted::CanUsb::close()
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

modm::Can::BusState
modm::hosted::CanUsb::getBusState()
{
	return busState;
}

bool
modm::hosted::CanUsb::getMessage(can::Message& message)
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
modm::hosted::CanUsb::sendMessage(const can::Message& message)
{
	char str[128];
	modm::CanLawicelFormatter::convertToString(message, str);
	MODM_LOG_DEBUG.printf("Sending ");
	char *p = str;
	while (*p != '\0') {
		MODM_LOG_DEBUG.printf("%02x %c, ", *p, *p);
		++p;
	}
	this->serialPort.write(str);
	this->serialPort.write("\r");
	return true;
}

void
modm::hosted::CanUsb::update()
{
	while (1)
	{
		char a;
		if (this->serialPort.read(a))
		{
			MODM_LOG_DEBUG.printf("Received %02x\n", a);
			if (a == 'T' || a == 't' || a == 'r' || a == 'R')
			{
				this->tmpRead.clear();
			}
			this->tmpRead += a;

			can::Message message;
			if (modm::CanLawicelFormatter::convertToCanMessage(
					this->tmpRead.c_str(), message))
			{
				this->readBuffer.push(message);
			}
		}
		if (!this->active)
			break;
	}
}
