/*
 * Copyright (c) 2011-2012, Fabian Greif
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
	#error	"Don't include this file directly, use 'vector.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>::Vector()
{
}

template<typename T, uint8_t N>
modm::Vector<T, N>::Vector(const T *ptData)
{
	memcpy(coords, ptData, sizeof(T) * N);
}

template<typename T, uint8_t N>
modm::Vector<T, N>::Vector(const modm::Matrix<T, N, 1> &rhs)
{
	memcpy(coords, &rhs, sizeof(T) * N);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
uint8_t
modm::Vector<T, N>::getSize()
{
	return N;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>&
modm::Vector<T, N>::operator = (const modm::Matrix<T, N, 1> &rhs)
{
	memcpy(coords, &rhs, sizeof(T) * N);
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
modm::Vector<T, N>::operator == (const modm::Vector<T, N> &rhs) const
{
	return memcmp(coords, rhs.coords, sizeof(T)*N) == 0;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
modm::Vector<T, N>::operator != (const modm::Vector<T, N> &rhs) const
{
	return memcmp(coords, rhs.coords, sizeof(T)*N) != 0;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
modm::Vector<T, N>::operator < (const modm::Vector<T, N> &rhs) const
{
	for (uint_fast8_t i = 0; i < N; ++i)
	{
		if ((*this)[i] < rhs[i]) {
			return true;
		}
		else if ((*this)[i] > rhs[i]) {
			return false;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
modm::Vector<T, N>::operator <= (const modm::Vector<T, N> &rhs) const
{
	for (uint_fast8_t i = 0; i < N; ++i)
	{
		if ((*this)[i] < rhs[i]) {
			return true;
		}
		else if ((*this)[i] > rhs[i]) {
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
modm::Vector<T, N>::operator > (const modm::Vector<T, N> &rhs) const
{
	for (uint_fast8_t i = 0; i < N; ++i)
	{
		if ((*this)[i] > rhs[i]) {
			return true;
		}
		else if ((*this)[i] < rhs[i]) {
			return false;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
modm::Vector<T, N>::operator >= (const modm::Vector<T, N> &rhs) const
{
	for (uint_fast8_t i = 0; i < N; ++i)
	{
		if ((*this)[i] > rhs[i]) {
			return true;
		}
		else if ((*this)[i] < rhs[i]) {
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
const T&
modm::Vector<T, N>::operator [] (uint8_t index) const
{
	return coords[index];
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T&
modm::Vector<T, N>::operator [] (uint8_t index)
{
	return coords[index];
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T*
modm::Vector<T, N>::ptr()
{
	return reinterpret_cast<T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
const T*
modm::Vector<T, N>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>
modm::Vector<T, N>::operator + (const modm::Vector<T, N> &rhs) const
{
	modm::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] + rhs.coords[i];
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>
modm::Vector<T, N>::operator - (const modm::Vector<T, N> &rhs) const
{
	modm::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] - rhs.coords[i];
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Matrix<T, N, 1>&
modm::Vector<T, N>::asMatrix()
{
	return *reinterpret_cast<modm::Matrix<T, N, 1>*>(this);
}

template<typename T, uint8_t N>
const modm::Matrix<T, N, 1>&
modm::Vector<T, N>::asMatrix() const
{
	return *reinterpret_cast<const modm::Matrix<T, N, 1>*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
const modm::Matrix<T, 1, N>&
modm::Vector<T, N>::asTransposedMatrix() const
{
	return *reinterpret_cast<const modm::Matrix<T, 1, N>*>(this);
}

template<typename T, uint8_t N>
modm::Matrix<T, 1, N>&
modm::Vector<T, N>::asTransposedMatrix()
{
	return *reinterpret_cast<modm::Matrix<T, 1, N>*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T
modm::Vector<T, N>::operator * (const Vector &rhs) const
{
	T v = 0;
	for (uint_fast8_t i = 0; i < N; ++i) {
		v += (*this)[i]*rhs[i];
	}

	return v;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>
modm::Vector<T, N>::operator * (const T &rhs) const
{
	modm::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] * rhs;
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>
modm::Vector<T, N>::operator / (const T &rhs) const
{
	modm::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] / rhs;
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>&
modm::Vector<T, N>::operator += (const Vector &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] += rhs.coords[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>&
modm::Vector<T, N>::operator -= (const Vector &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] -= rhs.coords[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>&
modm::Vector<T, N>::operator *= (const T &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] -= rhs;
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>&
modm::Vector<T, N>::operator /= (const T &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] /= rhs;
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
modm::Vector<T, N>&
modm::Vector<T, N>::operator - ()
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] = -coords[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T
modm::Vector<T, N>::getLength() const
{
	return std::sqrt(getLengthSquared());
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T
modm::Vector<T, N>::getLengthSquared() const
{
	T len2 = 0;
	for (uint_fast8_t i = 0; i < N; ++i) {
		len2 += (*this)[i]*(*this)[i];
	}

	return len2;
}
