/*
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HOSTED_SERIAL_PORT_HPP
#define XPCC_HOSTED_SERIAL_PORT_HPP

#include <string>
#include <queue>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

#include <xpcc/io/iodevice.hpp>

namespace xpcc
{
	namespace hosted
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

#endif // XPCC_HOSTED_SERIAL_PORT_HPP
