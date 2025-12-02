/**
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 *
 * This software component is provided to you as part of a software package and
 * applicable license terms are in the  Package_license file. If you received this
 * software component outside of a package or without applicable license terms,
 * the terms of the BSD-3-Clause license shall apply.
 * You may obtain a copy of the BSD-3-Clause at:
 * https://opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#ifndef VL53_PLATFORM_H_
#define VL53_PLATFORM_H_
#pragma once

#if __has_include("modm_config_vl53.h")
#include "modm_config_vl53.h"
#endif

#include <stdint.h>
#include <string.h>

/*
 * @brief The macro below is used to define the number of target per zone sent
 * through I2C. This value can be changed by user, in order to tune I2C
 * transaction, and also the total memory size (a lower number of target per
 * zone means a lower RAM). The value must be between 1 and 4.
 */

#define VL53L5CX_NB_TARGET_PER_ZONE 1U

/*
 * @brief The macro below can be used to avoid data conversion into the driver.
 * By default there is a conversion between firmware and user data. Using this macro
 * allows to use the firmware format instead of user format. The firmware format allows
 * an increased precision.
 */

// #define 	VL53L5CX_USE_RAW_FORMAT

/*
 * @brief All macro below are used to configure the sensor output. User can
 * define some macros if he wants to disable selected output, in order to reduce
 * I2C access.
 */

// #define VL53L5CX_DISABLE_AMBIENT_PER_SPAD
// #define VL53L5CX_DISABLE_NB_SPADS_ENABLED
// #define VL53L5CX_DISABLE_NB_TARGET_DETECTED
// #define VL53L5CX_DISABLE_SIGNAL_PER_SPAD
// #define VL53L5CX_DISABLE_RANGE_SIGMA_MM
// #define VL53L5CX_DISABLE_DISTANCE_MM
// #define VL53L5CX_DISABLE_REFLECTANCE_PERCENT
// #define VL53L5CX_DISABLE_TARGET_STATUS
// #define VL53L5CX_DISABLE_MOTION_INDICATOR

/*
 * @brief The macro below is used to define the number of target per zone sent
 * through I2C. This value can be changed by user, in order to tune I2C
 * transaction, and also the total memory size (a lower number of target per
 * zone means a lower RAM usage).
 */

#define VL53LMZ_NB_TARGET_PER_ZONE 1

/*
 * @brief All macro below are used to configure the sensor output. User can
 * define some macros if he wants to disable selected output, in order to reduce
 * I2C access.
 */

// #define VL53LMZ_DISABLE_AMBIENT_PER_SPAD
// #define VL53LMZ_DISABLE_NB_SPADS_ENABLED
// #define VL53LMZ_DISABLE_NB_TARGET_DETECTED
// #define VL53LMZ_DISABLE_SIGNAL_PER_SPAD
// #define VL53LMZ_DISABLE_RANGE_SIGMA_MM
// #define VL53LMZ_DISABLE_DISTANCE_MM
// #define VL53LMZ_DISABLE_REFLECTANCE_PERCENT
// #define VL53LMZ_DISABLE_TARGET_STATUS
// #define VL53LMZ_DISABLE_SCI

/**
 * @brief Structure VL53_Platform needs to be filled by the customer,
 * depending on his platform. At least, it contains the VL53 I2C address.
 * Some additional fields can be added, as descriptors, or platform
 * dependencies. Anything added into this structure is visible into the platform
 * layer.
 */

typedef struct
{
	/* To be filled with customer's platform. At least an I2C address/descriptor
	 * needs to be added */
	/* Example for most standard platform : I2C address of sensor */
	uint16_t address;

	void *transport;

} VL53_Platform;

/*
 * @brief The macro below is used to define the number of target per zone sent
 * through I2C. This value can be changed by user, in order to tune I2C
 * transaction, and also the total memory size (a lower number of target per
 * zone means a lower RAM). The value must be between 1 and 4.
 */

#define VL53_NB_TARGET_PER_ZONE 1U

/*
 * @brief The macro below can be used to avoid data conversion into the driver.
 * By default there is a conversion between firmware and user data. Using this macro
 * allows to use the firmware format instead of user format. The firmware format allows
 * an increased precision.
 */

// #define 	VL53_USE_RAW_FORMAT

/*
 * @brief All macro below are used to configure the sensor output. User can
 * define some macros if he wants to disable selected output, in order to reduce
 * I2C access.
 */

// #define VL53_DISABLE_AMBIENT_PER_SPAD
// #define VL53_DISABLE_NB_SPADS_ENABLED
// #define VL53_DISABLE_NB_TARGET_DETECTED
// #define VL53_DISABLE_SIGNAL_PER_SPAD
// #define VL53_DISABLE_RANGE_SIGMA_MM
// #define VL53_DISABLE_DISTANCE_MM
// #define VL53_DISABLE_REFLECTANCE_PERCENT
// #define VL53_DISABLE_TARGET_STATUS
// #define VL53_DISABLE_MOTION_INDICATOR

/**
 * @param (VL53_Platform*) p_platform : Pointer of VL53 platform
 * structure.
 * @param (uint16_t) Address : I2C location of value to read.
 * @param (uint8_t) *p_values : Pointer of value to read.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t
VL53_RdByte(VL53_Platform *p_platform, uint16_t RegisterAdress, uint8_t *p_value);

/**
 * @brief Mandatory function used to write one single byte.
 * @param (VL53_Platform*) p_platform : Pointer of VL53 platform
 * structure.
 * @param (uint16_t) Address : I2C location of value to read.
 * @param (uint8_t) value : Pointer of value to write.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t
VL53_WrByte(VL53_Platform *p_platform, uint16_t RegisterAdress, uint8_t value);

/**
 * @brief Mandatory function used to read multiples bytes.
 * @param (VL53_Platform*) p_platform : Pointer of VL53 platform
 * structure.
 * @param (uint16_t) Address : I2C location of values to read.
 * @param (uint8_t) *p_values : Buffer of bytes to read.
 * @param (uint32_t) size : Size of *p_values buffer.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t
VL53_RdMulti(VL53_Platform *p_platform, uint16_t RegisterAdress, uint8_t *p_values, uint32_t size);

/**
 * @brief Mandatory function used to write multiples bytes.
 * @param (VL53_Platform*) p_platform : Pointer of VL53 platform
 * structure.
 * @param (uint16_t) Address : I2C location of values to write.
 * @param (uint8_t) *p_values : Buffer of bytes to write.
 * @param (uint32_t) size : Size of *p_values buffer.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t
VL53_WrMulti(VL53_Platform *p_platform, uint16_t RegisterAdress, uint8_t *p_values, uint32_t size);

/**
 * @brief Optional function, only used to perform an hardware reset of the
 * sensor. This function is not used in the API, but it can be used by the host.
 * This function is not mandatory to fill if user don't want to reset the
 * sensor.
 * @param (VL53_Platform*) p_platform : Pointer of VL53 platform
 * structure.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t
VL53_Reset_Sensor(VL53_Platform *p_platform);

/**
 * @brief Mandatory function, used to swap a buffer. The buffer size is always a
 * multiple of 4 (4, 8, 12, 16, ...).
 * @param (uint8_t*) buffer : Buffer to swap, generally uint32_t
 * @param (uint16_t) size : Buffer size to swap
 */

void
VL53_SwapBuffer(uint8_t *buffer, uint16_t size);
/**
 * @brief Mandatory function, used to wait during an amount of time. It must be
 * filled as it's used into the API.
 * @param (VL53_Platform*) p_platform : Pointer of VL53 platform
 * structure.
 * @param (uint32_t) TimeMs : Time to wait in ms.
 * @return (uint8_t) status : 0 if wait is finished.
 */

uint8_t
VL53_WaitMs(VL53_Platform *p_platform, uint32_t TimeMs);

#endif  // _PLATFORM_H_