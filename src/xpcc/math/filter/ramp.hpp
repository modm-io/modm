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

#ifndef XPCC_FILTER__RAMP_HPP
#define XPCC_FILTER__RAMP_HPP

namespace xpcc
{
	namespace filter
	{
		/**
		 * \brief	Ramp
		 * 
		 * The output value is incremented or decremented at every call to update
		 * until target has been reached by increment or decrement.
		 * 
		 * The final value is always set to target after several calls to update.
		 * 
		 * Example:
		 * \todo	example
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
			 * \param	increment		Stepwith for positive direction
			 * \param	decrement		Stepwith for the negative direction.
			 * 							<b>Needs to be positive!</b>
			 * \param	initialValue	Starting value
			 * 
			 * \warning	Both values for increment and decrement needs to
			 * 			be positive!
			 */
			Ramp(const T& increment,
				 const T& decrement,
				 const T& initialValue = T());
			
			inline void
			setTarget(const T& target);
			
			/// Calculate the next step
			void
			update();
			
			inline void
			reset(const T& value = T());
			
			inline const T&
			getValue() const;
			
			inline bool
			isTargetReached() const;
		
		private:
			T target;
			T value;
			bool targetReached;
			
			T increment;
			T decrement;
		};
	}
}

#include "ramp_impl.hpp"

#endif // XPCC_FILTER__RAMP_HPP
