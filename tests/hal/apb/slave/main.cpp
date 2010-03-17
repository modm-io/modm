// coding: utf-8
// ----------------------------------------------------------------------------

#include <avr/interrupt.h>

#include <xpcc/communication/io/iostream.hpp>
#include <xpcc/communication/apb/interface.hpp>

#include <xpcc/architecture/general/time/delay.hpp>
#include <xpcc/workflow/time/timeout.hpp>

#include <xpcc/architecture/general/gpio.hpp>
#include <xpcc/architecture/atmega/uart/uart.hpp>
#include <xpcc/driver/debounce.hpp>


// LEDs
CREATE_OUTPUT_PIN(LedGreen, B, 0);
CREATE_OUTPUT_PIN(LedRed, B, 1);

static xpcc::apb::Interface <xpcc::BufferedUart0> interface;
static xpcc::Debounce keys;

// ----------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect)
{
	static uint8_t counter = 0;
	
	xpcc::Clock::increment();
	
	counter++;
	if (counter > 10) {
		counter = 0;
		
		keys.update(PINB);
	}
}

// ----------------------------------------------------------------------------
int
main(void) __attribute__((OS_main));

int
main()
{
	LedGreen::setOutput();
	LedGreen::low();
	
	LedRed::setOutput();
	LedRed::high();
	
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;			// 1 ms at 14.7456 MHz
	
	// set PB3 as input with pullup
	DDRB &= ~(1 << PB3);
	PORTB |= (1 << PB3);
	
	interface.initialize();
	
	sei();
	
	LedRed::low();
	
	uint8_t status = 0;
	xpcc::Timeout<> timeout(200);
	while (1)
	{
		if (timeout.isExpired()) {
			timeout.restart(200);
			
			status ^= 0x1;
			LedGreen::toggle();
		}
		
		if (keys.getPress(xpcc::Debounce::KEY3)) {
			status ^= 0x2;
			LedRed::toggle();
		}
		
		interface.update();
		if (interface.isMessageAvailable())
		{
			const uint8_t *received = interface.getData();
			if (received[0] == 0xaa && interface.getLength() == 1)
			{
				uint8_t data[2] = { 0xaa, status };
				interface.sendMessage(xpcc::apb::Interface <xpcc::BufferedUart0>::ACK, data, 2);
			}
			interface.dropMessage();
		}
	}
}
