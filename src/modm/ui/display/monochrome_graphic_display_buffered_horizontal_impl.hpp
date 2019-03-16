/*
 * Copyright (c) 2019, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "monochrome_graphic_display_buffered_horizontal.hpp"

namespace modm
{
template <int16_t Width, int16_t Height>
void
MonochromeGraphicDisplayBufferedHorizontal<Width, Height>::clear()
{
    for (int_fast16_t y = 0; y < displayBufferHeight; ++y)
    {
        for (int_fast16_t x = 0; x < displayBufferWidth; ++x)
        {
            mDisplayBuffer[y][x] = 0;
        }
    }

    // reset the cursor
    this->cursor = glcd::Point(0, 0);
}

template <int16_t Width, int16_t Height>
void
MonochromeGraphicDisplayBufferedHorizontal<Width, Height>::setPixel(int16_t x, int16_t y)
{
    if ((x < Width) && (y < Height))
    {
        mDisplayBuffer[y][x / 8] |= (1 << (x & 0x07));
    }
}

template <int16_t Width, int16_t Height>
void
MonochromeGraphicDisplayBufferedHorizontal<Width, Height>::clearPixel(int16_t x, int16_t y)
{
    if ((x < Width) && (y < Height))
    {
        mDisplayBuffer[y][x / 8] &= ~(1 << (x & 0x07));
    }
}

template <int16_t Width, int16_t Height>
bool
MonochromeGraphicDisplayBufferedHorizontal<Width, Height>::getPixel(int16_t x, int16_t y)
{
    if ((x < Width) && (y < Height))
    {
        return (mDisplayBuffer[y][x / 8] & (1 << (x & 0x07)));
    }
    else
    {
        return false;
    }
}
}  // namespace modm
