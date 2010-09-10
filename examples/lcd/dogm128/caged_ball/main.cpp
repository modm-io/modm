
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
	
	int8_t xSpeed = 3; 
	int8_t ySpeed = 1;
	uint8_t x = 15;
	uint8_t y = 15;
	const uint8_t radius = 5;
	while (1)
	{
		display.clear();
		display.drawRectangle(10, 10, 104, 44);
		
		x += xSpeed;
		y += ySpeed;
		
		if (x < 10 + radius + 3 || x > 114 - radius - 3) {
			xSpeed = -xSpeed;
		}
		if (y < 10 + radius + 1 || y > 54 - radius - 1) {
			ySpeed = -ySpeed;
		}
		
		display.drawCircle(x, y, radius);
		display.update();
		
		xpcc::delay_ms(50);
	}
}
