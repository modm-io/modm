/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PC_TERMINAL_HPP
#define MODM_PC_TERMINAL_HPP

#include <modm/io/iodevice.hpp>

namespace modm
{
	namespace pc
	{
		/**
		 * \brief	Standard text terminal
		 * 
		 * Uses the STL IOStream for in- and output.
		 * 
		 * \ingroup	hosted
		 */
		class Terminal : public IODevice
		{
		public :
			virtual void
			write(char c);
			
			virtual void
			write(const char* s);
			
			virtual void
			flush();
			
			virtual bool
			read(char& value);
		};
	}
}

#endif	// MODM_PC_TERMINAL_HPP
