
// avrdude -px128a1 -cavrisp2 -Pusb -Uflash:w:build-tests/blink_test.hex

#include <avr/io.h>
#include <util/delay.h>

#ifdef __AVR_ATxmega128A1__

void
configureInternalOscillator()
{
	// enable internal 32MHz oscillator
	OSC.CTRL = OSC_RC32MEN_bm;
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)) {
		// wait for stability
	}
	
	// secured access & choose this osc source as clk
	CCP = CCP_IOREG_gc;
	CLK.CTRL = 0x01;
}

void
configureExternalOscillator()
{
	// frequency range 12-16 MHz
	OSC.XOSCCTRL = OSC_FRQRANGE0_bm | OSC_FRQRANGE1_bm | \
					OSC_XOSCSEL0_bm | OSC_XOSCSEL1_bm;
	
	// XOSC is PLL Source - 2x Factor (32MHz)
	OSC.PLLCTRL = OSC_PLLSRC0_bm | OSC_PLLSRC1_bm | 2;
	
	// Enable PLL & External Oscillator 
	OSC.CTRL = OSC_XOSCEN_bm | OSC_PLLEN_bm;
	
	while (!(OSC_STATUS & OSC_PLLRDY_bm)) {
		// wait for stability
	}
	
	// select PLL as clock source
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL2_bm;
}

void
configureUart()
{
	// PC2 (RXD0) as input, PC3 (TXD0) as output
	PORTC.DIRSET = (1 << 3);
	PORTC.DIRCLR = (1 << 2);
	
	// USARTC0, 8 Data bits, No Parity, 1 Stop bit.
	USARTC0.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc;
	
	// Set Baudrate to 9600 bps
	USARTC0.BAUDCTRLA = 207;
	USARTC0.BAUDCTRLB = (0 << USART_BSCALE0_bp) | (207 >> 8);
	
	// Enable TX
	USARTC0.CTRLB |= USART_TXEN_bm;
}

int
main(void)
{
	PORTD.DIR = (1 << 0) | (1 << 1);
	PORTD.OUT = (1 << 0);
	
	configureExternalOscillator();
	
	configureUart();
	
	// Send data from 255 down to 0
	uint8_t sendData = 255;
	
	while (sendData)
	{
	    // Send one char.
		do{
			/* Wait until it is possible to put data into TX data register.
			 * NOTE: If TXDataRegister never becomes empty this will be a DEADLOCK. */
		}
		while ((USARTC0.STATUS & USART_DREIF_bm) == 0);
		
		USARTC0.DATA = sendData;
		sendData--;
		
		//_delay_ms(10);
		//PORTD.OUTTGL = (1 << 0) | (1 << 1);
	}
	
	while (1) {
		_delay_ms(300);
		PORTD.OUTTGL = (1 << 0) | (1 << 1);
	}
}

#else

int
main(void)
{
	
	
	while (1) {
		
	}
}

#endif
