
#include <xpcc/architecture.hpp>
#include <xpcc/driver/i2c/software_i2c.hpp>
#include <xpcc/driver/i2c/ds1631.hpp>

#include <xpcc/io/iostream.hpp>

GPIO__IO(Scl, C, 0);
GPIO__IO(Sda, C, 1);

// Create a new UART object and configure it to a baudrate of 9600
xpcc::BufferedUart0 uart(9600);

typedef xpcc::SoftwareI2C<Scl, Sda> I2C;

void
die()
{
	xpcc::delay_ms(100);
	abort();
}

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "Thermometer" << xpcc::endl;
	
	// Initialize the I2C interface.
	I2C::initialize();
	
	// Create a wrapper object for an 24C256 (32K I2C eeprom) connected
	// at address 0xA0
	xpcc::i2c::Ds1631< I2C > ds1631(0x90);
	
	// Check if we can access the DS1631
	if (!ds1631.isAvailable())
	{
		output << "Could not connect to the DS1631" << xpcc::endl;
		die();
	}
	
	// Enable the 12 bit resolution
	ds1631.configure(xpcc::i2c::RESOLUTION_12BIT, false);
	
	// start a new conversion
	ds1631.startConversion();
	
	while (1)
	{
		if (ds1631.isConversionDone())
		{
			uint16_t temperature = ds1631.readTemperature();
			
			uint8_t high = temperature >> 8;
			uint8_t low  = temperature & 0xff;
			
			output << xpcc::hex << high << low << xpcc::flush << xpcc::endl;
			
			ds1631.startConversion();
		}
		
		xpcc::delay_ms(100);
	}
}
