// coding: utf-8
// ----------------------------------------------------------------------------
/**
 * \version		$Id: location.h 8104 2009-08-06 19:00:14Z fabian $
 */
// ----------------------------------------------------------------------------

#ifndef	LOCATION_H
#define	LOCATION_H

#include "angle.h"
#include "position.h"

class Location
{
	public:
		/** \brief	Add a position increment */
		void
		update(Location& diff);
		
		/** \brief	Add a increment only in x-direction */
		void
		update(int16_t x, Angle& phi);
		
		// Attributes
		Position position;
		Angle phi;
};

// ----------------------------------------------------------------------------
void
Location::update(Location& diff) {
	this->position += diff.position.rotate(this->phi);
	this->phi += diff.phi;
	this->phi.normalize();
}

// ----------------------------------------------------------------------------
void
Location::update(int16_t x, Angle& phi) {
	this->position += Position(x * cos(this->phi), x * sin(this->phi));
	this->phi += phi;
	this->phi.normalize();
}

#endif	// LOCATION_H
