
#include <modm/debug/logger.hpp>
#include <modm/architecture/architecture.hpp>
#include <modm/platform/platform.hpp>

typedef modm::platform::StaticSerialInterface<42> staticPort;
// about 42, see documentation of StaticSerialInterface

int
main()
{
	modm::platform::SerialInterface port("/dev/ttyUSB0", 115200);
	staticPort::initialize<115200>(port);

	MODM_LOG_INFO << "-" << modm::endl; // TODO linking fails if missing

	if (not port.isOpen()) {
		MODM_LOG_ERROR << "Could not open port: " << port.getDeviceName().c_str() << modm::endl;
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		uint8_t a = 0;
		if (staticPort::read(a))
		{
			MODM_LOG_INFO << "Read: " << a << modm::endl;
		}
	}

	port.close();
}
