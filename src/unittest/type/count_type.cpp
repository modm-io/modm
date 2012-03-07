// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: count_type.cpp 607 2011-09-13 19:51:03Z dergraaf $
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
