#include <xpcc/architecture/platform.hpp>

#include <xpcc/architecture/peripheral/gpio.hpp>
#include <xpcc/ui/button_group.hpp>

static xpcc::ButtonGroup<> buttons(0);

GPIO__OUTPUT(Led, B, 0);

// Timer interrupt used to query the button status
ISR(TIMER2_COMPA_vect)
{
	buttons.update(PINB & (1 << PB3));
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
		if (buttons.isPressed(1 << PB3))
		{
			Led::toggle();
		}
	}
}
