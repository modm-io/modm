
#include "monster.hpp"

namespace bitmap
{
	EXTERN_FLASH_STORAGE(uint8_t monster[]);
}

Monster::Monster(xpcc::glcd::Point start,
		xpcc::GraphicDisplay *display) :
	targetPosition(start),
	position(start),
	display(display),
	timer(300)
{
}

void
Monster::draw()
{
	this->run();
	this->display->drawImage(position,
			xpcc::accessor::asFlash(bitmap::monster));
}

bool
Monster::run()
{
	PT_BEGIN();
	
	while (1)
	{
		for (i = 0; i < 3; i++)
		{
			position.setY(targetPosition.getY() + i);
			
			PT_WAIT_UNTIL(timer.isExpired());
		}
		for (i = 3; i > 0; i--)
		{
			position.setY(targetPosition.getY() + i);
			
			PT_WAIT_UNTIL(timer.isExpired());
		}
	}
	
	PT_END();
}
