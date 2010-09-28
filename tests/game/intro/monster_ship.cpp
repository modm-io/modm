
#include "monster_ship.hpp"

namespace bitmap
{
	EXTERN_FLASH(uint8_t monster_ship[]);
	EXTERN_FLASH(uint8_t monster_ship_hidden[]);
}

MonsterShip::MonsterShip(xpcc::glcd::Point start,
		xpcc::GraphicDisplay *display) :
	position(start),
	display(display),
	visible(false),
	hidden(true)
{
}

void
MonsterShip::draw()
{
	this->run();
	
	if (visible)
	{
		if (hidden) {
			this->display->drawImage(position,
					xpcc::accessor::asFlash(bitmap::monster_ship_hidden));
		}
		else {
			this->display->drawImage(position,
					xpcc::accessor::asFlash(bitmap::monster_ship));
		}
	}
}

bool
MonsterShip::run()
{
	PT_BEGIN();
	
	while (1)
	{
		timer.restart(5000);
		PT_WAIT_UNTIL(timer.isExpired());
		
		visible = true;
		for (i = 0; i < 11; i++)
		{
			timer.restart(80);
			PT_WAIT_UNTIL(timer.isExpired());
			hidden = !hidden;
		}
		visible = !hidden;
		
		timer.restart(3000);
		PT_WAIT_UNTIL(timer.isExpired());
	}
	
	PT_END();
}
