// coding: utf-8
// ----------------------------------------------------------------------------
/**
 * \version		$Id: position.h 8104 2009-08-06 19:00:14Z fabian $
 */
// ----------------------------------------------------------------------------

#ifndef	POSITION_H
#define	POSITION_H

#include <stdint.h>

#include "cartesian_coordinate.h"

typedef CartesianCoordinate<int16_t> Position;

typedef PolarCoordinate<int16_t> PositionPolar;

#endif	// POSITION_H
