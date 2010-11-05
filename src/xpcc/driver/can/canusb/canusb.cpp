#include "../canusb.hpp"
#include "xpcc/driver/can/canusb/canusb_formater.hpp"
#include <xpcc/architecture/driver.hpp>
#include <iostream>

xpcc::CanUsb::CanUsb(std::string deviceName, unsigned int baudRate):
active(false),serialPort(deviceName, baudRate)
{

}

xpcc::CanUsb::~CanUsb()
{
	if(this->active)
	{
		{
			MutexGuard stateGuard( this->stateLock);
			this->active=false;
		}
		this->thread->join();
		delete this->thread;
		this->thread = 0;
	}
	this->serialPort.close();
	while(this->serialPort.isOpen()){
		//wait for port to close;
	}
}

bool xpcc::CanUsb::open()
{
	
	if (this->serialPort.open())
	{
		this->serialPort.clearWriteBuffer();
		this->serialPort.clearReadBuffer();
		this->serialPort.write("C\r");
		char a;
		while(!this->serialPort.read(a)){
		}
		this->serialPort.write("S4\r");
		while(!this->serialPort.read(a)){
		}
		if( a != '\r') return false;
		this->serialPort.write("O\r");
		while(!this->serialPort.read(a));
		if(a != '\r') return false;
		{
			MutexGuard stateGuard( this->stateLock);
			this->active=true;
		}
		this->thread = new boost::thread(boost::bind(&xpcc::CanUsb::update, this));
		return true;
	}
	else
		return false;
};

void xpcc::CanUsb::close()
{
	{
		MutexGuard stateGuard( this->stateLock);
		this->active=false;
	}
	this->thread->join();
	delete this->thread;
	this->thread = 0;
	this->serialPort.close();
};

bool
xpcc::CanUsb::getMessage(can::Message& message)
{
	if(!this->readBuffer.empty())
	{
		message = this->readBuffer.front();
		this->readBuffer.pop();
		return true;
	}
	else{
		return false;
	}
};

bool
xpcc::CanUsb::sendMessage(const can::Message& message)
{	char str[128];
	xpcc::CanUsbFormater::convertToString(message, str);
	this->serialPort.write(str);
	return true;
}

void
xpcc::CanUsb::update()
{
	while(true)
	{
		char a;
		if(this->serialPort.read(a)){
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
		if(!this->active) break;
	}
}


