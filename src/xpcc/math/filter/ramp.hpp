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

#ifndef XPCC__RAMP_HPP
#define XPCC__RAMP_HPP

namespace xpcc
{
	/**
	 * \brief	Ramp
	 * 
	 * \todo	documentation
	 * \ingroup	filter
	 */
	template<typename T>
	class Ramp
	{
	public:
		/**
		 * \brief	Create a ramp generator
		 * 
		 * \param	positiveIncrement	Step with for positive direction
		 * \param	negativeIncrement	Step with for the negative direction.
		 * 								<B>Need to be positive!</b>
		 * \param	initialValue
		 * 
		 * Example:
		 * \code
		 * 
		 * \endcode
		 * 
		 * \warning	Both values for positive- and negative increment needs to
		 * 			be positive!
		 */
		Ramp(const T& positiveIncrement,
			 const T& negativeIncrement,
			 const T& initialValue = T());
		
		inline void
		setTarget(const T& target);
		
		/**
		 * \brief	Calculate the next step
		 */
		void
		update();
		
		inline const T&
		getValue() const
		{
			return value;
		}
		
		inline bool
		isTargetReached() const
		{
			return targetReached;
		}
	
	private:
		T target;
		T value;
		bool targetReached;
		
		T positiveIncrement;
		T negativeIncrement;
	};
}

#include "ramp_impl.hpp"

#endif // XPCC__RAMP_HPP
