
#include <xpcc/architecture.hpp>
#include <xpcc/driver/software_i2c.hpp>
#include <xpcc/io/iostream.hpp>

GPIO__IO(Scl, C, 0);
GPIO__IO(Sda, C, 1);

// Create a new UART object and configure it to a baudrate of 9600
xpcc::BufferedUart0 uart(9600);

xpcc::SoftwareI2C<Scl, Sda> i2c;

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "Welcome!" << xpcc::endl;
	
	// Initialize the I2C interface.
	i2c.initialize();
	
	while (1)
	{
		uint16_t data = 0;
		
		i2c.start(0x14 | xpcc::i2c::WRITE);
		i2c.write(0x00);
		
		i2c.repeatedStart(0x14 | xpcc::i2c::READ);
		data += i2c.read(xpcc::i2c::ACK);
		data += i2c.read(xpcc::i2c::ACK);
		data += i2c.read(xpcc::i2c::NACK);
		i2c.stop();
		
		i2c.start(0x16 | xpcc::i2c::WRITE);
		i2c.write(0x00);
		i2c.write(0x03);
		i2c.write(0x61);
		i2c.stop();
	}
}
