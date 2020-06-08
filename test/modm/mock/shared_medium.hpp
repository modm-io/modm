/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <vector>

namespace modm_test
{

class SharedMedium
{
public:
	inline static bool
	write(uint8_t data)
	{
		raw_transmitted.push_back(data);
		transmitted.push_back(data);
		// corrupt the data on the shared medium
		if (fail_tx_index < ++tx_count)
			data ^= 0x10;
		// loop back on shared medium
		received.push_back(data);
		raw_received.push_back(data);
		return true;
	}

	inline static bool
	read(uint8_t& byte)
	{
		if (received.empty()) {
			if (rx_queue.empty()) return false;
			received = std::move(rx_queue);
			rx_queue_index = size_t(-1);
		}
		byte = received.front();
		received.erase(received.begin());
		rx_count++;
		return true;
	}

	inline static bool
	hasError()
	{
		return (fail_rx_index < rx_count);
	}

	inline static void
	clearError()
	{
	}

	inline static void
	discardTransmitBuffer()
	{
		transmitted.clear();
		received.clear();
		tx_count = 0;
		fail_tx_index = size_t(-1);
	}

	inline static void
	discardReceiveBuffer()
	{
		received.clear();
		rx_count = 0;
		fail_rx_index = size_t(-1);
	}

	inline static size_t
	receiveBufferSize()
	{
		if (rx_queue_index <= tx_count) return 1;
		return received.size();
	}

public:
	inline static void
	reset()
	{
		discardTransmitBuffer();
		discardReceiveBuffer();
		clearError();
		rx_queue.clear();
		raw_transmitted.clear();
		raw_received.clear();
		rx_queue_index = size_t(-1);
	}

	inline static void
	add_rx(std::initializer_list<uint8_t> data)
	{
		received.insert(received.end(), data);
	}
	inline static void
	add_queued_rx(std::initializer_list<uint8_t> data, size_t index=0)
	{
		rx_queue_index = index;
		rx_queue.insert(rx_queue.end(), data);
	}

public:
	static inline std::vector<uint8_t> raw_received;
	static inline std::vector<uint8_t> raw_transmitted;

	static inline std::vector<uint8_t> received;
	static inline std::vector<uint8_t> transmitted;
	static inline size_t tx_count{0};
	static inline size_t rx_count{0};

	static inline size_t fail_tx_index{size_t(-1)};
	static inline size_t fail_rx_index{size_t(-1)};

	static inline std::vector<uint8_t> rx_queue;
	static inline size_t rx_queue_index{size_t(-1)};
};

}
