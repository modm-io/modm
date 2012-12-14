
#include <xpcc/architecture.hpp>

#include <xpcc/driver/connectivity/spi/software_spi.hpp>
#include <xpcc/driver/ui/display/siemens_s65.hpp>
#include <xpcc/driver/ui/button_group.hpp>

GPIO__OUTPUT(Mosi, E, 5);
GPIO__OUTPUT(Sck, E, 7);
typedef xpcc::SoftwareSpi<Sck, Mosi, xpcc::gpio::Unused> Spi;

GPIO__OUTPUT(Cs, E, 2);
GPIO__OUTPUT(Rs, E, 3);
GPIO__OUTPUT(Reset, K, 3);
typedef xpcc::SiemensS65Portrait<Spi, Cs, Rs, Reset> Display;

GPIO__OUTPUT(Backlight, F, 0);

MAIN_FUNCTION
{
	Display display;
	
	Backlight::setOutput();
	Backlight::set();
	
	display.initialize();
	display.setFont(xpcc::font::Assertion);
	
	bool dir = true;
	uint8_t y = 0;
	while (1)
	{
		display.clear();
		display.setCursor(46, 2);
		display << "Hello";
		display.setCursor(46, 16);
		display << "World!";
		
		display.drawLine(0, y, 40, 31 - y);
		display.drawLine(132 - 40, 31 - y, 132, y);
		display.update();
		
		if (dir) {
			y++;
			if (y >= 31) {
				dir = false;
			}
		}
		else {
			y--;
			if (y == 0) {
				dir = true;
			}
		}
	}
}
