#include <avr/interrupt.h>

#include <xpcc/driver/debounce.hpp>
#include <xpcc/architecture/general/gpio.hpp>

GPIO__OUTPUT(Led, B, 0);

static xpcc::Debounce buttons;

ISR(TIMER2_COMPA_vect)
{
	buttons.update(PINB);
}

int
main()
{
	Led::setOutput();
	Led::reset();
	
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 23;			// 10 ms at 14.7456 MHz
	
	// set PB3 as input with pullup
	DDRB &= ~(1 << PB3);
	PORTB |= (1 << PB3);
	
	sei();
	
	while (1)
	{
		if (buttons.getPress(1 << PB3))
		{
			Led::toggle();
		}
	}
}
