/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <modm/architecture/utils.hpp>

/// @ingroup modm_platform_cortex_m
/// @{

/// Call `function` during boot process.
/// @hideinitializer
#define MODM_HARDWARE_INIT(function) \
	modm_section(".hardware_init") \
	void* MODM_CONCAT(__modm_hardware_init_ptr_, function) = (void*)&function

/// Call `function` during boot process with a unique name.
/// @hideinitializer
#define MODM_HARDWARE_INIT_NAME(name, function) \
	modm_section(".hardware_init") \
	void* MODM_CONCAT(__modm_hardware_init_ptr_, name) = (void*)&function

/// Call `function` during boot process in a global order.
/// @hideinitializer
#define MODM_HARDWARE_INIT_ORDER(function, order) \
	modm_section(".hardware_init.order_" MODM_STRINGIFY(order)) \
	void* MODM_CONCAT(__modm_hardware_init_ptr_, function) = (void*)&function

/// Call `function` during boot process in a global order with a unique name.
/// @hideinitializer
#define MODM_HARDWARE_INIT_NAME_ORDER(name, function, order) \
	modm_section(".hardware_init.order_" MODM_STRINGIFY(order)) \
	void* MODM_CONCAT(__modm_hardware_init_ptr_, name) = (void*)&function

/// @}
