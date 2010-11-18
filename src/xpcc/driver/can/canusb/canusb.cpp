
#include <iostream>
#include <xpcc/architecture/driver.hpp>

#include "../canusb.hpp"
#include "canusb_formater.hpp"

// ----------------------------------------------------------------------------
xpcc::CanUsb::CanUsb():
	active(false)
{
}

xpcc::CanUsb::~CanUsb()
{
	if (this->active)
	{
		{
			MutexGuard stateGuard( this->stateLock);
			this->active=false;
		}
		
		this->thread->join();
		delete this->thread;
		
		this->thread = 0;
	}
	
	this->serial.close();
	while (this->serial.isOpen()) {
		// wait for the port to close;
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::CanUsb::open(std::string deviceName, unsigned int baudRate)
{
	if (!this->serial.open(deviceName, baudRate)) {
		return false;
	}
	
	this->serial.clearWriteBuffer();
	this->serial.clearReadBuffer();
	
	this->serial.write("C\r");
	
	char a;
	while(!this->serial.read(a))
		;
	this->serial.write("S4\r");
	while(!this->serial.read(a))
		;
	
	if (a != '\r')
		return false;
	
	this->serial.write("O\r");
	while(!this->serial.read(a))
		;
	
	if (a != '\r')
		return false;
	
	{
		MutexGuard stateGuard(this->stateLock);
		this->active = true;
	}
	
	this->thread = new boost::thread(
			boost::bind(&xpcc::CanUsb::update, this));
	return true;
}

void
xpcc::CanUsb::close()
{
	{
		MutexGuard stateGuard(this->stateLock);
		this->active = false;
	}
	
	this->thread->join();
	delete this->thread;
	
	this->thread = 0;
	
	this->serial.close();
}

// ----------------------------------------------------------------------------
bool
xpcc::CanUsb::getMessage(can::Message& message)
{
	if (!this->readBuffer.empty()) {
		return false;
	}
	
	message = this->readBuffer.front();
	this->readBuffer.pop();
	
	return true;
}

// ----------------------------------------------------------------------------
bool
xpcc::CanUsb::sendMessage(const can::Message& message)
{
	char str[128];
	
	xpcc::CanUsbFormater::convertToString(message, str);
	this->serial.write(str);
	
	return true;
}

// ----------------------------------------------------------------------------
void
xpcc::CanUsb::update()
{
	while (true)
	{
		char a;
		if(this->serial.read(a)){
			if(a == 'T' || a == 't' || a == 'r' || a == 'R'){
				this->tmpRead.clear();
				this->tmpRead += a;
			}
			else{
				this->tmpRead += a;
			}

			can::Message message;
			if(xpcc::CanUsbFormater::convertToCanMessage(this->tmpRead.c_str(), message))
			{
				this->readBuffer.push(message);
			}
		}
		if (!this->active)
			break;
	}
}
