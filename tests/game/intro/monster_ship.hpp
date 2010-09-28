
#ifndef MONSTER_SHIP_HPP
#define MONSTER_SHIP_HPP

#include <xpcc/workflow/protothread.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <xpcc/driver/lcd/graphic_display.hpp>

class MonsterShip : public xpcc::Protothread
{
public:
	MonsterShip(xpcc::glcd::Point start, xpcc::GraphicDisplay *display);
	
	void
	draw();
	
protected:
	bool
	run();
	
	xpcc::glcd::Point position;
	xpcc::GraphicDisplay *display;
	xpcc::Timeout<> timer;
	
	uint8_t i;
	bool visible;
	bool hidden;
};

#endif // MONSTER_SHIP_HPP
