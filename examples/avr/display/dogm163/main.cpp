
#include <xpcc/architecture.hpp>
#include <xpcc/driver/display.hpp>
#include <xpcc/ui/display/font.hpp>

// Graphic LCD
namespace lcd
{
	GPIO__OUTPUT(Scl, B, 7);
	GPIO__INPUT(Miso, B, 6);
	GPIO__OUTPUT(Mosi, B, 5);
	
	GPIO__OUTPUT(CS, D, 2);
	GPIO__OUTPUT(RS, D, 3);
}

typedef xpcc::SoftwareSpiMaster< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

xpcc::DogM163< SPI, lcd::CS, lcd::RS > display;

MAIN_FUNCTION
{
	display.initialize();
	
	display.setCursor(0, 0);
	display << "ABCDEFGHIJKLMNOP" << xpcc::endl;
	display << "abcdefghijklmnop" << xpcc::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%" << xpcc::endl;
	
	while (1)
	{
	}
}
