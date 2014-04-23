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


	typedef xpcc::glcd::Color ColorPalette;

	/*
	 * TODO: make DEFAULT_COLORPALETTE const or even better constexpr
	 */
	extern ColorPalette DEFAULT_COLORPALETTE[xpcc::gui::Color::PALETTE_SIZE];

}
}

#endif /* COLORPALETTE_HPP_ */
