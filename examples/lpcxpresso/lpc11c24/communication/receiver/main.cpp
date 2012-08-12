
#include <xpcc/architecture.hpp>
#include <xpcc/communication.hpp>
#include <xpcc/communication/backend/can/can_connector.hpp>
#include <xpcc/debug/logger.hpp>

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

#include "component_receiver/receiver.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

// ----------------------------------------------------------------------------
// Logging

xpcc::lpc::Uart1 loggerUart(115200);
xpcc::IODeviceWrapper<xpcc::lpc::Uart1> loggerDevice(loggerUart);

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------
// CAN communication

typedef xpcc::lpc::Can CanDevice;

static CanDevice device;
static xpcc::CanConnector< CanDevice > connector(&device);

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

namespace component
{
	Receiver receiver(robot::component::RECEIVER, &dispatcher);
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// Configure CAN device and set the filters
	device.initialize(xpcc::can::BITRATE_125_KBPS);
//	device.setFilter(xpcc::accessor::asFlash(canFilter));
	
	XPCC_LOG_INFO << "Welcome to the communication test!" << xpcc::endl; 
	
	while (1)
	{
		// deliver received messages
		dispatcher.update();
		
		component::receiver.update();
		
		xpcc::delay_us(100);
	}
}
