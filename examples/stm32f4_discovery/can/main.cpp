#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#include "../stm32f4_discovery.hpp"

using namespace xpcc::stm32;

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2 > loggerDevice;

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
			if (i == 4) {
				// wrap around to the next line
				XPCC_LOG_INFO<< xpcc::endl;
			}
			XPCC_LOG_INFO<< xpcc::hex << message.data[i] << xpcc::ascii << ' ';
		}
		XPCC_LOG_INFO<< xpcc::endl;
	}
	XPCC_LOG_INFO<< "# received=" << receiveCounter << xpcc::endl;
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	SystemClock<Pll<ExternalOscillator<MHz8>, MHz168, MHz48> >::enable();

	// Initialize Usart
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx);
	Usart2::initialize(115200, 10);

	// Initialize CAN
	GpioInputB8::connect(Can1::Rx);
	GpioOutputB9::connect(Can1::Tx);
	Can1::initialize(xpcc::can::BITRATE_125_KBPS, 9);

	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	// Send a message
	XPCC_LOG_INFO << "Sending message..." << xpcc::endl;
	xpcc::can::Message msg1(1, 0);
	msg1.setExtended(true);
	Can1::sendMessage(msg1);

	while (1)
	{
		if (Can1::isMessageAvailable())
		{
			XPCC_LOG_INFO << "Message is available..." << xpcc::endl;
			xpcc::can::Message message;
			Can1::getMessage(message);
			displayMessage(message);
		}
	}

	return 0;
}
