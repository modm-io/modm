
#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/temperature/ds1631.hpp>
#include <xpcc/io/iostream.hpp>

using namespace xpcc::atmega;

typedef GpioOpenDrain< I2c::Scl > Scl;
typedef GpioOpenDrain< I2c::Sda > Sda;

// Create a new UART object and configure it to a baudrate of 9600
Uart0 uart(9600);

//#define USE_SOFTWARE
#define USE_HARDWARE

#if defined USE_SOFTWARE
typedef xpcc::SoftwareI2cMaster<Scl, Sda> Twi;
#elif defined USE_HARDWARE
typedef I2cMaster Twi;
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
	enableInterrupts();

	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<Uart0> device(uart);
	xpcc::IOStream output(device);

	output << "Thermometer" << xpcc::endl;

	// Initialize the I2C interface.
	Twi::initialize<xpcc::I2cMaster::DataRate::Standard>();

	uint8_t data[2];
	xpcc::Ds1631< Twi > ds1631(data, 0x90);

	// Enable the 12 bit resolution
	ds1631.configure(xpcc::ds1631::RESOLUTION_12BIT, false);

	xpcc::delayMilliseconds(1);

	// start a new conversion
	ds1631.startConversion();

	while (1)
	{
		if (ds1631.isConversionDone())
			ds1631.readTemperature();

		if (ds1631.isNewDataAvailable())
		{
			int16_t temperature = ds1631.getTemperature();
			// print the integer part
			output << (temperature >> 8) << ".";

			// print the fractional part
			uint16_t low  = temperature & 0xff;
			for (uint8_t i = 0; i < 4; ++i)
			{
				low *= 10;
				uint8_t c = low >> 8;
				low &= 0x00ff;
				output << c;
			}
			output << xpcc::endl;

			// start a new conversion
			ds1631.startConversion();
		}
	}
}
