
#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/i2c/software_i2c.hpp>
#include <xpcc/architecture/platform/avr/atmega/twi_master.hpp>
#include <xpcc/driver/storage/i2c_eeprom.hpp>

#include <xpcc/io/iostream.hpp>

GPIO__IO(Scl, C, 0);
GPIO__IO(Sda, C, 1);

// Create a new UART object and configure it to a baudrate of 9600
xpcc::atmega::BufferedUart0 uart(9600);

//#define USE_SOFTWARE
#define USE_HARDWARE

#if defined USE_SOFTWARE
typedef xpcc::SoftwareI2C<Scl, Sda> I2C;
#endif

#if defined USE_HARDWARE
typedef xpcc::atmega::AsynchronousTwiMaster I2C;
#endif

void
die()
{
	while (1) {
		// wait forever
	}
}

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::atmega::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "I2C eeprom test" << xpcc::endl;
	
#if defined USE_SOFTWARE
	// Initialize the I2C interface.
	I2C::initialize();
#endif

#if defined USE_HARDWARE
	I2C::initialize(65, 0);
#endif

	// Create a wrapper object for an 24C256 (32K I2C eeprom) connected
	// at address 0xA0
	xpcc::I2cEeprom< I2C > eeprom(0xA0);

	// Check if we can access the Eeprom
	if (!eeprom.isAvailable())
	{
		output << "Could not connect to the 24C256" << xpcc::endl;
		die();
	}

	// write a small pattern in the first eight bytes if it isn't
	// already present
	output << "Write pattern ... ";
	uint8_t key = 0x0;
	uint8_t data = key+1;
	if (eeprom.readByte(0, data))
	{
		if (data != key)
		{
			uint8_t array[8] = { key, 1, 2, 3, 4, 5, 6, 7 };
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
				output << xpcc::ascii << xpcc::endl << xpcc::hex;
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
