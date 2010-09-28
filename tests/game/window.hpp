
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <xpcc/driver/lcd/graphic_display.hpp>

enum Action
{
	UP = 0x10,
	DOWN = 0x01,
	FRONT = 0x02,
	BACK = 0x08,
	OK = 0x04
};

class Window
{
public:
	Window(xpcc::GraphicDisplay *display);
	
	/**
	 * \brief	Update screen
	 * 	
	 * \return	\c true while running, \c false if done.
	 */
	virtual bool
	update(Action action) = 0;
	
protected:
	xpcc::GraphicDisplay *display;
};

#endif // WINDOW_HPP
