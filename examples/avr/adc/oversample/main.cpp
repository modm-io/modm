
#include <xpcc/architecture.hpp>
#include <xpcc/processing.hpp>

using namespace xpcc::atmega;

// Create a new UART object
typedef xpcc::avr::SystemClock clock;

#include <xpcc/io/iostream.hpp>
// Create a IOStream for complex formatting tasks
xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > device;
xpcc::IOStream output(device);


// the three sensors are mapped: x = ch1, y = ch2, z = ch0
Adc::Channel sensorMapping[3] = {1,2,0};
// 3 channels and averages of 2^5 bits = 32 oversamples
#include <xpcc/driver/adc/adc_sampler.hpp>
typedef xpcc::AdcSampler< AdcInterrupt, 3, 5 > sensors;
// the results are up to 16 bit wide
sensors::DataType sensorData[3];

xpcc::Timeout<> timeout(100);

int
main()
{
    GpioOutputD1::connect(Uart0::Tx);
    GpioInputD0::connect(Uart0::Rx);
	Uart0::initialize<clock, 115200>();

	// Initialize the analog to digital converter
	// With the AVR running at 14.7456Mhz and a prescaler of 128 the
	// ADC is running at 115kHz.
	Adc::initialize<clock, 115000>();
	Adc::setReference(Adc::Reference::InternalVcc);
	Adc::enableInterrupt();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	output << "Welcome!" << xpcc::endl;

	sensors::initialize(sensorMapping, sensorData);
	sensors::startReadout();

	while (1)
	{
		if (timeout.isExpired() && sensors::isReadoutFinished())
		{
			uint16_t* data = sensors::getData();
			// send it via UART
			output <<"x="  << data[0] << xpcc::endl;
			output <<"y="  << data[1] << xpcc::endl;
			output <<"z="  << data[2] << xpcc::endl;

			// start another readout
			sensors::startReadout();
			timeout.restart(200);
		}
	}
}
