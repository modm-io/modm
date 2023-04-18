/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <functional>

#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/math/utils/integer_traits.hpp>
#include <modm/io/iostream.hpp>

#include "../matrix.hpp"
#include "angle.hpp"

namespace modm
{

/**
 * \brief	Class for handling common vector operations
 *
 * Arithmetic Operations:
 * \code
 *	+ : plus translation
*	- : minus translation
*	* : scalar multiplication / dot product
*	/ : scalar division
*	^ : cross product / determinant
*	~ : perpendicular / orthogonal
* \endcode
*
* \author	Fabian Greif
* \author	Niklas Hauser
* \author	Thomas Sommer
* \ingroup	modm_math_geometry
*/
template<class T, std::size_t N>
requires (N > 0)
class Vector : public std::array<T, N>
{
	using WideType = std::conditional<std::is_floating_point_v<T>, T, modm::WideType<T>>::type;
	using FloatType = std::conditional<std::is_floating_point_v<T>, T, float>::type;

	template<std::size_t K>
	constexpr void assign()
	{
		static_assert(K == N, "Number of components passed to constructor must be N!");
	};

	template<std::size_t K, typename U, class... Args>
	requires std::convertible_to<U, T>
	constexpr void assign(U v, Args... args)
	{
		*(this->begin() + K) = modm::round_smart<T, U>(v);
		assign<K + 1>(args...);
	}

	template<std::size_t K, typename U, std::size_t M, class... Args>
	constexpr void assign(const std::array<U, M> &arr, Args... args)
	{
		std::transform(arr.begin(), arr.end(), this->begin() + K, [](U v) { return modm::round_smart<T, U>(v); });
		assign<K + M>(args...);
	}

public:
	constexpr Vector()
	{
		this->fill(0);
	}

	template <typename U>
	requires std::convertible_to<U, T>
	constexpr explicit Vector(U v)
	{
		this->fill(modm::round_smart<T, U>(v));
	}

	template <typename U>
	constexpr Vector(const U (&arr)[N])
	{
		std::transform(std::begin(arr), std::end(arr), this->begin(), [] (U v) { return modm::round_smart<T, U>(v); });
	}

	/**
	 * @brief 			Multi purpose constructor.
	 *
	 * @param args		Scalar type(s), modm::Vector(s) or std::array(s) in any order
	 */
	template <class... Args>
	constexpr Vector(Args... args)
	{
		assign<0>(args...);
	}

	template<typename U>
	[[deprecated("Use constructor instead!")]]
	Vector<U, N> convert() const
	{
		return {*this};
	}

	[[deprecated("Use Vector<T, N>::data() instead!")]]
	T* ptr() {
		return this->data();
	}

	[[deprecated("Use Vector<T, N>::data() instead!")]]
	const T* ptr() const {
		return this->data();
	}

	// TODO matrix constructor
	/* constexpr Vector(const Matrix<T, N, 1> &rhs)
	{
		std::copy(std::begin(rhs.element), std::end(rhs.element), this->begin());
	} */

	// matrix assignment
	/* constexpr Vector& operator= (const Matrix<T, N, 1> &rhs)
	{
		std::copy(std::begin(rhs.element), std::end(rhs.element), this->begin());
		return *this;
	} */

	// accessors for x, y, z, w, v, u
	constexpr T x() const { return this->operator[](0); }
	constexpr T& x() { return this->operator[](0); }

	constexpr T y() const requires (N > 1) { return this->operator[](1); }
	constexpr T& y() requires (N > 1) { return this->operator[](1); }

	constexpr T z() const requires (N > 2) { return this->operator[](2); }
	constexpr T& z() requires (N > 2) { return this->operator[](2); }

	constexpr T w() const requires (N > 3) { return this->operator[](3); }
	constexpr T& w() requires (N > 3) { return this->operator[](3); }

	constexpr T v() const requires (N > 4) { return this->operator[](4); }
	constexpr T& v() requires (N > 4) { return this->operator[](4); }

	constexpr T u() const requires (N > 5) { return this->operator[](5); }
	constexpr T& u() requires (N > 5) { return this->operator[](5); }

private:
	/**
	 * @brief 			Creates a new Vector by applying a binary function between each
	 * 					item of this and each item of other
	 *
	 * @tparam 	Func 	Binary function
	 */
	template <class BinFunc, typename U = T>
	constexpr Vector<U, N>
	calcNewVector(const Vector &other) const
	{
		Vector<U, N> res;

		// no clue why, but RVO / Loop unrolling does not apply here ...
		// std::transform(this->begin(), this->end(), other.begin(), res.begin(), Func{});

		// ... but this optimizes perfectly.
		res.x() = BinFunc{}(this->x(), other.x());
		if constexpr(N > 1) res.y() = BinFunc{}(this->y(), other.y());
		if constexpr(N > 2) res.z() = BinFunc{}(this->z(), other.z());
		if constexpr(N > 3) res.w() = BinFunc{}(this->w(), other.w());
		if constexpr(N > 4) res.v() = BinFunc{}(this->v(), other.v());
		if constexpr(N > 5) res.u() = BinFunc{}(this->u(), other.u());

		return res;
	}

	/**
	 * @brief 				Creates a new Vector by applying a binary function on each
	 * 						item of this and a scalar
	 *
	 * @tparam 	BinFunc 	Binary function
	 */
	template <class BinFunc, typename U = T>
	constexpr Vector<U, N>
	calcNewVector(const U scalar) const
	{
		Vector<U, N> res;

		// no clue why, but RVO / Loop unrolling does not apply here ...
		// std::transform(this->begin(), this->end(), res.begin(), Func{});

		// ... but this optimizes perfectly.
		res.x() = BinFunc{}(this->x(), scalar);
		if constexpr(N > 1) res.y() = BinFunc{}(this->y(), scalar);
		if constexpr(N > 2) res.z() = BinFunc{}(this->z(), scalar);
		if constexpr(N > 3) res.w() = BinFunc{}(this->w(), scalar);
		if constexpr(N > 4) res.v() = BinFunc{}(this->v(), scalar);
		if constexpr(N > 5) res.u() = BinFunc{}(this->u(), scalar);

		return res;
	}

	/**
	 * @brief 			Creates a new Vector by applying a unary function on each
	 * 					item of this
	 *
	 * @tparam 	UnFunc 	Unary function
	 */
	template <class UnFunc, typename U = T>
	constexpr Vector<U, N>
	calcNewVector() const
	{
		Vector<U, N> res;

		// no clue why, but RVO / Loop unrolling does not apply here ...
		// std::transform(this->begin(), this->end(), res.begin(), Func{});

		// ... but this optimizes perfectly.
		res.x() = UnFunc{}(this->x());
		if constexpr(N > 1) res.y() = UnFunc{}(this->y());
		if constexpr(N > 2) res.z() = UnFunc{}(this->z());
		if constexpr(N > 3) res.w() = UnFunc{}(this->w());
		if constexpr(N > 4) res.v() = UnFunc{}(this->v());
		if constexpr(N > 5) res.u() = UnFunc{}(this->u());

		return res;
	}

public:

	// arithmetic operators
	constexpr Vector
	operator+(const Vector &other) const
	{ return calcNewVector< std::plus<T> >(other); }

	constexpr Vector
	operator-(const Vector &other) const
	{ return calcNewVector< std::minus<T> >(other); }

	template<typename U>
	constexpr Vector
	operator* (U scale) const
	{ return calcNewVector< std::multiplies<U> >(scale); }

	template<typename U>
	constexpr Vector
	operator/ (U scale) const
	{ return calcNewVector< std::divides<U> >(scale); }

	constexpr Vector
	operator-() const
	requires std::is_signed_v<T>
	{ return calcNewVector< std::negate<T> >(); }

	// TODO treat optimal return type
	constexpr WideType
	operator*(const Vector &other) const
	{
		auto tmp = calcNewVector<std::multiplies<WideType>, WideType>(other);
		return std::accumulate(tmp.begin(), tmp.end(), WideType(0));
	}

	[[deprecated("Use Vector<T, N>::operator*() instead!")]]
	constexpr WideType
	dot(const Vector &other) const
	{ return operator*(other); }

	Vector& operator+= (const Vector &other)
	{
		std::transform(this->begin(), this->end(), other.begin(), this->begin(), std::plus<T>());
		return *this;
	}

	[[deprecated("Use Vector<T, N>::operator+= instead!")]]
	Vector& translate(Vector v)
	{ operator+=(v); return *this; }

	Vector& operator-= (const Vector &other) {
		std::transform(this->begin(), this->end(), other.begin(), this->begin(), std::minus<T>());
		return *this;
	}

	template<typename U>
	Vector& operator*= (U scale) {
		std::transform(this->begin(), this->end(), this->begin(), [=] (T v) { return v * scale; });
		return *this;
	}

	template<typename U>
	Vector& operator/= (U scale) {
		std::transform(this->begin(), this->end(), this->begin(), [=] (T v) { return v / scale; });
		return *this;
	}

private:

	// TODO treat optimal return type
	constexpr WideType sum() const
	{ return std::accumulate(this->begin(), this->end(), WideType(0)); }

public:
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
	WideType operator^ (const Vector &other) const requires (N == 2)
	{
		return WideType(x()) * WideType(other.y()) - WideType(y()) * WideType(other.x());
	}

	Vector operator^ (const Vector &other) const requires (N == 3)
	{
		return Vector(
			y() * other.z() - z() * other.y(),
			z() * other.x() - x() * other.z(),
			x() * other.y() - y() * other.x()
		);
	}

	[[deprecated("Use Vector<T, N>::operator^() instead!")]]
	constexpr WideType cross(const Vector& other) const
	{ return operator^(other); }
	// auto cross = operator^; // Why is this 'function alias' not accepted?

	// various methods for the lengh of the vector

	constexpr auto getLengthSquared() const {
		// TODO treat the right CalcType
		// - is always unsigned
		// - Must fit std::pow(std::numeric_limits<T>::max(), 2) * N
		using CalcType = WideType;

		CalcType sum{0};

		for(auto &v: *this)
			sum += WideType(v) * WideType(v);
		return sum;
	}

	template<typename TR = T>
	constexpr TR getLength() const
	{ return modm::round_smart<TR>(std::sqrt(getLengthSquared())); }

	constexpr WideType getDistanceTo(const Vector& other) const
	{ return (other - *this).getLength(); }

	constexpr Vector& scale(float length)
	{
		operator*=( length / getLength() );
		return *this;
	}

	constexpr Vector scaled(float length) const
	{ return *this * (length / getLength()); }

	// methods for float Vectors only

	// Normalize length to 1
	constexpr Vector& normalize()
	requires std::floating_point<T>
	{
		operator/=( getLength() );
		return *this;
	}

	constexpr Vector normalized() const
	requires std::floating_point<T>
	{ return *this / getLength(); }

	constexpr bool hasNan() const
	requires std::floating_point<T>
	{ return std::any_of(this->begin(), this->end(), [](T c){return std::isnan(c);}); }

	constexpr bool hasInf() const
	requires std::floating_point<T>
	{ return std::any_of(this->begin(), this->end(), [](T c){return std::isinf(c);}); }

	constexpr bool hasNan() const
	{ return false; }

	constexpr bool hasInf() const
	{ return false; }

	// methods for 2D only

	constexpr Vector operator~ () const
	requires std::is_signed_v<T> && (N == 2)
	{ return Vector(y(), -x()); }

	[[deprecated("Use Vector<T, N>::operator~() instead!")]]
	Vector perpendicular() const requires std::is_signed_v<T> && (N == 2)
	{ return operator~(); }

	[[deprecated("Use Vector<T, N>::operator~() instead!")]]
	Vector toOrthogonalVector() const requires std::is_signed_v<T> && (N == 2)
	{ return operator~(); }

	constexpr float getAngle() const requires (N == 2)
	{ return std::atan2(y(), x()); }

	constexpr float getAngleTo(const Vector& other) const requires (N == 2)
	{ return (other - *this).getAngle(); }

	// TODO implement as operator+=(Angle phi), operator-=(Angle phi) ??
	constexpr Vector& rotate(float phi) requires (N == 2)
	{
		const float c = std::cos(phi);
		const float s = std::sin(phi);
		const Vector<float, 2> tmp(c * x() - s * y(), s * x() + c * y());
		*this = tmp;

		return *this;
	}

private:
	template<typename U, std::size_t M>
	friend IOStream &
	operator<<(IOStream &os, const Vector<U, M> &c);
};

template<typename U, std::size_t N>
Vector<U, N>
operator* (float scale, const Vector<U, N> &vector)
{ return vector * scale; }

template<typename U, std::size_t N>
Vector<U, N>
operator/ (float scale, const Vector<U, N> &vector)
{ return vector / scale; }

using Vector1f = Vector<float, 1>;
using Vector1i = Vector<int16_t, 1>;
using Vector1u = Vector<uint16_t, 1>;

using Vector2f = Vector<float, 2>;
using Vector2i = Vector<int16_t, 2>;
using Vector2u = Vector<uint16_t, 2>;

using Vector3f = Vector<float, 3>;
using Vector3i = Vector<int16_t, 3>;
using Vector3u = Vector<uint16_t, 3>;

using Vector4f = Vector<float, 4>;
using Vector4i = Vector<int16_t, 4>;
using Vector4u = Vector<uint16_t, 4>;

#if __has_include(<modm/io/iostream.hpp>)
template<typename U, std::size_t M>
IOStream &
operator<<(IOStream &os, const Vector<U, M> &v)
{
	// Whitout index
	// for (auto &i : v) {
	// 	os << i << "\t";

	// With letter index
	for (std::size_t i = 0; i < M; ++i)
		os << char(i < 3 ? i + 120 : 122 - i) << ":" << v[i] << "\t";

	return os;
}
#endif

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
template<typename T>
constexpr int8_t
ccw(Vector<T, 2> a, Vector<T, 2> b, Vector<T, 2> c) {
	using WideType = modm::WideType<T>;

	const Vector<WideType, 2> v1 = b - a;
	const Vector<WideType, 2> v2 = c - a;
	const WideType d1 = v1.x() * v2.y();
	const WideType d2 = v1.y() * v2.x();

	if (d1 > d2)
		return 1;
	else if (d1 < d2)
		return -1;
	else
	{
		if ((v1.x() * v2.x() < 0) || (v1.y() * v2.y() < 0))
			return -1;
		else
			return (v1.getLengthSquared()) < (v2.getLengthSquared()) ? 1 : 0;
	}
}

}  // namespace modm