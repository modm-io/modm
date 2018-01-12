#ifndef ENVIRONMENT_HARDWARE_HPP
#define ENVIRONMENT_HARDWARE_HPP

#include <modm/board/board.hpp>

// ----------------------------------------------------------------------------
// Sensors at I2C

// using MyI2cMaster = SoftwareI2cMaster<Board::D15, Board::D14>;
using MyI2cMaster = I2cMaster1;

#endif // ENVIRONMENT_HARDWARE_HPP
