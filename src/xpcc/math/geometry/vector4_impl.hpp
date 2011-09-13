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

#ifndef XPCC__VECTOR4_HPP
	#error	"Don't include this file directly, use 'vector4.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector()
:
	x(),
	y(),
	z(),
	w()
{
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(T inX, T inY, T inZ, T inW)
:
	x(inX),
	y(inY),
	z(inZ),
	w(inW)
{
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 1> &inY, const xpcc::Vector<T, 1> &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ.x),
	w(inW.x)
{
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 1> &inY, const xpcc::Vector<T, 1> &inZ, const T &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 1> &inY, const T &inZ, const T &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const T &inY, const xpcc::Vector<T, 1> &inZ, const T &inW)
:
	x(inX.x),
	y(inY),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 1> &inY, const xpcc::Vector<T, 1> &inZ, const T &inW)
:
	x(inX),
	y(inY.x),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const T &inY, const T &inZ, const T &inW)
:
	x(inX.x),
	y(inY),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 1> &inY, const T &inZ, const T &inW)
:
	x(inX),
	y(inY.x),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 1> &inY, const T &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const T &inY, const T &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 1> &inY, const T &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX),
	y(inY.x),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const T &inY, const T &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX),
	y(inY),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const T &inY, const xpcc::Vector<T, 1> &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const T &inY, const xpcc::Vector<T, 1> &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX),
	y(inY),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 1> &inY, const xpcc::Vector<T, 1> &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX),
	y(inY.x),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 2> &inXY, const xpcc::Vector<T, 1> &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 2> &inXY, const xpcc::Vector<T, 1> &inZ, const T &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 2> &inXY, const T &inZ, const T &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 2> &inXY, const T &inZ, const xpcc::Vector<T, 1> &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 2> &inYZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inYZ.x),
	z(inYZ.y),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 2> &inYZ, const T &inW)
:
	x(inX.x),
	y(inYZ.x),
	z(inYZ.y),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 2> &inYZ, const T &inW)
:
	x(inX),
	y(inYZ.x),
	z(inYZ.y),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 2> &inYZ, const xpcc::Vector<T, 1> &inW)
:
	x(inX),
	y(inYZ.x),
	z(inYZ.y),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 1> &inY, const xpcc::Vector<T, 2> &inZW)
:
	x(inX.x),
	y(inY.x),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const T &inY, const xpcc::Vector<T, 2> &inZW)
:
	x(inX.x),
	y(inY),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const T &inY, const xpcc::Vector<T, 2> &inZW)
:
	x(inX),
	y(inY),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 1> &inY, const xpcc::Vector<T, 2> &inZW)
:
	x(inX),
	y(inY.x),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 2> &inXY, const xpcc::Vector<T, 2> &inZW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZW.x),
	w(inZW.y)
{
}

	
// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 3> &inXYZ, const xpcc::Vector<T, 1> &inW)
:
	x(inXYZ.x),
	y(inXYZ.y),
	z(inXYZ.z),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 3> &inXYZ, const T &inW)
:
	x(inXYZ.x),
	y(inXYZ.y),
	z(inXYZ.z),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 1> &inX, const xpcc::Vector<T, 3> &inYZW)
:
	x(inX.x),
	y(inYZW.x),
	z(inYZW.y),
	w(inYZW.z)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const T &inX, const xpcc::Vector<T, 3> &inYZW)
:
	x(inX),
	y(inYZW.x),
	z(inYZW.y),
	w(inYZW.z)
{
}


// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Matrix<T, 4, 1> &rhs)
:
	x(reinterpret_cast<const T*>(&rhs)[0]),
	y(reinterpret_cast<const T*>(&rhs)[1]),
	z(reinterpret_cast<const T*>(&rhs)[2]),
	w(reinterpret_cast<const T*>(&rhs)[3])
{
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(const xpcc::Vector<T, 4> &rhs)
:
	x(rhs.x),
	y(rhs.y),
	z(rhs.z),
	w(rhs.w)
{
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>::Vector(T inVal)
:
	x(inVal),
	y(inVal),
	z(inVal),
	w(inVal)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector<T, 4>::set(const T& x, const T& y, const T& z, const T& w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector<T, 4>::setX(const T& value)
{
	this->x = value;
}

template<typename T>
void
xpcc::Vector<T, 4>::setY(const T& value)
{
	this->y = value;
}

template<typename T>
void
xpcc::Vector<T, 4>::setZ(const T& value)
{
	this->z = value;
}

template<typename T>
void
xpcc::Vector<T, 4>::setW(const T& value)
{
	this->w = value;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Vector<T, 4>::getX() const
{
	return this->x;
}

template<typename T>
const T&
xpcc::Vector<T, 4>::getY() const
{
	return this->y;
}

template<typename T>
const T&
xpcc::Vector<T, 4>::getZ() const
{
	return this->z;
}

template<typename T>
const T&
xpcc::Vector<T, 4>::getW() const
{
	return this->w;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>&
xpcc::Vector<T, 4>::operator = (const xpcc::Matrix<T, 4, 1> &rhs)
{
	x = reinterpret_cast<const T*>(&rhs)[0];
	y = reinterpret_cast<const T*>(&rhs)[1];
	z = reinterpret_cast<const T*>(&rhs)[2];
	w = reinterpret_cast<const T*>(&rhs)[3];
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>&
xpcc::Vector<T, 4>::operator = (const xpcc::Vector<T, 4> &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 4>::operator == (const xpcc::Vector<T, 4> &rhs) const
{
	return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 4>::operator != (const xpcc::Vector<T, 4> &rhs) const
{
	return (rhs.x != x) || (rhs.y != y) || (rhs.z != z) || (rhs.w != w);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 4>::operator < (const xpcc::Vector<T, 4> &rhs) const
{
	return (x < rhs.x) || ((x == rhs.x) && ((y < rhs.y) || ((y == rhs.y) && ((z < rhs.z) || ((z == rhs.z) && (w < rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 4>::operator <= (const xpcc::Vector<T, 4> &rhs) const
{
	return (x < rhs.x) || ((x == rhs.x) && ((y < rhs.y) || ((y == rhs.y) && ((z < rhs.z) || ((z == rhs.z) && (w <= rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 4>::operator > (const xpcc::Vector<T, 4> &rhs) const
{
	return (x > rhs.x) || ((x == rhs.x) && ((y > rhs.y) || ((y == rhs.y) && ((z > rhs.z) || ((z == rhs.z) && (w > rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 4>::operator >= (const xpcc::Vector<T, 4> &rhs) const
{
	return (x > rhs.x) || ((x == rhs.x) && ((y > rhs.y) || ((y == rhs.y) && ((z > rhs.z) || ((z == rhs.z) && (w >= rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Vector<T, 4>::operator [] (uint8_t index) const
{
	return reinterpret_cast<const T*>(this)[index];
}

// ----------------------------------------------------------------------------
template<typename T>
T&
xpcc::Vector<T, 4>::operator [] (uint8_t index)
{
	return reinterpret_cast<T*>(this)[index];
}

// ----------------------------------------------------------------------------
template<typename T>
T*
xpcc::Vector<T, 4>::ptr()
{
	return reinterpret_cast<T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T>
const T*
xpcc::Vector<T, 4>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>
xpcc::Vector<T, 4>::operator + (const xpcc::Vector<T, 4> &rhs) const
{
	return xpcc::Vector<T, 4>(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>
xpcc::Vector<T, 4>::operator - (const xpcc::Vector<T, 4> &rhs) const
{
	return xpcc::Vector<T, 4>(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::Vector<T, 4>::operator * (const xpcc::Vector<T, 4> &rhs) const
{
	return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>
xpcc::Vector<T, 4>::operator * (const T &rhs) const
{
	return xpcc::Vector<T, 4>(x*rhs, y*rhs, z*rhs, w*rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>
xpcc::Vector<T, 4>::operator / (const T &rhs) const
{
	return xpcc::Vector<T, 4>(x/rhs, y/rhs, z/rhs, w/rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>&
xpcc::Vector<T, 4>::operator += (const xpcc::Vector<T, 4> &rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>&
xpcc::Vector<T, 4>::operator -= (const xpcc::Vector<T, 4> &rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>&
xpcc::Vector<T, 4>::operator *= (const T &rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;
	w *= rhs;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>&
xpcc::Vector<T, 4>::operator /= (const T &rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;
	w /= rhs;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>
xpcc::Vector<T, 4>::operator - () const
{
	return xpcc::Vector<T, 4>(-x, -y, -z, -w);
}

// ----------------------------------------------------------------------------
template<typename T>
float
xpcc::Vector<T, 4>::getLength() const
{
	return sqrt(getLengthSquared());
}

// ----------------------------------------------------------------------------
template<typename T>
float
xpcc::Vector<T, 4>::getLengthSquared() const
{
	return x*x+y*y+z*z+w*w;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector<T, 4>::scale(float newLength)
{
	*this = scaled(newLength);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>
xpcc::Vector<T, 4>::scaled(float newLength) const
{
	float scale = newLength/getLength();
	return *this * scale;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector<T, 4>::normalize()
{
	scale(1.0f);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 4>
xpcc::Vector<T, 4>::normalized() const
{
	return scaled(1.0);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Matrix<T, 4, 1>&
xpcc::Vector<T, 4>::asMatrix()
{
	return *(xpcc::Matrix<T, 4, 1>*)this;
}

template<typename T>
const xpcc::Matrix<T, 4, 1>&
xpcc::Vector<T, 4>::asMatrix() const
{
	return *(xpcc::Matrix<T, 4, 1>*)this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Matrix<T, 1, 4>&
xpcc::Vector<T, 4>::asTransposedMatrix()
{
	return *(xpcc::Matrix<T, 1, 4>*)this;
}


template<typename T>
const xpcc::Matrix<T, 1, 4>&
xpcc::Vector<T, 4>::asTransposedMatrix() const
{
	return *(xpcc::Matrix<T, 1, 4>*)this;
}

// ----------------------------------------------------------------------------
template<typename T>
static inline xpcc::Vector<T,4>
operator * (const T &lhs, const xpcc::Vector<T,4> &rhs)
{
	return rhs * lhs;
}

template<class T, class U>
static inline xpcc::Vector<U,4>
operator * (const xpcc::Matrix<T, 4, 4> &lhs, const xpcc::Vector<U,4> &rhs)
{
	return lhs * rhs.asTMatrix();
}

