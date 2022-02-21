#ifndef MODM_COLOR_GRAPHIC_DISPLAY_HPP
#define MODM_COLOR_GRAPHIC_DISPLAY_HPP

#include <modm/platform.hpp>

#include "graphic_display.hpp"

using namespace modm::platform;

namespace modm
{
class ColorGraphicDisplay : public GraphicDisplay
{
public:
	ColorGraphicDisplay()
		: foregroundColor(color::html::White), backgroundColor(color::html::Black)
	{}

	virtual color::Rgb565
	getPixel(int16_t x, int16_t y) const = 0;

	/**
	 * Set a new foreground color.
	 * Used for drawing operations. Default is white.
	 */
	inline void
	setColor(const color::Rgb565 color)
	{
		foregroundColor = color;
	}

	inline color::Rgb565
	getColor() const
	{
		return foregroundColor;
	}

	/**
	 * Set background color.
	 * Used when clearing the screen. Default is black.
	 */
	inline void
	setBackgroundColor(const color::Rgb565 color)
	{
		backgroundColor = color;
	}

	inline color::Rgb565
	getBackgroundColor() const
	{
		return backgroundColor;
	}

	/**
	 * Draw a bitmap with 16-bit 565 colored pixels.
	 *
	 * \p data is series of pixel color values
	 */
	virtual void drawBitmap(
		glcd::Point upperLeft,
		uint16_t width,
		uint16_t height,
		modm::accessor::Flash<uint8_t> data)
	{
		modm_assert(false, "noDrawBitmap",
			"drawBitmap method is not implemented on ColorGraphicsDisplay implementation");
	};

protected:
	color::Rgb565 foregroundColor;
	color::Rgb565 backgroundColor;
};
}  // namespace modm

#endif  // MODM_COLOR_GRAPHIC_DISPLAY_HPP
