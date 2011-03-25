#include "../canusb.hpp"
#include "xpcc/driver/can/canusb/canusb_formater.hpp"
#include <xpcc/architecture/driver.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <iostream>

xpcc::CanUsb::CanUsb():
active(false)
{
}

xpcc::CanUsb::~CanUsb(
)
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

bool xpcc::CanUsb::open(std::string deviceName, unsigned int baudRate)
{
	if(this->serialPort.isOpen()) this->serialPort.close();
	while(this->serialPort.isOpen()){
		//wait for port to close;
	}
	
	if (this->serialPort.open(deviceName, baudRate))
	{
		std::cout<<" SerialPort opened in canusb"<<std::endl;
		std::cout<< "write C" << std::endl;
		this->serialPort.write("C\r");
		char a;
		xpcc::Timeout<> timer;
		timer.restart(500);
		while(!timer.isExpired()){}

		this->serialPort.clearWriteBuffer();
		this->serialPort.clearReadBuffer();
		
		std::cout<< "write S4" << std::endl;
		this->serialPort.write("S4\r");
		
		timer.restart(500);
		while(!this->serialPort.read(a)){
			if(timer.isExpired())
			{
				std::cout<< "Timer expired"<<std::endl;
				this->serialPort.close();
				while(this->serialPort.isOpen()){
					//wait for port to close;
				}
				return false;
			}
		}
		if( a != '\r')
		{ 	
			std::cout<<"Wrong answere on S4: "<< std::hex << (int) a << std::endl;
			this->serialPort.close();
			while(this->serialPort.isOpen()){
			//wait for port to close;
			}
			return false;
		}
		this->serialPort.write("O\r");
		std::cout<< "written O" << std::endl;
		timer.restart(500);
		while(!this->serialPort.read(a))
		{
			if(timer.isExpired())
			{
				std::cout<< "Timer expired"<<std::endl;
				this->serialPort.close();
				while(this->serialPort.isOpen()){
					//wait for port to close;
				}
				return false;
			}
		}
		if(a != '\r')
		{
			std::cout<<"Wrong answere on O: "<< std::hex << (int) a << std::endl;
			this->serialPort.close();
			while(this->serialPort.isOpen()){
				//wait for port to close;
			}
			return false;
		}
		{
			MutexGuard stateGuard( this->stateLock);
			this->active=true;
		}
		this->thread = new boost::thread(boost::bind(&xpcc::CanUsb::update, this));
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


