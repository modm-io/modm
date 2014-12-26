
#include <xpcc/architecture.hpp>
#include <xpcc/driver/temperature/ds18b20.hpp>

#include <xpcc/io/iostream.hpp>

using namespace xpcc::atmega;

typedef GpioC2 OneWirePin;
xpcc::SoftwareOneWireMaster<OneWirePin> ow;

int
main()
{
	typedef xpcc::avr::SystemClock c;
	Uart0::initialize<c, 9600>();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > device;
	xpcc::IOStream output(device);

	output << "Welcome" << xpcc::endl;
	xpcc::delayMilliseconds(100);

	ow.initialize();

	if (!ow.touchReset()) {
		output << "No devices found!" << xpcc::endl;
		xpcc::delayMilliseconds(100);
		while (1) {
			// wait forever
		}
	}

	// search for connected DS18B20 devices
	ow.resetSearch(0x28);

	uint8_t rom[8];
	while (ow.searchNext(rom)) {
		output << "found: " << xpcc::hex;
		for (uint8_t i = 0; i < 8; ++i) {
			output << rom[i];
		}
		output << xpcc::ascii << xpcc::endl;
		xpcc::delayMilliseconds(100);
	}
	output << "finished!" << xpcc::endl;

	// read the temperature from a connected DS18B20
	xpcc::Ds18b20< xpcc::SoftwareOneWireMaster<OneWirePin> > ds18b20(rom);

	ds18b20.startConversion();

	while (1)
	{
		if (ds18b20.isConversionDone())
		{
			int16_t temperature = ds18b20.readTemperature();

			output << "Temperature: " << temperature << xpcc::endl;
			xpcc::delayMilliseconds(100);

			ds18b20.startConversion();
		}
	}
}

