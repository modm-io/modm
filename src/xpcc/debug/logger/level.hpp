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
 */
#ifndef XPCC_LOG__LEVEL_HPP
#define XPCC_LOG__LEVEL_HPP

#pragma push_macro("ERROR") // avoid collision with ERROR defined macro in winsock.h
#undef ERROR

namespace xpcc
{
	namespace log
	{
		/**
		 * \brief	Log levels
		 *
		 * DEBUG < INFO < WARNING < ERROR < DISABLED
		 *
		 * \ingroup logger
		 */
		enum Level
		{
			DEBUG,
			INFO,
			WARNING,
			ERROR,
			DISABLED
		};
	}
}

#ifndef XPCC_LOG_LEVEL
	/**
	 * \brief	Default log level
	 * 
	 * Define to set the level of the logging to a defined value for each file.
	 * To change the logging level in a source file use \c \#undef
	 * 
	 * \code
	 * #undef  XPCC_LOG_LEVEL
	 * #define XPCC_LOG_LEVEL xpcc::log::INFO
	 * \endcode
	 * 
	 * DEBUG < INFO < WARNING < ERROR < DISABLED
	 * 
	 * \ingroup logger
	 */
	#define XPCC_LOG_LEVEL xpcc::log::DEBUG
#endif // XPCC_LOG_LEVEL

#pragma pop_macro("ERROR")

#endif // XPCC_LOG__LEVEL_HPP
