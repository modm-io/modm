/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
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

#ifndef UNITTEST_COUNT_TYPE_HPP
#define UNITTEST_COUNT_TYPE_HPP

#include <cstddef>
#include <stdint.h>

namespace unittest
{
	/**
	 * \brief	Data type to count the number of constructor etc. calls
	 * 
	 * \ingroup	unittest
	 */
	class CountType
	{
	public:
		CountType();
		
		CountType(const CountType& other);
		
		~CountType();
		
		CountType&
		operator = (const CountType& other);
		
		static void
		reset();
		
		static std::size_t numberOfDefaultConstructorCalls;
		static std::size_t numberOfCopyConstructorCalls;
		static std::size_t numberOfAssignments;
		static std::size_t numberOfDestructorCalls;
		static std::size_t numberOfReallocs;
		
		static std::size_t numberOfOperations;
	};
}

#endif // UNITTEST_COUNT_TYPE_HPP
