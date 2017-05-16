#include <modm/architecture/platform.hpp>
#include <modm/debug/logger.hpp>
#include <modm/architecture/platform.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

static void
displayMessage(const modm::can::Message& message)
{
	static uint32_t receiveCounter = 0;
	receiveCounter++;

	MODM_LOG_INFO<< "id  =" << message.getIdentifier();
	if (message.isExtended()) {
		MODM_LOG_INFO<< " extended";
	}
	else {
		MODM_LOG_INFO<< " standard";
	}
	if (message.isRemoteTransmitRequest()) {
		MODM_LOG_INFO<< ", rtr";
	}
	MODM_LOG_INFO<< modm::endl;

	MODM_LOG_INFO<< "dlc =" << message.getLength() << modm::endl;
	if (!message.isRemoteTransmitRequest())
	{
		MODM_LOG_INFO << "data=";
		for (uint32_t i = 0; i < message.getLength(); ++i) {
			MODM_LOG_INFO<< modm::hex << message.data[i] << modm::ascii << ' ';
		}
		MODM_LOG_INFO<< modm::endl;
	}
	MODM_LOG_INFO<< "# received=" << receiveCounter << modm::endl;
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

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Dividing filter bank..." << modm::endl;
	CanFilter::setStartFilterBankForCan2(14);

	MODM_LOG_INFO << "Initializing Can1..." << modm::endl;

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

	MODM_LOG_INFO << "Setting up Filter for Can1..." << modm::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	CanFilter::setFilter(1, CanFilter::FIFO0,
			CanFilter::StandardIdentifier(0),
			CanFilter::StandardFilterMask(0));

	// Send a message
	MODM_LOG_INFO << "Sending message on Can1... nn" << modm::endl;
	modm::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can1::sendMessage(msg1);

	while (true)
	{
		if (Can1::isMessageAvailable())
		{
			MODM_LOG_INFO << "Can1: Message is available..." << modm::endl;
			modm::can::Message message;
			Can1::getMessage(message);
			displayMessage(message);
		}
	}

	return 0;
}
