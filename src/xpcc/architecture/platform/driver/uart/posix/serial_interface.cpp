/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../hosted/serial_interface.hpp"

#include <iostream>
#include <ios>
#include <cstdio>
#include <cstdlib>

#include <fcntl.h>		// file control
#include <sys/ioctl.h>	// I/O control routines
#include <termios.h>	// POSIX terminal control
#include <unistd.h>
#include <sys/socket.h>

#include <errno.h>

#include <modm/debug/logger.hpp>

#undef XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL 	xpcc::log::ERROR

// ----------------------------------------------------------------------------
xpcc::hosted::SerialInterface::SerialInterface() :
	isConnected(false),
	deviceName("unknown"),
	baudRate(0),
	fileDescriptor(0)
{
}

xpcc::hosted::SerialInterface::SerialInterface(
		const std::string& device, unsigned int baudRate) :
	isConnected(false),
	deviceName(device),
	baudRate(baudRate),
	fileDescriptor(0)
{
}

// ----------------------------------------------------------------------------
xpcc::hosted::SerialInterface::~SerialInterface()
{
	this->close();
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::setDeviceName(const std::string& name)
{
	this->deviceName = name;
}

// ----------------------------------------------------------------------------
const std::string&
xpcc::hosted::SerialInterface::getDeviceName() const
{
	return this->deviceName;
}

// ----------------------------------------------------------------------------
bool
xpcc::hosted::SerialInterface::setBaudRate(unsigned int rate)
{
	struct termios configuration;
	int result1, result2, result3;

	// Read current configuration structure
	tcgetattr(this->fileDescriptor, &configuration);

	XPCC_LOG_INFO << "Set baud rate to '" << this->baudRate << "'" << xpcc::endl;

	this->baudRate = rate;

	speed_t baudRateConstant =
		(rate == 2400) ? B2400 :
		(rate == 4800) ? B4800 :
		(rate == 9600) ? B9600 :
		(rate == 19200) ? B19200 :
		(rate == 38400) ? B38400 :
		(rate == 57600) ? B57600 :
		(rate == 115200) ? B115200 : B0;

	// Change the configuration structure
	result1 = cfsetispeed(&configuration, baudRateConstant);
	result2 = cfsetospeed(&configuration, baudRateConstant);

	// Set the modified configuration structure
	result3 = tcsetattr(this->fileDescriptor, TCSANOW, &configuration);

	if (result1 < 0 || result2 < 0 || result3 < 0)
	{
		XPCC_LOG_ERROR << "Could not set the baud rate!" << xpcc::endl;
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
unsigned int
xpcc::hosted::SerialInterface::getBaudRate() const
{
	return this->baudRate;
}

// ----------------------------------------------------------------------------
bool
xpcc::hosted::SerialInterface::open()
{
	XPCC_LOG_INFO
		<< XPCC_FILE_INFO
		<< "Opening port '"	<< this->deviceName.c_str()
		<< "' at speed '" << this->baudRate << "'"
		<< xpcc::endl;

	// Complain if device is already opened.
	if (this->isConnected)
	{
		XPCC_LOG_ERROR << "Port is already open." << xpcc::endl;
	}
	else
	{
		XPCC_LOG_INFO << "Trying to create file descriptor ... ";

		this->fileDescriptor = ::open(this->deviceName.c_str(), O_RDWR | O_NOCTTY | O_EXCL | O_NDELAY);

		XPCC_LOG_INFO << this->fileDescriptor << xpcc::endl;

		if (this->fileDescriptor == -1) {
			XPCC_LOG_ERROR << "Invalid file descriptor!" << xpcc::endl;
		}
		else {
			// Set parameter for this port
			this->initSerial();

			// FNDELAY : Wenn keine Daten verfuegbar sind,
			//           soll 'read' 0 zurueckliefern
			fcntl(this->fileDescriptor, F_SETFL, FNDELAY);
			this->isConnected = true;

			XPCC_LOG_INFO << "Connected!" << xpcc::endl;
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::initSerial()
{
	struct termios configuration;

	// Read old configuration
	tcgetattr(this->fileDescriptor, &configuration);

	XPCC_LOG_INFO
		<< "Read old configuration:" << xpcc::endl
		<< " - iflag = " << configuration.c_iflag << xpcc::endl
		<< " - oflag = " << configuration.c_oflag << xpcc::endl
		<< " - cflag = " << configuration.c_cflag << xpcc::endl
		<< " - lflag = " << configuration.c_lflag << xpcc::endl;

	XPCC_LOG_INFO << "Set new configuration" << xpcc::endl;

	configuration.c_cflag &= ~PARENB;   // no parity
	configuration.c_cflag &= ~CSIZE;    // clear old data bit value
	configuration.c_cflag |= CS8;       // 8 data bits
	configuration.c_cflag &= ~CSTOPB;   // 1 stop bit
	configuration.c_cflag |= CLOCAL;    // don't take ownership of the port (should not be necessary)
	configuration.c_cflag |= CREAD;     // set port ready to receive data
	configuration.c_cflag &= ~CRTSCTS;  // no flow-control
	configuration.c_cflag &= ~HUPCL;    // don't hung up when the process is terminated
	configuration.c_lflag &= ~ISIG;     // don't allow signals
	configuration.c_lflag &= ~ICANON;   // RAW mode
	configuration.c_lflag &= ~ECHO;     // no echo
	configuration.c_lflag &= ~ECHOE;    // no echo for the backspace character
	configuration.c_lflag &= ~(ECHOK | ECHONL | NOFLSH | IEXTEN | ECHOCTL | ECHOPRT | ECHOKE | FLUSHO | PENDIN | TOSTOP); // ??
	configuration.c_iflag &= ~INPCK;    // no parity check
	configuration.c_iflag |= IGNPAR;    // ignore parity errors (as we don't use parity)
	configuration.c_iflag &= ~(IXOFF | IXON | IXANY); // no soft-handshake
	configuration.c_iflag |= IGNBRK;    // ignore connection break
	configuration.c_iflag &= ~(INLCR | IGNCR | ICRNL); // don't do anything funny with my data :)
	configuration.c_oflag &= ~OPOST;    // no post-processing
	configuration.c_oflag &= ~ONLCR;    //
//	configStatus.c_cc[VMIN] = 1;        // read() should block until a character is read

	// write new configuration
	tcsetattr(this->fileDescriptor, TCSANOW, &configuration);

	int status = 0;
	ioctl(this->fileDescriptor, TIOCMGET, &status);

	this->setBaudRate(this->baudRate);

	status |= TIOCM_DTR;
	status &= ~TIOCM_RTS;

	ioctl(this->fileDescriptor, TIOCMSET, &status);
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::close()
{
	if (this->isConnected) {
		XPCC_LOG_INFO << "Closing port!!" << xpcc::endl;

		int result = ::close(this->fileDescriptor);
		(void) result;

		this->isConnected = false;
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::hosted::SerialInterface::isOpen()
{
	return this->isConnected;
}

// ----------------------------------------------------------------------------
bool
xpcc::hosted::SerialInterface::read(char& c)
{
	if (::read(this->fileDescriptor, &c, 1) > 0)
	{
		XPCC_LOG_DEBUG << "0x" << xpcc::hex << c << " " << xpcc::endl;
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::readBytes(uint8_t* data, std::size_t length)
{
	int delta = length;
	int result = 0 ;
	while (delta > 0)
	{
		result = ::read(this->fileDescriptor, (data + (length - delta)), delta);
		if (result < 0) {
			usleep(20);	// swap the thread, so that something could be done while waiting
			continue;
		}
		delta -= result;
	}

	for (std::size_t i = 0; i < length; i++) {
		XPCC_LOG_DEBUG << "0x" << xpcc::hex << data[i] << xpcc::ascii << " ";
	}
	XPCC_LOG_DEBUG << xpcc::endl;
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::write(char c)
{
/*	SUB_LOGGER_LOG(logger, Logger::ERROR, "writeByte")
		<< "0x" << std::hex << (int)data << "; ";
 */
	int reply = ::write(this->fileDescriptor, &c, 1);
	if (reply <= 0) {
		this->dumpErrorMessage();
	}
//	SUB_LOGGER_LOG( logger, Logger::DEBUG, "writeByte" )
//		<< "0x" << std::hex << data << " => " << reply << std::dec;
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::write(const char* str)
{
	char c;
	while ((c = *str++)) {
		this->write(c);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::writeBytes(const uint8_t* data, std::size_t length)
{
	for (std::size_t i = 0; i < length; ++i) {
		this->write(static_cast<char>(*data++));
	}
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::dumpErrorMessage()
{
	switch(errno)
	{
		case EBADF:
			XPCC_LOG_ERROR << "The argument is not a valid file descriptor." << xpcc::endl;
			break;

		case EINVAL:
			XPCC_LOG_ERROR << "Invalid argument." << xpcc::endl;
			break;

		case EFAULT:
			XPCC_LOG_ERROR << "Bad Address. Pointer address outside the address "\
							  "space of the process." << xpcc::endl;
			break;

		case EAGAIN:
			XPCC_LOG_ERROR << "Normally, when no input is immediately available, "\
				"read waits for some input. But if the O_NONBLOCK flag is set "\
				"for the file (see section File Status Flags), read returns "\
				"immediately without reading any data, and reports this error." << xpcc::endl;
			break;

		case ENOSPC:
			XPCC_LOG_ERROR << "The device is full." << xpcc::endl;
			break;

		case EPIPE:
			XPCC_LOG_ERROR << "Trying to write to a pipe or FIFO that isn't "\
							  "open for reading by any process" << xpcc::endl;
			break;

		case EINTR:
			XPCC_LOG_ERROR << "The call was interrupted by a signal." << xpcc::endl;
			break;

		default:
			XPCC_LOG_ERROR	<< "Unknown error: " << errno << xpcc::endl;
			break;
	}
}
// ----------------------------------------------------------------------------
std::size_t
xpcc::hosted::SerialInterface::bytesAvailable() const
{
	std::size_t bytesAvailable;

	ioctl(this->fileDescriptor, FIONREAD, &bytesAvailable);

	return bytesAvailable;
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::flush()
{
	// TODO flush buffers
}

// ----------------------------------------------------------------------------
void
xpcc::hosted::SerialInterface::dump()
{
	if (!this->isConnected) {
		XPCC_LOG_DEBUG	<< "Port is not connected device!!" << xpcc::endl;
	}
	else {
		struct termios configStatus;

		// Fill the configuration structure
		tcgetattr( this->fileDescriptor, &configStatus);

		XPCC_LOG_DEBUG
			<< "parity:       " << ((configStatus.c_cflag & PARENB) ? "yes" : "no") << xpcc::endl
			<< "8 data bits:  " << ((configStatus.c_cflag & CS8) 	? "yes" : "no") << xpcc::endl
			<< "ready:        " << ((configStatus.c_cflag & CREAD) 	? "yes" : "no") << xpcc::endl
			<< "input speed:  " << configStatus.c_ispeed << xpcc::endl
			<< "output speed: " << configStatus.c_ospeed << xpcc::endl
			<< "speed:        " << ( configStatus.c_cflag ) << xpcc::endl;
	}
}

// ----------------------------------------------------------------------------
/*std::ostream&
operator << (std::ostream& os, const xpcc::hosted::SerialInterface& c)
{
	os << "\nPort-Identifier: " << c.portIdentifier_;
	os << "\nBaud-Rate:       "	<< ::std::dec << c.baudRate_;
	os << "\nIs Connected:    " << c.isConnected_;

	return os;
}*/
