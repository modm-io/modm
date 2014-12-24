#include <xpcc/architecture.hpp>
#include <xpcc/processing.hpp>
#include <xpcc/debug/logger.hpp>
#include "../stm32f3_discovery.hpp"

/**
 * Example of CAN Hardware on STM32 F3 Discovery Board.
 *
 * Connect PB8 / PB9 to a CAN transceiver which is connected to a CAN bus.
 *
 * Tested in hardware on 2013-12-17 by strongly-typed.
 */

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IODevice::BufferBehavior::BusyWait > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

static void
displayMessage(const xpcc::can::Message& message)
{
	static uint32_t receiveCounter = 0;
	receiveCounter++;

	XPCC_LOG_INFO<< "id  =" << message.getIdentifier();
	if (message.isExtended()) {
		XPCC_LOG_INFO<< " extended";
	}
	else {
		XPCC_LOG_INFO<< " standard";
	}
	if (message.isRemoteTransmitRequest()) {
		XPCC_LOG_INFO<< ", rtr";
	}
	XPCC_LOG_INFO<< xpcc::endl;

	XPCC_LOG_INFO<< "dlc =" << message.getLength() << xpcc::endl;
	if (!message.isRemoteTransmitRequest())
	{
		XPCC_LOG_INFO << "data=";
		for (uint32_t i = 0; i < message.getLength(); ++i) {
			XPCC_LOG_INFO<< xpcc::hex << message.data[i] << xpcc::ascii << ' ';
		}
		XPCC_LOG_INFO<< xpcc::endl;
	}
	XPCC_LOG_INFO<< "# received=" << receiveCounter << xpcc::endl;
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	xpcc::cortex::SysTickTimer::enable();

	LedNorth::setOutput(xpcc::Gpio::High);

	// Initialize Usart
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<defaultSystemClock, 115200>(10);

	XPCC_LOG_INFO << "CAN Test Program" << xpcc::endl;

	XPCC_LOG_INFO << "Initializing Can ..." << xpcc::endl;
	// Initialize Can
	GpioInputB8::connect(Can1::Rx, Gpio::InputType::PullUp);
	GpioOutputB9::connect(Can1::Tx, Gpio::OutputType::PushPull);
	Can1::initialize<defaultSystemClock, Can1::Bitrate::kBps125>(9);

	XPCC_LOG_INFO << "Setting up Filter for Can ..." << xpcc::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	// Send a message
	XPCC_LOG_INFO << "Sending message on Can ..." << xpcc::endl;
	xpcc::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can1::sendMessage(msg1);

	xpcc::PeriodicTimer<> pTimer(100);

	while (1)
	{
		if (Can1::isMessageAvailable())
		{
			XPCC_LOG_INFO << "Can: Message is available..." << xpcc::endl;
			xpcc::can::Message message;
			Can1::getMessage(message);
			displayMessage(message);
		}

		if (pTimer.isExpired()) {
			LedNorth::toggle();

			static uint8_t idx = 0;
			xpcc::can::Message msg1(1, 1);
			msg1.setExtended(true);
			msg1.data[0] = idx;
			Can1::sendMessage(msg1);

			++idx;
		}
	}

	return 0;
}
