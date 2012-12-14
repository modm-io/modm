
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

extern xpcc::lpc::Uart1 loggerUart;
xpcc::IODeviceWrapper<xpcc::lpc::Uart1> loggerDevice(loggerUart);

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------

namespace led
{
	GPIO__OUTPUT(Xpresso,  0, 7);
	GPIO__OUTPUT(Onboard,  3, 3);
	GPIO__OUTPUT(DuoGreen, 3, 2);
	GPIO__OUTPUT(DuoRed,   3, 1);
}

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
	SystemInit();

	xpcc::lpc11::SysTickTimer::enable();

	xpcc::lpc::Uart1 loggerUart(115200);

	// Configure CAN device and set the filters
	device.initialize(xpcc::can::BITRATE_125_KBPS);

	// FIFO of 4 MOBs (0 to 3) for extended messages with XPCC destination RECEIVER

	xpcc::lpc11c::CanFilter::setFilter(
			xpcc::lpc11c::CanFilter::ExtendedIdentifier(
					XPCC_CAN_PACKET_DESTINATION(robot::component::RECEIVER),
					xpcc::lpc11c::CanFilter::NO_RTR),
			xpcc::lpc11c::CanFilter::ExtendedFilterMask(
					XPCC_CAN_PACKET_DESTINATION_MASK,
					xpcc::lpc11c::CanFilter::RTR_MATCH),
			0, 4);

	// FIFO of 8 MOBs (4 to 11) for extended messages for XPCC events.
	xpcc::lpc11c::CanFilter::setFilter(
			xpcc::lpc11c::CanFilter::ExtendedIdentifier(
					XPCC_CAN_PACKET_EVENT,
					xpcc::lpc11c::CanFilter::NO_RTR),
			xpcc::lpc11c::CanFilter::ExtendedFilterMask(
					XPCC_CAN_PACKET_DESTINATION_MASK,
					xpcc::lpc11c::CanFilter::RTR_MATCH),
			4, 8);

	
	component::receiver.initialize();

	XPCC_LOG_INFO << "Welcome to the communication test at Receiver!" << xpcc::endl;
	
	led::DuoRed::setOutput();
	led::DuoGreen::setOutput();

	while (1)
	{
		led::DuoRed::set(CanDevice::isMessageAvailable());
		led::DuoGreen::set(CanDevice::isReadyToSend());

		// deliver received messages
		dispatcher.update();
		
		component::receiver.update();
		
		xpcc::delay_us(100);
	}
}
