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

#ifndef XPCC__LOGGER_HPP
#define XPCC__LOGGER_HPP

#include <xpcc/utils/macros.hpp>
#include <xpcc/io/iostream.hpp>

#include "level.hpp"

namespace xpcc
{
	/**
	 * \ingroup	logger
	 * \brief	Logger
	 */
	namespace log
	{
		/**
		 * \ingroup logger
		 * \brief 	Interface to the Logger.
		 *
		 * This class provides an interface to the logger. The Logger is an
		 * IOStream  singleton. It is used by the
		 * macro defined below. This class overloads the << operator so that it is
		 * possible to write different message types to the logger.
		 *
		 * \since 	04 December 2006
		 * \author	Christofer Hedbrand,
		 * 			Carsten Schmidt,
		 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		class Logger : public ::xpcc::IOStream
		{
			public:
				Logger(::xpcc::IODevice* device) :
					IOStream(device)
				{
				}
				
				template<typename T>
				inline Logger&
				operator << (const T& msg)
				{
					*(xpcc::IOStream*)this << msg;
					return *this;
				}

			private:
				Logger(const Logger&);

				Logger&
				operator = (const Logger&);
		};

		/**
		 * \ingroup logger
		 * \name	Output streams
		 */
		//\{
		extern Logger debug;	//!< log device to take messages on DEBUG level
		extern Logger info;		//!< log device to take messages on INFO level
		extern Logger warning;	//!< log device to take messages on WARNING level
		extern Logger error;	//!< log device to take messages on ERROR level
		//\}
	}
}

#define XPCC_LOG_DEBUG \
	if ( XPCC_LOG_LEVEL <= xpcc::log::DEBUG )	\
		xpcc::log::debug

#define XPCC_LOG_INFO \
	if ( XPCC_LOG_LEVEL <= xpcc::log::INFO )	\
		xpcc::log::info

#define XPCC_LOG_WARNING \
	if ( XPCC_LOG_LEVEL <= xpcc::log::WARNING )	\
		xpcc::log::warning

#define XPCC_LOG_ERROR \
	if ( XPCC_LOG_LEVEL <= xpcc::log::ERROR )	\
		xpcc::log::error

#ifndef	BASENAME
	#define	FILENAME	__FILE__
#else
	#define	FILENAME	STRINGIFY(BASENAME)
#endif

#define	XPCC_FILE_INFO		FILENAME "(" STRINGIFY(__LINE__) ") >> "

#endif // XPCC__LOGGER_HPP
