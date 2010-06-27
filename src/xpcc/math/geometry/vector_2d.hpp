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

#ifndef	XPCC__VECTOR_2D_HPP
#define	XPCC__VECTOR_2D_HPP

#include <cmath>
#include <xpcc/io/iostream.hpp>

#include "geometric_traits.hpp"

namespace xpcc
{
	// forward declaration
	template <typename T>
	class Point2D;
	
	// ------------------------------------------------------------------------
	/**
	 * \brief	2D Vector
	 * 
	 * \ingroup	geometry
	 */
	template<typename T = int16_t>
	class Vector2D
	{
		friend class Point2D<T>;
		
	public:
		typedef typename GeometricTraits<T>::WideType WideType;
		typedef typename GeometricTraits<T>::FloatType FloatType;
		
	public:
		/**
		 * \brief	Default-Constructor
		 * 
		 * Creates a Vector with coordinates (0, 0).
		 */
		Vector2D();
		
		Vector2D(const T& x, const T& y);
		
		/**
		 * \brief	Construct the vector AB
		 */
		Vector2D(const Point2D<T>& A, const Point2D<T>& B);
		
		inline void
		setX(const T& value);
		
		inline void
		setY(const T& value);
		
		inline void
		set(const T& x, const T& y);
		
		
		inline const T&
		getX() const;
		
		inline const T&
		getY() const;
		
		/**
		 * \brief	Calculate length
		 * 
		 * Length of the vector from the origin to the point.
		 */
		T
		getLength() const;
		
		/**
		 * \brief	Calculate squared length of the vector
		 * 
		 * \return	squared length (x*x + y*y)
		 */
		WideType
		getLengthSquared() const;
		
		/**
		 * \brief	Calculate the absolute angle
		 * 
		 * Absolute angle of the vector from the origin to the point.
		 */
		float
		getAngle() const;
		
		/**
		 * \brief	Normalize length to 1
		 */
		Vector2D&
		normalize();

		/**
		 * \brief	Scale the length by \p factor
		 */
		template<typename U>
		Vector2D&
		scale(const U& factor);

		Vector2D&
		rotate(const float phi);
		
		
		Vector2D&
		operator += (const Vector2D &other);
		
		Vector2D&
		operator -= (const Vector2D &other);
		
		/**
		 * \brief	Convert between Point-objects with different base-types
		 */
		template<typename U>
		Vector2D<U>
		convert() const;
		
		Point2D<T>
		toPoint() const;
		
	public:
		/**
		 * \brief	Calculate the dot-product
		 * 
		 * Also known as the scalar product.
		 */
		static WideType
		dotProduct(const Vector2D<T>& a, const Vector2D<T>& b);
	
	private:
		T x;
		T y;
		
	private:
		template<typename U>
		friend IOStream&
		operator <<(IOStream& s, const Vector2D<U>& c);
		
		
		template<typename U>
		friend Vector2D<U>
		operator - (const Vector2D<U> &a);
		
		template<typename U>
		friend Vector2D<U>
		operator - (const Vector2D<U> &a, const Vector2D<U> &b);

		template<typename U>
		friend Vector2D<U>
		operator + (const Vector2D<U> &a, const Vector2D<U> &b);

		template<typename U>
		friend bool
		operator == (const Vector2D<U> &a, const Vector2D<U> &b);
		
		template<typename U>
		friend bool
		operator != (const Vector2D<U> &a, const Vector2D<U> &b);
	};
	
	// ------------------------------------------------------------------------
	// Global functions
	// ------------------------------------------------------------------------
	/**
	 * \brief	Stream operator for \b xpcc::Point<U>
	 * \ingroup	geometry
	 */
	template<typename U>
	IOStream&
	operator << (IOStream& s, const Vector2D<U>& c);
	
	// ------------------------------------------------------------------------
	// Global Operators
	// ------------------------------------------------------------------------
	template<typename U>
	Vector2D<U>
	operator - (const Vector2D<U> &a);
	
	template<typename U>
	Vector2D<U>
	operator - (const Vector2D<U> &a, const Vector2D<U> &b);

	template<typename U>
	Vector2D<U>
	operator + (const Vector2D<U> &a, const Vector2D<U> &b);

	template<typename U>
	bool
	operator == (const Vector2D<U> &a, const Vector2D<U> &b);
	
	template<typename U>
	bool
	operator != (const Vector2D<U> &a, const Vector2D<U> &b);
	
	// ------------------------------------------------------------------------
	// Declaration of specialized methods
	// ------------------------------------------------------------------------
	template<>
	Vector2D<float>&
	Vector2D<float>::rotate(const float phi);
	
	template<>
	float
	Vector2D<float>::getLength() const;
	
	template<>
	double
	Vector2D<double>::getLength() const;
	
#if defined(__AVR__) && defined(__AVR_HAVE_MUL__)
	template<>
	int16_t
	Vector2D<int16_t>::getLength() const;
	
	template<>
	int32_t
	Vector2D<int16_t>::getLengthSquared() const;
	
	template<>
	int32_t
	Vector2D<int16_t>::dot(const xpcc::Vector2D<int16_t>& a,
						   const xpcc::Vector2D<int16_t>& b);
#endif
	
	template<> template<typename U>
	Vector2D<U>
	Vector2D<float>::convert() const
	{
		return Vector2D<U>(round(this->x), round(this->y));
	}
	
	template<> template<typename U>
	Vector2D<U>
	Vector2D<double>::convert() const
	{
		return Vector2D<U>(round(this->x), round(this->y));
	}
	
	template<> template<>
	Vector2D<double>
	Vector2D<float>::convert() const;
	
	template<> template<>
	Vector2D<float>
	Vector2D<double>::convert() const;
}

#include "point_2d.hpp"
#include "vector_2d_impl.hpp"

#endif	// XPCC__VECTOR_2D_HPP
