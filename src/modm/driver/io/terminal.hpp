/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2014, 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <modm/io/iodevice.hpp>

namespace modm
{

/**
 * Standard text terminal
 *
 * Uses the STL IOStream for in- and output.
 *
 * @ingroup	modm_platform_uart
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
