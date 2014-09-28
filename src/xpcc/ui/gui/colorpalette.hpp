#ifndef COLORPALETTE_HPP_
#define COLORPALETTE_HPP_

#include <xpcc/ui/display/graphic_display.hpp>


namespace xpcc {

namespace gui {

	/*
	 * TODO: find a way so that color options can be defined in user program
	 */

	enum Color {
		BLACK,
		WHITE,
		GRAY,
		RED,
		GREEN,
		BLUE,
		YELLOW,
		BORDER,
		TEXT,
		BACKGROUND,
		ACTIVATED,
		DEACTIVATED,
		PALETTE_SIZE
	};


	typedef xpcc::glcd::Color ColorPalette [Color::PALETTE_SIZE];

//	typedef xpcc::glcd::Color ColorPalette;
//	typedef std::array<xpcc::glcd::Color, Color::PALETTE_SIZE> ColorPalette;


	/*
	 * TODO: make DEFAULT_COLORPALETTE const or even better constexpr
	 */
	extern ColorPalette DEFAULT_COLORPALETTE; //[xpcc::gui::Color::PALETTE_SIZE];

	static inline void
	copyColorPalette(ColorPalette& src, ColorPalette& dest)
	{
		for(int ii = 0; ii < Color::PALETTE_SIZE; ++ii){
			dest[ii] = src[ii];
		}
	}

}
}

#endif /* COLORPALETTE_HPP_ */
