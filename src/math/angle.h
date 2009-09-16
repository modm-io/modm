// coding: utf-8
// ----------------------------------------------------------------------------
/**
 * \file		angle.h	
 * 
 * \author		Fabian Greif <fabian.greif@rwth-aachen.de>
 * \version		$Id: angle.h 8106 2009-08-17 14:57:59Z fabian $
 */
// ----------------------------------------------------------------------------

#ifndef	ANGLE_H
#define	ANGLE_H

// ----------------------------------------------------------------------------
class Angle
{
	public:
		Angle(float angle = 0.0) : angle(angle) {
		}
		
		//! \brief	Limit angle to +-Pi
		void
		normalize();
		
		//! \brief	reverse the angle
		void
		reverse();
		
		float
		getAngle() const {
			return this->angle;
		}
		
		Angle&
		operator=(const float& angle) {
			this->angle = angle;
			return *this;
		}
		
		Angle&
		operator+=(const Angle &other);
		
		Angle&
		operator-=(const Angle &other);
	
	private:
		float angle;
};

// ----------------------------------------------------------------------------
Angle&
Angle::operator+=(const Angle &other) {
	this->angle += other.angle;
	return *this;
}

// ----------------------------------------------------------------------------	
Angle&
Angle::operator-=(const Angle &other) {
	this->angle -= other.angle;
	return *this;
}

// ----------------------------------------------------------------------------
Angle
operator-(const Angle &a) {
	return Angle(-a.getAngle());
}


// ----------------------------------------------------------------------------
Angle
operator-(const Angle &a, const Angle &b) {
	return Angle(a.getAngle() - b.getAngle());
}

// ----------------------------------------------------------------------------
Angle
operator+(const Angle &a, const Angle &b) {
	return Angle(a.getAngle() + b.getAngle());
}

// ----------------------------------------------------------------------------
bool
operator==(const Angle &a, const Angle &b) {
	return (a.getAngle() == b.getAngle());
}

// ----------------------------------------------------------------------------
bool
operator!=(const Angle &a, const Angle &b) {
	return (a.getAngle() != b.getAngle());
}

#endif	// ANGLE_H
