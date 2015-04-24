#ifndef LEDS_HPP
#define LEDS_HPP

#include "../stm32f4_discovery.hpp"
#include <xpcc/ui/led.hpp>

xpcc::accessor::Flash<uint16_t> ledTable(xpcc::ui::table16_256);

// create the leds with these lambda callbacks
xpcc::ui::Led orange([](uint8_t brightness)
{
	Timer4::setCompareValue(2, ledTable[brightness]);
});

xpcc::ui::Led red([](uint8_t brightness)
{
	Timer4::setCompareValue(3, ledTable[brightness]);
});

xpcc::ui::Led green([](uint8_t brightness)
{
	Timer4::setCompareValue(1, ledTable[brightness]);
});

xpcc::ui::Led blue([](uint8_t brightness)
{
	Timer4::setCompareValue(4, ledTable[brightness]);
});

#endif // LEDS_HPP
