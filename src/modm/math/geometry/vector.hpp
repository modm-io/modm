/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
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

#ifndef MODM_VECTOR_HPP
#define MODM_VECTOR_HPP

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <stdint.h>
#include <type_traits>

#include <modm/io/iostream.hpp>
#include <modm/math/matrix.hpp>
#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/math/utils/operator.hpp>

#include "angle.hpp"
#include "geometric_traits.hpp"

namespace modm
{
	// forward declaration
	template<typename T, uint8_t W, uint8_t H> class Matrix;
	template<typename T> class Location2D;
	template<typename T, uint8_t N> class Vector;

	/// @cond
	namespace detail
	{
		template<typename T, uint8_t N>
		struct VectorStorage
		{
			T coords[N];
			constexpr bool operator==(const VectorStorage&) const = default;
		};

		template<typename T>
		struct VectorStorage<T, 1>
		{
			T x;
			constexpr bool operator==(const VectorStorage&) const = default;
		};

		template<typename T>
		struct VectorStorage<T, 2>
		{
			T x;
			T y;
			using WideType = typename GeometricTraits<T>::WideType;
			using FloatType = typename GeometricTraits<T>::FloatType;
			constexpr bool operator==(const VectorStorage&) const = default;
		};

		template<typename T>
		struct VectorStorage<T, 3>
		{
			T x;
			T y;
			T z;
			constexpr bool operator==(const VectorStorage&) const = default;
		};

		template<typename T>
		struct VectorStorage<T, 4>
		{
			T x;
			T y;
			T z;
			T w;
			constexpr bool operator==(const VectorStorage&) const = default;
		};

		template<typename T, typename Arg>
		constexpr uint8_t vectorArgSize = std::is_convertible_v<Arg, T> ? 1 : 0;

		template<typename T, uint8_t M>
		constexpr uint8_t vectorArgSize<T, Vector<T, M>> = M;
	}
	/// @endcond

#define IMPLEMENT_VECTOR_ACCESSOR2(a, b)								\
		constexpr Vector<T, 2> a##b() const								\
			requires (hasComponent(#a[0]) and hasComponent(#b[0]))		\
		{																\
			return Vector<T, 2>(this->a, this->b);						\
		}

#define IMPLEMENT_VECTOR_ACCESSOR3(a, b, c)								\
		constexpr Vector<T, 3> a##b##c() const							\
			requires (hasComponent(#a[0]) and hasComponent(#b[0]) and	\
					  hasComponent(#c[0]))								\
		{																\
			return Vector<T, 3>(this->a, this->b, this->c);				\
		}

#define IMPLEMENT_VECTOR_ACCESSOR4(a, b, c, d)							\
		constexpr Vector<T, 4> a##b##c##d() const						\
			requires (hasComponent(#a[0]) and hasComponent(#b[0]) and	\
					  hasComponent(#c[0]) and hasComponent(#d[0]))		\
		{																\
			return Vector<T, 4>(this->a, this->b, this->c, this->d);	\
		}

	/**
	 * \brief	Class for handling common point operations
	 *
	 * Basic data type of all geometric operations. Used to represent vectors
	 * as well as particular points in the coordinate system.
	 *
	 * \section	point_vector	Point vs. vector
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
	 * Therefore there isn't a Point-class, but only a Vector class.
	 *
	 * Vectors with up to four dimensions name their components `x`, `y`,
	 * `z` and `w`, larger vectors store them in `coords`.
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Niklas Hauser
	 */
	template<typename T, uint8_t N>
	class Vector : public detail::VectorStorage<T, N>
	{
		friend class Location2D<T>;

		static constexpr bool
		hasComponent(char c)
		{
			return N <= 4 and uint8_t(c == 'w' ? 3 : c - 'x') < N;
		}

	public:
		constexpr Vector() : detail::VectorStorage<T, N>{} {}

		/// Implicit conversion from a scalar
		constexpr Vector(T inX) requires (N == 1)
		{ this->x = inX; }

		/// Sets all components to the same value
		explicit constexpr Vector(T inVal) requires (2 <= N and N <= 4)
		{ for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] = inVal; }

		/// Concatenates scalars and smaller vectors, e.g. Vector3(Vector2, z)
		template<typename... Args>
			requires (sizeof...(Args) >= 2 and
					  ((detail::vectorArgSize<T, Args> > 0) and ...) and
					  (detail::vectorArgSize<T, Args> + ...) == N)
		constexpr Vector(const Args&... args)
		{
			uint8_t index = 0;
			(append(index, args), ...);
		}

		constexpr Vector(const T *ptData) requires (N > 4)
		{ std::copy_n(ptData, N, this->coords); }

		template<typename U>
		explicit constexpr Vector(const U *array) requires (N == 3)
		{ for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] = array[ii]; }

		template<typename U>
		constexpr Vector(const Vector<U, N> &rhs) requires (N == 3 and not std::is_same_v<T, U>)
		{ for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] = rhs[ii]; }

		constexpr Vector(const Matrix<T, N, 1> &rhs)
		{ *this = rhs; }

		constexpr Vector&
		operator = (const Matrix<T, N, 1> &rhs)
		{
			for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] = rhs.element[ii];
			return *this;
		}

		// Setters and getters
		constexpr void
		set(const T& x) requires (N == 1)
		{ this->x = x; }

		constexpr void
		set(const T& x, const T& y) requires (N == 2)
		{ *this = Vector(x, y); }

		constexpr void
		set(const T& x, const T& y, const T& z) requires (N == 3)
		{ *this = Vector(x, y, z); }

		constexpr void
		set(const T& x, const T& y, const T& z, const T& w) requires (N == 4)
		{ *this = Vector(x, y, z, w); }

		constexpr void setX(const T& value) requires (N <= 4) { this->x = value; }
		constexpr void setY(const T& value) requires (2 <= N and N <= 4) { this->y = value; }
		constexpr void setZ(const T& value) requires (3 <= N and N <= 4) { this->z = value; }
		constexpr void setW(const T& value) requires (N == 4) { this->w = value; }

		constexpr const T& getX() const requires (N <= 4) { return this->x; }
		constexpr const T& getY() const requires (2 <= N and N <= 4) { return this->y; }
		constexpr const T& getZ() const requires (3 <= N and N <= 4) { return this->z; }
		constexpr const T& getW() const requires (N == 4) { return this->w; }

		static constexpr uint8_t
		getSize()
		{ return N; }

		// Element access
		constexpr const T&
		operator [] (uint8_t index) const
		{ return const_cast<Vector&>(*this)[index]; }

		constexpr T&
		operator [] (uint8_t index)
		{
			if constexpr (N > 4) return this->coords[index];
			else if consteval
			{
				if constexpr (N >= 4) { if (index == 3) return this->w; }
				if constexpr (N >= 3) { if (index == 2) return this->z; }
				if constexpr (N >= 2) { if (index == 1) return this->y; }
				return this->x;
			}
			else { return ptr()[index]; }
		}

		constexpr T*
		ptr()
		{ return reinterpret_cast<T*>(this); }

		constexpr const T*
		ptr() const
		{ return reinterpret_cast<const T*>(this); }

		// Comparison is lexicographic
		constexpr bool operator == (const Vector &rhs) const = default;

		constexpr bool
		operator < (const Vector &rhs) const
		{ return compare(rhs, std::less{}, std::less{}); }

		constexpr bool
		operator <= (const Vector &rhs) const
		{ return compare(rhs, std::less{}, std::less_equal{}); }

		constexpr bool
		operator > (const Vector &rhs) const
		{ return compare(rhs, std::greater{}, std::greater{}); }

		constexpr bool
		operator >= (const Vector &rhs) const
		{ return compare(rhs, std::greater{}, std::greater_equal{}); }

		// Arithmetic
		constexpr Vector
		operator - () const
		{ return map([](const T& a) { return T(-a); }); }

		constexpr Vector
		operator + (const Vector &rhs) const
		{ return Vector(*this) += rhs; }

		constexpr Vector
		operator - (const Vector &rhs) const
		{ return Vector(*this) -= rhs; }

		/// Dot product
		constexpr T
		operator * (const Vector &rhs) const
		{
			T result{};
			for (uint8_t ii = 0; ii < N; ++ii) result += (*this)[ii] * rhs[ii];
			return result;
		}

		constexpr Vector
		operator * (const T &rhs) const requires (N != 2)
		{ return map([&](const T& a) { return T(a * rhs); }); }

		constexpr Vector
		operator / (const T &rhs) const requires (N != 2)
		{ return map([&](const T& a) { return T(a / rhs); }); }

		/// Scales and rounds to the nearest integer
		constexpr Vector
		operator * (float scale) const requires (N == 2)
		{ return map([&](const T& a) { return GeometricTraits<T>::round(a * scale); }); }

		/// Scales and rounds to the nearest integer
		constexpr Vector
		operator / (float scale) const requires (N == 2)
		{ return map([&](const T& a) { return GeometricTraits<T>::round(a / scale); }); }

		/// Cross product of the 2D vectors, which is the z component of the 3D cross product
		constexpr T
		operator ^ (const Vector &rhs) const requires (N == 2)
		{ return this->x * rhs.y - this->y * rhs.x; }

		/// Cross product
		constexpr Vector
		operator ^ (const Vector &rhs) const requires (N == 3)
		{
			return Vector(this->y * rhs.z - this->z * rhs.y,
						  this->z * rhs.x - this->x * rhs.z,
						  this->x * rhs.y - this->y * rhs.x);
		}

		/// Rotates the vector by 90° clockwise
		constexpr Vector&
		operator ~ () requires (N == 2)
		{ return *this = perpendicular(); }

		constexpr Vector&
		operator += (const Vector &rhs)
		{
			for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] += rhs[ii];
			return *this;
		}

		constexpr Vector&
		operator -= (const Vector &rhs)
		{
			for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] -= rhs[ii];
			return *this;
		}

		constexpr Vector&
		operator *= (const T &rhs)
		{
			for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] *= rhs;
			return *this;
		}

		constexpr Vector&
		operator /= (const T &rhs)
		{
			for (uint8_t ii = 0; ii < N; ++ii) (*this)[ii] /= rhs;
			return *this;
		}

		// Length
		/// 1D: absolute value, 2D: rounded to `T`, 3D and 4D: float, otherwise `T`
		constexpr auto
		getLength() const
		{
			if constexpr (N == 1) return T(std::abs(this->x));
			else if constexpr (N == 2)
			{
				if constexpr (std::is_same_v<T, int16_t>)
					return T(math::sqrt(getLengthSquared()));
				else
				{
					float tx = this->x;
					float ty = this->y;
					return GeometricTraits<T>::round(std::sqrt(tx*tx + ty*ty));
				}
			}
			else if constexpr (N <= 4) return float(std::sqrt(getLengthSquared()));
			else return T(std::sqrt(getLengthSquared()));
		}

		/// 2D: `WideType`, 3D and 4D: float, otherwise `T`
		constexpr auto
		getLengthSquared() const
		{
			if constexpr (N == 2)
			{
				using W = typename detail::VectorStorage<T, 2>::WideType;
				return W(W(this->x) * W(this->x) + W(this->y) * W(this->y));
			}
			else
			{
				decltype(T{} * T{}) result{};
				for (uint8_t ii = 0; ii < N; ++ii) result += (*this)[ii] * (*this)[ii];
				if constexpr (N == 3 or N == 4) return float(result);
				else return T(result);
			}
		}

		constexpr Vector&
		normalize() requires (N == 2)
		{
			const T length = getLength();
			this->x = this->x / length;
			this->y = this->y / length;
			return *this;
		}

		constexpr void
		normalize() requires (N == 3 or N == 4)
		{ scale(1.f); }

		constexpr Vector
		normalized() const requires (2 <= N and N <= 4)
		{
			if constexpr (N == 2) return Vector(*this).normalize();
			else return scaled(1.f);
		}

		constexpr Vector&
		scale(float length) requires (N == 2)
		{
			const float factor = length / getLength();
			this->x = this->x * factor;
			this->y = this->y * factor;
			return *this;
		}

		constexpr void
		scale(float newLength) requires (N == 3 or N == 4)
		{ *this = scaled(newLength); }

		constexpr Vector
		scaled(float newLength) const requires (2 <= N and N <= 4)
		{
			if constexpr (N == 2) return Vector(*this).scale(newLength);
			else return *this * T(newLength / getLength());
		}

		// 2D specific operations
		constexpr float
		getAngle() const requires (N == 2)
		{ return std::atan2(this->y, this->x); }

		constexpr Vector&
		rotate(float phi) requires (N == 2)
		{
			const float c = std::cos(phi);
			const float s = std::sin(phi);
			// without rounding the result might be false for T = integer
			const T tx = GeometricTraits<T>::round(c * this->x - s * this->y);
			this->y = GeometricTraits<T>::round(s * this->x + c * this->y);
			this->x = tx;
			return *this;
		}

		constexpr Vector&
		translate(const Vector& vector) requires (N == 2)
		{ return *this += vector; }

		constexpr auto
		getDistanceTo(const Vector& other) const requires (N == 2)
		{ return typename detail::VectorStorage<T, 2>::WideType((other - *this).getLength()); }

		constexpr float
		getAngleTo(const Vector& other) const requires (N == 2)
		{ return (other - *this).getAngle(); }

		constexpr auto
		dot(const Vector& other) const requires (N == 2)
		{
			using W = typename detail::VectorStorage<T, 2>::WideType;
			return W(W(this->x) * W(other.x) + W(this->y) * W(other.y));
		}

		constexpr auto
		cross(const Vector& other) const requires (N == 2)
		{
			using W = typename detail::VectorStorage<T, 2>::WideType;
			return W(W(this->x) * W(other.y) - W(this->y) * W(other.x));
		}

		/// Converts to another base type, float to integer conversions are rounded
		template<typename U>
		constexpr Vector<U, 2>
		convert() const requires (N == 2)
		{
			if constexpr (std::is_floating_point_v<T> and
						  (std::is_same_v<T, U> or not std::is_floating_point_v<U>))
				return Vector<U, 2>(U(std::round(this->x)), U(std::round(this->y)));
			else
				return Vector<U, 2>(U(this->x), U(this->y));
		}

		constexpr Vector
		toOrthogonalVector() const requires (N == 2)
		{ return Vector(this->y, -this->x); }

		constexpr Vector
		perpendicular() const requires (N == 2)
		{ return Vector(this->y, -this->x); }

		/**
		 * Checks if three points are in counter-clock wise order.
		 *
		 * @return	1 if counter-clockwise, -1 if clockwise and 0 if c is
		 *			on the line between a and b.
		 */
		static constexpr int_fast8_t
		ccw(const Vector& a, const Vector& b, const Vector& c) requires (N == 2)
		{
			using W = typename detail::VectorStorage<T, 2>::WideType;
			const W dx1 = b.x - a.x;
			const W dy1 = b.y - a.y;
			const W dx2 = c.x - a.x;
			const W dy2 = c.y - a.y;
			const W d1 = dx1 * dy2;
			const W d2 = dy1 * dx2;

			if (d1 > d2) return 1;
			if (d1 < d2) return -1;
			if ((dx1 * dx2 < 0) or (dy1 * dy2 < 0)) return -1;
			if ((dx1 * dx1 + dy1 * dy1) >= (dx2 * dx2 + dy2 * dy2)) return 0;
			return 1;
		}

		// Matrix views
		constexpr Matrix<T, N, 1>&
		asMatrix()
		{ return *reinterpret_cast<Matrix<T, N, 1>*>(this); }

		constexpr const Matrix<T, N, 1>&
		asMatrix() const
		{ return *reinterpret_cast<const Matrix<T, N, 1>*>(this); }

		constexpr Matrix<T, 1, N>&
		asTransposedMatrix()
		{ return *reinterpret_cast<Matrix<T, 1, N>*>(this); }

		constexpr const Matrix<T, 1, N>&
		asTransposedMatrix() const
		{ return *reinterpret_cast<const Matrix<T, 1, N>*>(this); }

		constexpr bool
		hasNan() const
		{
			for (uint8_t ii = 0; ii < N; ++ii) if (std::isnan((*this)[ii])) return true;
			return false;
		}

		constexpr bool
		hasInf() const
		{
			for (uint8_t ii = 0; ii < N; ++ii) if (std::isinf((*this)[ii])) return true;
			return false;
		}

#ifndef __DOXYGEN__
		IMPLEMENT_VECTOR_ACCESSOR2(x,x) IMPLEMENT_VECTOR_ACCESSOR2(x,y) IMPLEMENT_VECTOR_ACCESSOR2(x,z) IMPLEMENT_VECTOR_ACCESSOR2(x,w)
		IMPLEMENT_VECTOR_ACCESSOR2(y,x) IMPLEMENT_VECTOR_ACCESSOR2(y,y) IMPLEMENT_VECTOR_ACCESSOR2(y,z) IMPLEMENT_VECTOR_ACCESSOR2(y,w)
		IMPLEMENT_VECTOR_ACCESSOR2(z,x) IMPLEMENT_VECTOR_ACCESSOR2(z,y) IMPLEMENT_VECTOR_ACCESSOR2(z,z) IMPLEMENT_VECTOR_ACCESSOR2(z,w)
		IMPLEMENT_VECTOR_ACCESSOR2(w,x) IMPLEMENT_VECTOR_ACCESSOR2(w,y) IMPLEMENT_VECTOR_ACCESSOR2(w,z) IMPLEMENT_VECTOR_ACCESSOR2(w,w)

		IMPLEMENT_VECTOR_ACCESSOR3(x,x,x) IMPLEMENT_VECTOR_ACCESSOR3(x,x,y) IMPLEMENT_VECTOR_ACCESSOR3(x,x,z) IMPLEMENT_VECTOR_ACCESSOR3(x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(x,y,x) IMPLEMENT_VECTOR_ACCESSOR3(x,y,y) IMPLEMENT_VECTOR_ACCESSOR3(x,y,z) IMPLEMENT_VECTOR_ACCESSOR3(x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(x,z,x) IMPLEMENT_VECTOR_ACCESSOR3(x,z,y) IMPLEMENT_VECTOR_ACCESSOR3(x,z,z) IMPLEMENT_VECTOR_ACCESSOR3(x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR3(x,w,x) IMPLEMENT_VECTOR_ACCESSOR3(x,w,y) IMPLEMENT_VECTOR_ACCESSOR3(x,w,z) IMPLEMENT_VECTOR_ACCESSOR3(x,w,w)
		IMPLEMENT_VECTOR_ACCESSOR3(y,x,x) IMPLEMENT_VECTOR_ACCESSOR3(y,x,y) IMPLEMENT_VECTOR_ACCESSOR3(y,x,z) IMPLEMENT_VECTOR_ACCESSOR3(y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(y,y,x) IMPLEMENT_VECTOR_ACCESSOR3(y,y,y) IMPLEMENT_VECTOR_ACCESSOR3(y,y,z) IMPLEMENT_VECTOR_ACCESSOR3(y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(y,z,x) IMPLEMENT_VECTOR_ACCESSOR3(y,z,y) IMPLEMENT_VECTOR_ACCESSOR3(y,z,z) IMPLEMENT_VECTOR_ACCESSOR3(y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR3(y,w,x) IMPLEMENT_VECTOR_ACCESSOR3(y,w,y) IMPLEMENT_VECTOR_ACCESSOR3(y,w,z) IMPLEMENT_VECTOR_ACCESSOR3(y,w,w)
		IMPLEMENT_VECTOR_ACCESSOR3(z,x,x) IMPLEMENT_VECTOR_ACCESSOR3(z,x,y) IMPLEMENT_VECTOR_ACCESSOR3(z,x,z) IMPLEMENT_VECTOR_ACCESSOR3(z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(z,y,x) IMPLEMENT_VECTOR_ACCESSOR3(z,y,y) IMPLEMENT_VECTOR_ACCESSOR3(z,y,z) IMPLEMENT_VECTOR_ACCESSOR3(z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(z,z,x) IMPLEMENT_VECTOR_ACCESSOR3(z,z,y) IMPLEMENT_VECTOR_ACCESSOR3(z,z,z) IMPLEMENT_VECTOR_ACCESSOR3(z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR3(z,w,x) IMPLEMENT_VECTOR_ACCESSOR3(z,w,y) IMPLEMENT_VECTOR_ACCESSOR3(z,w,z) IMPLEMENT_VECTOR_ACCESSOR3(z,w,w)
		IMPLEMENT_VECTOR_ACCESSOR3(w,x,x) IMPLEMENT_VECTOR_ACCESSOR3(w,x,y) IMPLEMENT_VECTOR_ACCESSOR3(w,x,z) IMPLEMENT_VECTOR_ACCESSOR3(w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(w,y,x) IMPLEMENT_VECTOR_ACCESSOR3(w,y,y) IMPLEMENT_VECTOR_ACCESSOR3(w,y,z) IMPLEMENT_VECTOR_ACCESSOR3(w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(w,z,x) IMPLEMENT_VECTOR_ACCESSOR3(w,z,y) IMPLEMENT_VECTOR_ACCESSOR3(w,z,z) IMPLEMENT_VECTOR_ACCESSOR3(w,z,w)
		IMPLEMENT_VECTOR_ACCESSOR3(w,w,x) IMPLEMENT_VECTOR_ACCESSOR3(w,w,y) IMPLEMENT_VECTOR_ACCESSOR3(w,w,z) IMPLEMENT_VECTOR_ACCESSOR3(w,w,w)

		IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,w,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,w,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,w,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,w,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,w,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,w,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,w,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,w,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,w,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,w,w,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,w,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,w,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,w,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,w,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,w,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,w,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,w,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,w,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,w,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,w,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,w,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,w,w,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,w,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,w,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,w,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,w,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,w,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,w,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,w,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,w,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,w,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,w,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,w,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,w,w,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,w,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,w,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,x,w,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,w,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,w,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,y,w,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,w,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,w,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,z,w,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,w,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,w,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,w,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,w,w,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,w,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,w,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,w,w)
#endif

	private:
		template<typename Arg>
		constexpr void
		append(uint8_t& index, const Arg& arg)
		{
			if constexpr (std::is_convertible_v<Arg, T>) (*this)[index++] = static_cast<T>(arg);
			else for (uint8_t ii = 0; ii < Arg::getSize(); ++ii) (*this)[index++] = arg[ii];
		}

		template<typename F>
		constexpr Vector
		map(F&& function) const
		{
			Vector result;
			for (uint8_t ii = 0; ii < N; ++ii) result[ii] = function((*this)[ii]);
			return result;
		}

		/// Compares the leading components strictly and the last one with `last`
		template<typename Strict, typename Last>
		constexpr bool
		compare(const Vector &rhs, Strict strict, Last last) const
		{
			for (uint8_t ii = 0; ii < N - 1; ++ii)
			{
				if (strict((*this)[ii], rhs[ii])) return true;
				if (not ((*this)[ii] == rhs[ii])) return false;
			}
			return last((*this)[N - 1], rhs[N - 1]);
		}
	};

	template<typename U, typename T, uint8_t N>
		requires (N == 1 or N == 3 or N == 4)
	constexpr Vector<T, N>
	operator * (const U &lhs, const Vector<T, N> &rhs)
	{ return rhs * lhs; }

	template<typename T, typename U, uint8_t N>
		requires (N == 3 or N == 4)
	constexpr Vector<U, N>
	operator * (const Matrix<T, N, N> &lhs, const Vector<U, N> &rhs)
	{ return lhs * rhs.asMatrix(); }

	template<typename U>
	constexpr Vector<U, 2>
	operator * (float scale, const Vector<U, 2> &vector)
	{ return vector * scale; }

	template<typename U>
	constexpr Vector<U, 2>
	operator / (float scale, const Vector<U, 2> &vector)
	{ return vector / scale; }

	template<typename U>
	IOStream&
	operator << (IOStream& os, const Vector<U, 2>& c)
	{ return os << "x=" << c.x << ", y=" << c.y; }

	using Vector1f = Vector<float, 1>;
	using Vector1i = Vector<int16_t, 1>;

	using Vector2f = Vector<float, 2>;
	using Vector2i = Vector<int16_t, 2>;
	using Vector2u = Vector<uint16_t, 2>;

	using Vector3f = Vector<float, 3>;
	using Vector3i = Vector<int16_t, 3>;
	using Vector3u = Vector<uint16_t, 3>;
	using Vector3li = Vector<int32_t, 3>;
	using Vector3lu = Vector<uint32_t, 3>;

	using Vector4f = Vector<float, 4>;
	using Vector4i = Vector<int16_t, 4>;
	using Vector4u = Vector<uint16_t, 4>;

	template< typename T, uint8_t N >
	struct detail::MakeSigned< Vector<T, N> >
	{ using type = Vector< std::make_signed_t<T>, N >; };

	template< typename T, uint8_t N >
	struct detail::MakeUnsigned< Vector<T, N> >
	{ using type = Vector< std::make_unsigned_t<T>, N >; };

	template< typename T, uint8_t N >
	struct detail::WideType< Vector<T, N> >
	{ using type = Vector< WideType<T>, N >; };
}

#endif // MODM_VECTOR_HPP
