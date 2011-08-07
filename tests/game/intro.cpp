
#include <xpcc/driver/lcd/font.hpp>

#include "intro.hpp"

namespace bitmap
{
	EXTERN_FLASH_STORAGE(uint8_t moon_surface[]);
	EXTERN_FLASH_STORAGE(uint8_t lunar_lander[]);
}

using namespace xpcc::glcd;

Intro::Intro(xpcc::GraphicDisplay *display) :
	Window(display),
	monster(Point(103, 3), display),
	ship(Point(12, 3), display),
	human(Point(65, 30), display),
	missle(display),
	missleLaunched(false)
{
}

bool
Intro::update(Action action)
{
	PT_BEGIN();
	
	display->clear();
	
	while (1)
	{
		display->drawImage(Point(0, 0), xpcc::accessor::asFlash(bitmap::moon_surface));
		display->drawImage(Point(30, 31), xpcc::accessor::asFlash(bitmap::lunar_lander));
		
		display->setFont(xpcc::font::ArcadeClassic);
		display->setCursor(Point(52, 5));
		display->write("Moon");
		display->setCursor(Point(52, 13));
		display->write("Buggy");
		
		human.draw();
		ship.draw();
		monster.draw();
		
		if (action & OK) {
			missleLaunched = true;
		}
		
		if (missleLaunched) {
			if (!missle.draw()) {
				break;
			}
		}
		
		PT_YIELD();
	}
	
	PT_END();
}
