
#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/i2c/software_i2c.hpp>
#include <xpcc/driver/connectivity/i2c/sync_master.hpp>

#include <xpcc/io/iostream.hpp>

GPIO__IO(Scl, C, 0);
GPIO__IO(Sda, C, 1);

// Create a new UART object and configure it to a baudrate of 9600
xpcc::atmega::BufferedUart0 uart(9600);

typedef xpcc::SoftwareI2C<Scl, Sda> RawI2c;
typedef xpcc::i2c::SyncMaster< RawI2c > I2c;

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::atmega::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	// Initialize the I2C interface.
	RawI2c::initialize();
	
	// Scan the I2C Bus for all available devices
	output << "Scan for connected I2C devices ..." << xpcc::endl;
	for (uint8_t i = 0; i < 252; i += 2)
	{
		if (I2c::startCheck(i)) {
			if (I2c::write(0, 0) != xpcc::i2c::BUS_RESET)
				output << "- found device " << xpcc::hex << i << xpcc::endl;
		}
		
		xpcc::delay_us(5);
	}
	output << "finished" << xpcc::endl;
	
	while (1)
	{
	}
}

