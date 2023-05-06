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

#include "task.hpp"

namespace modm
{

/**
 * This class is a convenience extension of a fiber task, which contains the
 * `modm::fiber::Stack` as part of the class. The class is constructed at
 * runtime only, therefore it is placed into the `.bss` section by default and
 * does not count towards your `.data` section size. To speed up booting you
 * may place the object into the `modm_faststack` section.
 *
 * @ingroup modm_processing_fiber
 */
template< size_t StackSize = fiber::StackSizeDefault >
class Fiber : public fiber::Task
{
	fiber::Stack<StackSize> stack;
public:
	template<class T>
	Fiber(T&& task, fiber::Start start=fiber::Start::Now)
	: Task(stack, std::forward<T>(task), start)
	{}
};

}
