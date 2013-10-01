
#include <xpcc/architecture.hpp>
#include <xpcc/io/iostream.hpp>
#include <xpcc/processing.hpp>

using namespace xpcc::atmega;

// Create a new UART object and configure it to a baudrate of 9600
Uart0 uart(9600);

// the three sensors are mapped: x = ch1, y = ch2, z = ch0
uint8_t sensorMapping[3] = {1,2,0};
// 3 channels and averages of 2^5 bits = 32 oversamples
typedef AnalogSensors< 3,5 > sensor;
// the results are up to 16 bit wide
uint16_t sensorData[3];
xpcc::PeriodicTimer<> sensorTimer(100);

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<Uart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "Welcome!" << xpcc::endl;
	
	// Initialize the analog to digital converter
	// With the AVR running at 14.7456Mhz and a prescaler of 128 the
	// ADC is running at 115kHz.
	Adc::initialize(Adc::Reference::InternalAVcc, Adc::Prescaler::Div128);
	
	sensor::initialize(sensorMapping, sensorData);
	
	while (1)
	{
		if (sensorTimer.isExpired())
		{
			sensor::readSensors();
		}

		if (sensor::isNewDataAvailable())
		{
			uint16_t* data = sensor::readData();
			// send it via UART
			output <<"data[0]="  << data[0] << xpcc::endl;
		}
	}
}
