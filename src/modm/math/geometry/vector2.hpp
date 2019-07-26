/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VECTOR2_HPP
#define MODM_VECTOR2_HPP

#include <cmath>
#include <stdint.h>
#include <modm/io/iostream.hpp>

#include "geometric_traits.hpp"
#include "angle.hpp"
#include "vector.hpp"

namespace modm
{
	// forward declaration
	template <typename T>
	class Location2D;

	/**
	 * \brief	Class for handling common vector operations (2D)
	 *
	 * Operations:
	 * \code
	 *	+ : addition of points
	 *	- : different of points
	 *	* : dot product or scalar multiplication
	 *	/ : scalar division
	 *	^ : cross product (determinant)
	 *	~ : perpendicular
	 * \endcode
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com)
	 * and heavily modified.
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 *
	 * \author	Fabian Greif
	 * \author	Niklas Hauser
	 *
	 * \ingroup	modm_math_geometry
	 */
	template<typename T>
	class Vector<T, 2>
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
		Vector();

		Vector(const T& inX, const T& inY);

		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY);
		Vector(const T &inX, const Vector<T, 1> &inY);
		Vector(const Vector<T, 1> &inX, const T &inY);
		explicit Vector(T inVal);
		Vector(const Matrix<T, 2, 1> &rhs);

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
		Vector&
		normalize();

		Vector
		normalized() const;

		/**
		 * \brief	Scale the vector to \p length
		 */
		Vector&
		scale(float length);

		Vector
		scaled(float length) const;


		Vector&
		rotate(float phi);

		/**
		 * \brief	Move the point in x and y direction
		 */
		Vector&
		translate(const Vector& vector);


		WideType
		getDistanceTo(const Vector& other) const;

		float
		getAngleTo(const Vector& other) const;

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
		dot(const Vector& other) const;

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
		cross(const Vector& other) const;

		/**
		 * \brief	Convert between Point-objects with different base-types
		 */
		template<typename U>
		Vector<U, 2>
		convert() const;

		/**
		 * \brief	Returns a perpendicular copy of the vector
		 *
		 * \return	(y, -x)
		 */
		Vector
		toOrthogonalVector() const;

		// TODO
		Vector
		perpendicular() const;

		/**
		 * \brief	Check if three points are in a counter-clock wise direction
		 *
		 * Check if we move counter-clock wise if we move from the first point
		 * to the second and the third.
		 *
		 * If all three points are in a line there are three possibilities:
		 *  1) strait line: third point behind the second (returns 1)
		 *  2) last point between the other two (returns 0)
		 *  3) third point before the first one (returns -1)
		 *
		 * This definition is useful for inclusion or intersection testing.
		 */
		static int_fast8_t
		ccw(const Vector& a, const Vector& b, const Vector& c);

		Vector& operator = (const Matrix<T, 2, 1> &rhs);

		bool operator == (const Vector &rhs) const;
		bool operator != (const Vector &rhs) const;
		bool operator < (const Vector &rhs) const;
		bool operator <= (const Vector &rhs) const;
		bool operator > (const Vector &rhs) const;
		bool operator >= (const Vector &rhs) const;

		const T& operator [] (uint8_t index) const;
		T& operator [] (uint8_t index);

		T* ptr();
		const T* ptr() const;

		Vector operator - () const;
		Vector operator - (const Vector &rhs) const;
		Vector operator + (const Vector &rhs) const;
		T operator * (const Vector &rhs) const;
		T operator ^ (const Vector &rhs) const;
		Vector operator * (float rhs) const;
		Vector operator / (float rhs) const;

		Vector& operator += (const Vector &rhs);
		Vector& operator -= (const Vector &rhs);
		Vector& operator *= (const T &rhs);
		Vector& operator /= (const T &rhs);
		Vector& operator ~ ();

		Matrix<T, 2, 1>&
		asMatrix();

		const Matrix<T, 2, 1>&
		asMatrix() const;

		Matrix<T, 1, 2>&
		asTransposedMatrix();

		const Matrix<T, 1, 2>&
		asTransposedMatrix() const;

		bool hasNan() const;
		bool hasInf() const;

	#ifndef __DOXYGEN__
		IMPLEMENT_VECTOR_ACCESSOR2(x,x); IMPLEMENT_VECTOR_ACCESSOR2(x,y);
		IMPLEMENT_VECTOR_ACCESSOR2(y,x); IMPLEMENT_VECTOR_ACCESSOR2(y,y);

		IMPLEMENT_VECTOR_ACCESSOR3(x,x,x); IMPLEMENT_VECTOR_ACCESSOR3(x,x,y);
		IMPLEMENT_VECTOR_ACCESSOR3(x,y,x); IMPLEMENT_VECTOR_ACCESSOR3(x,y,y);
		IMPLEMENT_VECTOR_ACCESSOR3(y,x,x); IMPLEMENT_VECTOR_ACCESSOR3(y,x,y);
		IMPLEMENT_VECTOR_ACCESSOR3(y,y,x); IMPLEMENT_VECTOR_ACCESSOR3(y,y,y);

		IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,y);
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,y);
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,y);
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,y);

		IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,y);
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,y);
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,y);
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,y);
	#endif

	public:
		T x;
		T y;

	protected:
		template<typename U>
		friend IOStream&
		operator <<(IOStream& os, const Vector<U, 2>& c);

		template<typename U>
		friend Vector<U, 2>
		operator * (float scale, const Vector<U, 2> &vector);
	};

	typedef Vector<float, 2> 		Vector2f;
	typedef Vector<int16_t, 2> 		Vector2i;
	typedef Vector<uint16_t, 2> 	Vector2u;

	// ------------------------------------------------------------------------
	// Global functions
	// ------------------------------------------------------------------------
	/**
	 * \brief	Stream operator for \b modm::Vector<U, 2>
	 * \ingroup	modm_math_geometry
	 */
	template<typename U>
	IOStream&
	operator << (IOStream& os, const Vector<U, 2>& c);

	/**
	 * \brief	Scalar multiplication
	 * \ingroup	modm_math_geometry
	 */
	template<typename U>
	Vector<U, 2>
	operator * (float scale, const Vector<U, 2> &vector);

	/**
	 * \brief	Scalar division
	 * \ingroup	modm_math_geometry
	 */
	template<typename U>
	Vector<U, 2>
	operator / (float scale, const Vector<U, 2> &vector);

	// ------------------------------------------------------------------------
	// Declaration of specialized methods
	// ------------------------------------------------------------------------
	template<>
	int16_t
	Vector<int16_t, 2>::getLength() const;

	template<>
	int32_t
	Vector<int16_t, 2>::getLengthSquared() const;

	template<>
	int32_t
	Vector<int16_t, 2>::dot(const modm::Vector<int16_t, 2>& other) const;


	template<> template<>
	Vector<double, 2>
	Vector<float, 2>::convert() const;

	template<> template<>
	Vector<float, 2>
	Vector<double, 2>::convert() const;

	// round for everything that's not float => double or double => float
	template<> template<typename U>
	Vector<U, 2>
	Vector<float, 2>::convert() const
	{
		return Vector<U, 2>(round(this->x), round(this->y));
	}

	template<> template<typename U>
	Vector<U, 2>
	Vector<double, 2>::convert() const
	{
		return Vector<U, 2>(round(this->x), round(this->y));
	}
}

#include "vector2_impl.hpp"

#endif // MODM_VECTOR2_HPP
