#include "serial_port.hpp"
#include <iostream>

xpcc::pc::SerialPort::SerialPort(std::string deviceName, unsigned int baudRate):
	shutdown(false),
	serialPort(io_service, deviceName)
{
	if (!this->serialPort.is_open()) {
		std::cerr << "Failed to open serial port" << deviceName << "\n";
		return;
	}
	boost::asio::serial_port_base::baud_rate baud(baudRate);
	this->serialPort.set_option(baud);
	this->readStart();

	this->thread = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
}

xpcc::pc::SerialPort::~SerialPort()
{
	this->thread->join();
	delete this->thread;
}

void
xpcc::pc::SerialPort::write(char c)
{
	this->io_service.post(boost::bind(&xpcc::pc::SerialPort::doWrite, this, c));
}

void
xpcc::pc::SerialPort::write(const char* s)
{
	const char c = *s;
	this->io_service.post(
			boost::bind(&xpcc::pc::SerialPort::doWrite, this, c));
}

void
xpcc::pc::SerialPort::flush()
{
}

void
xpcc::pc::SerialPort::readStart()
{
	serialPort.async_read_some(boost::asio::buffer(&this->tmpRead, sizeof(this->tmpRead)),
			boost::bind(&xpcc::pc::SerialPort::readComplete,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

bool
xpcc::pc::SerialPort::read(char& value)
{
	if(this->readBuffer.empty())
		return false;
	else
	{
		MutexGuard queueGuard( this->queueLock);
		value=this->readBuffer.front();
		this->readBuffer.pop();
		return true;
	}
}

bool
xpcc::pc::SerialPort::open()
{
	//TODO
	return true;
}


bool
xpcc::pc::SerialPort::isOpen()
{
	return this->serialPort.is_open();
}

void
xpcc::pc::SerialPort::close()
{
	this->io_service.post(boost::bind(&xpcc::pc::SerialPort::doClose, this,
			boost::system::error_code()));
}

void
xpcc::pc::SerialPort::doClose(const boost::system::error_code& error)
{
	//TODO next lines out of Example:
	//std::cerr << "do_close() "  << std::endl;
	if (error == boost::asio::error::operation_aborted)
		return;
	if (error)
		std::cerr << "Error: " << error.message() << std::endl;

	if( this->writeBuffer.empty() ) {
		this->serialPort.close();
	}
	else {
		this->shutdown = true;
	}
}

void
xpcc::pc::SerialPort::doWrite(const char c) {
	bool writing = !this->writeBuffer.empty();
	this->writeBuffer.push(c);
	if (!writing) {
		this->writeStart();
	}
	else if( this->shutdown ) {
		this->serialPort.close();
	}
}

void
xpcc::pc::SerialPort::writeStart(void)
{
	boost::asio::async_write(this->serialPort, boost::asio::buffer(
			&this->writeBuffer.front(), 1), boost::bind(
			&xpcc::pc::SerialPort::writeComplete, this,
			boost::asio::placeholders::error));
}

void
xpcc::pc::SerialPort::writeComplete(const boost::system::error_code& error)
{
	if (!error) {
		this->writeBuffer.pop();
		if (!this->writeBuffer.empty())
			this->writeStart();
	} else{
		this->doClose(error);
	}
}

void
xpcc::pc::SerialPort::readComplete(const boost::system::error_code& error, size_t /*bytes_transferred*/)
{
    if (!error)
    {
    	{
			MutexGuard queueGuard( this->queueLock);
			this->readBuffer.push(this->tmpRead);
    	}
        this->readStart();
    }
    else
    {
            doClose(error);
    }
}

