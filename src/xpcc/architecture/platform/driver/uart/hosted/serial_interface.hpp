/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2012-2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HOSTED_SERIAL_INTERFACE_HPP
#define XPCC_HOSTED_SERIAL_INTERFACE_HPP

#include <string>
#include <stdint.h>
#include <ostream>

#include <xpcc/io/iodevice.hpp>

namespace xpcc
{
	namespace hosted
	{
		/**
		 * SerialInterface represents a simple interface for handling
		 * connections with serial ports.
		 *
		 * The normal life cycle should look like as follows:
		 *	- Construct the interface via SerialInterface()
		 *	- setDeviceName()
		 *	- setBaudRate()
		 *	- open()
		 *	- Read & write, whatever you want... Note: Use bytesAvailable() before read operation.
		 *	- close()
		 *
		 * @author	Philipp & Metty
		 * @ingroup	linux
		 */
		class SerialInterface : public IODevice
		{
		public:
			/**
			 * Constructor.
			 *
			 * Note that, at this time, the connection to the serial port is not
			 * established yet. Call setBaudrate(), setDeviceName and then open()
			 * in order to connect to the serial port.
			 *
			 * @see	setBaudrate()
			 * @see setDeviceName()
			 * @see	open()
			 */
			SerialInterface();

			/**
			 * Constructor.
			 *
			 * Note that, at this time, the connection to the serial port is not
			 * established yet. Call open() in order to connect to the
			 * serial port.
			 *
			 * @param	device		Device name
			 * @param	baudRate	Baudrate+
			 *
			 * @see		open()
			 */
			SerialInterface(const std::string& device, unsigned int baudRate);

			/**
			 * Destroy the interface.
			 */
			~SerialInterface();

			/**
			 * Set the PORT name (e.g. /dev/ttyS0).
			 *
			 * Note that this may only make sense if you not use the method
			 * configure.
			 */
			void
			setDeviceName(const std::string& device);

			const std::string&
			getDeviceName() const;

			/**
			 * Set the baud rate of the device.
			 *
			 * Possible values for the baudrate:
			 * - 2400
			 * - 4800
			 * - 9600
			 * - 19200
			 * - 38400
			 * - 57600
			 * - 115200
			 */
			bool
			setBaudRate(unsigned int baudRate);

			/**
			 * Get the baud rate of the device.
			 */
			unsigned int
			getBaudRate() const;

			/**
			 * Establish a connection to the serial port.
			 *
			 * @return	\c true if the connection could be established, \c false
			 * 			on any error.
			 */
			bool
			open();

			/**
			 * Quit the existing connection.
			 */
			void
			close();

			/// Check if the port is open
			bool
			isOpen();

			/**
			 * Read exactly one byte from device.
			 *
			 * @return if one byte could read successfully then true
			 */
			virtual bool
			read(char& c);


			/**
			 * Read length bytes from device.
			 *
			 * Tries until `length` bytes are read.
			 */
			void
			readBytes(uint8_t* data, std::size_t length);

			/**
			 * Write exactly one byte to device.
			 */
			virtual void
			write(char c);

			/// Write a C-string
			virtual void
			write(const char* str);

			/**
			 * Write length bytes to device.
			 */
			void
			writeBytes(const uint8_t* data, std::size_t length);

			/**
			 * Return the number of bytes waiting to be read.
			 */
			std::size_t
			bytesAvailable() const;

			virtual void
			flush();

			/**
			 * Output information about device to the Logger (Level:DEBUG).
			 */
			void
			dump();

		protected:
			void
			initSerial();

			void
			dumpErrorMessage();

			bool 			isConnected;	///< Is there an existing connection?
			std::string 	deviceName;		///< The port (e.g. /dev/ttyS0)
			unsigned int 	baudRate;

			/// The file descriptor that is internally needed for handling the read/ write/ close operations
			int 			fileDescriptor;
		};
	}
}

#endif	// XPCC_HOSTED_SERIAL_INTERFACE_HPP
