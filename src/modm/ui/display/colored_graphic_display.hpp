#ifndef MODM_COLORED_GRAPHIC_DISPLAY_HPP
#define MODM_COLORED_GRAPHIC_DISPLAY_HPP

#include <modm/platform.hpp>

#include "graphic_display.hpp"

using namespace modm::platform;

namespace modm
{
namespace glcd
{
// RGB16 (565) Format
/// @ingroup modm_ui_display
class Color
{
public:
	static modm_always_inline Color white()			{ return Color(0xffff); };
	static modm_always_inline Color yellow()		{ return Color(0xFFE0); };
	static modm_always_inline Color magenta()		{ return Color(0xF81F); };
	static modm_always_inline Color red()			{ return Color(0xF800); };
	static modm_always_inline Color orange()		{ return Color(0xFD20); };
	static modm_always_inline Color sliver()		{ return Color(0xC618); };
	static modm_always_inline Color gray()			{ return Color(0x8410); };
	static modm_always_inline Color maroon()		{ return Color(0x8000); };
	static modm_always_inline Color lime()			{ return Color(0x07E0); };
	static modm_always_inline Color green()			{ return Color(0x0400); };
	static modm_always_inline Color blue()			{ return Color(0x001F); };
	static modm_always_inline Color navy()			{ return Color(0x0010); };
	static modm_always_inline Color black()			{ return Color(0x0000); };
	static modm_always_inline Color signalViolet()	{ return Color(0x8010); }; //0x84D0
	static modm_always_inline Color emeraldGreen()	{ return Color(0x5DCC); };

	/**
	 * @param	red
	 * 		Range [0..255]
	 * @param	green
	 * 		Range [0..255]
	 * @param	blue
	 * 		Range [0..255]
	 */
	Color(uint8_t red, uint8_t green, uint8_t blue)
		: color(((static_cast<uint16_t>(red >> 3) << 11) |
				 (static_cast<uint16_t>(green >> 2) << 5) | static_cast<uint16_t>(blue >> 3)))
	{}

	Color(uint16_t color) : color(color) {}

	Color() : color(0) {}

	inline uint16_t
	getValue() const
	{
		return color;
	}

	bool
	operator==(const Color& other) const
	{
		return (color == other.color);
	}

private:
	uint16_t color;
};
}  // namespace glcd

class ColoredGraphicDisplay : public GraphicDisplay
{
public:
	ColoredGraphicDisplay()
		: foregroundColor(glcd::Color::white()), backgroundColor(glcd::Color::black())
	{}
	/**
	 * Set a new foreground color.
	 *
	 * The foreground color is used for all drawing operations. Default
	 * is white.
	 *
	 * @see	setBackgroundColor()
	 */
	void
	setColor(const glcd::Color& color);

	inline glcd::Color
	getForegroundColor() const
	{
		return this->foregroundColor;
	}

	/**
	 * Set new background color.
	 *
	 * The background color used when clearing the screen. Default is black.
	 *
	 * @see	setColor()
	 */
	void
	setBackgroundColor(const glcd::Color& color);

protected:
	glcd::Color foregroundColor;
	glcd::Color backgroundColor;
};
}  // namespace modm

#endif  // MODM_COLORED_GRAPHIC_DISPLAY_HPP