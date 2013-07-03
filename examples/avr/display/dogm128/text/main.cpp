
#include <xpcc/architecture.hpp>

#include <xpcc/driver/display.hpp>
#include <xpcc/ui/display/font.hpp>

// LCD Backlight
namespace led
{
	GPIO__OUTPUT(R, D, 7);
	GPIO__OUTPUT(G, D, 6);
	GPIO__OUTPUT(B, D, 5);
}

// Graphic LCD
namespace lcd
{
	GPIO__OUTPUT(Scl, B, 7);
	GPIO__INPUT(Miso, B, 6);
	GPIO__OUTPUT(Mosi, B, 5);
	
	GPIO__OUTPUT(CS, D, 2);
	GPIO__OUTPUT(A0, D, 3);
	GPIO__OUTPUT(Reset, D, 4);
}

typedef xpcc::SoftwareSpiMaster< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

xpcc::DogM128< SPI, lcd::CS, lcd::A0, lcd::Reset, true > display;

MAIN_FUNCTION
{
	// Enable a yellow backlight
	led::R::set();
	led::G::set();
	led::B::reset();
	
	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();
	
	display.initialize();
	
	display.setFont(xpcc::font::ScriptoNarrow);
	display.setCursor(xpcc::glcd::Point(0, 0));
	display << "Hello World!\n";
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	display << "abcdefghijklmnopqrstuvwxyz\n";
	display << "0123456789!\"§$%&/()=?`´,;:-<>";
	
	display.setFont(xpcc::font::AllCaps3x5);
	display.setCursor(xpcc::glcd::Point(0, 32));
	display << "Hello World!" << xpcc::endl;
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << xpcc::endl;
	display << "abcdefghijklmnopqrstuvwxyz" << xpcc::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%&/()=?`´,;:-<>";
	display.update();
	
	xpcc::delay_ms(2000);
	
	display.clear();
	display.setFont(xpcc::font::Assertion);
	display.setCursor(xpcc::glcd::Point(0, 0));
	display << "Hello World!\n";
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	display << "abcdefghijklmnopqrstuvwxyz\n";
	display << "0123456789!\"§$%&/()=?`´,;:-<>";
	display.update();
	
	xpcc::delay_ms(2000);
	
	display.clear();
	display.setFont(xpcc::font::ArcadeClassic);
	display.setCursor(xpcc::glcd::Point(0, 0));
	display << "Hello World!\n\n";
	display << "ABCDEFGHIJKLMNOP\nQRSTUVWXYZ\n";
	display << "0123456789!\"§$%&/\n()=?`´,;:-<>";
	display.update();
	
	while (1)
	{
	}
}
