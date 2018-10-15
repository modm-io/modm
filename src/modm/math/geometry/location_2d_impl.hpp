/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_LOCATION_2D_HPP
	#error	"Don't include this file directly use 'location.hpp' instead!"
#endif

#include <cmath>
#include <cstdlib>

// -----------------------------------------------------------------------------
template <typename T>
modm::Location2D<T>::Location2D() :
	position(), orientation()
{
}

template <typename T>
modm::Location2D<T>::Location2D(const Vector<T, 2>& position,
		const float& orientation) :
	position(position),
	orientation(orientation)
{
}

template <typename T>
modm::Location2D<T>::Location2D(const T& x, const T& y, const float& orientation) :
	position(x, y),
	orientation(orientation)
{
}

// ----------------------------------------------------------------------------
template <typename T>
const modm::Vector<T, 2>&
modm::Location2D<T>::getPosition() const
{
	return this->position;
}

template <typename T>
inline const T&
modm::Location2D<T>::getX() const
{
	return this->position.x;
}

template <typename T>
inline const T&
modm::Location2D<T>::getY() const
{
	return this->position.y;
}

template <typename T>
void
modm::Location2D<T>::setPosition(const Vector<T, 2>& point)
{
	this->position = point;
}

template <typename T>
void
modm::Location2D<T>::setPosition(const T& x, const T& y)
{
	this->position.set(x, y);
}

template <typename T>
float
modm::Location2D<T>::getOrientation() const
{
	return this->orientation;
}

template <typename T>
void
modm::Location2D<T>::setOrientation(const float& orientation)
{
	this->orientation = orientation;
}

// -----------------------------------------------------------------------------
template <typename T>
void
modm::Location2D<T>::move(const Location2D<T>& diff)
{
	Vector<T, 2> movement = diff.position;
	movement.rotate(this->orientation);

	this->position.translate(movement);
	this->orientation = Angle::normalize(this->orientation + diff.orientation);
}

template <typename T>
void
modm::Location2D<T>::move(const Vector<T, 2>& diff)
{
	Vector<T, 2> movement(diff);
	movement.rotate(this->orientation);

	this->position.translate(movement);
}

template <typename T>
void
modm::Location2D<T>::move(T x, float phi)
{
	Vector<T, 2> vector(GeometricTraits<T>::round(x * std::cos(this->orientation)),
					   GeometricTraits<T>::round(x * std::sin(this->orientation)));
	position.translate(vector);

	this->orientation = Angle::normalize(this->orientation + phi);
}

// ----------------------------------------------------------------------------
template <typename T>
modm::Vector<T, 2>
modm::Location2D<T>::translated(const Vector<T, 2>& vector) const
{
	Vector<T, 2> result(vector);
	result.rotate(this->orientation);
	result.translate(this->position);

	return result;
}

// ----------------------------------------------------------------------------
template<typename T> template<typename U>
modm::Location2D<U>
modm::Location2D<T>::convert() const
{
	return Location2D<U>(this->position.template convert<U>(), this->orientation);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Location2D<T>::operator == (const Location2D &other) const
{
	return ((this->position == other.position) &&
			(std::abs(this->orientation - other.orientation) < __FLT_EPSILON__));
}

template<typename T>
bool
modm::Location2D<T>::operator != (const Location2D &other) const
{
	return ((this->position != other.position) ||
			(std::abs(this->orientation - other.orientation) > __FLT_EPSILON__));
}

// ----------------------------------------------------------------------------
template<class T>
modm::IOStream&
modm::operator << (modm::IOStream& os, const modm::Location2D<T>& location)
{
	os << location.position << ", phi=" << location.orientation;
	return os;
}
