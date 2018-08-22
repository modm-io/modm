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

#ifndef CALC_HPP
#define CALC_HPP


class Calc
{
public:
	static void
	calc();

	static double
	get();

protected:
	static double
	myDouble;
};

#endif // CALC_HPP
