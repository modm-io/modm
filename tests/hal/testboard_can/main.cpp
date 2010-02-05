// coding: utf-8
// ----------------------------------------------------------------------------

#include <avr/wdt.h>

#include <xpcc/hal/time/delay.hpp>
#include <xpcc/hal/peripheral/gpio.hpp>
#include <xpcc/hal/peripheral/avr/mega/spi/spi.hpp>

#include <xpcc/driver/can/mcp2515.hpp>
#include <xpcc/driver/debounce.hpp>

#include <xpcc/hal/time/internal_clock.hpp>
#include <xpcc/workflow/time/timeout.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc;

CREATE_OUTPUT_PIN(Cs, B, 2);
CREATE_INPUT_PIN(Int, B, 1);

FLASH(uint8_t filter[]) = 
{
	MCP2515_FILTER(100),	// Filter 0
	MCP2515_FILTER(100),	// Filter 1
	
	MCP2515_FILTER(100),	// Filter 2
	MCP2515_FILTER(100),	// Filter 3
	MCP2515_FILTER(100),	// Filter 4
	MCP2515_FILTER(100),	// Filter 5
	
	MCP2515_FILTER(0x7ff),	// Mask 0
	MCP2515_FILTER(0x7ff),	// Mask 1
};

Mcp2515 <Spi, Cs, Int> can;

Can::Message defaultMessage;
Debounce buttons;

// ----------------------------------------------------------------------------
uint8_t mcusr_mirror __attribute__ ((section (".noinit")));

void
get_mcusr(void)	__attribute__((naked)) \
				__attribute__((section(".init3")));

void
get_mcusr(void)
{
	mcusr_mirror = MCUSR;
	MCUSR = 0;
	wdt_disable();
}

// ----------------------------------------------------------------------------
int
main(void) __attribute__((OS_main));

int
main(void)
{
	
	// configure output pins
	DDRC = 0x3f;
	PORTC = 0;
	
	// create a small blink light
	uint8_t i = 1;
	while (i & 0x3f)
	{
		PORTC = i;
		i <<= 1;
		
		delay_ms(50);
	}
	
	DDRB = (1 << PB5) | (1 << PB3) | (1 << PB2);
	PORTB = (1 << PB4) | (1 << PB1);
	
	Spi::initialize(Spi::MODE_0, Spi::PRESCALER_4);
	
	Cs::output();
	Int::configure(gpio::PULLUP);
	
	// configure the CAN controller
	if (!can.initialize(Can::BITRATE_125_KBPS))
	{
		// software reset
		asm (
			"push r1"	"\n\t"
			"push r1"	"\n\t"
			"ret"		"\n"
		);
	}
	can.setFilter(xpcc::modifier::flash(filter));
	
	// configure input pins
	DDRD = 0;
	PORTD = 0xff;
	
	defaultMessage.identifier = 200;
	defaultMessage.length = 8;
	
	// inverse blink light
	i = 0x20;
	while (i & 0x3f)
	{
		PORTC = i;
		i >>= 1;
		
		delay_ms(50);
	}
	PORTC = 0;
	
	// configuration finished, enable watchdog
	wdt_enable(WDTO_120MS);
	
	Timeout<> timeout;
	while (1)
	{
		delay_ms(10);
		InternalClock::increment(10);
		
		buttons.update(PIND);
		
		if (timeout.isExpired())
		{
			defaultMessage.data[0] = buttons.getState(0xff);
			can.sendMessage(defaultMessage);
			
			timeout.restart(50);
		}
		
		// receive messages
		if (can.isMessageAvailable())
		{
			Can::Message message;
			
			if (can.getMessage(message) && message.length >= 1)
			{
				PORTC = (PORTC & ~0x3f) | (message.data[0] & 0x3f);
			}
		}
		
		wdt_reset(); 
	}
}
