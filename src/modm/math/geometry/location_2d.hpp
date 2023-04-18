/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
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

#include "angle.hpp"
#include "vector.hpp"

#include <modm/io/iostream.hpp>

namespace modm
{
/**
 * \brief	Location in a 2D coordinate system
 *
 * This class is primarily used to describe the location of a robot
 * on the field. The robot has a position (x- and y-coordinate) and a
 * orientation (absolute angle in the range (-pi, pi)).
 *
 * \ingroup	modm_math_geometry
 */
template <typename T = int16_t>
class Location2D
{
public:
	Vector<T, 2> position;
	float orientation = 0;

	constexpr Location2D() = default;

	constexpr Location2D(const Vector<T, 2>& position, const float& orientation)
		: position(position), orientation(orientation) {}

	template<typename U>
	constexpr explicit Location2D(const Location2D<U> &l) : position(l.position), orientation(l.orientation) {}

	constexpr bool operator== (const Location2D &other) const {
		return (
			position == other.position and
			// orientation == other.orientation
			std::abs(orientation - other.orientation) < __FLT_EPSILON__
		);
	}

	/// Add a position increment
	void move(const Location2D& delta) {
		Vector<T, 2> movement = delta.position;
		movement.rotate(orientation);

		position += movement;
		orientation = Angle::normalize(orientation + delta.orientation);
	}

	void move(const Vector<T, 2>& delta) {
		Vector<T, 2> movement(delta);

		movement.rotate(orientation);
		position += movement;
	}

	/**
	 * \brief	Add a increment only in x-direction
	 *
	 * Our robots mostly use a differential drive with two driven wheels
	 * side by side, allowing the robot to move only in the drive direction
	 * (x-direction in the local coordinate system of the robot) and
	 * rotate. A movement perpendicular to the drive direction is
	 * impossible without an external force.
	 *
	 * To estimate the position of the robot over time, we use odometry.
	 * Therefore it is necessary to add a lot small increments of
	 * movement over time.
	 * Because the y-component will always be zero, we created this
	 * method, which avoids unnecessary computations for the y-component
	 * and is therefore faster than the universal move-method.
	 *
	 * \param	x		movement in x-direction
	 * \param	phi		rotation
	 */
	void
	move(T x, float phi) {
		Vector<T, 2> vector(Vector<float, 2>(x * std::cos(orientation), x * std::sin(orientation)));
		position += vector;
		orientation = Angle::normalize(orientation + phi);
	}

	constexpr Vector<T, 2> operator+=(const Vector<T, 2>& vector) const {
		Vector<T, 2> result(vector);
		result.rotate(orientation);
		result += position;

		return result;
	}

	constexpr Vector<T, 2> operator-=(const Vector<T, 2>& vector) const {
		Vector<T, 2> result(vector);
		result.rotate(orientation);
		result -= position;

		return result;
	}

	// deprecated constructor
	[[deprecated("Use 'setPosition({x, y}, orientation)' instead!")]]
	constexpr Location2D(const T& x, const T& y, float orientation)
		: position(x, y), orientation(orientation) {}

	// deprecated getters and setters
	[[deprecated("Assign public member directly!")]]
	void setPosition(const Vector<T, 2>& position) { this->position = position; }

	[[deprecated("Assign public member directly!")]]
	void setPosition(T x, T y) { this->position.x() = x; this->position.y() = y; }

	[[deprecated("Assign public member directly!")]]
	void setOrientation(float orientation) { this->orientation = orientation; }

	[[deprecated("Assign public member directly!")]]
	Vector<T, 2> getPosition() const { return position; }

	[[deprecated("Assign public member directly!")]]
	inline float getOrientation() const { return orientation; }

	[[deprecated("Assign public member directly!")]]
	T getX() const { return position.x(); }

	[[deprecated("Assign public member directly!")]]
	T getY() const { return position.y(); }

private:
	template <typename U>
	friend IOStream&
	operator<<( IOStream&, const Location2D<U>&);
};


#if __has_include(<modm/io/iostream.hpp>)
template<typename T>
IOStream&
operator<< (IOStream& os, const Location2D<T>& location) {
	os << location.position << ", phi=" << location.orientation;
	return os;
}
#endif

} // namespace modm