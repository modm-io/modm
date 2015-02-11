#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/driver/display/ssd1306.hpp>

/**
 * Example to demonstrate a XPCC graphics display SSD1306.
 *
 * PB9	SDA
 * PB8	SCL
 *
 * GND and +5V or +3V3 are connected to the display module.
 */

typedef GpioB9 Sda;
typedef GpioB8 Scl;
typedef I2cMaster1 MyI2cMaster;
xpcc::Ssd1306<MyI2cMaster> display;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	Sda::connect(MyI2cMaster::Sda);
	Scl::connect(MyI2cMaster::Scl);
	MyI2cMaster::initialize<defaultSystemClock, 420000>();

	display.initializeBlocking();
	display.setFont(xpcc::font::Assertion);
	display << "Hello World!";
	display.update();

	xpcc::ShortPeriodicTimer timer(1000);
	uint16_t counter(0);

	while (1)
	{
		if (timer.isExpired())
		{
			display.setCursor(0,20);
			display << counter++;
			display.update();
		}
	}

	return 0;
}
