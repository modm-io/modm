/*
 * Copyright (c) 2011-2012, Fabian Greif
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

#ifndef MODM_QUATERNION_HPP
#	error	"Don't include this file directly, use 'quaternion.hpp' instead!"
#endif

#include "quaternion.hpp"

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>::Quaternion()
:
	w(),
	x(),
	y(),
	z()
{
}

template<typename T>
template<typename U>
modm::Quaternion<T>::Quaternion(const U *array) :
	w(array[0]),
	x(array[1]),
	y(array[2]),
	z(array[3])
{
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>::Quaternion(T inW, T inX, T inY, T inZ)
:
	w(inW),
	x(inX),
	y(inY),
	z(inZ)
{
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>::Quaternion(const Vector<T, 3> &axis, T angle)
:
	w(),
	x(),
	y(),
	z()
{
	float sinAngleOver2 = std::sin(angle / 2);

	w = std::cos(angle / 2);
	x = reinterpret_cast<const T*>(&axis)[0]*sinAngleOver2;
	y = reinterpret_cast<const T*>(&axis)[1]*sinAngleOver2;
	z = reinterpret_cast<const T*>(&axis)[2]*sinAngleOver2;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>::Quaternion(const Matrix<T, 4, 1> &rhs)
:
	w(reinterpret_cast<const T*>(&rhs)[0]),
	x(reinterpret_cast<const T*>(&rhs)[1]),
	y(reinterpret_cast<const T*>(&rhs)[2]),
	z(reinterpret_cast<const T*>(&rhs)[3])
{
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>::Quaternion(const Quaternion<T> &rhs)
:
	w(rhs.w),
	x(rhs.x),
	y(rhs.y),
	z(rhs.z)
{
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>& modm::Quaternion<T>::operator = (const Matrix<T, 4, 1> &rhs)
{
	w = reinterpret_cast<const T*>(&rhs)[0];
	x = reinterpret_cast<const T*>(&rhs)[1];
	y = reinterpret_cast<const T*>(&rhs)[2];
	z = reinterpret_cast<const T*>(&rhs)[3];
	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>& modm::Quaternion<T>::operator = (const Quaternion<T> &rhs)
{
	w = rhs.w;
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
bool
modm::Quaternion<T>::operator == (const Quaternion<T> &rhs) const
{
	return (rhs.w == w) && (rhs.x == x) && (rhs.y == y) && (rhs.z == z);
}

// ----------------------------------------------------------------------------
template<class T>
bool
modm::Quaternion<T>::operator != (const Quaternion<T> &rhs) const
{
	return (rhs.w != w) || (rhs.x != x) || (rhs.y != y) || (rhs.z != z);
}

// ----------------------------------------------------------------------------
template<class T>
bool
modm::Quaternion<T>::operator < (const Quaternion<T> &rhs) const
{
	return (w < rhs.w) || ((w == rhs.w) && ((x < rhs.x) || ((x == rhs.x) && ((y < rhs.y) || ((y == rhs.y) && (z < rhs.z))))));
}

// ----------------------------------------------------------------------------
template<class T>
bool
modm::Quaternion<T>::operator <= (const Quaternion<T> &rhs) const
{
	return (w < rhs.w) || ((w == rhs.w) && ((x < rhs.x) || ((x == rhs.x) && ((y < rhs.y) || ((y == rhs.y) && (z <= rhs.z))))));
}

// ----------------------------------------------------------------------------
template<class T>
bool
modm::Quaternion<T>::operator > (const Quaternion<T> &rhs) const
{
	return (w > rhs.w) || ((w == rhs.w) && ((x > rhs.x) || ((x == rhs.x) && ((y > rhs.y) || ((y == rhs.y) && (z > rhs.z))))));
}

// ----------------------------------------------------------------------------
template<class T>
bool
modm::Quaternion<T>::operator >= (const Quaternion<T> &rhs) const
{
	return (w > rhs.w) || ((w == rhs.w) && ((x > rhs.x) || ((x == rhs.x) && ((y > rhs.y) || ((y == rhs.y) && (z >= rhs.z))))));
}

// ----------------------------------------------------------------------------
template<class T>
const T&
modm::Quaternion<T>::operator [] (int index) const
{
	return reinterpret_cast<const T*>(this)[index];
}

// ----------------------------------------------------------------------------
template<class T>
T*
modm::Quaternion<T>::ptr()
{
	return reinterpret_cast<T*>(this);
}

// ----------------------------------------------------------------------------
template<class T>
const T*
modm::Quaternion<T>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::operator + (const Quaternion<T> &rhs) const
{
	return Quaternion(w+rhs.w, x+rhs.x, y+rhs.y, z+rhs.z);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::operator - (const Quaternion<T> &rhs) const
{
	return Quaternion(w-rhs.w, x-rhs.x, y-rhs.y, z-rhs.z);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::operator * (const Quaternion<T> &rhs) const
{
	return Quaternion(
		w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z,
		w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y,
		w*rhs.y + y*rhs.w + z*rhs.x - x*rhs.z,
		w*rhs.z + z*rhs.w + x*rhs.y - y*rhs.x
	);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::operator * (const T &rhs) const
{
	return Quaternion(w*rhs, x*rhs, y*rhs, z*rhs);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::operator / (const T &rhs) const
{
	return Quaternion(w/rhs, x/rhs, y/rhs, z/rhs);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>&
modm::Quaternion<T>::operator += (const Quaternion<T> &rhs)
{
	w+= rhs.w; x += rhs.x; y += rhs.y; z += rhs.z;
	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>&
modm::Quaternion<T>::operator -= (const Quaternion<T> &rhs)
{
	w -= rhs.w; x -= rhs.x; y -= rhs.y; z -= rhs.z;
	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>&
modm::Quaternion<T>::operator *= (const T &rhs)
{
	*this = *this * rhs;

	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>&
modm::Quaternion<T>::operator /= (const T &rhs)
{
	w /= rhs; x /= rhs; y /= rhs; z /= rhs;
	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::operator - ()
{
	return Quaternion(-w, -x, -y, -z);
}

// ----------------------------------------------------------------------------
template<class T>
float
modm::Quaternion<T>::getLength() const
{
	return std::sqrt(getLengthSquared());
}

// ----------------------------------------------------------------------------
template<class T>
float
modm::Quaternion<T>::getLengthSquared() const
{
	return w*w + x*x + y*y + z*z;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>&
modm::Quaternion<T>::scale(float newLength)
{
	float s = newLength / getLength();
	w *= s;
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>&
modm::Quaternion<T>::normalize()
{
	return scale(1.0f);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>&
modm::Quaternion<T>::conjugate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::scaled(float newLength) const
{
	float s = newLength / getLength();
	return Quaternion(w*s, x*s, y*s, z*s);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::normalized() const
{
	return scaled(1.0f);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::conjugated() const
{
	return Quaternion(w, -x, -y, -z);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::unitInverse()	//	assumes we have a unit quaternion
{
	return conjugate();
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::Quaternion<T>::inverse()
{
	return conjugate() / getLengthSquared();
}

// ----------------------------------------------------------------------------
template<class T>
void
modm::Quaternion<T>::to4x4Matrix(Matrix<T, 4, 4> *outMatrix)
{
	// avoid depending on Matrix.h
	T* m = reinterpret_cast<T*>(outMatrix);

	float xx = x*x;	float xy = x*y;
	float xz = x*z;	float xw = x*w;

	float yy = y*y;	float yz = y*z;
	float yw = y*w;

	float zz = z*z;	float zw = z*w;

	m[0*4+0] = 1-2*(yy+zz); m[1*4+0] =   2*(xy-zw); m[2*4+0] =   2*(xz+yw); m[3*4+0] = 0;
	m[0*4+1] =   2*(xy+zw); m[1*4+1] = 1-2*(xx+zz); m[2*4+1] =   2*(yz-xw); m[3*4+1] = 0;
	m[0*4+2] =   2*(xz-yw); m[1*4+2] =   2*(yz+xw); m[2*4+2] = 1-2*(xx+yy); m[3*4+2] = 0;
	m[0*4+3] =           0; m[1*4+3] =           0; m[2*4+3] =           0; m[3*4+3] = 1;
}

// ----------------------------------------------------------------------------
template<class T>
void
modm::Quaternion<T>::to3x3Matrix(Matrix<T, 3, 3> *outMatrix)
{
	// avoid depending on Matrix.h
	T* m = reinterpret_cast<T*>(outMatrix);

	float xx = x*x;	float xy = x*y;
	float xz = x*z;	float xw = x*w;

	float yy = y*y;	float yz = y*z;
	float yw = y*w;

	float zz = z*z;	float zw = z*w;

	m[0*3+0] = 1-2*(yy+zz); m[1*3+0] =   2*(xy-zw); m[2*3+0] =   2*(xz+yw);
	m[0*3+1] =   2*(xy+zw); m[1*3+1] = 1-2*(xx+zz); m[2*3+1] =   2*(yz-xw);
	m[0*3+2] =   2*(xz-yw); m[1*3+2] =   2*(yz+xw); m[2*3+2] = 1-2*(xx+yy);
}

// ----------------------------------------------------------------------------
template<class T>
modm::Quaternion<T>
modm::operator * (const T &lhs, const modm::Quaternion<T> &rhs)
{
	return rhs * lhs;
}
