
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

int
main(void)
{
	PORTD.DIR = (1 << 0) | (1 << 1);
	PORTD.OUT = (1 << 0) | (1 << 1);
	
	configureExternalOscillator();
	
	while (1) {
		_delay_ms(500);
		
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
