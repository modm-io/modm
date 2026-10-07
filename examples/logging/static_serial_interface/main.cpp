/*
 * Copyright (c) 2017, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#include <modm/debug/logger.hpp>
#include <modm/architecture.hpp>
#include <modm/platform.hpp>

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
