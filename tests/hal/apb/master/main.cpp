// coding: utf-8
// ----------------------------------------------------------------------------

#include <xpcc/communication/io/iostream.hpp>
#include <xpcc/communication/apb/interface.hpp>

#include <xpcc/architecture/general/gpio.hpp>
#include <xpcc/architecture/atmega/uart/uart.hpp>
#include <xpcc/driver/lcd/hd44780.hpp>

// LEDs
CREATE_OUTPUT_PIN(LedGreen, B, 0);
CREATE_OUTPUT_PIN(LedRed, B, 1);

// LCD
namespace lcd
{
	CREATE_OUTPUT_PIN(Backlight, A, 7);
	CREATE_OUTPUT_PIN(E, A, 6);
	CREATE_OUTPUT_PIN(RW, A, 5);
	CREATE_OUTPUT_PIN(RS, A, 4);
	
	CREATE_LOW_NIBBLE(Data, A);
}

xpcc::Hd447800<lcd::E, lcd::RW, lcd::RS, lcd::Data> display;

typedef xpcc::apb::Interface <xpcc::BufferedUart0> PeripheralInterface;

// ----------------------------------------------------------------------------
int
main(void) __attribute__((OS_main));

int
main()
{
	LedGreen::setOutput();
	LedGreen::reset();
	
	LedRed::setOutput();
	LedRed::set();
	
	lcd::Backlight::setOutput();
	lcd::Backlight::reset();
	
	display.initialize();
	
	xpcc::IOStream stream(display);
	stream << "Hallo Welt!";
	
	PeripheralInterface::initialize();
	
	LedRed::reset();
	
	uint8_t i = 0;
	while (1)
	{
		delay_ms(100);
		LedGreen::toggle();
		
		display.setPosition(1, 4);
		stream << static_cast<int8_t>(i) << ' ' << ' ';
		--i;
	}
}
