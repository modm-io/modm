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

#ifndef MODM_LOG_STYLE_HPP
#define MODM_LOG_STYLE_HPP

#include <modm/io/iodevice.hpp>

namespace modm
{
	namespace log
	{
		/**
		 * \brief 	This is an empty style.
		 *
		 * \ingroup modm_debug
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
			write( char c )
			{
				(void) c;
			}

			/// Write a string that terminates with \c '\\0' to the sink.
			inline void
			write( const char* s )
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
		 * \ingroup modm_debug
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
			write( char c );

			/// Write a string that terminates with \c '\\0' to the sink.
			inline void
			write( const char* s );

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

#endif // MODM_LOG_STYLE_HPP
