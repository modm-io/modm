
#include <xpcc/architecture.hpp>

#include <xpcc/driver/ui/display/ea_dog.hpp>
#include <xpcc/driver/connectivity/software_spi.hpp>

#include <xpcc/io/iostream.hpp>

using namespace xpcc::atmega;

namespace touch
{
	GPIO__IO(Bottom, A, 0);
	GPIO__IO(Left, A, 1);
	GPIO__IO(Top, A, 2);
	GPIO__IO(Right, A, 3);
}

BufferedUart0 uart(9600);

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

void
drawCross(uint8_t x, uint8_t y)
{
	display.drawPixel(x - 1, y - 1);
	display.drawPixel(x - 1, y + 1);
	display.drawPixel(x, y);
	display.drawPixel(x + 1, y - 1);
	display.drawPixel(x + 1, y + 1);
}

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::atmega::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "Welcome" << xpcc::endl;
	
	led::R::set();
	led::G::set();
	led::B::reset();
	
	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();
	
	display.initialize();
	
	// draw some testpoints
	drawCross( 10, 10);
	drawCross( 10, 54);
	drawCross(118, 10);
	drawCross(118, 54);
	drawCross( 62, 32);
	
	display.update();
	
	Adc::initialize(
			Adc::REFERENCE_INTERNAL_2V56,
			Adc::PRESCALER_64);
	
	touch::Bottom::setInput();
	touch::Bottom::reset();
	touch::Left::setInput();
	touch::Left::reset();
	touch::Top::setInput();
	touch::Top::reset();
	touch::Right::setInput();
	touch::Right::reset();
	
	while (1)
	{
		// read x value
		touch::Left::reset();
		touch::Left::setOutput();
		touch::Right::set();
		touch::Right::setOutput();
		xpcc::delay_ms(2);
		
		int16_t xAdc = Adc::readChannel(0);
		
		touch::Left::setInput();
		touch::Right::setInput();
		touch::Right::reset();
		
		// read y value
		touch::Bottom::reset();
		touch::Bottom::setOutput();
		touch::Top::set();
		touch::Top::setOutput();
		xpcc::delay_ms(2);
		
		int16_t yAdc = Adc::readChannel(1);
		
		touch::Bottom::setInput();
		touch::Top::setInput();
		touch::Top::reset();
		
		// print result
		output << "x=" << xAdc << ", y=" << yAdc << xpcc::endl;
		
		int16_t x = (static_cast<int32_t>(xAdc - 528) * (120 - 10)) / (115 - 528) + 10;
		int16_t y = (static_cast<int32_t>(yAdc - 236) * (58 - 10)) / (690 - 236) + 10;
		
		if (x < 0 || x > 128) {
			x = -1;
		}
		if (y < 0 || y > 64) {
			y = -1;
		}
		
		if (x >= 0 && y >= 0)
		{
			display.drawPixel(x, y);
			display.drawPixel(x, y+1);
			
			display.drawPixel(x+1, y);
			display.drawPixel(x+1, y+1);
		}
		
		display.update();
	}
}
