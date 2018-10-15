/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LOG_PREFIX_HPP
#define MODM_LOG_PREFIX_HPP

#include <cstring>

#include "../style.hpp"

namespace modm
{
	namespace log
	{
		/**
		 * \class 	Prefix
		 * \brief 	Add a prefix to the log message
		 *
		 * \code
		 *	modm::log::StyleWrapper< modm::log::Prefix< char[9] > > loggerDevicePrefix (
		 * 			modm::log::Prefix< char[9] > ( "Prefix: ", loggerDevice ) );
		 * 	modm::log::Logger loggerPrefix( loggerDevicePrefix );
		 * \endcode
		 *
		 * \ingroup modm_debug
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		template <typename T, typename STYLE = DefaultStyle>
		class Prefix : public Style<STYLE>
		{
		public:
			Prefix(const T& str, STYLE style);

			Prefix(const T& str, IODevice &device);

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
			bool	flushed;
			T		value;

		};
	}
}

#include "prefix_impl.hpp"

#endif // MODM_LOG_PREFIX_HPP
