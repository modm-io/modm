/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__POINT_SET_2D_HPP
	#error	"Don't include this file directly, use 'point_set_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::PointSet2D<T>::PointSet2D(SizeType n) :
	points(n)
{
}

template <typename T>
xpcc::PointSet2D<T>::PointSet2D(std::initializer_list<xpcc::PointSet2D<T>::PointType> init) :
	points(init)
{
}

template <typename T>
xpcc::PointSet2D<T>::PointSet2D(const PointSet2D<T>& other) :
	points(other.points)
{
}

template <typename T>
xpcc::PointSet2D<T>&
xpcc::PointSet2D<T>::operator = (const PointSet2D<T>& other)
{
	this->points = other.points;
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T>
typename xpcc::PointSet2D<T>::SizeType
xpcc::PointSet2D<T>::getNumberOfPoints() const
{
	return points.getSize();
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::PointSet2D<T>::append(const xpcc::PointSet2D<T>::PointType& point)
{
	points.append(point);
}

// ----------------------------------------------------------------------------
template <typename T>
typename xpcc::PointSet2D<T>::PointType&
xpcc::PointSet2D<T>::operator [](SizeType index)
{
	return points[index];
}

template <typename T>
const typename xpcc::PointSet2D<T>::PointType&
xpcc::PointSet2D<T>::operator [](SizeType index) const
{
	return points[index];
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::PointSet2D<T>::removeAll()
{
	points.removeAll();
}

// ----------------------------------------------------------------------------
template <typename T>
typename xpcc::PointSet2D<T>::iterator 
xpcc::PointSet2D<T>::begin()
{
	return points.begin();
}

template <typename T>
typename xpcc::PointSet2D<T>::iterator
xpcc::PointSet2D<T>::end()
{
	return points.end();
}

template <typename T>
typename xpcc::PointSet2D<T>::const_iterator 
xpcc::PointSet2D<T>::begin() const
{
	return points.begin();
}

template <typename T>
typename xpcc::PointSet2D<T>::const_iterator
xpcc::PointSet2D<T>::end() const
{
	return points.end();
}
