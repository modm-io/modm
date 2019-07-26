/*
 * Copyright (c) 2011, Fabian Greif
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

#ifndef MODM_VECTOR1_HPP
	#error	"Don't include this file directly, use 'vector1.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>::Vector() :
	x()
{
}

template<typename T>
modm::Vector<T, 1>::Vector(T inX) :
	x(inX)
{
}

template<typename T>
modm::Vector<T, 1>::Vector(const modm::Matrix<T, 1, 1> &rhs) :
	x(*reinterpret_cast<const T*>(&rhs))
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::Vector<T, 1>::set(const T& value)
{
	this->x = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::Vector<T, 1>::setX(const T& value)
{
	this->x = value;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
modm::Vector<T, 1>::getX() const
{
	return this->x;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>&
modm::Vector<T, 1>::operator = (const modm::Matrix<T, 1, 1> &rhs)
{
	x = *reinterpret_cast<const T*>(&rhs);
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::operator == (const modm::Vector<T, 1> &rhs) const
{
	return (rhs.x == x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::operator != (const modm::Vector<T, 1> &rhs) const
{
	return (rhs.x != x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::operator < (const modm::Vector<T, 1> &rhs) const
{
	return (x < rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::operator <= (const modm::Vector<T, 1> &rhs) const
{
	return (x <= rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::operator > (const modm::Vector<T, 1> &rhs) const
{
	return (x > rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::operator >= (const modm::Vector<T, 1> &rhs) const
{
	return (x >= rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
modm::Vector<T, 1>::operator [] (uint8_t index) const
{
	return reinterpret_cast<const T*>(this)[index];
}

template<typename T>
T&
modm::Vector<T, 1>::operator [] (uint8_t index)
{
	return reinterpret_cast<T*>(this)[index];
}

// ----------------------------------------------------------------------------
template<typename T>
T*
modm::Vector<T, 1>::ptr()
{
	return reinterpret_cast<T*>(this);
}

template<typename T>
const T*
modm::Vector<T, 1>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>
modm::Vector<T, 1>::operator + (const modm::Vector<T, 1> &rhs) const
{
	return modm::Vector<T, 1>(x+rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>
modm::Vector<T, 1>::operator - (const modm::Vector<T, 1> &rhs) const
{
	return modm::Vector<T, 1>(x-rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
T
modm::Vector<T, 1>::operator * (const modm::Vector<T, 1> &rhs) const
{
	return x*rhs.x;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>
modm::Vector<T, 1>::operator * (const T &rhs) const
{
	return modm::Vector<T, 1>(x*rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>
modm::Vector<T, 1>::operator / (const T &rhs) const
{
	return modm::Vector<T, 1>(x/rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>&
modm::Vector<T, 1>::operator += (const modm::Vector<T, 1> &rhs)
{
	x += rhs.x;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>&
modm::Vector<T, 1>::operator -= (const modm::Vector<T, 1> &rhs)
{
	x -= rhs.x;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>&
modm::Vector<T, 1>::operator *= (const T &rhs)
{
	x *= rhs;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>&
modm::Vector<T, 1>::operator /= (const T &rhs)
{
	x /= rhs;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 1>
modm::Vector<T, 1>::operator - () const
{
	return Vector<T, 1>(-x);
}

// ----------------------------------------------------------------------------
template<typename T>
T
modm::Vector<T, 1>::getLength() const
{
	return std::abs(x);
}

// ----------------------------------------------------------------------------
template<typename T>
T
modm::Vector<T, 1>::getLengthSquared() const
{
	return x * x;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Matrix<T, 1, 1>&
modm::Vector<T, 1>::asMatrix()
{
	return *(modm::Matrix<T, 1, 1>*) this;
}

// ----------------------------------------------------------------------------
template<typename T>
const modm::Matrix<T, 1, 1>&
modm::Vector<T, 1>::asMatrix() const
{
	return *(modm::Matrix<T, 1, 1>*) this;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::hasNan() const
{
	return std::isnan(x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 1>::hasInf() const
{
	return std::isinf(x);
}

// ----------------------------------------------------------------------------
//template<typename U, typename T>
//static modm::Vector<T, 1> operator * (const U &lhs, const modm::Vector<T, 1> &rhs)
//{
//	return rhs * lhs;
//}
