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

#ifndef MODM_TEST_MOCK_SPI_MASTER_HPP
#define MODM_TEST_MOCK_SPI_MASTER_HPP

#include <modm/architecture/interface/spi_master.hpp>
#include <modm/container/doubly_linked_list.hpp>

namespace modm_test
{

namespace platform
{

/**
 * Mock serial peripheral interface for unittests.
 *
 * @author	Raphael Lehmann
 *
 * @ingroup modm_test_mock_spi_master
 */
class SpiMaster : public modm::SpiMaster
{
private:
	static inline uint8_t count{0};
	static inline void* context{nullptr};
	static inline ConfigurationHandler configuration{nullptr};

	static inline DataMode dataMode{DataMode::Mode0};
	static inline DataOrder dataOrder{DataOrder::MsbFirst};

public:
	using ArrayContainer = modm::DoublyLinkedList<uint8_t>;

private:
	static inline ArrayContainer rxBuffer;
	static inline ArrayContainer txBuffer;

	static inline uint8_t tmp{0};

public:
	static void
	initialize()
	{
	}

	static void
	setDataMode(DataMode mode)
	{
		dataMode = mode;
	}

	static void
	setDataOrder(DataOrder order)
	{
		dataOrder = order;
	}


	static uint8_t
	acquire(void *ctx, ConfigurationHandler handler = nullptr);

	static uint8_t
	release(void *ctx);


	static uint8_t
	transferBlocking(uint8_t data)
	{
		return RF_CALL_BLOCKING(transfer(data));
	}

	static void
	transferBlocking(uint8_t *tx, uint8_t *rx, std::size_t length)
	{
		RF_CALL_BLOCKING(transfer(tx, rx, length));
	}


	static modm::ResumableResult<uint8_t>
	transfer(uint8_t data);

	static modm::ResumableResult<void>
	transfer(uint8_t *tx, uint8_t *rx, std::size_t length);

public:
	static std::size_t getTxBufferLength() {
		return txBuffer.getSize();
	}

	static void popTxBuffer(uint8_t* buf)
	{
		std::size_t j = 0;
		for(ArrayContainer::iterator i = txBuffer.begin(); i != txBuffer.end(); ++i) {
			buf[j++] = (*i);
		}
		while(txBuffer.getSize() > 0) {
			txBuffer.removeFront();
		}
	}

	static void appendRxBuffer(uint8_t* data, std::size_t length)
	{
		for(std::size_t i = 0; i < length; ++i) {
			rxBuffer.append(data[i]);
		}
	}

	static void clearBuffers()
	{
		while(txBuffer.getSize() > 0) {
			txBuffer.removeFront();
		}
		while(rxBuffer.getSize() > 0) {
			rxBuffer.removeFront();
		}
	}
};

} // namespace platform

} // namespace modm_test

#endif // MODM_TEST_MOCK_SPI_MASTER_HPP
