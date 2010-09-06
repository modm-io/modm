
#include <xpcc/architecture/driver.hpp>

#include <xpcc/driver/lcd/st7565.hpp>
#include <xpcc/driver/software_spi.hpp>

#include <xpcc/io/iostream.hpp>

namespace touch
{
	GPIO__IO(Bottom, A, 0);
	GPIO__IO(Left, A, 1);
	GPIO__IO(Top, A, 2);
	GPIO__IO(Right, A, 3);
}

xpcc::BufferedUart0 uart(9600);

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

xpcc::St7565< lcd::SPI, lcd::CS, lcd::A0, lcd::Reset > st7565;

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "Welcome" << xpcc::endl;
	
	led::R::set();
	led::G::set();
	led::B::reset();
	
	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();
	
	st7565.initialize();
	
	/*st7565.setPixel(9, 9);
	st7565.setPixel(9, 11);
	st7565.setPixel(10, 10);
	st7565.setPixel(11, 9);
	st7565.setPixel(11, 11);
	
	st7565.setPixel(9, 53);
	st7565.setPixel(9, 55);
	st7565.setPixel(10, 54);
	st7565.setPixel(11, 53);
	st7565.setPixel(11, 55);
	
	st7565.setPixel(117, 9);
	st7565.setPixel(117, 11);
	st7565.setPixel(118, 10);
	st7565.setPixel(119, 9);
	st7565.setPixel(119, 11);
	
	st7565.setPixel(117, 53);
	st7565.setPixel(117, 55);
	st7565.setPixel(118, 54);
	st7565.setPixel(119, 53);
	st7565.setPixel(119, 55);
	
	st7565.setPixel(61, 31);
	st7565.setPixel(61, 33);
	st7565.setPixel(62, 32);
	st7565.setPixel(63, 31);
	st7565.setPixel(63, 33);*/
	
	st7565.update();
	
	xpcc::Adc::initialize(
			xpcc::Adc::REFERENCE_INTERNAL_2V56,
			xpcc::Adc::PRESCALER_64);
	
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
		int16_t xAdc, yAdc;
		
		// read x value
		touch::Left::reset();
		touch::Left::setOutput();
		touch::Right::set();
		touch::Right::setOutput();
		xpcc::delay_us(500);
		
		xAdc = xpcc::Adc::readChannel(0);
		
		touch::Left::setInput();
		touch::Right::setInput();
		touch::Right::reset();
		
		// read y value
		touch::Bottom::reset();
		touch::Bottom::setOutput();
		touch::Top::set();
		touch::Top::setOutput();
		xpcc::delay_us(700);
		
		yAdc = xpcc::Adc::readChannel(1);
		
		touch::Bottom::setInput();
		touch::Top::setInput();
		touch::Top::reset();
		
		int16_t x = (static_cast<int32_t>(xAdc - 528) * (120 - 10)) / (115 - 528) + 10;
		int16_t y = (static_cast<int32_t>(yAdc - 236) * (58 - 10)) / (690 - 236) + 10;
		
		if (x < 0 || x > 128) {
			x = -1;
		}
		if (y < 0 || y > 64) {
			y = -1;
		}
		
		// output result
		//output << "x=" << x << ", y=" << y << xpcc::endl;
		
		st7565.setPixel(x, y);
		st7565.setPixel(x, y+1);
		
		st7565.setPixel(x+1, y);
		st7565.setPixel(x+1, y+1);
		
		st7565.update();
	}
}
