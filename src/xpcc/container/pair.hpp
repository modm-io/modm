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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__PAIR_HPP
#define	XPCC__PAIR_HPP

namespace xpcc
{
	/**
	 * \brief		Pair<FirstType, SecondType> is a heterogeneous pair
	 * 
	 * This class holds one object of type FirstType and one object of type
	 * SecondType.
	 * 
	 * It provides only the default constructors and public variables to enable
	 * usage as POD (plain old data) object. Only with this characteristic it
	 * is possible to storage objects of this type inside the flash-memory
	 * which is desirable for example for the interpolation classes.
	 * 
	 * \see	xpcc::interpolation::Linear
	 * 
	 * This constraint make a the usage rather unusual. See this example:
	 * \code
	 * // construction
	 * xpcc::Pair<uint8_t, uint16_t> pair = { 12, -12345 };
	 * 
	 * // this typedef is necessary to be able to use
	 * // the type inside the FLASH macro
	 * typedef xpcc::Pair<int8_t, int8_t> MyPair;
	 * 
	 * // put some values into the flash memory
	 * FLASH_STORAGE(MyPair pairs[3]) =
	 * {
	 *     { 0, 10 },
	 *     { 2, 30 },
	 *     { 5, 60 }
	 * };
	 * \endcode
	 * 
	 * As this class has no constructor you need to use the following syntax
	 * to initialize an instance of this class: 
	 * \code
	 *	xpcc::Pair<int, float> pair = { 1203, 1035.234 };
	 * \endcode
	 * 
	 * \note	Even if this class provides public attributes it is recommend to use
	 * 			the access methods, because as soon as C++0x is available the
	 * 			implementation will be changed and the attributes will be made private.
	 * 			Until then just ignore that you can access them directly ;-)
	 * 
	 * \author		Fabian Greif
	 * \ingroup		container
	 */
	template<typename T1, typename T2>
	class Pair
	{
	public:
		typedef T1 FirstType;
		typedef T2 SecondType;
		
	public:
		// No non-trivial constructor is allowed, otherwise this class
		// won't be POD (plain old data) :-(
		// (this behavior changes with C++0x)
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
		
	// ... not allowed either, only public attributes :-(
	//private:
		FirstType first;
		SecondType second;
	};
}

#endif	// XPCC__PAIR_HPP
