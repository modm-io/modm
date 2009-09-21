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

#include "log_level.hpp"
#include "log_singleton.hpp"
#include "../../hal/io/iostream.hpp"

namespace xpcc {
	/**
	 * @class 	Logger
	 * @brief 	Interface to the Logger.
	 *
	 * This class provides an interface to the logger singleton. It is used by the
	 * macro defined below. This class overloads the << operator so that it is
	 * possible to write different message types to the logger.
	 *
	 * @ingroup logger
	 * @version	$Id$
	 * @since 	04 December 2006
	 * @author	Christofer Hedbrand,
	 * 			Carsten Schmidt,
	 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	template<Log::Level L = Log::DEBUG>
	class Logger : public IOStream {
		public:

			Logger() :
				IOStream( LogSingleton::instance() ),
				level( LogSingleton::instance().getLevel() )
			{};

			template <typename T>
			inline Logger<L>&
			operator <<(const T& msg)
			{
				if( L >= this->level ) {
					*(IOStream*)this << msg;
				}
				return *this;
			};

		private:
			const Log::Level&	level;
	};

	extern Logger<Log::DEBUG> dout;
};


#endif /*XPCC_LOGGER__HPP*/
