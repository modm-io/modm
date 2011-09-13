// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <iostream>

#include <xpcc/architecture/driver.hpp>
#include <xpcc/workflow/timeout.hpp>

#include "canusb_formater.hpp"
#include "../canusb.hpp"

xpcc::CanUsb::CanUsb() :
	active(false)
{
}

xpcc::CanUsb::~CanUsb()
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
	while (this->serialPort.isOpen()) {
		//wait for port to close;
	}
}

bool xpcc::CanUsb::open(std::string deviceName, unsigned int baudRate)
{
	if (this->serialPort.isOpen())
		this->serialPort.close();
	
	while (this->serialPort.isOpen())
	{
		//wait for port to close;
	}

	if (this->serialPort.open(deviceName, baudRate))
	{
		std::cout << " SerialPort opened in canusb" << std::endl;
		std::cout << "write C" << std::endl;
		this->serialPort.write("C\r");
		char a;
		xpcc::Timeout<> timer;
		timer.restart(500);
		while (!timer.isExpired())
		{
		}

		this->serialPort.clearWriteBuffer();
		this->serialPort.clearReadBuffer();

		std::cout << "write S4" << std::endl;
		this->serialPort.write("S4\r");

		timer.restart(500);
		while (!this->serialPort.read(a))
		{
			if (timer.isExpired()) {
				std::cout << "Timer expired" << std::endl;
				this->serialPort.close();
				while (this->serialPort.isOpen()) {
					//wait for port to close;
				}
				return false;
			}
		}
		if (a != '\r')
		{
			std::cout << "Wrong answere on S4: " << std::hex << (int) a	<< std::endl;
			this->serialPort.close();
			while (this->serialPort.isOpen()) {
				//wait for port to close;
			}
			return false;
		}
		this->serialPort.write("O\r");
		std::cout << "written O" << std::endl;
		timer.restart(500);
		while (!this->serialPort.read(a))
		{
			if (timer.isExpired())
			{
				std::cout << "Timer expired" << std::endl;
				this->serialPort.close();
				while (this->serialPort.isOpen()) {
					//wait for port to close;
				}
				return false;
			}
		}
		if (a != '\r')
		{
			std::cout << "Wrong answere on O: " << std::hex << (int) a << std::endl;
			this->serialPort.close();
			while (this->serialPort.isOpen()) {
				//wait for port to close;
			}
			return false;
		}
		{
			MutexGuard stateGuard(this->stateLock);
			this->active = true;
		}
		this->thread = new boost::thread(
				boost::bind(&xpcc::CanUsb::update, this));
		return true;
	}
	else
	{
		std::cerr << " Could not open Canusb" << std::endl;
		return false;
	}
}

void xpcc::CanUsb::close()
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
}
;

bool xpcc::CanUsb::getMessage(can::Message& message)
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
;

bool xpcc::CanUsb::sendMessage(const can::Message& message)
{
	char str[128];
	xpcc::CanUsbFormater::convertToString(message, str);
	this->serialPort.write(str);
	return true;
}

void xpcc::CanUsb::update()
{
	while (1)
	{
		char a;
		if (this->serialPort.read(a))
		{
			if (a == 'T' || a == 't' || a == 'r' || a == 'R') {
				this->tmpRead.clear();
				this->tmpRead += a;
			}
			else {
				this->tmpRead += a;
			}

			can::Message message;
			if (xpcc::CanUsbFormater::convertToCanMessage(
					this->tmpRead.c_str(), message))
			{
				this->readBuffer.push(message);
			}
		}
		if (!this->active)
			break;
	}
}
