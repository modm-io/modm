#include "cartesian_coordinate.hpp"

template<>
xpcc::CartesianCoordinate<float>&
xpcc::CartesianCoordinate<float>::rotate(const xpcc::Angle& phi)
{
	float c = cos(phi).toFloat();
	float s = sin(phi).toFloat();

	float tx = (c * this->x - s * this->y);
	this->y =  (s * this->x + c * this->y);
	this->x = tx;

	return *this;
}
