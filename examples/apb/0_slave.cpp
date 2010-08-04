
#include <avr/interrupt.h>

#include <xpcc/architecture/atmega/uart/uart0.hpp>
#include <xpcc/driver/apb/slave.hpp>

typedef xpcc::apb::Slave< xpcc::apb::Interface <xpcc::BufferedUart0> > Slave;

class Sensor : public xpcc::apb::Callable
{
public:
	void
	sendValue(xpcc::apb::Response& response)
	{
		response.send(this->value);
	}
	
	void
	doSomething (xpcc::apb::Response& response, const uint32_t* parameter)
	{
		if (*parameter > 1000) {
			response.error();
		}
		else {
			// do something usefull ...
			response.send();
		}
	}
	
	int8_t value;
};

Sensor sensor;

// ----------------------------------------------------------------------------
FLASH(xpcc::apb::Action actionList[]) =
{
	APB__ACTION('S',  sensor, Sensor::sendValue,	0),
	APB__ACTION(0x03, sensor, Sensor::doSomething,	sizeof(uint32_t)),
};

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// initialize ABP interface
	Slave slave(0x02,
			xpcc::accessor::asFlash(actionList),
			sizeof(actionList) / sizeof(xpcc::apb::Action));
	
	// enable interrupts
	sei();
	
	sensor.value = 10;
	
	while (1)
	{
		slave.update();
	}
}
