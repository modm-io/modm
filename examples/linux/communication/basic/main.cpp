
#include <xpcc/architecture.hpp>

#include <xpcc/communication.hpp>
#include <xpcc/communication/xpcc/backend/tipc/tipc.hpp>

#include <xpcc/debug/logger.hpp>

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

#include "component_receiver/receiver.hpp"
#include "component_sender/sender.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

xpcc::TipcConnector connector;

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

namespace component
{
	Sender sender(robot::component::SENDER, &dispatcher);
	Receiver receiver(robot::component::RECEIVER, &dispatcher);
}

int
main(void)
{
	connector.addReceiverId(robot::component::SENDER);
	connector.addReceiverId(robot::component::RECEIVER);
	
	XPCC_LOG_INFO << "Welcome to the communication test!" << xpcc::endl; 
	
	while (1)
	{
		// deliver received messages
		dispatcher.update();
		
		component::receiver.update();
		component::sender.update();
		
		xpcc::delayMicroseconds(100);
	}
}
