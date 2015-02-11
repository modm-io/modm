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

#ifndef XPCC__LOGGER_HPP
#define XPCC__LOGGER_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/io/iostream.hpp>

#include "level.hpp"
#include "style.hpp"
#include "style_wrapper.hpp"
#include "style/prefix.hpp"

namespace xpcc
{
	/**
	 * \ingroup	logger
	 * \brief	Logger
	 */
	namespace log
	{
		/**
		 * \brief 	Interface to the Logger.
		 * 
		 * It is used by the macros defined below. This class overloads
		 * the shift-operator so that it is possible to write different
		 * message types to the logger.
		 * 
		 * \ingroup logger
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		class Logger : public ::xpcc::IOStream
		{
			public:
				Logger(::xpcc::IODevice& outputDevice) :
					IOStream(outputDevice)
				{
				}
				
				/**
				 * @brief	Output forwarding
				 * 
				 * We must use ALWAYS_INLINE here to prevent the generation of
				 * specialized functions for every type. Especially for strings
				 * this might cause a lot of code size bloat.
				 * 
				 * Example without ALWAYS_INLINE or only \c inline:
				 * \code
				 * $ scons symbols | grep "Logger"
				 * ...
				 * 01049436 00000016 W xpcc::log::Logger& xpcc::log::Logger::operator<< <char [12]>(char const (&) [12])
				 * 01050808 00000016 W xpcc::log::Logger& xpcc::log::Logger::operator<< <char [13]>(char const (&) [13])
				 * 01050744 00000016 W xpcc::log::Logger& xpcc::log::Logger::operator<< <char [15]>(char const (&) [15])
				 * 01050792 00000016 W xpcc::log::Logger& xpcc::log::Logger::operator<< <char [16]>(char const (&) [16])
				 * 01050728 00000016 W xpcc::log::Logger& xpcc::log::Logger::operator<< <char [19]>(char const (&) [19])
				 * 01050760 00000016 W xpcc::log::Logger& xpcc::log::Logger::operator<< <char [22]>(char const (&) [22])
				 * 01050712 00000016 W xpcc::log::Logger& xpcc::log::Logger::operator<< <char [26]>(char const (&) [26])
				 * ...
				 * \endcode
				 * 
				 * With ALWAYS_INLINE all these functions are gone.
				 */
				template<typename T>
				ALWAYS_INLINE Logger&
				operator << (const T& msg)
				{
					*(xpcc::IOStream *) this << msg;
					return *this;
				}

			private:
				Logger(const Logger&);

				Logger&
				operator = (const Logger&);
		};

		/**
		 * \name	Output streams
		 * 
		 * Don't use this instances directly! Prefer an access through the
		 * XPCC_LOG_DEBUG, XPCC_LOG_INFO, XPCC_LOG_WARNING and
		 * XPCC_LOG_ERROR macros.
		 * 
		 * \ingroup logger
		 */
		//\{
		extern Logger debug;	///< log device to take messages on DEBUG level
		extern Logger info;		///< log device to take messages on INFO level
		extern Logger warning;	///< log device to take messages on WARNING level
		extern Logger error;	///< log device to take messages on ERROR level
		//\}
	}
}

// these macros are defined like this to avoid the dangling else problem.
// if (condition)
// 		XPCC_LOG_DEBUG << "string";
// else
//		expression;

/**
 * \brief	Turn off messages print
 * \ingroup logger
 */
#define XPCC_LOG_OFF \
	if ( true ){}	\
	else xpcc::log::debug

/**
 * \brief	Output stream for debug messages
 * \ingroup logger
 */
#define XPCC_LOG_DEBUG \
	if (XPCC_LOG_LEVEL > xpcc::log::DEBUG){} \
	else xpcc::log::debug

/**
 * \brief	Output stream for info messages
 * \ingroup logger
 */
#define XPCC_LOG_INFO \
	if (XPCC_LOG_LEVEL > xpcc::log::INFO){}	\
	else xpcc::log::info

/**
 * \brief	Output stream for warnings
 * \ingroup logger
 */
#define XPCC_LOG_WARNING \
	if (XPCC_LOG_LEVEL > xpcc::log::WARNING){}	\
	else xpcc::log::warning

/**
 * \brief	Output stream for error messages
 * \ingroup logger
 */
#define XPCC_LOG_ERROR \
	if (XPCC_LOG_LEVEL > xpcc::log::ERROR){}	\
	else xpcc::log::error

#ifdef __DOXYGEN__

/**
 * \brief	Filename of the current file
 * 
 * In contrast to \c __FILE__ the path to the file is omitted (if this feature
 * is available, otherwise this macro will resolve to \c __FILE__).
 * 
 * \ingroup	logger
 */
#define FILENAME

/**
 * \brief	String containing the filename and the current line
 * 
 * Will generate something like:
 * \code
 * "file.cpp (123) >> "
 * \endcode
 * 
 * This can be very useful the track the origin of log-messages:
 * \code
 * XPCC_LOG_DEBUG   << XPCC_FILE_INFO
 *                  << "... something has happened ..."
 *                  << xpcc::endl;
 * \endcode
 * 
 * This will result into:
 * \code
 * file.cpp (123) >> ... something has happened ...
 * \endcode
 * 
 * \ingroup	logger
 */
#define	XPCC_FILE_INFO

#else	// !__DOXYGEN__

#ifndef	BASENAME
#	ifndef __BASE_FILE__
#		define FILENAME	__FILE__
#	else
#		define FILENAME __BASE_FILE__
#	endif
#else
#	define	FILENAME	STRINGIFY(BASENAME)
#endif

#define	XPCC_FILE_INFO		"[" FILENAME "(" STRINGIFY(__LINE__) ")] "

#endif	// __DOXYGEN__
#endif // XPCC__LOGGER_HPP
