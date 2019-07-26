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

#ifndef MODM_VECTOR4_HPP
	#error	"Don't include this file directly, use 'vector4.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector()
:
	x(),
	y(),
	z(),
	w()
{
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(T inX, T inY, T inZ, T inW)
:
	x(inX),
	y(inY),
	z(inZ),
	w(inW)
{
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 1> &inY, const modm::Vector<T, 1> &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ.x),
	w(inW.x)
{
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 1> &inY, const modm::Vector<T, 1> &inZ, const T &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 1> &inY, const T &inZ, const T &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const T &inY, const modm::Vector<T, 1> &inZ, const T &inW)
:
	x(inX.x),
	y(inY),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 1> &inY, const modm::Vector<T, 1> &inZ, const T &inW)
:
	x(inX),
	y(inY.x),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const T &inY, const T &inZ, const T &inW)
:
	x(inX.x),
	y(inY),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 1> &inY, const T &inZ, const T &inW)
:
	x(inX),
	y(inY.x),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 1> &inY, const T &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY.x),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const T &inY, const T &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 1> &inY, const T &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX),
	y(inY.x),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const T &inY, const T &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX),
	y(inY),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const T &inY, const modm::Vector<T, 1> &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inY),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const T &inY, const modm::Vector<T, 1> &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX),
	y(inY),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 1> &inY, const modm::Vector<T, 1> &inZ, const modm::Vector<T, 1> &inW)
:
	x(inX),
	y(inY.x),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 2> &inXY, const modm::Vector<T, 1> &inZ, const modm::Vector<T, 1> &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ.x),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 2> &inXY, const modm::Vector<T, 1> &inZ, const T &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ.x),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 2> &inXY, const T &inZ, const T &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 2> &inXY, const T &inZ, const modm::Vector<T, 1> &inW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZ),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 2> &inYZ, const modm::Vector<T, 1> &inW)
:
	x(inX.x),
	y(inYZ.x),
	z(inYZ.y),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 2> &inYZ, const T &inW)
:
	x(inX.x),
	y(inYZ.x),
	z(inYZ.y),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 2> &inYZ, const T &inW)
:
	x(inX),
	y(inYZ.x),
	z(inYZ.y),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 2> &inYZ, const modm::Vector<T, 1> &inW)
:
	x(inX),
	y(inYZ.x),
	z(inYZ.y),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 1> &inY, const modm::Vector<T, 2> &inZW)
:
	x(inX.x),
	y(inY.x),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const T &inY, const modm::Vector<T, 2> &inZW)
:
	x(inX.x),
	y(inY),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const T &inY, const modm::Vector<T, 2> &inZW)
:
	x(inX),
	y(inY),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 1> &inY, const modm::Vector<T, 2> &inZW)
:
	x(inX),
	y(inY.x),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 2> &inXY, const modm::Vector<T, 2> &inZW)
:
	x(inXY.x),
	y(inXY.y),
	z(inZW.x),
	w(inZW.y)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 3> &inXYZ, const modm::Vector<T, 1> &inW)
:
	x(inXYZ.x),
	y(inXYZ.y),
	z(inXYZ.z),
	w(inW.x)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 3> &inXYZ, const T &inW)
:
	x(inXYZ.x),
	y(inXYZ.y),
	z(inXYZ.z),
	w(inW)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Vector<T, 1> &inX, const modm::Vector<T, 3> &inYZW)
:
	x(inX.x),
	y(inYZW.x),
	z(inYZW.y),
	w(inYZW.z)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const T &inX, const modm::Vector<T, 3> &inYZW)
:
	x(inX),
	y(inYZW.x),
	z(inYZW.y),
	w(inYZW.z)
{
}


// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(const modm::Matrix<T, 4, 1> &rhs)
:
	x(reinterpret_cast<const T*>(&rhs)[0]),
	y(reinterpret_cast<const T*>(&rhs)[1]),
	z(reinterpret_cast<const T*>(&rhs)[2]),
	w(reinterpret_cast<const T*>(&rhs)[3])
{
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>::Vector(T inVal)
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
modm::Vector<T, 4>::set(const T& x_, const T& y_, const T& z_, const T& w_)
{
	this->x = x_;
	this->y = y_;
	this->z = z_;
	this->w = w_;
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::Vector<T, 4>::setX(const T& value)
{
	this->x = value;
}

template<typename T>
void
modm::Vector<T, 4>::setY(const T& value)
{
	this->y = value;
}

template<typename T>
void
modm::Vector<T, 4>::setZ(const T& value)
{
	this->z = value;
}

template<typename T>
void
modm::Vector<T, 4>::setW(const T& value)
{
	this->w = value;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
modm::Vector<T, 4>::getX() const
{
	return this->x;
}

template<typename T>
const T&
modm::Vector<T, 4>::getY() const
{
	return this->y;
}

template<typename T>
const T&
modm::Vector<T, 4>::getZ() const
{
	return this->z;
}

template<typename T>
const T&
modm::Vector<T, 4>::getW() const
{
	return this->w;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>&
modm::Vector<T, 4>::operator = (const modm::Matrix<T, 4, 1> &rhs)
{
	x = reinterpret_cast<const T*>(&rhs)[0];
	y = reinterpret_cast<const T*>(&rhs)[1];
	z = reinterpret_cast<const T*>(&rhs)[2];
	w = reinterpret_cast<const T*>(&rhs)[3];

	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 4>::operator == (const modm::Vector<T, 4> &rhs) const
{
	return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 4>::operator != (const modm::Vector<T, 4> &rhs) const
{
	return (rhs.x != x) || (rhs.y != y) || (rhs.z != z) || (rhs.w != w);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 4>::operator < (const modm::Vector<T, 4> &rhs) const
{
	return (x < rhs.x) || ((x == rhs.x) && ((y < rhs.y) || ((y == rhs.y) && ((z < rhs.z) || ((z == rhs.z) && (w < rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 4>::operator <= (const modm::Vector<T, 4> &rhs) const
{
	return (x < rhs.x) || ((x == rhs.x) && ((y < rhs.y) || ((y == rhs.y) && ((z < rhs.z) || ((z == rhs.z) && (w <= rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 4>::operator > (const modm::Vector<T, 4> &rhs) const
{
	return (x > rhs.x) || ((x == rhs.x) && ((y > rhs.y) || ((y == rhs.y) && ((z > rhs.z) || ((z == rhs.z) && (w > rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Vector<T, 4>::operator >= (const modm::Vector<T, 4> &rhs) const
{
	return (x > rhs.x) || ((x == rhs.x) && ((y > rhs.y) || ((y == rhs.y) && ((z > rhs.z) || ((z == rhs.z) && (w >= rhs.w))))));
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
modm::Vector<T, 4>::operator [] (uint8_t index) const
{
	return reinterpret_cast<const T*>(this)[index];
}

// ----------------------------------------------------------------------------
template<typename T>
T&
modm::Vector<T, 4>::operator [] (uint8_t index)
{
	return reinterpret_cast<T*>(this)[index];
}

// ----------------------------------------------------------------------------
template<typename T>
T*
modm::Vector<T, 4>::ptr()
{
	return reinterpret_cast<T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T>
const T*
modm::Vector<T, 4>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>
modm::Vector<T, 4>::operator + (const modm::Vector<T, 4> &rhs) const
{
	return modm::Vector<T, 4>(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>
modm::Vector<T, 4>::operator - (const modm::Vector<T, 4> &rhs) const
{
	return modm::Vector<T, 4>(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

// ----------------------------------------------------------------------------
template<typename T>
T
modm::Vector<T, 4>::operator * (const modm::Vector<T, 4> &rhs) const
{
	return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>
modm::Vector<T, 4>::operator * (const T &rhs) const
{
	return modm::Vector<T, 4>(x*rhs, y*rhs, z*rhs, w*rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>
modm::Vector<T, 4>::operator / (const T &rhs) const
{
	return modm::Vector<T, 4>(x/rhs, y/rhs, z/rhs, w/rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>&
modm::Vector<T, 4>::operator += (const modm::Vector<T, 4> &rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;

	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>&
modm::Vector<T, 4>::operator -= (const modm::Vector<T, 4> &rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;

	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>&
modm::Vector<T, 4>::operator *= (const T &rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;
	w *= rhs;

	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>&
modm::Vector<T, 4>::operator /= (const T &rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;
	w /= rhs;

	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>
modm::Vector<T, 4>::operator - () const
{
	return modm::Vector<T, 4>(-x, -y, -z, -w);
}

// ----------------------------------------------------------------------------
template<typename T>
float
modm::Vector<T, 4>::getLength() const
{
	return std::sqrt(getLengthSquared());
}

// ----------------------------------------------------------------------------
template<typename T>
float
modm::Vector<T, 4>::getLengthSquared() const
{
	return x*x + y*y + z*z + w*w;
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::Vector<T, 4>::scale(float newLength)
{
	*this = scaled(newLength);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>
modm::Vector<T, 4>::scaled(float newLength) const
{
	float scale = newLength / getLength();
	return *this * scale;
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::Vector<T, 4>::normalize()
{
	scale(1.0f);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 4>
modm::Vector<T, 4>::normalized() const
{
	return scaled(1.0);
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Matrix<T, 4, 1>&
modm::Vector<T, 4>::asMatrix()
{
	return *(modm::Matrix<T, 4, 1>*)this;
}

template<typename T>
const modm::Matrix<T, 4, 1>&
modm::Vector<T, 4>::asMatrix() const
{
	return *(modm::Matrix<T, 4, 1>*)this;
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Matrix<T, 1, 4>&
modm::Vector<T, 4>::asTransposedMatrix()
{
	return *(modm::Matrix<T, 1, 4>*)this;
}


template<typename T>
const modm::Matrix<T, 1, 4>&
modm::Vector<T, 4>::asTransposedMatrix() const
{
	return *(modm::Matrix<T, 1, 4>*)this;
}

// ----------------------------------------------------------------------------
template<typename T>
static inline modm::Vector<T,4>
operator * (const T &lhs, const modm::Vector<T,4> &rhs)
{
	return rhs * lhs;
}

template<class T, class U>
static inline modm::Vector<U,4>
operator * (const modm::Matrix<T, 4, 4> &lhs, const modm::Vector<U,4> &rhs)
{
	return lhs * rhs.asTMatrix();
}

