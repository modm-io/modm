
#include "missle.hpp"

namespace bitmap
{
	EXTERN_FLASH_STORAGE(uint8_t rocket[]);
}

using namespace xpcc::glcd;

Missle::Missle(xpcc::GraphicDisplay *display) :
	display(display),
	flying(true)
{
}

bool
Missle::draw()
{
	this->run();
	
	if (flying) {
		display->drawImage(Point(x, 29),
				xpcc::accessor::asFlash(bitmap::rocket));
	}
	else
	{
		display->drawImage(Point(52, 29),
				xpcc::accessor::asFlash(bitmap::rocket));
		
		display->setColor(BLACK);
		display->drawCircle(Point(64, 32), x);
		if (x >= 12) {
			display->fillCircle(Point(20, 20), x - 11);
		}
		if (x >= 14) {
			display->fillCircle(Point(100, 35), x - 13);
		}
		
		display->setColor(WHITE);
		display->fillCircle(Point(64, 32), x - 1);
		if (x >= 12) {
			display->fillCircle(Point(20, 20), x - 12);
		}
		if (x >= 14) {
			display->fillCircle(Point(100, 35), x - 14);
		}
		
		display->setColor(BLACK);
	}
	return this->isRunning();
}

bool
Missle::run()
{
	PT_BEGIN();
	
	for (x = 0; x < 52; x++) {
		timer.restart(40);
		PT_WAIT_UNTIL(timer.isExpired());
	}
	
	flying = false;
	
	for (x = 1; x < 31; x++) {
		timer.restart(30);
		PT_WAIT_UNTIL(timer.isExpired());
	}
	
	
	PT_END();
}
