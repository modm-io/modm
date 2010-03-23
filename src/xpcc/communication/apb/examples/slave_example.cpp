
#include <xpcc/architecture/atmega/uart/uart0.hpp>
#include <xpcc/communication/apb/slave.hpp>

typedef xpcc::apb::Slave< xpcc::apb::Interface <xpcc::BufferedUart0> > Communicator;

static int8_t sendBuffer[4] = {
	0xff, 0xff, 0xff, 0xff,
};

// ----------------------------------------------------------------------------
static void
sendSensorValue(Communicator& communicator, const uint8_t *)
{
	communicator.sendResponse(sendBuffer, 4);
}

// ----------------------------------------------------------------------------
FLASH(Communicator::Action actionList[]) =
{
	{ 'S', 0, sendSensorValue },
};

// ----------------------------------------------------------------------------
int
main(void)
{
	// initialize ABP interface
	Communicator communicator(
		0x02,
		xpcc::accessor::asFlash(actionList),
		sizeof(actionList) / sizeof(Communicator::Action)
	);
	
	while (1)
	{
		communicator.update();
	}
}
