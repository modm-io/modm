/**
 * \brief	Minimal CAN example for LPC11C24 on LPCXpresso board.
 *
 * VCC of CAN (pin 20) MUST be supplied with 5.0V (4.5 ... 5.5V).
 * 3.3 V definitely does not work.
 *
 * LPC11C24 must be supplied with 3.3V. It does not work with 5.0V. 
 *
 * Tested in hardware 2017-10-26 by strongly-typed.
 */

#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

using Uart = Uart1;
xpcc::IODeviceWrapper< Uart, xpcc::IOBuffer::BlockIfFull > loggerDevice;

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

int
main()
{
	Board::initialize();

	Uart::initialize<Board::systemClock, 115200>();

	XPCC_LOG_DEBUG << "Hello to C_CAN demo." << xpcc::endl;

	xpcc::lpc::Can::initialize<Board::systemClock, Can::Bitrate::kBps125>();

	if (xpcc::lpc::Can::isReadyToSend()){
		XPCC_LOG_DEBUG << "Ready to send." << xpcc::endl;
	} else {
		XPCC_LOG_DEBUG << "NOT ready to send." << xpcc::endl;
	}

	XPCC_LOG_DEBUG << "Bus state is " << xpcc::lpc::Can::getBusState() << "." << xpcc::endl;

	xpcc::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;

	while (true)
	{
		// Success only acknowledge the successful transmission to the CAN controller.
		// It does not guarantee that the messages was placed on the bus.
		XPCC_LOG_DEBUG << (Can::sendMessage(msg1) ? "success" : "fail") << xpcc::endl;

		Board::LedRed::toggle();
		xpcc::delayMilliseconds(1000);
		++msg1.data[0];
	}
}
