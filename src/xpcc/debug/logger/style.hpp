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

#ifndef XPCC_LOG__STYLE_HPP
#define XPCC_LOG__STYLE_HPP

#include <xpcc/io/iodevice.hpp>
#include <xpcc/utils/typet.hpp>

namespace xpcc
{
	namespace log
	{
		/**
		 * \brief 	This is an empty style.
		 *
		 * \ingroup logger
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		class DefaultStyle
		{
		public :
			/// parse arguments that are given with the start of the program
			inline void
			parseArg( int argc, char * argv)
			{
				(void) argc;
				(void) argv;
			}

			/// Write one char to the sink.
			inline void
			put( char c )
			{
				(void) c;
			}

			/// Write a string that terminates with \c '\\0' to the sink.
			inline void
			put( const char* s )
			{
				(void) s;
			}

			/// The message is complete and can be written/send/displayed.
			inline void
			flush()
			{
			}
		};

		/**
		 * \class 	Style
		 * \brief 	This is the interface to all style-classes of the logger.
		 *
		 * For each log level the logger provides a singleton. Because the
		 * format the log messages should be presented depends on the system
		 * and other circumstances. With the style-classes the user can
		 * set in the main-file the desired format of the output stream.
		 *
		 * This classes take the stream from the loglevel device, modify it in
		 * the given way and pass it to the next style class or device.
		 *
		 *
		 * \ingroup logger
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		template < typename STYLE = DefaultStyle >
		class Style
		{
		public:
			typedef STYLE Type;

			Style( STYLE style );

			Style( IODevice &device);

			virtual
			~Style();

			/// parse arguments that are given with the start of the program
			inline void
			parseArg( int argc, char * argv);

			/// Write one char to the sink.
			inline void
			put( char c );

			/// Write a string that terminates with \c '\\0' to the sink.
			inline void
			put( const char* s );

			/// The message is complete and can be written/send/displayed.
			inline void
			flush();

		private:
			STYLE style;
			IODevice* device;
		};
	}
}

#include "style_impl.hpp"

#endif // XPCC_LOG__STYLE_HPP
