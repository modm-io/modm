
#ifndef MISSLE_HPP
#define MISSLE_HPP

#include <xpcc/workflow/protothread.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <xpcc/driver/lcd/graphic_display.hpp>

class Missle : public xpcc::Protothread
{
public:
	Missle(xpcc::GraphicDisplay *display);
	
	bool
	draw();
	
protected:
	bool
	run();
	
	xpcc::GraphicDisplay *display;
	xpcc::Timeout<> timer;
	
	bool flying;
	uint8_t x;
};

#endif // MISSLE_HPP
