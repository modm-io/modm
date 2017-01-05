/*
 * Copyright (c) 2010-2011, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

modm::sab::Interface < ... > interface;

while (1)
{
	interface.update();
	if (interface.isMessageAvailable())
	{
		// do something ... for example print the received message
		stream << modm::hex;
		
		const uint8_t *data = interface.getPayload();
		for (std::size_t i = 0; i < interface.getPayloadLength(); ++i)
		{
			stream << *data++;
		}
		
		stream << modm::endl;
		
		interface.dropMessage();
	}
}
