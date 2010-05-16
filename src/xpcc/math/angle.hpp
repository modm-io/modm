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

#ifndef	XPCC__ANGLE_HPP
#define	XPCC__ANGLE_HPP

#include <math.h>
#include <xpcc/communication/io/iostream.hpp>

namespace xpcc
{
	/**
	 * \ingroup	math
	 * \brief	Representation of a angle
	 */
	class Angle
	{
		public:
			Angle(float value = 0.0) : value(value) {
			}
			
			/// \brief	Limit angle to +-Pi
			void
			normalize();
			
			/// \brief	reverse the angle
			void
			reverse();
			
			float
			toFloat() const
			{
				return this->value;
			}
			
			Angle&
			operator =(const float& angle)
			{
				this->value = angle;
				return *this;
			}
			
			Angle&
			operator +=(const Angle &other);
			
			Angle&
			operator -=(const Angle &other);
		
		public:
			friend inline Angle
			operator -(const Angle &a);
			
			friend inline Angle
			operator -(const Angle &a, const Angle &b);
			
			friend inline Angle
			operator +(const Angle &a, const Angle &b);
			
			friend inline bool
			operator ==(const Angle &a, const Angle &b);
			
			friend inline bool
			operator !=(const Angle &a, const Angle &b);
			
			friend inline bool
			operator >(const Angle &a, const Angle &b);
			
			friend inline bool
			operator >=(const Angle &a, const Angle &b);
			
			friend inline bool
			operator <(const Angle &a, const Angle &b);
			
			friend inline bool
			operator <=(const Angle &a, const Angle &b);

			friend inline IOStream&
			operator <<(IOStream &os, const Angle &a);
			
			static inline float
			toRadian(float angle)
			{
				return angle * M_PI / 180.0;
			}
			
			static inline float
			toDegree(float angle)
			{
				return angle * 180.0 / M_PI;
			}
			
		private:
			float value;
	};

	/**
	 * \ingroup	math
	 * \brief	Stream operator to \b xpcc::Angle
	 */
	inline IOStream&
	operator <<(IOStream &os, const Angle &a);
}

// ----------------------------------------------------------------------------
// inline operator functions

inline xpcc::Angle&
xpcc::Angle::operator+=(const Angle &other) {
	this->value += other.value;
	return *this;
}

inline xpcc::Angle&
xpcc::Angle::operator-=(const Angle &other) {
	this->value -= other.value;
	return *this;
}

// ----------------------------------------------------------------------------
// overloaded global operator functions

namespace xpcc
{
	inline Angle
	operator -(const Angle &a) {
		return Angle(-a.value);
	}

	inline Angle
	operator -(const Angle &a, const Angle &b) {
		return Angle(a.value - b.value);
	}

	inline Angle
	operator +(const Angle &a, const Angle &b) {
		return Angle(a.value + b.value);
	}

	inline bool
	operator ==(const Angle &a, const Angle &b) {
		return (a.value == b.value);
	}

	inline bool
	operator !=(const Angle &a, const Angle &b) {
		return (a.value != b.value);
	}
	
	inline bool
	operator >(const Angle &a, const Angle &b) {
		return (a.value > b.value);
	}
	
	inline bool
	operator >=(const Angle &a, const Angle &b) {
		return (a.value >= b.value);
	}
	
	inline bool
	operator <(const Angle &a, const Angle &b) {
		return (a.value < b.value);
	}
	
	inline bool
	operator <=(const Angle &a, const Angle &b) {
		return (a.value <= b.value);
	}
}

xpcc::IOStream&
xpcc::operator <<(xpcc::IOStream &os, const xpcc::Angle &a) {
	os << a.toFloat();
	return os;
}

// ----------------------------------------------------------------------------	
// overloaded global functions

/// \ingroup	math
/// \brief		Calculate cosinus of an angle
inline xpcc::Angle
cos(const xpcc::Angle& angle) {
	return cos(angle.toFloat());
}

/// \ingroup	math
/// \brief		Calculate sinus of an angle
inline xpcc::Angle
sin(const xpcc::Angle& angle) {
	return sin(angle.toFloat());
}

/// \ingroup	math
/// \brief		Calculate tangens of an angle
inline xpcc::Angle
tan(const xpcc::Angle& angle) {
	return tan(angle.toFloat());
}

#endif	// XPCC__ANGLE_HPP
