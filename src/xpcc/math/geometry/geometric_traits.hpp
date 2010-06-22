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

#ifndef GEOMETRIC_TRAITS_HPP
#define GEOMETRIC_TRAITS_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	Traits for all geometric classes
	 * 
	 * 
	 * \ingroup	geometry
	 * \author	Fabian Greif
	 */
	template <typename T>
	struct GeometricTraits
	{
		static const bool isValidType = false;
	};
	
	template <>
	struct GeometricTraits<int8_t>
	{
		static const bool isValidType = true;
		
		typedef float FloatType;
		typedef int16_t WideType;
	};
	
	template <>
	struct GeometricTraits<int16_t>
	{
		static const bool isValidType = true;
		
		typedef float FloatType;
		typedef int32_t WideType;
	};
	
	template <>
	struct GeometricTraits<int32_t>
	{
		static const bool isValidType = true;
		
		typedef float FloatType;
		
		// Usually the range of a int32_t is big enough so that no
		// conversion to int64_t is required. This exception is made because
		// 64-bit operations are very, very slow on an AVR.
		typedef int32_t WideType;
	};
	
	template <>
	struct GeometricTraits<float>
	{
		static const bool isValidType = true;
		
		typedef float FloatType;
		typedef float WideType;
	};
	
#ifndef __AVR__
	template <>
	struct GeometricTraits<double>
	{
		static const bool isValidType = true;
		
		typedef double FloatType;
		typedef double WideType;
	};
#endif
}

#endif // GEOMETRIC_TRAITS_HPP
