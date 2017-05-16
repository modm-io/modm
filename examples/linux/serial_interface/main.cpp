/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/debug/logger.hpp>
#include <modm/architecture/architecture.hpp>

#include <cstdlib>

int
main()
{
	modm::hosted::SerialInterface port("/dev/ttyUSB0", 115200);
	
	if (not port.open()) {
		MODM_LOG_ERROR << "Could not open port: " << port.getDeviceName().c_str() << modm::endl;
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		uint8_t a;
		port.readBytes(&a, 1);
		MODM_LOG_DEBUG << "Read: " << a << modm::endl; 

		/*char a;
		if (port.read(a))
		{
			std::cout << "Read: " << a << std::endl; 
		}*/
	}

	port.close();
}
