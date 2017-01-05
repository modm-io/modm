/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "controller.hpp"

unittest::Controller::Controller() :
	reporter(0)	// FIXME need a dummy implementation
{
}

void
unittest::Controller::setReporter(Reporter& reporter)
{
	this->reporter = &reporter;
}

unittest::Reporter&
unittest::Controller::getReporter() const
{
	return *reporter;
}

void
unittest::Controller::nextTestSuite(modm::accessor::Flash<char> name) const
{
	reporter->nextTestSuite(name);
}
