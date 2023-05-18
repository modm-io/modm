/*
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "macros.hpp"
#include <modm/processing/fiber.hpp>

/// @ingroup modm_processing_protothread
#define MODM_PROTOTHREAD_IS_FIBER

#ifndef MODM_PROTOTHREAD_STACK_SIZE
/// @ingroup modm_processing_protothread
#define MODM_PROTOTHREAD_STACK_SIZE ::modm::fiber::StackSizeDefault
#endif

namespace modm::pt
{

/// @ingroup modm_processing_protothread
class Protothread : public modm::Fiber< MODM_PROTOTHREAD_STACK_SIZE >
{
public:
	Protothread(modm::fiber::Start start=modm::fiber::Start::Now)
	:	Fiber([this](){ while(update()) modm::this_fiber::yield(); }, start)
	{}

	void restart() { this->start(); }
	void stop();
	// isRunning() is implemented in fiber::Task

	// The run() function name was never enforced by the Protothread interface
	virtual bool run() { return false; };
	// Instead update() was often chosen to align it more with other parts of
	// modm that use an update() function to update their state periodically.
	// Therefore we cover both here to not have to change too much code.
	virtual bool update() { return run(); };
};

}
