/*
 * Copyright (c) 2015, Daniel Krebs
 * Copyright (c) 2015, 2017, Sascha Schade
 * Copyright (c) 2015-2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../radio.hpp"

// This example showcases a simple usage of the data layer implementation for
// the nRF24L01+ radio modules.

int main()
{
	Board::initialize();
	MODM_LOG_INFO << "Hello from nrf24-data example" << modm::endl;

	initializeNrf();

	while (true)
	{
		Nrf1Data::update();
		Nrf2Data::update();

		static modm::PeriodicTimer sendTimer{1s};
		if (Nrf1Data::Packet packet; sendTimer.execute())
		{
			static uint8_t counter{0};
			packet.setDestination(0x20);
			packet.payload[0] = counter++;
			Nrf1Data::sendPacket(packet);
			MODM_LOG_INFO << "Sending packet " << counter << " to " << packet.getDestination() << modm::endl;
		}

		if (Nrf2Data::Packet packet; Nrf2Data::getPacket(packet))
		{
			MODM_LOG_INFO << "Receiving packet " << packet.payload[0] << " from " << packet.getSource() << modm::endl;
		}
	}

	return 0;
}
