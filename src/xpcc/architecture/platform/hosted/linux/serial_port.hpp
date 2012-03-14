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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PC__SERIAL_PORT_HPP
#define XPCC_PC__SERIAL_PORT_HPP

#include <string>
#include <queue>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

#include <xpcc/io/iodevice.hpp>

namespace xpcc
{
	namespace pc
	{
		/**
		 * \brief	Output on Serial Port
		 *
		 * Port is closed right after construction.
		 *
		 * \ingroup	linux
		 */
		class SerialPort : IODevice
		{
		public :

			SerialPort();

			~SerialPort();

			using IODevice::write;

			virtual void
			write(char c);

			virtual void
			flush();

			virtual bool
			read(char& value);

			virtual bool
			open( std::string deviceName, unsigned int baudRate );

			virtual bool
			isOpen();

			virtual void
			close();

			void
			kill();

			void
			clearReadBuffer();

			void
			clearWriteBuffer();

		private:
			typedef boost::mutex				Mutex;
			typedef boost::mutex::scoped_lock	MutexGuard;

			bool shutdown;
			std::string deviceName;
			unsigned int baudRate;
			Mutex readMutex;
			Mutex writeMutex;

			char tmpRead[512];
			std::queue<char> writeBuffer;
			std::queue<char> readBuffer;

			boost::asio::io_service  io_service;
			boost::asio::serial_port port;
			boost::thread* 			 thread;

			void
			readStart();

	        void
	        doClose(const boost::system::error_code& error);

	        void
	        doAbort(const boost::system::error_code& error);

	        void
	        doWrite(const char c);

	        void
	        writeStart(void);

	        void
	        writeComplete(const boost::system::error_code& error);

			void
			readComplete(const boost::system::error_code& error, size_t bytes_transferred);
		};
	}
}

#endif // XPCC_PC__SERIAL_PORT_HPP
