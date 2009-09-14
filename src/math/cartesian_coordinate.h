// coding: utf-8
// ----------------------------------------------------------------------------
/**
 * \file		Cartesian coordinate system
 * 
 * \author		Fabian Greif <fabian.greif@rwth-aachen.de>
 * \version		$Id: cartesian_coordinate.h 8106 2009-08-17 14:57:59Z fabian $
 */
// ----------------------------------------------------------------------------

#ifndef	CARTESIAN_COORDINATE_H
#define	CARTESIAN_COORDINATE_H

#include <math.h>
#include <stdint.h>

#include "angle.h"
#include "polar_coordinate.h"

// forward declaration, needed because of circular reference 
// with class PolarCoordinate
template<typename T>
class PolarCoordinate;

// ----------------------------------------------------------------------------
/**
 * \brief	Cartesian coordinates
 * 
 */
template<typename T>
class CartesianCoordinate
{
	public:
		CartesianCoordinate(T x=0, T y=0);
		
		/** \brief	Calculate length */
		T
		length() const;
		
		/** \brief	Calculate absolute angle */
		Angle
		angle() const;
		
		/** \brief	Normalize length to 1 */
		CartesianCoordinate&
		normalize();
		
		CartesianCoordinate&
		rotate(Angle& phi);
		
		/** \brief	Transform to polar coordiante system */
		PolarCoordinate<T>
		polar();
		
		CartesianCoordinate &
		operator+=(const CartesianCoordinate &other);
		
		CartesianCoordinate &
		operator-=(const CartesianCoordinate &other);
		
		T x;
		T y;
};

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>::CartesianCoordinate(T x, T y) : x(x), y(y) {
	// nothing else to do here
}

// ----------------------------------------------------------------------------
template<typename T>
T
CartesianCoordinate<T>::length() const {
	float tx = x;
	float ty = y;
	
	return sqrt(tx*tx + ty*ty);
}

#if defined(__AVR__) && defined(__AVR_HAVE_MUL__)
#include "../hardware/avr.h"
// ----------------------------------------------------------------------------
template<>
int16_t
CartesianCoordinate<int16_t>::length() const {
	int32_t t;
	t = avr::mul32(x, x);
	t = avr::mac32(t, y, y);
	
	return avr::sqrt32_round(t);
}
#endif

// ----------------------------------------------------------------------------
template<typename T>
Angle
CartesianCoordinate<T>::angle() const {
	return Angle(atan2(this->y, this->x));
}

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>&
CartesianCoordinate<T>::normalize() {
	T length(this->length());
	
	this->x = this->x / length;
	this->y = this->y / length;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>&
CartesianCoordinate<T>::rotate(Angle& phi) {
	float c = cos(phi);
	float s = sin(phi);
	
	T tx = 		c * this->x - s * this->y;
	this->y =	s * this->x + c * this->y;
	this->x = tx;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
PolarCoordinate<T>
CartesianCoordinate<T>::polar() {
	return PolarCoordinate<T>(this->length(), this->angle());
}

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>&
CartesianCoordinate<T>::operator+=(const CartesianCoordinate &other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

// ----------------------------------------------------------------------------	
template<typename T>
CartesianCoordinate<T>&
CartesianCoordinate<T>::operator-=(const CartesianCoordinate &other) {
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>
operator-(const CartesianCoordinate<T> &a) {
	return CartesianCoordinate<T>(-a.x, -a.y);
}

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>
operator-(const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b) {
	return CartesianCoordinate<T>(a.x - b.x, a.y - b.y);
}

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>
operator+(const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b) {
	return CartesianCoordinate<T>(a.x + b.x, a.y + b.y);
}


// ----------------------------------------------------------------------------
template<typename T>
bool
operator==(const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b) {
	return (a.x == b.x) && (a.y == b.y);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator!=(const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b) {
	return (a.x != b.x) || (a.y != b.y);
}

#endif	// CARTESIAN_COORDINATE_H
