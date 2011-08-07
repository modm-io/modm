/*
 * Example for a simple IO extension
 * 
 * This program enables a master to use the A/D converter and PORTD
 * from extern.
 */

#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/apb/slave.hpp>

// ----------------------------------------------------------------------------
// wrapper class for the A/D converter
class AnalogDigital : public xpcc::apb::Callable
{
public:
	AnalogDigital()
	{
		// initialize the analog to digital converter
		xpcc::atmega::Adc::initialize(
				xpcc::atmega::Adc::REFERENCE_INTERNAL_AVCC, xpcc::atmega::Adc::PRESCALER_64);
	}
    
	void
	readChannel(xpcc::apb::Response& response, const uint8_t *channel)
	{
		uint16_t value = xpcc::atmega::Adc::readChannel(*channel);
		response.send(value);
	}
};

// ----------------------------------------------------------------------------
// wrapper for PORTD
class InOut : public xpcc::apb::Callable
{
public:
	void
	setDirection(xpcc::apb::Response& response, const uint8_t *direction)
	{
		DDRD = *direction;
		
		// send a response without any attached data
		response.send();
	}
	
	void
	readInput(xpcc::apb::Response& response)
	{
		uint8_t value = PIND;
		response.send(value);
	}
	
	void
	setOutput(xpcc::apb::Response& response, const uint8_t *output)
	{
		PORTD = *output;
		response.send();
	}
};

// ----------------------------------------------------------------------------
// create instances from the wrapper classes
AnalogDigital analogDigital;
InOut inOut;

// ----------------------------------------------------------------------------
// create a list of all possible actions
FLASH_STORAGE(xpcc::apb::Action actionList[]) =
{
	APB__ACTION( 'A', analogDigital,	AnalogDigital::readChannel,	1 ),
	APB__ACTION( 'D', inOut,			InOut::setDirection,		1 ),
	APB__ACTION( 'I', inOut,			InOut::readInput,			0 ),
	APB__ACTION( 'O', inOut,			InOut::setOutput,			1 ),
};

// wrap the type definition inside a typedef to make the code more readable
typedef xpcc::apb::Slave< xpcc::apb::Interface< xpcc::atmega::BufferedUart0 > > Slave;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// initialize ABP interface, set baudrate etc.
	Slave slave(0x02,
			xpcc::accessor::asFlash(actionList),
			sizeof(actionList) / sizeof(xpcc::apb::Action));
	
	// enable interrupts
	sei();
	
	while (1)
	{
		// decode received messages etc.
		slave.update();
	}
}
