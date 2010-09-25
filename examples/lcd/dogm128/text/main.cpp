
#include <xpcc/architecture.hpp>

#include <xpcc/driver/lcd/st7565.hpp>
#include <xpcc/driver/lcd/font.hpp>

#include <xpcc/driver/software_spi.hpp>
#include <xpcc/driver/gpio.hpp>

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

typedef xpcc::SoftwareSpi< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

xpcc::St7565< SPI, lcd::CS, lcd::A0, lcd::Reset > display;
xpcc::IOStream displayStream(display);

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
	
	display.setColor(xpcc::glcd::WHITE);
	display.setFont(xpcc::accessor::asFlash(xpcc::font::ScriptoNarrow));
	display.setCursor(xpcc::glcd::Point(0, 0));
	display.write("Hello World!\n");
	display.write("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	display.write("abcdefghijklmnopqrstuvwxyz\n");
	display.write("0123456789!\"§$%&/()=?`´,;:-<>");
	
	display.setFont(xpcc::accessor::asFlash(xpcc::font::AllCaps3x6));
	display.setCursor(xpcc::glcd::Point(0, 32));
	displayStream << "Hello World!" << xpcc::endl;
	displayStream << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << xpcc::endl;
	displayStream << "abcdefghijklmnopqrstuvwxyz" << xpcc::endl;
	displayStream << 0 << 12 << 345 << 6789 << "!\"§$%&/()=?`´,;:-<>";
	display.update();
	
	while (1)
	{
	}
}
