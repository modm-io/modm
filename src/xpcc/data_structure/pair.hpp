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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__PAIR_HPP
#define	XPCC__PAIR_HPP

namespace xpcc
{
	/**
	 * @ingroup		data_structure
	 * @headerfile	<xpcc/data_structure/pair.hpp>
	 * 
	 * @brief		Pair<FirstType, SecondType> is a heterogeneous pair
	 * 
	 * It holds one object of type FirstType and one object of type
	 * SecondType.
	 * 
	 * @todo	needs a better implementation without losing the POD attribute!
	 */
	template<typename FirstType, typename SecondType>
	class Pair
	{
	public:
		// No non-trivial constructor is allowed, otherwise this class
		// won't be POD (plain old data) :-(
		// (this changes with C++0x)
		/*Pair(const FirstType& first, const SecondType& second) :
			first(first), second(second)
		{
		}*/
		
		FirstType&
		getFirst()
		{
			return first;
		}
		
		const FirstType&
		getFirst() const
		{
			return first;
		}
		
		SecondType&
		getSecond()
		{
			return second;
		}
		
		const SecondType&
		getSecond() const
		{
			return second;
		}
		
	// not allowed either :-(
	//private:
		FirstType first;
		SecondType second;
	};
}

#endif	// XPCC__PAIR_HPP
