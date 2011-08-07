
#include <xpcc/architecture.hpp>
#include <xpcc/driver/one_wire/software_one_wire.hpp>
#include <xpcc/driver/one_wire/ds18b20.hpp>

#include <xpcc/io/iostream.hpp>

GPIO__IO(OneWirePin, C, 2);
xpcc::SoftwareOneWire<OneWirePin> ow;

xpcc::atmega::BufferedUart0 uart(9600);

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::atmega::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "Welcome" << xpcc::endl;
	xpcc::delay_ms(100);
	
	ow.initialize();
	
	if (!ow.touchReset()) {
		output << "No devices found!" << xpcc::endl;
		xpcc::delay_ms(100);
		exit(1);
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
		xpcc::delay_ms(100);
	}
	output << "finished!" << xpcc::endl;
	
	// read the temperature from a connected DS18B20
	xpcc::DS18B20< xpcc::SoftwareOneWire<OneWirePin> > ds18b20(rom);
	
	ds18b20.startConversion();
	
	while (1)
	{
		if (ds18b20.isConversionDone())
		{
			int16_t temperature = ds18b20.readTemperature();
			
			output << "Temperature: " << temperature << xpcc::endl;
			xpcc::delay_ms(100);
			
			ds18b20.startConversion();
		}
	}
}
