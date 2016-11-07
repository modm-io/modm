/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "calc.hpp"

double Calc::myDouble = double(1.0);

void
Calc::calc()
{
	myDouble += double(0.1);
}

double
Calc::get()
{
	return myDouble;
}
