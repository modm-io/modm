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
		: color(color::html::White)
	{}

	virtual color::Rgb565<true>
	getPixel(display::Point pos) const = 0;

	color::Rgb565<true> color;
};
}  // namespace modm

#endif  // MODM_COLOR_GRAPHIC_DISPLAY_HPP
