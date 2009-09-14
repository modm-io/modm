// coding: utf-8
// ----------------------------------------------------------------------------
/**
 * \file		Polar coordinate system
 * 
 * \author		Fabian Greif <fabian.greif@rwth-aachen.de>
 * \version		$Id: polar_coordinate.h 8106 2009-08-17 14:57:59Z fabian $
 */
// ----------------------------------------------------------------------------

#ifndef	POLAR_COORDINATE_H
#define	POLAR_COORDINATE_H

#include <math.h>

#include "angle.h"
#include "cartesian_coordinate.h"

// forward declaration, needed because of circular reference 
// with class CartesianCoordinate
template<typename T>
class CartesianCoordinate;

// ----------------------------------------------------------------------------
/**
 * \brief	Polar coordinates
 * 
 */
template<typename T>
class PolarCoordinate
{
	public:
		PolarCoordinate(T r=0, Angle& theta=Angle(0.0));
		
		/** \brief	Calculate length */
		T
		length() const;
		
		/** \brief	Calculate absolute angle */
		Angle
		angle() const;
		
		/** \brief	Normalize length to 1 */
		void
		normalize();
		
		void
		scale(float a);
		
		/** \brief	Transform to cartesian coordinate system */
		CartesianCoordinate<T>
		cartesian();
		
		operator CartesianCoordinate<T>() {
			return this->cartesian();
		}
		
		// attributes
		T r;			//!< radius
		Angle theta;	//!< azimuth
};

// ----------------------------------------------------------------------------
template<typename T>
PolarCoordinate<T>::PolarCoordinate(T r, Angle& theta) : r(r), theta(theta) {
	// nothing else to do here
}

// ----------------------------------------------------------------------------
template<typename T>
T PolarCoordinate<T>::length() const {
	return this->r;
}

// ----------------------------------------------------------------------------
template<typename T>
Angle PolarCoordinate<T>::angle() const {
	return this->theta;
}

// ----------------------------------------------------------------------------
template<typename T>
void PolarCoordinate<T>::normalize() {
	this->r = 1;
}

// ----------------------------------------------------------------------------
template<typename T>
void PolarCoordinate<T>::scale(float a) {
	this->r *= a;
}

// ----------------------------------------------------------------------------
template<typename T>
CartesianCoordinate<T>
PolarCoordinate<T>::cartesian() {
	return CartesianCoordinate<T>(r * cos(theta), r * sin(theta));
}

// ----------------------------------------------------------------------------
template<typename T>
PolarCoordinate<T>
operator-(const PolarCoordinate<T> &a) {
	return PolarCoordinate<T>(a.r, -a.theta);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator==(const PolarCoordinate<T> &a, const PolarCoordinate<T> &b) {
	return (a.r == b.r) && (a.theta == b.theta);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator!=(const PolarCoordinate<T> &a, const PolarCoordinate<T> &b) {
	return (a.r != b.r) || (a.theta != b.theta);
}

#endif	// POLAR_COORDINATE_H
