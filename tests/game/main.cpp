
#include <xpcc/architecture/driver.hpp>

#include <xpcc/driver/lcd/st7565.hpp>
#include <xpcc/driver/software_spi.hpp>

#include <xpcc/driver/gpio.hpp>

#include <xpcc/driver/lcd/font.hpp>

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

// GPIO Expanders (they use the same SPI as the LCD)
GPIO__OUTPUT(Cs0, C, 3);
GPIO__INPUT(Int0, C, 2);

GPIO__OUTPUT(Cs1, C, 5);
GPIO__INPUT(Int1, C, 4);

xpcc::Mcp23s08< SPI, Cs0, Int0 > gpio0;
xpcc::Mcp23s17< SPI, Cs1, Int1 > gpio1;

// Encoder input
GPIO__OUTPUT(ENCODER_A, C, 7);
GPIO__OUTPUT(ENCODER_B, C, 6);

// UART
xpcc::BufferedUart0 uart(115200);

#include <xpcc/debug/logger.hpp>

xpcc::IODeviceWrapper<xpcc::BufferedUart0> loggerDevice(uart);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

MAIN_FUNCTION
{
	// Enable a yellow backlight
	led::R::set();
	led::G::set();
	led::B::reset();
	
	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();
	
	// Configure all pins as input with pullup
	gpio0.initialize();
	gpio0.configure(0xff, 0xff);	// 0x3f used for buttons
	
	gpio1.initialize();
	gpio1.configure(0x001f, 0x001f);
	
	// Enable interrupts
	sei();
	
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
	display.write("Hello World!\n");
	display.write("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	display.write("abcdefghijklmnopqrstuvwxyz\n");
	display.write("0123456789!\"§$%&/()=?`´,;:-<>");
	display.update();
	
	xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
	xpcc::IOStream stream(device);
	
	stream << "Welcome!" << xpcc::endl;
	
	while (1)
	{
		/*uint8_t input0 = gpio0.read();
		uint16_t input1 = gpio1.read();
		
		stream << xpcc::hex
				<< input0
				<< static_cast<uint8_t>(input1 >> 8)
				<< static_cast<uint8_t>(input1 & 0xff) << xpcc::endl;
		xpcc::delay_ms(200);*/
	}
}
