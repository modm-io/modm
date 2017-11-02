
#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>
#include <xpcc/architecture/platform.hpp>

typedef xpcc::hosted::StaticSerialInterface<42> staticPort;
// about 42, see documentation of StaticSerialInterface

int
main()
{
	xpcc::hosted::SerialInterface port("/dev/ttyUSB0", 115200);
	staticPort::initialize<115200>(port);

	XPCC_LOG_INFO << "-" << xpcc::endl; // TODO linking fails if missing

	if (not port.isOpen()) {
		XPCC_LOG_ERROR << "Could not open port: " << port.getDeviceName().c_str() << xpcc::endl;
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		uint8_t a = 0;
		if (staticPort::read(a))
		{
			XPCC_LOG_INFO << "Read: " << a << xpcc::endl;
		}
	}

	port.close();
}
