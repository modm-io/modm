/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug.hpp>
#include <modm/processing.hpp>

using namespace std::chrono_literals;

// The name of the current fiber for log messages, like a thread name.
// Plain data should be constinit to avoid an initialization check per access.
constinit thread_local const char *fiber_name{"main"};
#define LOG MODM_LOG_INFO << '[' << fiber_name << "] "

// The error of the last operation of the current fiber, like errno.
enum class Error : uint8_t { None, Timeout };
constinit thread_local Error last_error{Error::None};

bool
read_sensor(uint8_t channel, uint16_t &value)
{
	static uint8_t requests;
	// Other fibers run while this one waits for the sensor
	modm::this_fiber::sleep_for(1ms * channel);
	if (++requests % 4 == 0)
	{
		last_error = Error::Timeout;
		return false;
	}
	value = 100 * channel + requests;
	last_error = Error::None;
	return true;
}

// A shared pool of buffers.
class Pool
{
	static constexpr uint8_t Blocks{2};
	uint16_t blocks[Blocks][8];
	bool used[Blocks]{};

public:
	uint16_t*
	acquire()
	{
		for (uint8_t ii = 0; ii < Blocks; ii++)
			if (not used[ii]) { used[ii] = true; return blocks[ii]; }
		modm_assert(false, "pool", "No free block!");
	}

	void
	release(uint16_t *block)
	{ used[(block - blocks[0]) / 8] = false; }

	uint8_t
	available() const
	{ uint8_t count{}; for (bool u : used) count += not u; return count; }
} pool;

// The destructors of thread_local objects are never called, so an object that
// owns a resource is a local variable of the fiber function, and the functions
// this fiber calls find it through a thread_local pointer.
struct Samples
{
	uint16_t *data{pool.acquire()};
	uint8_t count{};
	~Samples() { pool.release(data); }
};
constinit thread_local Samples *samples{nullptr};

void
record(uint8_t channel)
{
	uint16_t value;
	if (read_sensor(channel, value)) samples->data[samples->count++] = value;
	else
	{
		// The other fibers set their own error while this one waits
		modm::this_fiber::sleep_for(3ms);
		if (last_error == Error::Timeout) { LOG << "timeout" << modm::endl; }
	}
}

void
sensor(const char *name, uint8_t channel)
{
	fiber_name = name;
	Samples local; // returns its buffer to the pool when this function returns
	samples = &local;
	for (uint8_t ii = 0; ii < 6; ii++) record(channel);

	uint32_t sum{};
	for (uint8_t ii = 0; ii < local.count; ii++) sum += local.data[ii];
	LOG << local.count << " samples, average " << sum / local.count << modm::endl;
}

modm::Fiber<> fiber1, fiber2, fiber3;

int
main()
{
	Board::initialize();
	LOG << "free blocks: " << pool.available() << modm::endl;

	for (uint8_t round = 0; round < 3; round++)
	{
		(void) fiber1.start(sensor, "temperature", 1);
		(void) fiber2.start(sensor, "pressure", 2);
		// This fiber never uses the samples and does not lease a buffer
		(void) fiber3.start([] { fiber_name = "idle"; LOG << "idle" << modm::endl; });
		modm::fiber::Scheduler::run();
		// The buffers were returned when the fiber functions returned
		LOG << "round " << round << ", free blocks: " << pool.available() << modm::endl;
	}

	// returning from main() reboots before the UART buffer is sent
	MODM_LOG_INFO.flush();
	return 0;
}
