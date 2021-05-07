/*
 * Copyright (c) 2009-2011, 2013, 2019, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_HPP
#error "Don't include this file directly, use 'monochrome_graphic_display.hpp' instead!"
#endif

template<int16_t Width, int16_t Height, std::size_t BufferWidth, std::size_t BufferHeight>
void
modm::MonochromeGraphicDisplay<Width, Height, BufferWidth, BufferHeight>::clear()
{
	std::fill(&buffer[0][0], &buffer[0][0] + sizeof(buffer), 0);
	this->cursor = {0, 0};
}