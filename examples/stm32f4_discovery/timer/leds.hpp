#ifndef LEDS_HPP
#define LEDS_HPP

#include "../stm32f4_discovery.hpp"
#include <xpcc/ui/led.hpp>

xpcc::accessor::Flash<uint16_t> ledTable(xpcc::ui::table16_256);

void
setOrange(uint8_t brightness)
{
	Timer4::setCompareValue(2, ledTable[brightness]);
}

void
setRed(uint8_t brightness)
{
	Timer4::setCompareValue(3, ledTable[brightness]);
}

void
setGreen(uint8_t brightness)
{
	Timer4::setCompareValue(1, ledTable[brightness]);
}

void
setBlue(uint8_t brightness)
{
	Timer4::setCompareValue(4, ledTable[brightness]);
}

// create the leds with these callbacks
xpcc::ui::Led orange(setOrange);
xpcc::ui::Led red(setRed);
xpcc::ui::Led green(setGreen);
xpcc::ui::Led blue(setBlue);

#endif // LEDS_HPP
