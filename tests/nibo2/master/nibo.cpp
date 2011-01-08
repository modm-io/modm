
#include <avr/interrupt.h>
#include <xpcc/architecture/driver/time/clock.hpp>

#include "image/startscreen.hpp"

#include "nibo.hpp"

// ----------------------------------------------------------------------------
nibo::Display Nibo::display;
//xpcc::BufferedUart1 Nibo::uart(38400);
xpcc::BufferedUart1 Nibo::uart(19200);

xpcc::PeriodicTimer<> Nibo::displayTimer(500);

// ----------------------------------------------------------------------------
// used as a timebase, interrupt every 1ms
ISR(TIMER1_OVF_vect)
{
	xpcc::Clock::increment();
}

// ----------------------------------------------------------------------------
void
Nibo::initialize()
{
	// Configure LEDs
	PORTC = 0;
	DDRC = 0xff;
	
	PORTE = 0;
	DDRE = 0xff;
	
	PORTB &= ~((1 << PB7) | (1 << PB6) | (1 << PB5));
	DDRB |= (1 << PB7) | (1 << PB6) | (1 << PB5);
	
	setStatusLedIntensity(70);
	setHeadlights(0);
	setDisplayBacklight(0);
	
	// Clear pins on compare match (non inverted PWM)
	// Phase Correct PWM (ICR1 = TOP), Prescaler = 8
	// 
	// f_cpu = 16000000
	// N = 8
	// TOP = 1000
	// 
	// f_pwm = f_cpu / (2 * N * TOP)
	//       = 16000000 / (2 * 8 * 1000) = 1000 Hz
	ICR1 = 1000;
	TCNT1 = 0;
	TIMSK |= (1 << TOIE1);
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1) | (1 << WGM11);
	TCCR1B = (1 << CS11) | (1 << WGM13);
	
	// TODO Configure SPI
	/*nibo::Miso::setInput();
	nibo::Mosi::setOutput();
	nibo::Sck::reset();
	nibo::Sck::setOutput();
	nibo::SlaveSelect::set();
	nibo::SlaveSelect::setOutput();*/
	nibo::SlaveReset::set();
	nibo::SlaveReset::setOutput();
	
	// TODO SPI Interrupt etc.
	
	// The slave and LCD reset lines are combined.
	// Pulling down the SlaveReset line with SlaveSelect high will reset only
	// the co-processor but not the LCD. Pulling both lines down will reset
	// both. This needs to be done before the LCD initialization.
	nibo::SlaveReset::reset();
	nibo::SlaveSelect::setOutput();
	nibo::SlaveSelect::reset();
	xpcc::delay_ms(100);
	
	nibo::SlaveSelect::set();
	nibo::SlaveSelect::setInput();
	nibo::SlaveReset::set();
	xpcc::delay_ms(50);
	
	display.initialize();
	display.drawImage(xpcc::glcd::Point(0, 0),
			xpcc::accessor::asFlash(bitmap::startscreen));
	
	display.update();
	
	// Enable interrupts
	sei();
}

// ----------------------------------------------------------------------------
void
Nibo::update()
{
	if (displayTimer.isExpired()) {
		display.update();
		uart.write("update\n");
	}
}

// ----------------------------------------------------------------------------
void
Nibo::setStatusLed(Led led, Color color)
{
	// green
	if (color & 0x01) {
		PORTC |= led;
	}
	else {
		PORTC &= ~led;
	}
	
	// red
	if (color & 0x02) {
		PORTE |= led;
	}
	else {
		PORTE &= ~led;
	}
}

// ----------------------------------------------------------------------------
void
Nibo::setStatusLedIntensity(uint8_t intensity)
{
	OCR1C = calculateCompareValue(intensity);
}

// ----------------------------------------------------------------------------
void
Nibo::setHeadlights(uint8_t intensity)
{
	OCR1A = calculateCompareValue(intensity);
}

// ----------------------------------------------------------------------------
void
Nibo::setDisplayBacklight(uint8_t intensity)
{
	OCR1B = calculateCompareValue(intensity);
}

// ----------------------------------------------------------------------------
uint16_t
Nibo::calculateCompareValue(uint8_t intensity)
{
	uint16_t ocr = (intensity * intensity) / 10;
	if (ocr > 1000) {
		ocr = 1000;
	}
	return ocr;
}
