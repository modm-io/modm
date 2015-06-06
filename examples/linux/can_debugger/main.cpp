
#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>

#include <xpcc/architecture/interface/can.hpp>
#include <xpcc/architecture/platform/driver/can/hosted/canusb.hpp>

/**
 * Simple example that listens to a CAN bus connected by a CAN2USB.
 *
 * How to use:
 * - Connect a CAN2USB to a CAN bus with traffic.
 * - Connect this CAN2USB by USB to your computer.
 * - Make sure the user can access the /dev/ttyUSB0 device.
 *   Brute force method is: #sudo chmod 777 /dev/ttyUSB0
 *   A wiser method is to add the user the appropriate group
 * - Adjust the baud rate of the CAN bus in this example.
 * - Do
 *   scons run
 * - All CAN messages on the bus should appear on the screen.
 */

static constexpr uint32_t canBusBaudRate = 125000;

xpcc::hosted::CanUsb canUsb;

namespace xpcc {

	IOStream&
	operator << (IOStream& s, const can::Message &m)
	{
		s.printf("Id: %04x, ", m.getIdentifier());
		
		if (m.isExtended()) {
			s.printf("E, ");
		} else {
			s.printf("   ");
		}

		if (m.isRemoteTransmitRequest()) {
			s.printf("R, ");
		} else {
			s.printf("   ");
		}
		
		
		s.printf("L: %1d, ", m.getLength());
		
		for (std::size_t ii = 0; ii < m.getLength(); ++ii) {
			s.printf("%02x ", m.data[ii]);
		}

		return s;
	}

} // xpcc namespace

int
main()
{
	if (!canUsb.open("/dev/ttyUSB0", canBusBaudRate)) {
		XPCC_LOG_ERROR << "Could not open port" << xpcc::endl;
	}

	while(true)
	{
		if (canUsb.isMessageAvailable())
		{
			xpcc::can::Message message;
			canUsb.getMessage(message);
			
			XPCC_LOG_DEBUG << message << xpcc::endl;
		}
	}

	canUsb.close();
}
