#include "colored_graphic_display.hpp"

// ----------------------------------------------------------------------------
void
modm::ColoredGraphicDisplay::setColor(const glcd::Color& newColor)
{
//	if (newColor == glcd::Color::black()) {
//		draw = &modm::GraphicDisplay::clearPixel;
//	}
//	else {
//		draw = &modm::GraphicDisplay::setPixel;
//	}

	/* When using a multicolor display we don't need clearPixel(), or at least
	 * not the way it was implemented above. Maybe check if newColor equals
	 * backgroundColor.
	 * */
	draw = &modm::ColoredGraphicDisplay::setPixel;
	this->foregroundColor = newColor;
}

void
modm::ColoredGraphicDisplay::setBackgroundColor(const glcd::Color& newColor)
{
	this->backgroundColor = newColor;
}