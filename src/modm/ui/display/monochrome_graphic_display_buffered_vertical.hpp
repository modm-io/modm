/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, 2013, 2019, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011, Thorsten Lajewski
 * Copyright (c) 2012-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_BUFFERED_VERTICAL_HPP
#define MODM_MONOCHROME_GRAPHIC_DISPLAY_BUFFERED_VERTICAL_HPP

#include "graphic_display.hpp"

#include <stdlib.h>

namespace modm
{
/**
 * Base class for graphical displays with a RAM buffer.
 *
 * Every operation works on the internal RAM buffer, therefore the content
 * of the real display is not changed until a call of update().
 *
 * \tparam	Width	Width of the display.
 * \tparam	Height	Height of the display. Must be a multiple of 8!
 *
 * \author	Fabian Greif
 * \ingroup	modm_ui_display
 */
template <int16_t Width, int16_t Height>
class MonochromeGraphicDisplayBufferedVertical : public GraphicDisplay
{
    // Height must be a multiple of 8
    static_assert((Height % 8) == 0, "height must be a multiple of 8");

    static_assert(Width > 0, "width must be greater than 0");
    static_assert(Height > 0, "height must be greater than 0");

public:
    static constexpr int16_t displayBufferWidth = Width;
    static constexpr int16_t displayBufferHeight = Height / 8;

    virtual ~MonochromeGraphicDisplayBufferedVertical() = default;

    virtual inline uint16_t
    getWidth() const override
    {
        return Width;
    }

    virtual inline uint16_t
    getHeight() const override
    {
        return Height;
    }

    /**
     * \brief	Clear the complete screen
     *
     * Use fillRectangle() to clear certain areas of the screen.
     */
    virtual void
    clear() override;

    // Faster version adapted for the RAM buffer
    virtual void
    drawImageRaw(glcd::Point upperLeft,
                 uint16_t width,
                 uint16_t height,
                 modm::accessor::Flash<uint8_t> data) override;

protected:
    // Faster version adapted for the RAM buffer
    virtual void
    drawHorizontalLine(glcd::Point start, uint16_t length) override;

    // TODO Faster version adapted for the RAM buffer
    // virtual void
    // drawVerticalLine(glcd::Point start, uint8_t length);

    virtual void
    setPixel(int16_t x, int16_t y) override;

    virtual void
    clearPixel(int16_t x, int16_t y) override;

    virtual bool
    getPixel(int16_t x, int16_t y) override;

    uint8_t display_buffer[displayBufferWidth][displayBufferHeight];
};
}  // namespace modm

#include "monochrome_graphic_display_buffered_vertical_impl.hpp"

#endif  // MODM_GRAPHIC_DISPLAY_HPP
