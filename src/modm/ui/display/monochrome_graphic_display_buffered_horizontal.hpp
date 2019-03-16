/*
 * Copyright (c) 2019, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_BUFFERED_HORIZONTAL_HPP
#define MODM_MONOCHROME_GRAPHIC_DISPLAY_BUFFERED_HORIZONTAL_HPP

#include <modm/ui/display/graphic_display.hpp>

#include <stdlib.h>

namespace modm
{
/**
 * Base class for monochrome graphical displays with a RAM buffer.
 *
 * The pixel are compressed horizontally, this means that 8 pixel in
 * y-direction are combined into one byte in the RAM buffer.
 *
 * Every operation works on the internal RAM buffer, therefore the content
 * of the real display is not changed until a call of update().
 *
 * \tparam	Width	Width of the display. Must be a multiple of 8!
 * \tparam	Height	Height of the display.
 *
 * \ingroup	modm_ui_display
 */
template <int16_t Width, int16_t Height>
class MonochromeGraphicDisplayBufferedHorizontal : public GraphicDisplay
{
    // Height must be a multiple of 8
    static_assert((Width % 8) == 0, "width must be a multiple of 8");

    static_assert(Width > 0, "width must be greater than 0");
    static_assert(Height > 0, "height must be greater than 0");

public:
    static constexpr int16_t displayBufferWidth = Width / 8;
    static constexpr int16_t displayBufferHeight = Height;

    virtual ~MonochromeGraphicDisplayBufferedHorizontal() = default;

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

    virtual void
    clear() override;

protected:
    virtual void
    setPixel(int16_t x, int16_t y) override;

    virtual void
    clearPixel(int16_t x, int16_t y) override;

    virtual bool
    getPixel(int16_t x, int16_t y) override;

    uint8_t mDisplayBuffer[displayBufferHeight][displayBufferWidth];
};
}  // namespace modm

#include "monochrome_graphic_display_buffered_horizontal_impl.hpp"

#endif
