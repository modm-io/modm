
#include "human.hpp"

namespace bitmap
{
	EXTERN_FLASH_STORAGE(uint8_t dancing_man_0[]);
	EXTERN_FLASH_STORAGE(uint8_t dancing_man_1[]);
	EXTERN_FLASH_STORAGE(uint8_t dancing_man_1a[]);
	EXTERN_FLASH_STORAGE(uint8_t dancing_man_2[]);
	EXTERN_FLASH_STORAGE(uint8_t dancing_man_2a[]);
	EXTERN_FLASH_STORAGE(uint8_t dancing_man_3[]);
}

Human::Human(xpcc::glcd::Point start,
		xpcc::GraphicDisplay *display) :
	position(start),
	display(display),
	state(STAND)
{
}

void
Human::draw()
{
	this->run();
	
	if (state == STAND) {
		display->drawImage(position, xpcc::accessor::asFlash(bitmap::dancing_man_0));
	}
	else if (state == WAVE0) {
		display->drawImage(position, xpcc::accessor::asFlash(bitmap::dancing_man_1));
	}
	else if (state == WAVE1) {
		display->drawImage(position, xpcc::accessor::asFlash(bitmap::dancing_man_1a));
	}
}

bool
Human::run()
{
	PT_BEGIN();
	
	while (1)
	{
		timer.restart(3000);
		PT_WAIT_UNTIL(timer.isExpired());
		
		for (i = 0; i < 2; i++)
		{
			state = WAVE0;
			timer.restart(500);
			PT_WAIT_UNTIL(timer.isExpired());
			
			state = WAVE1;
			timer.restart(500);
			PT_WAIT_UNTIL(timer.isExpired());
		}
		state = STAND;
	}
	
	PT_END();
}
