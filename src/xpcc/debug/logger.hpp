/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "logger/logger.hpp"
#include "logger/style.hpp"

/**
\ingroup	debug
\defgroup	logger Logger
\brief 		Log messages to specified destiny.
 
The \ref xpcc::log::Logger "Logger" uses a \ref xpcc::IOStream "IOStream" to
read messages and write them to a given "Device".

Write a log message with streams:
\code
XPCC_LOG_DEBUG << "i=" << i << ", y=" << y << xpcc::flush;
\endcode

The name of the stream represents the log level of the message. Available are:
 - XPCC_LOG_DEBUG
 - XPCC_LOG_INFO
 - XPCC_LOG_WARNING
 - XPCC_LOG_ERROR

TODO check: The flush has to be used, to signal that the message is complete and can be
send. A log message can also be generated separately: 

\code
int a[3] = {1,2,3};
XPCC_LOG_DEBUG << "a = ";
for(int i = 0; i < sizeof(a); ++i) {
	XPCC_LOG_DEBUG << a[i] << ", ";
}
XPCC_LOG_DEBUG << xpcc::flush;
\endcode
TODO check: But remember that without a xpcc::flush your message will not be forwarded.

\section call_flow Flow of a call

This is to give an estimation how many resources a call of the logger use.
All considerations are for a PC program.

The given call is:
\code
XPCC_LOG_DEBUG << 123456;
\endcode

-# The macro resolves to:
   \code
if (XPCC_LOG_LEVEL <= xpcc::log::DEBUG)
    xpcc::log::debug
   \endcode
-# xpcc::log::debug is an instance of xpcc::Logger: \n
   Call of xpcc::Logger::operator << (T) (with T = int32_t) which is inline
   - IOStream::operator << (T) (with T = int32_t) is inline
   - IOStream::writeInteger(int32_t)
-# IOStream::writeInteger(int32_t) will create the formatted string via snprintf()
-# virtual call of IODevice::write(const char*)
-# redirect to std::cout

In sum there are two nested method calls with one of them being virtual plus
the call of snprintf().
*/
