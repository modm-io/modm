/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../logger.hpp"
#include "../style_wrapper.hpp"
#include "../style/prefix.hpp"
#include "../style/std_colour.hpp"

#include <modm/architecture/utils.hpp>
#include <modm/driver/io/terminal.hpp>

static modm::Terminal device;

namespace modm
{
	namespace log
	{
		template < typename T, Colour TEXT, Colour BACKGROUND >
		class Wrapper : public StyleWrapper< Prefix< T, StdColour<TEXT, BACKGROUND > > >
		{
			public:
				Wrapper(const T& str, ::modm::IODevice& device) :
					StyleWrapper< Prefix< T, StdColour<TEXT, BACKGROUND > > > (
							Prefix< T, StdColour<TEXT, BACKGROUND > >(
									str,
									StdColour<TEXT, BACKGROUND >(device) ) )
				{
				}
		};

		static Wrapper< char[10], TURQUOISE, NONE > debugWrapper("Debug:   ", device);
		Logger modm_weak debug(debugWrapper);

		static Wrapper< char[10], GREEN, NONE > debugInfo("Info:    ", device);
		Logger modm_weak info(debugInfo);

		static Wrapper< char[10], YELLOW, NONE > warningInfo("Warning: ", device);
		Logger modm_weak warning(warningInfo);

		static Wrapper< char[10], RED, NONE > errorInfo("Error:   ", device);
		Logger modm_weak error(errorInfo);
	}
}
