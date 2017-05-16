/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "count_type.hpp"

std::size_t unittest::CountType::numberOfDefaultConstructorCalls = 0;
std::size_t unittest::CountType::numberOfCopyConstructorCalls = 0;
std::size_t unittest::CountType::numberOfAssignments = 0;
std::size_t unittest::CountType::numberOfDestructorCalls = 0;
std::size_t unittest::CountType::numberOfReallocs = 0;

std::size_t unittest::CountType::numberOfOperations = 0;

unittest::CountType::CountType()
{
	++numberOfDefaultConstructorCalls;
	++numberOfOperations;
}

unittest::CountType::CountType(const CountType&)
{
	++numberOfCopyConstructorCalls;
	++numberOfOperations;
}

unittest::CountType::~CountType()
{
	++numberOfDestructorCalls;
	if (numberOfDestructorCalls == (numberOfCopyConstructorCalls - numberOfDefaultConstructorCalls)) {
		++numberOfReallocs;
	}
	++numberOfOperations;
}

unittest::CountType&
unittest::CountType::operator = (const CountType&)
{
	++numberOfAssignments;
	++numberOfOperations;
	
	return *this;
}

void
unittest::CountType::reset()
{
	numberOfDefaultConstructorCalls = 0;
	numberOfCopyConstructorCalls = 0;
	numberOfAssignments = 0;
	numberOfDestructorCalls = 0;
	numberOfReallocs = 0;
	
	numberOfOperations = 0;
}
