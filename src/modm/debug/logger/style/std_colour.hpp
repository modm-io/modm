/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LOG_STD_COLOUR_HPP
#define MODM_LOG_STD_COLOUR_HPP

#include "../style.hpp"

namespace modm
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
		 * \ingroup modm_debug
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

#endif // MODM_LOG_STD_COLOUR_HPP
