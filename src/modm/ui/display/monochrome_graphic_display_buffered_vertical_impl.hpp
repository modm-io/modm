/*
 * Copyright (c) 2009-2011, 2013, 2019, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Antal Szabó
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_BUFFERED_VERTICAL_IMPL_HPP
#define MODM_MONOCHROME_GRAPHIC_DISPLAY_BUFFERED_VERTICAL_IMPL_HPP

#include "monochrome_graphic_display_buffered_vertical.hpp"

template <int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayBufferedVertical<Width, Height>::clear()
{
    for (int_fast16_t y = 0; y < Height / 8; ++y)
    {
        for (int_fast16_t x = 0; x < Width; ++x)
        {
            this->display_buffer[x][y] = 0;
        }
    }

    // reset the cursor
    this->cursor = glcd::Point(0, 0);
}

template <int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayBufferedVertical<Width, Height>::drawHorizontalLine(glcd::Point start,
                                                                                  uint16_t length)
{
    const int16_t y = start.getY() / 8;

    if (this->foregroundColor == glcd::Color::black())
    {
        const uint8_t mask = 1 << (start.getY() & 0x07);
        for (int_fast16_t x = start.getX(); x < static_cast<int16_t>(start.getX() + length); ++x)
        {
            if (x < Width && y < Height)
            {
                this->display_buffer[x][y] |= mask;
            }
        }
    }
    else
    {
        const uint8_t mask = ~(1 << (start.getY() & 0x07));
        for (int_fast16_t x = start.getX(); x < static_cast<int16_t>(start.getX() + length); ++x)
        {
            if (x < Width && y < Height)
            {
                this->display_buffer[x][y] &= mask;
            }
        }
    }
}

template <int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayBufferedVertical<Width, Height>::drawImageRaw(
        glcd::Point upperLeft, uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
    if ((upperLeft.getY() & 0x07) == 0)
    {
        uint16_t row = upperLeft.getY() / 8;
        uint16_t rowCount = (height + 7) / 8;  // always round up

        if ((height & 0x07) == 0)
        {
            for (uint_fast16_t i = 0; i < width; i++)
            {
                for (uint_fast16_t k = 0; k < rowCount; k++)
                {
                    uint16_t x = upperLeft.getX() + i;
                    uint16_t y = k + row;

                    if (x < Width && y < Height)
                    {
                        this->display_buffer[x][y] = data[i + k * width];
                    }
                }
            }
            return;
        }
    }

    GraphicDisplay::drawImageRaw(upperLeft, width, height, data);
}

template <int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayBufferedVertical<Width, Height>::setPixel(int16_t x, int16_t y)
{
    if (x < Width && y < Height)
    {
        this->display_buffer[x][y / 8] |= (1 << (y & 0x07));
    }
}

template <int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayBufferedVertical<Width, Height>::clearPixel(int16_t x, int16_t y)
{
    if (x < Width && y < Height)
    {
        this->display_buffer[x][y / 8] &= ~(1 << (y & 0x07));
    }
}

template <int16_t Width, int16_t Height>
bool
modm::MonochromeGraphicDisplayBufferedVertical<Width, Height>::getPixel(int16_t x, int16_t y)
{
    if (x < Width && y < Height)
    {
        return (this->display_buffer[x][y / 8] & (1 << (y & 0x07)));
    }
    else
    {
        return false;
    }
}

#endif
