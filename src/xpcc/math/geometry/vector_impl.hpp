// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#ifndef XPCC__VECTOR_HPP
	#error	"Don't include this file directly, use 'vector.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>::Vector()
{
}

template<typename T, uint8_t N>
xpcc::Vector<T, N>::Vector(const T *ptData)
{
	memcpy(coords, ptData, sizeof(T) * N);
}

template<typename T, uint8_t N>
xpcc::Vector<T, N>::Vector(const xpcc::Matrix<T, N, 1> &rhs)
{
	memcpy(coords, &rhs, sizeof(T) * N);
}

template<typename T, uint8_t N>
xpcc::Vector<T, N>::Vector(const xpcc::Vector<T, N> &rhs)
{
	memcpy(coords, rhs.coords, sizeof(T) * N);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
uint8_t
xpcc::Vector<T, N>::getSize()
{
	return N;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>&
xpcc::Vector<T, N>::operator = (const xpcc::Matrix<T, N, 1> &rhs)
{
	memcpy(coords, &rhs, sizeof(T) * N);
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>&
xpcc::Vector<T, N>::operator = (const xpcc::Vector<T, N> &rhs)
{
	memcpy(coords, rhs.coords, sizeof(T)*N);
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
xpcc::Vector<T, N>::operator == (const xpcc::Vector<T, N> &rhs) const
{
	return memcmp(coords, rhs.coords, sizeof(T)*N) == 0;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
xpcc::Vector<T, N>::operator != (const xpcc::Vector<T, N> &rhs) const
{
	return memcmp(coords, rhs.coords, sizeof(T)*N) != 0;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
bool
xpcc::Vector<T, N>::operator < (const xpcc::Vector<T, N> &rhs) const
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
xpcc::Vector<T, N>::operator <= (const xpcc::Vector<T, N> &rhs) const
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
xpcc::Vector<T, N>::operator > (const xpcc::Vector<T, N> &rhs) const
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
xpcc::Vector<T, N>::operator >= (const xpcc::Vector<T, N> &rhs) const
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
xpcc::Vector<T, N>::operator [] (uint8_t index) const
{
	return coords[index];
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T&
xpcc::Vector<T, N>::operator [] (uint8_t index)
{
	return coords[index];
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T*
xpcc::Vector<T, N>::ptr()
{
	return reinterpret_cast<T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
const T*
xpcc::Vector<T, N>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>
xpcc::Vector<T, N>::operator + (const xpcc::Vector<T, N> &rhs) const
{
	xpcc::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] + rhs.coords[i];
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>
xpcc::Vector<T, N>::operator - (const xpcc::Vector<T, N> &rhs) const
{
	xpcc::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] - rhs.coords[i];
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Matrix<T, N, 1>&
xpcc::Vector<T, N>::asMatrix()
{
	return *reinterpret_cast<xpcc::Matrix<T, N, 1>*>(this);
}

template<typename T, uint8_t N>
const xpcc::Matrix<T, N, 1>&
xpcc::Vector<T, N>::asMatrix() const
{
	return *reinterpret_cast<const xpcc::Matrix<T, N, 1>*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
const xpcc::Matrix<T, 1, N>&
xpcc::Vector<T, N>::asTransposedMatrix() const
{
	return *reinterpret_cast<const xpcc::Matrix<T, 1, N>*>(this);
}

template<typename T, uint8_t N>
xpcc::Matrix<T, 1, N>&
xpcc::Vector<T, N>::asTransposedMatrix()
{
	return *reinterpret_cast<xpcc::Matrix<T, 1, N>*>(this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T
xpcc::Vector<T, N>::operator * (const Vector &rhs) const
{
	T v = 0;
	for (uint_fast8_t i = 0; i < N; ++i) {
		v += (*this)[i]*rhs[i];
	}

	return v;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>
xpcc::Vector<T, N>::operator * (const T &rhs) const
{
	xpcc::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] * rhs;
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>
xpcc::Vector<T, N>::operator / (const T &rhs) const
{
	xpcc::Vector<T, N> pt;
	for (uint_fast8_t i = 0; i < N; ++i) {
		pt[i] = coords[i] / rhs;
	}
	return pt;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>&
xpcc::Vector<T, N>::operator += (const Vector &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] += rhs.coords[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>&
xpcc::Vector<T, N>::operator -= (const Vector &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] -= rhs.coords[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>&
xpcc::Vector<T, N>::operator *= (const T &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] -= rhs;
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>&
xpcc::Vector<T, N>::operator /= (const T &rhs)
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] /= rhs;
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
xpcc::Vector<T, N>&
xpcc::Vector<T, N>::operator - ()
{
	for (uint_fast8_t i = 0; i < N; ++i) {
		coords[i] = -coords[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T
xpcc::Vector<T, N>::getLength() const
{
	return sqrt(getLengthSquared());
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T
xpcc::Vector<T, N>::getLengthSquared() const
{
	T len2 = 0;
	for (uint_fast8_t i = 0; i < N; ++i) {
		len2 += (*this)[i]*(*this)[i];
	}
	
	return len2;
}
