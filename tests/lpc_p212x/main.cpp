
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedButton1, 0, 12);
GPIO__OUTPUT(LedButton2, 0, 13);

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led0, 1, 16);
GPIO__OUTPUT(Led1, 1, 17);
GPIO__OUTPUT(Led2, 1, 18);
GPIO__OUTPUT(Led3, 1, 19);
GPIO__OUTPUT(Led4, 1, 20);
GPIO__OUTPUT(Led5, 1, 21);
GPIO__OUTPUT(Led6, 1, 22);
GPIO__OUTPUT(Led7, 1, 23);

typedef xpcc::gpio::Port<Led7, Led6, Led5, Led4, Led3, Led2, Led1, Led0> Leds;

// ----------------------------------------------------------------------------
// Graphic LCD
/*namespace lcd
{
	GPIO__OUTPUT(Sck, 0, 4);
	GPIO__INPUT(Miso, 0, 5);
	GPIO__OUTPUT(Mosi, 0, 6);
	
	GPIO__OUTPUT(CS, 1, 25);
	GPIO__OUTPUT(A0, 1, 24);
	GPIO__OUTPUT(Reset, 0, 11);
}

typedef xpcc::SoftwareSpi< lcd::Sck, lcd::Mosi, lcd::Miso, 1000000UL > SPI;
xpcc::DogS102< SPI, lcd::CS, lcd::A0, lcd::Reset, false > display;

// GPIO Expander (uses the same SPI as the LCD)
GPIO__OUTPUT(CsGpio, 0, 7);
GPIO__INPUT(IntGpio, 0, 15);

xpcc::Mcp23s08< SPI, CsGpio, IntGpio > gpio;*/

// ----------------------------------------------------------------------------
/*void
pitHandler(void)
{
	xpcc::Clock::increment();
	xpcc::at91::Pit::acknowledgeInterrupt();
	
	uint32_t status = 0;
	if (Button1::read()) {
		status |= BUTTON1;
	}
	if (Button2::read()) {
		status |= BUTTON2;
	}
	button.update(~status);
}*/

/*#define	LCD_BL_RED		0x01
#define	LCD_BL_GREEN	0x02

#define	BUTTON_LEFT		0x04
#define	BUTTON_DOWN		0x08
#define	BUTTON_UP		0x10
#define	BUTTON_OK		0x20
#define	BUTTON_RIGHT	0x40

#define	LED_GREEN		0x80*/

// ----------------------------------------------------------------------------
int
main(void)
{
	//xpcc::at91::Pio::enableClock();
	
	/*Led0::setOutput(xpcc::gpio::LOW);
	Led1::setOutput(xpcc::gpio::LOW);
	Led2::setOutput(xpcc::gpio::LOW);
	Led3::setOutput(xpcc::gpio::LOW);
	Led4::setOutput(xpcc::gpio::LOW);
	Led5::setOutput(xpcc::gpio::LOW);
	Led6::setOutput(xpcc::gpio::LOW);
	Led7::setOutput(xpcc::gpio::LOW);*/
	
	//LedButton1::setOutput(xpcc::gpio::HIGH);
	//LedButton2::setOutput(xpcc::gpio::LOW);
	
	LedButton1::setOutput();
	LedButton2::setOutput();
	
	LedButton1::toggle();
	LedButton2::set();
	
	/*display.initialize();
	
	display.setFont(xpcc::font::FixedWidth5x8);
	display.setCursor(0, 0);
	display << "SAM7-P256 Board";
	display.update();
	
	// IO-Expander have to initalized after LCD because they use the same
	// Reset line
	gpio.initialize();
	gpio.configure(0x7c, 0x7c);
	gpio.write(0x83);
	
	xpcc::at91::Pit::start();
	xpcc::at91::Pit::registerInterruptHandler(pitHandler);*/
	
	//XPCC_LOG_DEBUG << "SAM7-P256 Board (extended)" << xpcc::endl;
	
	/*uint32_t *ptr = 0; 
	for (uint32_t i = 0; i < 128; ++i)
	{
		if (i % 8 == 0) {
			XPCC_LOG_DEBUG.printf("\n0x%08lx :", (uint32_t) ptr);
		}
		XPCC_LOG_DEBUG.printf(" %08lx", *ptr++);
	}*/
	
	//LedButton1::reset();
	//LedButton2::reset();
	
	uint32_t pattern = 0x01;
	bool up = true;
	//xpcc::PeriodicTimer<> timer(100);
	while (1)
	{
		/*if (button.isPressed(BUTTON1)) {
			LedButton1::toggle();
		}
		if (button.isPressed(BUTTON2)) {
			LedButton2::toggle();
		}*/
		
		//if (timer.isExpired())
		{
			if (up) {
				pattern <<= 1;
				if (pattern == 0x80) {
					up = false;
				}
			}
			else {
				pattern >>= 1;
				if (pattern == 0x01) {
					up = true;
				}
			}
			//Leds::write(pattern);
			
			
			/*display.setCursor(0, 10);
			display << xpcc::hex << gpio.read();
			display.update();*/
		}
	}
}
