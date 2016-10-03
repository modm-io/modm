#ifndef ENVIRONMENT_HARDWARE_HPP
#define ENVIRONMENT_HARDWARE_HPP

#include <xpcc/architecture/platform.hpp>

// ----------------------------------------------------------------------------
// Sensors at I2C

using SensorsAI2cMaster = xpcc::stm32::I2cMaster1;
using SensorsBI2cMaster = xpcc::stm32::I2cMaster2;

#endif // ENVIRONMENT_HARDWARE_HPP
