
#include <xpcc/architecture/driver.hpp>

#include <xpcc/driver/lcd/st7565.hpp>
#include <xpcc/driver/software_spi.hpp>

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

xpcc::St7565< lcd::SPI, lcd::CS, lcd::A0, lcd::Reset > display;

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
	
	/*
	display.drawLine(0, 0, 50, 50);
	
	display.drawLine(20, 20, 70, 20);
	display.drawLine(20, 20, 70, 30);
	
	display.drawLine(50, 20, 50, 50);
	display.drawLine(50, 20, 60, 50);
	
	display.drawHorizontalLine(10, 10, 50);
	display.drawVerticalLine(20, 20, 30);
	
	display.drawRoundedRectangle(80, 10, 20, 20, 3);
	
	display.drawLine(30, 0, 10, 50);
	display.fillRectangle(80, 40, 10, 20);
	
	display.drawCircle(105, 25, 20);
	*/
	
	display.drawEllipse(40, 25, 20, 18);
	
	display.drawEllipse(105, 25, 10, 25);
	display.drawEllipse(60, 40, 20, 10);
	
	display.drawEllipse(63, 31, 63, 31);
	
	display.update();
	
	while (1)
	{
	}
}
