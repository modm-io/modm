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

modm::ResumableResult<uint8_t>
modm_test::platform::SpiMaster::transfer(uint8_t data)
{
	txBuffer.append(data);

	if(rxBuffer.isNotEmpty()) {
		tmp = rxBuffer.getFront();
		rxBuffer.removeFront();
	}
	else {
		tmp = 0;
	}

	return {modm::rf::Stop, tmp};
}

modm::ResumableResult<void>
modm_test::platform::SpiMaster::transfer(uint8_t * tx, uint8_t * rx, std::size_t length)
{
	for(std::size_t i = 0; i < length; ++i) {
		//if(tx != nullptr)
		//	txVector.push_back(tx[i]);

		//txBuffer.append(tx != nullptr ? tx[i] : 0);

		if(tx != nullptr) {
			txBuffer.append(tx[i]);
		}
		else {
			txBuffer.append(0);
		}

		if(rx != nullptr) {
			if(rxBuffer.isNotEmpty()) {
				rx[i] = rxBuffer.getFront();
			}
			else {
				rx[i] = 0;
			}
		}
		if(rxBuffer.isNotEmpty()) {
			rxBuffer.removeFront();
		}
	}

	return {modm::rf::Stop};
}
