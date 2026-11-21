/*
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <mutex>

// ----------------------------------------------------------------------------
// Set the log level
#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

// The logger yields while the UART is full, so waiting for this lock must yield
// too. A spinlock or multicore::Mutex would block the fiber holding it.
static modm::fiber::mutex log_mutex;
#define LOG_GUARD() std::lock_guard g(log_mutex)

template<size_t Core, size_t Instance>
class Thread : public modm::Fiber<>
{
	static constexpr auto delay = 10ms + 1ms * Instance;

public:
	// Only start the Core0 threads here, otherwise they run on the Core0 scheduler!
	Thread() { if constexpr (Core == 0) start(); }

	bool
	start()
	{
		return Fiber::start([this]{ run(); });
	}

	void
	run()
	{
		uint32_t uptime{};
		while (true)
		{
			modm::this_fiber::sleep_for(delay);
			{
				// try without this line for intermixed output
				LOG_GUARD();
				MODM_LOG_INFO << "Core: " << multicore::Core::cpuId()
							  << " thread: " << Instance << " uptime: " << ++uptime << modm::endl;
			}
		}
	}
};

Thread<0, 0> t00;
Thread<0, 1> t01;
Thread<0, 2> t02;
Thread<0, 3> t03;

Thread<1, 0> t10;
Thread<1, 1> t11;
Thread<1, 2> t12;
Thread<1, 3> t13;

void
core1_main()
{
	t10.start();
	t11.start();
	t12.start();
	t13.start();
	modm::fiber::Scheduler::run();
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();


	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG << "debug" << modm::endl;
	MODM_LOG_INFO << "info" << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR << "error" << modm::endl;

	multicore::Core1::run(core1_main);
	modm::fiber::Scheduler::run();
	return 0;
}
