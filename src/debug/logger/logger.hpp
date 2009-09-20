// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
#ifndef XPCC_LOGGER__HPP
#define XPCC_LOGGER__HPP
/**
 * @defgroup logger Logger
 * @brief Log messages to specifyed destiny.
 * 
 * The Logger write messages a destiny specifyed by the used OutputWriter.
 * The Logger is a singelten an act as server to the log messages and 
 * send them to the OutputWriter.\n
 * To log a message use the LoggerMessageForwarder, that will filter the 
 * messages after level an formward them to the Logger.\n
 * 
 * @section howto HowTo
 * - create a logger singelton with a outputWriter
 * - create a loggerFrowarder in the class or methode you want write a message
 * - use the loggerForwarder to write the log message
 * 
 * @section flow Message Flow
 * LoggerMessageForwarder -> Logger -> LoggerOutputWriter -> destiny
 * 
 * @section loglevels Log Levels
 * The following log levels are supported: \n
 * DEBUG < INFO < WARNING < ERROR < FATAL
 * 
 * \section call_flow Flow of a call
 * This is to give a overview how many resources a call of the logger is.
 * The given call is:
 * \code
 * 	xpcc::dlog << 100
 * \endcode
 * - call of LoggerMessageForwarder<L>::operator << (T) (with L = xpcc::DEBUG, T = int)
 *   - IOStream::operator << (T) (with T = int) is inline
 *   - IntegerWriter::operator()(IOStream& os, const T& v) (with T=int) is inline
 * \code
 * 	if( xpcc::DEBUGL >= xpcc::LoggerMessageForwarder<xpcc::DEBUG>::level ) {
 * 		xpcc::IOStream::putInteger( 100 )
 * 	}
 * \endcode
 * - IOStream::putInteger() will create the formated string and calls sprintf() to do this
 * - the resulting string is send to the device via the Logger: call of
 *   Logger::put(const char*) (using vTable)
 * - direct call of ConsoleOutputWriter::put(const char*) (using vTable)
 * - using of the std::cout ...
 *
 * In sum there are 3 nested method calls with two of them using vTables) plus
 * the call of sprintf().
 * 
 * \todo 	Finding a solution with less method calls (at least with only one using
 * 			vTables).
 */

#include "abstract_output_writer.hpp"
#include "level.hpp"
#include "../../hal/io/iodevice.hpp"

namespace xpcc {
	/**
	 * @class 	Logger
	 * @brief 	Logger singelton, connect the LoggerMessageForwarder to the
	 * 			LoggerOutputWriter
	 *
	 * @todo	Logging of templates is not possible, because the static variable
	 * 			of the logger will be also seen in the file, that include the
	 * 			header file.
	 *
	 * @todo	HowTo: using of OutputWriter
	 *
	 * @ingroup logger
	 * @version	$Id$
	 * @since 	01 December 2006
	 * @author	Christofer Hedbrand,
	 * 			Carsten Schmidt,
	 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	class Logger : public IODevice {
		public:
			static Logger&
			instance();

			//! set a new output writer in the singleton.
			//!
			//! The singleton takes ownership of the output writer (the
			//! singleton will delete the object)
			//!
			//! \code
			//! 	Logger::instance.setOutputWriter(new MyOutputWriter);
			//! \endcode
			void
			setOutputWriter(AbstractOutputWriter* outputWriter);

			void
			setLevel(Level level);

			inline const Level&
			getLevel() const
			{
				return this->level;
			}

			virtual void
			put(char c);

			virtual void
			put(const char* s);

			virtual void
			flush();

			virtual bool
			get(char& value);

		private:
			Logger();						// Private constructor because of singleton
			Logger(const Logger & logger);	// Private copy constructor because of singleton
			~Logger();

			AbstractOutputWriter*		outputWriter;
			Level 						level;
	};
};


#endif /* RCA__LOGGER__HPP */
