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
	class Location2D;
	
	/**
	 * \brief	2D Vector
	 * 
	 * Basic data type of all geometric operations. Used to represent vectors
	 * as well as particular points in the coordinate system.
	 * 
	 * \subsection	point_vector	Point vs. vector
	 * 
	 * In geometry, it is often convenient to use vector arithmetic to
	 * represent points.
	 * 
	 * A vector, by its definition, has no fixed starting point, but if we
	 * imagine the starting point of a vector to be the origin, then the
	 * endpoint of the vector represents a particular point.
	 * 
	 * In this manner, every vector can be said to identify a unique point,
	 * which is the endpoint of the vector when its starting point is the
	 * origin.
	 * 
	 * Therefore there isn't a Point2D-class, but only a Vector2D class.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	geometry
	 */
	template<typename T = int16_t>
	class Vector2D
	{
		friend class Location2D<T>;
		
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
		 * \brief	Construct the vector from A to B
		 * 
		 * Same as
		 * \code
		 * vector = -A + B
		 * \endcode
		 */
		Vector2D(const Vector2D<T>& A, const Vector2D<T>& B);
		
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
		 * \brief	Calculate length of the vector
		 */
		T
		getLength() const;
		
		/**
		 * \brief	Calculate squared length of the vector
		 * 
		 * This method is considerably faster than getLength() because it
		 * doesn't need to calculate the square root.
		 * 
		 * \return	squared length (x*x + y*y)
		 */
		WideType
		getLengthSquared() const;
		
		/**
		 * \brief	Calculate the absolute angle
		 * 
		 * \code
		 * atan2(y, x)
		 * \endcode
		 */
		float
		getAngle() const;
		
		/**
		 * \brief	Normalize length to 1
		 * 
		 * \warning	This method is only useful if T is a floating point type.
		 * 			For integer types the result might be wrong!
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
		rotate(float phi);
		
		/**
		 * \brief	Move the point in x and y direction
		 */
		Vector2D&
		translate(const Vector2D<T>& vector);
		
		
		WideType
		getDistanceTo(const Vector2D<T>& other) const;
		
		/**
		 * \brief	Calculate the dot-product
		 * 
		 * Also known as the scalar product.
		 * 
		 * \code
		 * this.x * other.x + this.y * other.y
		 * \endcode
		 */
		WideType
		dot(const Vector2D<T>& other) const;
		
		/**
		 * \brief	Calculate the cross-product
		 * 
		 * In 2D there is no clear definition of this operation. 
		 * 
		 * This implementation is the most common one and will return the
		 * magnitude of the vector that would result from a regular 
		 * 3D cross product of the input vectors, taking their Z values
		 * implicitly as 0 (i.e. treating the 2D space as a plane in the 3D space).
		 * The 3D cross product will be perpendicular to that plane, and thus
		 * have 0 X & Y components (thus the scalar returned is the Z value of
		 * the 3D cross product vector).
		 * 
		 * \code
		 * this.x * other.y - this.y * other.x
		 * \endcode
		 * 
		 * Other implementations take no arguments and returns a vector
		 * perpendicular to the input vector. This can be reached with the
		 * toOrthogonalVector() method, which returns a perpendicular copy
		 * of the vector.
		 */
		WideType
		cross(const Vector2D<T>& other) const;
		
		/**
		 * \brief	Convert between Point-objects with different base-types
		 */
		template<typename U>
		Vector2D<U>
		convert() const;
		
		/**
		 * \brief	Returns a perpendicular copy of the vector
		 * 
		 * \return	(y, -x)
		 */
		Vector2D
		toOrthogonalVector() const;
		
		
		Vector2D
		operator - () const;
		
		Vector2D
		operator - (const Vector2D &other) const;
	
		Vector2D
		operator + (const Vector2D &other) const;
		
		Vector2D&
		operator += (const Vector2D &other);
		
		Vector2D&
		operator -= (const Vector2D &other);
		
		Vector2D
		operator * (float scale) const;
		
		Vector2D
		operator / (float scale) const;
		
		bool
		operator == (const Vector2D &other) const;
		
		bool
		operator != (const Vector2D &other) const;
		
		/**
		 * \brief	Check if three points are in a counter-clock wise direction
		 * 
		 * Check if we move counter-clock wise if we move from the first point
		 * to the second and the third.
		 * 
		 * \returns	1 if counter-clock wise, -1 if clock wise and 0 if \p c is
		 * 			between \p a and \p b.
		 */
		static int_fast8_t
		ccw(const Vector2D<T>& a, const Vector2D<T>& b, const Vector2D<T>& c);
		
	protected:
		T x;
		T y;
		
	protected:
		template<typename U>
		friend IOStream&
		operator <<(IOStream& s, const Vector2D<U>& c);
		
		template<typename U>
		friend Vector2D<U>
		operator * (float scale, const Vector2D<U> &vector);
	};
	
	// ------------------------------------------------------------------------
	// Global functions
	// ------------------------------------------------------------------------
	/**
	 * \brief	Stream operator for \b xpcc::Vector2D<U>
	 * \ingroup	geometry
	 */
	template<typename U>
	IOStream&
	operator << (IOStream& s, const Vector2D<U>& c);
	
	/**
	 * \brief	Scalar multiplication
	 * \ingroup	geometry
	 */
	template<typename U>
	Vector2D<U>
	operator * (float scale, const Vector2D<U> &vector);
	
	template<typename U>
	Vector2D<U>
	operator / (float scale, const Vector2D<U> &vector);
	
	// ------------------------------------------------------------------------
	// Declaration of specialized methods
	// ------------------------------------------------------------------------
	template<>
	int16_t
	Vector2D<int16_t>::getLength() const;
	
	template<>
	int32_t
	Vector2D<int16_t>::getLengthSquared() const;
	
	template<>
	int32_t
	Vector2D<int16_t>::dot(const xpcc::Vector2D<int16_t>& other) const;
	
	
	template<> template<>
	Vector2D<double>
	Vector2D<float>::convert() const;
	
	template<> template<>
	Vector2D<float>
	Vector2D<double>::convert() const;
	
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
}

#include "vector_2d_impl.hpp"

#endif	// XPCC__VECTOR_2D_HPP
