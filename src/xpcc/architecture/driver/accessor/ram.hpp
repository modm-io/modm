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

#ifndef	XPCC_ACCESSOR__RAM_HPP
#define	XPCC_ACCESSOR__RAM_HPP

#include <cstddef>
#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	namespace accessor
	{
		/**
		 * \brief	Pointer to RAM
		 * 
		 * \ingroup	accessor
		 * \author	Fabian Greif
		 */
		template<typename T>
		class Ram
		{
		public:
			ALWAYS_INLINE
			Ram(const T* address = 0) :
				address(address)
			{
			}
			
			template <typename U>
			ALWAYS_INLINE
			explicit Ram(const Ram<U>& rhs) :
				address((T*) rhs.address)
			{
			}
			
			ALWAYS_INLINE
			const T
			operator *() const
			{
				return *address;
			}
			
			ALWAYS_INLINE
			const T
			operator [](std::size_t index) const
			{
				return *(address + index);
			}
			
			ALWAYS_INLINE
			Ram&
			operator ++ ()
			{
				*this += 1;
				return *this;
			}

			ALWAYS_INLINE
			Ram
			operator ++ (int)
			{
				Ram ret = *this;
				++*this;
				return ret;
			}

			ALWAYS_INLINE
			Ram&
			operator -- ()
			{
				*this -= 1;
				return *this;
			}

			ALWAYS_INLINE
			Ram&
			operator -- (int)
			{
				Ram ret = *this;
				--*this;
				return ret;
			}
			
			ALWAYS_INLINE
			Ram&
			operator += (std::size_t rhs)
			{
				address += rhs;
				return *this;
			}
			
			ALWAYS_INLINE
			Ram&
			operator -= (std::size_t rhs)
			{
				address -= rhs;
				return *this;
			}
			
			ALWAYS_INLINE
			const T*
			getPointer() const
			{
				return address;
			}
		
		private:
			const T* address;
		};
	}
}

#endif	// XPCC_ACCESSOR__RAM_HPP
