#ifndef LEDS_HPP
#define LEDS_HPP

#include "../stm32f4_discovery.hpp"
#include <xpcc/ui/led.hpp>

// create the leds with these lambda callbacks
xpcc::ui::Led orange([](uint8_t brightness)
{
	Timer4::setCompareValue(2, xpcc::ui::table22_16_256[brightness]);
});

xpcc::ui::Led red([](uint8_t brightness)
{
	Timer4::setCompareValue(3, xpcc::ui::table22_16_256[brightness]);
});

xpcc::ui::Led green([](uint8_t brightness)
{
	Timer4::setCompareValue(1, xpcc::ui::table22_16_256[brightness]);
});

xpcc::ui::Led blue([](uint8_t brightness)
{
	Timer4::setCompareValue(4, xpcc::ui::table22_16_256[brightness]);
});

#endif // LEDS_HPP
