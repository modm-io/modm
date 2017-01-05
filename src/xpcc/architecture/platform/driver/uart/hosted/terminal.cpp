/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <iostream>

#include "terminal.hpp"

// ----------------------------------------------------------------------------
void
modm::pc::Terminal::write(char c)
{
	std::cout << c;
}

void
modm::pc::Terminal::write(const char* s)
{
	std::cout << s;
}

void
modm::pc::Terminal::flush()
{
	std::cout << std::flush;
}

bool
modm::pc::Terminal::read(char& value)
{
	std::cin.get(value);
	return std::cin.good();
}
