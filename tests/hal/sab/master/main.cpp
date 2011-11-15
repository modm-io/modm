// coding: utf-8
// ----------------------------------------------------------------------------

#include <avr/interrupt.h>

#include <xpcc/io/iostream.hpp>
#include <xpcc/communication/sab/interface.hpp>

#include <xpcc/architecture/general/gpio.hpp>
#include <xpcc/architecture/general/time/delay.hpp>

#include <xpcc/architecture/atmega/uart/uart.hpp>
#include <xpcc/driver/lcd/hd44780.hpp>

#include <xpcc/workflow/time/timeout.hpp>

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

xpcc::Hd447800 <lcd::E, lcd::RW, lcd::RS, lcd::Data> display;

xpcc::sab::Interface <xpcc::BufferedUart0> interface;

// ----------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect) {
	xpcc::Clock::increment();
}

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
	stream << "Hallo Welt!\n";
	
	//interface.initialize();
	
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;			// 1 ms at 14.7456 MHz
	
	sei();
	
	LedRed::reset();
	
	xpcc::Timeout<> timeout;
	while (1)
	{
		if (timeout.isExpired()) {
			timeout.restart(50);
			
			uint8_t data = 0xaa;
			interface.sendMessage(xpcc::sab::Interface<xpcc::BufferedUart0>::ACK, &data, 1);
		}
		
		interface.update();
		if (interface.isMessageAvailable())
		{
			const uint8_t *data = interface.getData();
			if ((interface.getLength() == 2) && (data[0] == 0xaa))
			{
				LedRed::write(data[1] & 0x2);
				LedGreen::write(data[1] & 0x1);
			}
			interface.dropMessage();
		}
	}
}
