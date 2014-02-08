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
		BORDER,
		TEXT,
		BACKGROUND,
		ACTIVATED,
		DEACTIVATED,
		PALETTE_SIZE
	};


	typedef xpcc::glcd::Color ColorPalette;


}
}

#endif /* COLORPALETTE_HPP_ */
