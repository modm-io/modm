
#include <xpcc/architecture.hpp>
#include <xpcc/driver/i2c/software_i2c.hpp>
#include <xpcc/driver/i2c/eeprom.hpp>

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
	
	output << "I2C eeprom test" << xpcc::endl;
	
	// Initialize the I2C interface.
	I2C::initialize();
	
	// Create a wrapper object for an 24C256 (32K I2C eeprom) connected
	// at address 0xA0
	xpcc::i2c::Eeprom< I2C > eeprom(0xA0);
	
	// Check if we can access the Eeprom
	if (!eeprom.isAvailable())
	{
		output << "Could not connect to the 24C256" << xpcc::endl;
		die();
	}
	
	// write a small pattern in the first eight bytes if it isn't
	// already present
	output << "Write pattern ... ";
	uint8_t data = 0xff;
	if (eeprom.readByte(0, data))
	{
		if (data != 0x00)
		{
			uint8_t array[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
			if (eeprom.write(0, array, 8))
			{
				while (!eeprom.isAvailable()) {
					// wait until the write operation is finished
				}
				output << "finished" << xpcc::endl;
			}
			else {
				output << "Error while writing!" << xpcc::endl;
				die();
			}
		}
		else {
			output << "found pattern" << xpcc::endl;
		}
	}
	else
	{
		output << "Error while reading!" << xpcc::endl;
		die();
	}
	
	// Read and display the first 128 bytes of the eeprom
	output << "Read block:" << xpcc::endl;
	uint8_t array[128];
	if (eeprom.read(0, array, 128))
	{
		output << xpcc::hex;
		for (uint8_t i = 0; i < 128; ++i)
		{
			output << array[i];
			
			if ((i & 0x07) == 0x07) {
				// insert a linebreak after eight bytes
				output << xpcc::flush << xpcc::endl << xpcc::hex;
			}
		}
	}
	else {
		output << "Error while reading!" << xpcc::endl;
		die();
	}
	
	while (1)
	{
	}
}
