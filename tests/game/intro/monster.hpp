
#ifndef MONSTER_HPP
#define MONSTER_HPP

#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/periodic_timer.hpp>
#include <xpcc/driver/lcd/graphic_display.hpp>

class Monster : public xpcc::Protothread
{
public:
	Monster(xpcc::glcd::Point start, xpcc::GraphicDisplay *display);
	
	void
	draw();
	
protected:
	bool
	run();
	
	xpcc::glcd::Point targetPosition;
	xpcc::glcd::Point position;
	xpcc::GraphicDisplay *display;
	xpcc::ShortPeriodicTimer timer;
	int8_t i;
};

#endif // MONSTER_HPP
