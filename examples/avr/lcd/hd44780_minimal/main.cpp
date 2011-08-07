
#include <xpcc/architecture.hpp>
#include <xpcc/driver/lcd.hpp>

#include "hd44780_minimal.hpp"

// define the pins used by the LCD
namespace lcd
{
	GPIO__OUTPUT(E, C, 6);
	GPIO__OUTPUT(Rw, C, 5);
	GPIO__OUTPUT(Rs, C, 4);
	
	GPIO__IO(Data0, A, 4);
	GPIO__IO(Data1, B, 1);
	GPIO__IO(Data2, C, 7);
	GPIO__IO(Data3, D, 2);
	
	typedef xpcc::gpio::Nibble<Data3, Data2, Data1, Data0> Data;
	
	//GPIO__NIBBLE_LOW(Data, C);
}

// create a LCD object
xpcc::Hd44780Minimal< lcd::E, lcd::Rw, lcd::Rs, lcd::Data > display(20, 4);

int
main()
{
	// The LCD needs at least 50ms after power-up until it can be
	// initialized.
	xpcc::delay_ms(50);
	
	display.initialize();
	display.setCursor(0, 0);
	
	// write the standard welcome message ;-)
	display.write("Hello World!\n");
	
	while (1)
	{
	}
}
