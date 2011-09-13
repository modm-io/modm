
#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/i2c/software_i2c.hpp>

#include <xpcc/io/iostream.hpp>

GPIO__IO(Scl, C, 0);
GPIO__IO(Sda, C, 1);

// Create a new UART object and configure it to a baudrate of 9600
xpcc::atmega::BufferedUart0 uart(9600);

xpcc::SoftwareI2C<Scl, Sda> i2c;

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::atmega::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	// Initialize the I2C interface.
	i2c.initialize();
	
	// Scan the I2C Bus for all available devices
	output << "Scan for connected I2C devices ..." << xpcc::endl;
	for (uint8_t i = 0; i < 252; i += 2)
	{
		if (i2c.start(i | xpcc::i2c::READ)) {
			output << "- found device " << xpcc::hex << i << xpcc::endl;
		}
		i2c.stop();
		
		xpcc::delay_us(5);
	}
	output << "finished" << xpcc::endl;
	
	while (1)
	{
	}
}

