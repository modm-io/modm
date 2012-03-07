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
 * $Id: std_colour.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LOG__STD_COLOUR_HPP
#define XPCC_LOG__STD_COLOUR_HPP

#include "../style.hpp"
#include <xpcc/utils/template_metaprogramming.hpp>

namespace xpcc
{
	namespace log
	{
		enum Colour
		{
			BLACK,
			RED,
			GREEN,
			YELLOW,
			BLUE,
			MAGENTA,
			TURQUOISE,
			WHITE,
			NONE
		};

		/**
		 * \class 	StdColour
		 * \brief 	This style colours the given stream in the color given by the
		 * 			template argument.
		 *
		 * \see http://mathias-kettner.de/lw_farbige_ausgabe_auf_der_konsole.html
		 *
		 * \ingroup logger
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		template <Colour TEXT, Colour BACKGROUND, typename STYLE = DefaultStyle >
		class StdColour : public Style<STYLE>
		{
		public:
			StdColour(STYLE style);

			StdColour(IODevice &device);

			inline void
			parseArg( int argc, char * argv);

			~StdColour();

			/// Write one char to the sink.
			void
			write( char c );

			/// Write a string that terminates with \c '\\0' to the sink.
			void
			write( const char* s );

			/// The message is complete and can be written/send/displayed.
			void
			flush();

		private:
			inline const char*
			getTextColour();

			inline const char*
			getBackgroundColour();
		};
	}
}

#include "std_colour_impl.hpp"

#endif // XPCC_LOG__STD_COLOUR_HPP
