
#include <xpcc/architecture.hpp>

#include <xpcc/driver/ui/display.hpp>

// define the pins used by the LCD
namespace lcd
{
	GPIO__OUTPUT(Backlight, C, 7);
	GPIO__OUTPUT(E, C, 6);
	GPIO__OUTPUT(Rw, C, 5);
	GPIO__OUTPUT(Rs, C, 4);
	GPIO__NIBBLE_LOW(Data, C);
}

// create a LCD object
xpcc::Hd44780< lcd::E, lcd::Rw, lcd::Rs, lcd::Data > display(20, 4);

int
main()
{
	lcd::Backlight::setOutput();
	lcd::Backlight::reset();
	
	// The LCD needs at least 50ms after power-up until it can be
	// initialized. To make sure this is met we wait here
	xpcc::delay_ms(50);
	
	display.initialize();
	display.setCursor(0, 0);
	
	// write the standard welcome message ;-)
	display << "Hello World!\n";
	
	uint8_t counter = 0;
	while (1)
	{
		// Go to the beginning of the second line of the display and
		// write the value of 'counter'
		display.setCursor(1, 0);
		display << counter << "   ";
		
		counter++;
		
		xpcc::delay_ms(200);
	}
}
