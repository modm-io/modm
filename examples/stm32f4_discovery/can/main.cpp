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
			XPCC_LOG_INFO<< xpcc::hex << message.data[i] << xpcc::ascii << ' ';
		}
		XPCC_LOG_INFO<< xpcc::endl;
	}
	XPCC_LOG_INFO<< "# received=" << receiveCounter << xpcc::endl;
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	typedef SystemClock<Pll<ExternalOscillator<MHz8>, MHz168, MHz48> > systemClock;
	systemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::HIGH);

	// Initialize Usart
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx);
	Usart2::initialize<systemClock, 115200>(10);

	XPCC_LOG_INFO << "CAN Test Program" << xpcc::endl;

	XPCC_LOG_INFO << "Dividing filter bank..." << xpcc::endl;
	CanFilter::setStartFilterBankForCan2(14);

	XPCC_LOG_INFO << "Initializing Can1..." << xpcc::endl;
	// Initialize Can1
	GpioInputB8::configure(GpioInputB8::InputType::PullUp);
	GpioInputB8::connect(Can1::Rx);
	GpioOutputB9::configure(GpioOutputB9::OutputType::PushPull);
	GpioOutputB9::connect(Can1::Tx);
	Can1::initialize<systemClock, Can1::Bitrate::kBps125>(9);

	XPCC_LOG_INFO << "Setting up Filter for Can1..." << xpcc::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	XPCC_LOG_INFO << "Initializing Can2..." << xpcc::endl;
	// Initialize Can2
	GpioInputB5::configure(GpioInputB5::InputType::PullUp);
	GpioInputB5::connect(Can2::Rx);
	GpioOutputB6::configure(GpioOutputB6::OutputType::PushPull);
	GpioOutputB6::connect(Can2::Tx);
	Can2::initialize<systemClock, Can2::Bitrate::kBps125>(12);

	XPCC_LOG_INFO << "Setting up Filter for Can2..." << xpcc::endl;
	// Receive every message
	CanFilter::setFilter(14, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	// Send a message
	XPCC_LOG_INFO << "Sending message on Can1..." << xpcc::endl;
	xpcc::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can1::sendMessage(msg1);

	// Send a message
	XPCC_LOG_INFO << "Sending message on Can2..." << xpcc::endl;
	msg1.data[0] = 0x22;
	Can2::sendMessage(msg1);


	while (1)
	{
		if (Can1::isMessageAvailable())
		{
			XPCC_LOG_INFO << "Can1: Message is available..." << xpcc::endl;
			xpcc::can::Message message;
			Can1::getMessage(message);
			displayMessage(message);
		}
		if (Can2::isMessageAvailable())
		{
			XPCC_LOG_INFO << "Can2: Message is available..." << xpcc::endl;
			xpcc::can::Message message;
			Can2::getMessage(message);
			displayMessage(message);
		}
	}

	return 0;
}
