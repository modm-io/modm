#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture/platform.hpp>

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

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
int
main()
{
	Board::initialize();

	// Initialize Usart
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(10);

	XPCC_LOG_INFO << "CAN Test Program" << xpcc::endl;

	XPCC_LOG_INFO << "Dividing filter bank..." << xpcc::endl;
	CanFilter::setStartFilterBankForCan2(14);

	XPCC_LOG_INFO << "Initializing Can1..." << xpcc::endl;

	// Initialize Can1
	// Pin remapping only works in groups, so the interface provides more
	// flexibility than the hardware. So check datasheet before using.
	if (false) {
		GpioInputA11::connect(Can1::Rx, Gpio::InputType::PullUp);
		GpioOutputA12::connect(Can1::Tx, Gpio::OutputType::PushPull);
	} else {
		GpioInputB8::connect(Can1::Rx, Gpio::InputType::PullUp);
		GpioOutputB9::connect(Can1::Tx, Gpio::OutputType::PushPull);
	}
	Can1::initialize<Board::systemClock, Can1::Bitrate::kBps125>(9);

	XPCC_LOG_INFO << "Setting up Filter for Can1..." << xpcc::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	CanFilter::setFilter(1, CanFilter::FIFO0,
			CanFilter::StandardIdentifier(0),
			CanFilter::StandardFilterMask(0));

	// Send a message
	XPCC_LOG_INFO << "Sending message on Can1... nn" << xpcc::endl;
	xpcc::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can1::sendMessage(msg1);

	while (true)
	{
		if (Can1::isMessageAvailable())
		{
			XPCC_LOG_INFO << "Can1: Message is available..." << xpcc::endl;
			xpcc::can::Message message;
			Can1::getMessage(message);
			displayMessage(message);
		}
	}

	return 0;
}
