#include "serial_port.hpp"
#include <iostream>

xpcc::pc::SerialPort::SerialPort(boost::asio::io_service& io_service, std::string deviceName, unsigned int baudRate):
	serialPort(io_service, deviceName), io_service(io_service){
	boost::asio::serial_port_base::baud_rate baud(baudRate);
	if (!this->serialPort.is_open()) {
		std::cerr << "Failed to open serial port" << deviceName << "\n";
		return;
	}
	this->serialPort.set_option(baud);
	this->readState = xpcc::pc::SerialPort::NOT_READING;
}

void
xpcc::pc::SerialPort::write(char c)
{
	this->io_service.post(boost::bind(&xpcc::pc::SerialPort::do_write, this, c));
}

void
xpcc::pc::SerialPort::write(const char* s)
{
	const char c = *s;
	this->io_service.post(
			boost::bind(&xpcc::pc::SerialPort::do_write, this, c));
}

void
xpcc::pc::SerialPort::flush()
{

}

bool
xpcc::pc::SerialPort::read(char& value)
{
	if(this->readState == xpcc::pc::SerialPort::NOT_READING)
	{
		this->readState = xpcc::pc::SerialPort::READING;
		this->serialPort.async_read_some(boost::asio::buffer(&value, xpcc::pc::SerialPort::max_read_length),
				boost::bind(&xpcc::pc::SerialPort::read_complete, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		return true ;
	}
	else
		return false;
}

bool
xpcc::pc::SerialPort::open()
{
	return true;
}

bool
xpcc::pc::SerialPort::readFinished()
{
	if(this->readState == xpcc::pc::SerialPort::NOT_READING)
		return true;
	else
		return false;
}

bool
xpcc::pc::SerialPort::isOpen()
{
	return this->serialPort.is_open();
}

void
xpcc::pc::SerialPort::close()
{
	this->io_service.post(boost::bind(&xpcc::pc::SerialPort::do_close, this,
			boost::system::error_code()));
}

void
xpcc::pc::SerialPort::do_close(const boost::system::error_code& error)
{
	//TODO next lines out of Example:
	if (error == boost::asio::error::operation_aborted)
		return;
	if (error)
		std::cerr << "Error: " << error.message() << std::endl;
	else
		std::cout << "Error: Connection did not succeed.\n";

	this->serialPort.close();
}

void
xpcc::pc::SerialPort::do_write(const char c) {
	bool writing = !this->writeBuffer.empty();
	this->writeBuffer.push(c);
	if (!writing) {
		this->write_start();
	}
}

void
xpcc::pc::SerialPort::write_start(void)
{
	boost::asio::async_write(this->serialPort, boost::asio::buffer(
			&this->writeBuffer.front(), 1), boost::bind(
			&xpcc::pc::SerialPort::write_complete, this,
			boost::asio::placeholders::error));
}

void
xpcc::pc::SerialPort::write_complete(const boost::system::error_code& error)
{
	if (!error) {
		this->writeBuffer.pop();
		if (!this->writeBuffer.empty())
			this->write_start();
	} else{
		this->do_close(error);
	}
}

void
xpcc::pc::SerialPort::read_complete(const boost::system::error_code& error, size_t bytes_transferred)
{
		this->readState = xpcc::pc::SerialPort::NOT_READING;
        if (error)
        {
                this->do_close(error);
        }
}

