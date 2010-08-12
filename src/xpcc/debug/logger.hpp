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

The flush has to be used, to signal that the message is complete and can be
send. A log message can also be generated separately: 

\code
int a[3] = {1,2,3};
XPCC_LOG_DEBUG << "a = ";
for(int i = 0; i < sizeof(a); ++i) {
	XPCC_LOG_DEBUG << a[i] << ", ";
}
XPCC_LOG_DEBUG << xpcc::flush;
\endcode
But remember that without a xpcc::flush your message will not be forwarded.

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
-# IOStream::writeInteger(int32_t) will create the formated string via snprintf()
-# virtual call of IODevice::write(const char*)
-# redirect to std::cout

In sum there are two nested method calls with one of them being virtual plus
the call of snprintf().
*/
