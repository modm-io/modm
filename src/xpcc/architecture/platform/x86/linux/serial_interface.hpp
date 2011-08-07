// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#ifndef XPCC_PC__SERIAL_INTERFACE_HPP
#define XPCC_PC__SERIAL_INTERFACE_HPP

#include <string>
#include <stdint.h>
#include <ostream>

#include <xpcc/io/iodevice.hpp>

namespace xpcc
{
	namespace pc
	{
		/**
		 * @brief 	SerialInterface represents a simple interface for handling
		 * 			connections with serial ports.
		 *
		 * The normal life cycle should look like as follows:
		 * 
		 *	- Construct the interface via SerialInterface()
		 *	- setDeviceName()
		 *	- setBaudRate()
		 *	- open()
		 *	- Read & write, whatever you want... Note: Use bytesAvailable() before read operation.
		 *	- close()
		 * 
		 * @author	Philipp & Metty
		 * @ingroup	driver
		 */
		class SerialInterface : public IODevice
		{
		public:
			/**
			 * @brief	Constructor.
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
			 * @brief	Constructor.
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
			 * 	@brief	Destroy the interface.
			 * 
			 * Make shure to
			 */
			~SerialInterface();
			
			/**
			 * @brief	Set the PORT name (e.g. /dev/ttyS0)
			 * 
			 * Note that this may only make sense if you not use the methode
			 * configure.
			 */
			void
			setDeviceName(const std::string& device);
			
			const std::string&
			getDeviceName() const;
			
			/**
			 * @brief	Set the baud rate of the device.
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
			 *  @brief	Get the baud rate of the device.
			 */
			unsigned int
			getBaudRate() const;
			
			/**
			 * @brief	Establish a connection to the serial port.
			 * 
			 * @return	\c true if the connection could be established, \c false
			 * 			on any error.
			 */
			bool
			open();
			
			/**
			 * @brief	Quit the existing connection.
			 */
			void
			close();
			
			/// Check if the port is open
			bool
			isOpen();
			
			/**
			 * @brief	Read exactly one byte from device.
			 * 
			 * @return if one byte could read successfully then true
			 */
			virtual bool
			read(char& c);
			
			
			/**
			 * @brief	Read length bytes from device.
			 * 
			 * Tries until \c length bytes are read.
			 */
			void
			readBytes(char* data, std::size_t length);
			
			/**
			 * @brief	Write exactly one byte to device.
			 */
			virtual void
			write(char c);
			
			/// Write a C-string
			virtual void
			write(const char* str);
			
			/**
			 * @brief	Write length bytes to device.
			 */
			void
			writeBytes(const char* data, std::size_t length);
			
			/**
			 * @brief 	Return the number of bytes waiting to be read.
			 */
			std::size_t
			bytesAvailable() const;
			
			virtual void
			flush();
			
			/**
			 * @brief	Output information about device to the Logger (Level:DEBUG).
			 */
			void
			dump();
			
		protected:
			void
			initSerial();
			
			void
			dumpErrorMessage();
			
			bool 			isConnected;		///< Is there an existing connection?
	
			std::string 	deviceName;	///< The port (e.g. /dev/ttyS0)
			
			unsigned int 	baudRate;
			
			/// The file descriptor that is internally needed for handling the read/ write/ close operations
			int 			fileDescriptor;
		};
	}
}

#endif	// XPCC_PC__SERIAL_INTERFACE_HPP
