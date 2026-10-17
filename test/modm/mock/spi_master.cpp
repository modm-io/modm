/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "spi_master.hpp"

uint8_t
modm_test::platform::SpiMaster::acquire(void *ctx, ConfigurationHandler handler)
{
	if (context == nullptr)
	{
		context = ctx;
		count = 1;
		// if handler is not nullptr and is different from previous configuration
		if (handler and configuration != handler) {
			configuration = handler;
			configuration();
		}
		return 1;
	}

	if (ctx == context)
		return ++count;

	return 0;
}

uint8_t
modm_test::platform::SpiMaster::release(void *ctx)
{
	if (ctx == context)
	{
		if (--count == 0)
			context = nullptr;
	}
	return count;
}
// ----------------------------------------------------------------------------

uint8_t
modm_test::platform::SpiMaster::transfer(uint8_t data)
{
	txBuffer.push_back(data);

	if(!rxBuffer.empty()) {
		tmp = rxBuffer.front();
		rxBuffer.pop_front();
	}
	else {
		tmp = 0;
	}
	return tmp;
}

void
modm_test::platform::SpiMaster::transfer(uint8_t * tx, uint8_t * rx, std::size_t length)
{
	for(std::size_t i = 0; i < length; ++i) {
		//if(tx != nullptr)
		//	txVector.push_back(tx[i]);

		//txBuffer.push_back(tx != nullptr ? tx[i] : 0);

		if(tx != nullptr) {
			txBuffer.push_back(tx[i]);
		}
		else {
			txBuffer.push_back(0);
		}

		if(rx != nullptr) {
			if(!rxBuffer.empty()) {
				rx[i] = rxBuffer.front();
			}
			else {
				rx[i] = 0;
			}
		}
		if(!rxBuffer.empty()) {
			rxBuffer.pop_front();
		}
	}
}
