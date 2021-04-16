#ifndef MODM_COLOR_GRAPHIC_DISPLAY_HPP
#define MODM_COLOR_GRAPHIC_DISPLAY_HPP

#include <modm/platform.hpp>

#include "graphic_display.hpp"

using namespace modm::platform;

namespace modm
{

// Backwards compatibility
namespace glcd
{
/// @ingroup modm_ui_display
using Point = Vector<int16_t, 2>;

// RGB16 (565) Format
/// @ingroup modm_ui_display
class Color
{
public:
	static constexpr Color white()          { return Color(0xffff); };
	static constexpr Color yellow()         { return Color(0xFFE0); };
	static constexpr Color magenta()        { return Color(0xF81F); };
	static constexpr Color red()            { return Color(0xF800); };
	static constexpr Color orange()         { return Color(0xFD20); };
	static constexpr Color silver()         { return Color(0xC618); };
	static constexpr Color gray()           { return Color(0x8410); };
	static constexpr Color maroon()         { return Color(0x8000); };
	static constexpr Color lime()           { return Color(0x07E0); };
	static constexpr Color green()          { return Color(0x0400); };
	static constexpr Color blue()           { return Color(0x001F); };
	static constexpr Color navy()           { return Color(0x0010); };
	static constexpr Color black()          { return Color(0x0000); };
	static constexpr Color signalViolet()   { return Color(0x8010); };
	static constexpr Color emeraldGreen()   { return Color(0x5DCC); };

	constexpr Color() = default;

	constexpr Color(uint16_t color) : color(color) {}

	/**
	 * @param	red
	 * 		Range [0..255]
	 * @param	green
	 * 		Range [0..255]
	 * @param	blue
	 * 		Range [0..255]
	 */
	constexpr Color(uint8_t red, uint8_t green, uint8_t blue) :
		color(uint16_t((red >> 3) << 11) | uint16_t((green >> 2) << 5) | uint16_t(blue >> 3))
	{}

	inline uint16_t
	getValue() const
	{
		return color;
	}

	bool
	operator==(const Color &other) const
	{
		return (color == other.color);
	}

private:
	uint16_t color{0x000};
};
}  // namespace glcd

class ColorGraphicDisplay : public GraphicDisplay
{
public:
	ColorGraphicDisplay()
		: foregroundColor(glcd::Color::white()), backgroundColor(glcd::Color::black())
	{}

	virtual glcd::Color
	getPixel(int16_t x, int16_t y) const = 0;

	/**
	 * Set a new foreground color.
	 * Used for drawing operations. Default is white.
	 */
	inline void
	setColor(const glcd::Color color)
	{
		foregroundColor = color;
	}

	inline glcd::Color
	getColor() const
	{
		return foregroundColor;
	}

	/**
	 * Set background color.
	 * Used when clearing the screen. Default is black.
	 */
	inline void
	setBackgroundColor(const glcd::Color color)
	{
		backgroundColor = color;
	}

	inline glcd::Color
	getBackgroundColor() const
	{
		return backgroundColor;
	}

protected:
	glcd::Color foregroundColor;
	glcd::Color backgroundColor;
};
}  // namespace modm

#endif  // MODM_COLOR_GRAPHIC_DISPLAY_HPP
