/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/filter/pid.hpp>

#include "pid_test.hpp"

void
PidTest::testCreation()
{
	modm::Pid<int16_t, 10> controller;
	
	modm::Pid<int16_t, 10>::Parameter parameter(0.1, 0.2, 0.3, 32767, 100);
	
	controller.setParameter(parameter);
	
	controller.reset();
	
	controller.update(13);
	
	controller.getValue();
}
