#include "colorpalette.hpp"

xpcc::gui::ColorPalette xpcc::gui::DEFAULT_COLORPALETTE = {{
	xpcc::glcd::Color::black(),		// BLACK
	xpcc::glcd::Color::white(),		// WHITE
	xpcc::glcd::Color::gray(),		// GRAY
	xpcc::glcd::Color::red(),		// RED
	xpcc::glcd::Color::green(),		// GREEN
	xpcc::glcd::Color::blue(),		// BLUE
	xpcc::glcd::Color::yellow(),	// YELLOW
	xpcc::glcd::Color::blue(),		// BORDER
	xpcc::glcd::Color::yellow(),	// TEXT
	xpcc::glcd::Color::black(),		// BACKGROUND
	xpcc::glcd::Color::red(),		// ACTIVATED
	xpcc::glcd::Color::blue(),		// DEACTIVATED
}};
