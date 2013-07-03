
#include <xpcc/architecture.hpp>

#include <xpcc/ui/display/ea_dog.hpp>
#include <xpcc/communication/spi/software_spi.hpp>

namespace led
{
	GPIO__OUTPUT(R, D, 7);
	GPIO__OUTPUT(G, D, 6);
	GPIO__OUTPUT(B, D, 5);
}

// define the pins used by the LCD
namespace lcd
{
	GPIO__OUTPUT(Scl, B, 7);
	GPIO__OUTPUT(Mosi, B, 5);
	
	GPIO__OUTPUT(CS, D, 2);
	GPIO__OUTPUT(A0, D, 3);
	GPIO__OUTPUT(Reset, D, 4);
	
	typedef xpcc::SoftwareSpi< Scl, Mosi, xpcc::gpio::Unused > SPI;
}

xpcc::DogM128< lcd::SPI, lcd::CS, lcd::A0, lcd::Reset, true > display;

using namespace xpcc::glcd;

int
main()
{
	led::R::set();
	led::G::set();
	led::B::reset();
	
	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();
	
	display.initialize();
	
	display.drawLine(Point(68, 50), Point(90, 50));
	display.drawLine(Point(78, 45), Point(78, 58));
	
	display.drawRoundedRectangle(Point(70, 13), 30, 20, 3);
	
	display.drawCircle(Point(105, 25), 20);
	
	display.drawEllipse(Point(60, 25), 20, 18);
	display.drawEllipse(Point(63, 31), 63, 31);
	
	display.fillRectangle(Point(10, 10), 20, 20);
	display.fillCircle(Point(45, 40), 23);
	
	display.setColor(WHITE);
	display.fillRectangle(Point(20, 20), 20, 20);
	display.fillCircle(Point(55, 50), 8);
	
	display.setColor(BLACK);
	display.drawLine(Point(0, 0), Point(127, 63));
	
	display.update();
	
	while (1)
	{
	}
}
