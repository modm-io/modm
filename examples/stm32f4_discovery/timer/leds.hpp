#ifndef LEDS_HPP
#define LEDS_HPP

#include "../stm32f4_discovery.hpp"
#include <xpcc/ui/led.hpp>

class OrangeLed : public xpcc::ui::Led
{
	xpcc::accessor::Flash<uint16_t> table;

	virtual void
	setValue(uint8_t brightness)
	{
		Timer4::setCompareValue(2, table[brightness]);
	}

public:
	OrangeLed(const uint16_t* table=xpcc::ui::table16_256)
			:	Led(), table(table)
	{
	}
};

class RedLed : public xpcc::ui::Led
{
	xpcc::accessor::Flash<uint16_t> table;

	virtual void
	setValue(uint8_t brightness)
	{
		Timer4::setCompareValue(3, table[brightness]);
	}

public:
	RedLed(const uint16_t* table=xpcc::ui::table16_256)
			:	Led(), table(table)
	{
	}
};

class GreenLed : public xpcc::ui::Led
{
	xpcc::accessor::Flash<uint16_t> table;

	virtual void
	setValue(uint8_t brightness)
	{
		Timer4::setCompareValue(1, table[brightness]);
	}

public:
	GreenLed(const uint16_t* table=xpcc::ui::table16_256)
			:	Led(), table(table)
	{
	}
};

class BlueLed : public xpcc::ui::Led
{
	xpcc::accessor::Flash<uint16_t> table;

	virtual void
	setValue(uint8_t brightness)
	{
		Timer4::setCompareValue(4, table[brightness]);
	}

public:
	BlueLed(const uint16_t* table=xpcc::ui::table16_256)
			:	Led(), table(table)
	{
	}
};



#endif // LEDS_HPP
