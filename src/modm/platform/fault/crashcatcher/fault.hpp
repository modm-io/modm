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
#include "fault_storage.hpp"
#include <modm/architecture/interface/build_id.hpp>
#include <span>

/**
 * Called first after a HardFault occurred.
 * Use this to put your hardware in a safe mode, since generating
 * and storing the fault report may take a second or two before rebooting.
 *
 * @warning This is executed in the HardFault interrupt directly, which has the
 *          highest interrupt priority, so no other interrupt will be able to
 *          fire during this time. BEWARE OF DEADLOCKS!!!
 *
 * @ingroup modm_platform_fault
 */
extern "C" void
modm_hardfault_entry();

namespace modm::platform
{

/**
 * Public interface to accessing fault reports, providing a const_iterator
 * for accessing the read only report.
 *
 * @ingroup modm_platform_fault
 */
class FaultReporter
{
public:
	using const_iterator = FaultStorage::Iterator;
	static inline const_iterator begin() { return FaultStorage::begin(); }
	static inline const_iterator end() { return FaultStorage::end(); }

	/// @returns report size > 0
	static inline bool hasReport() { return begin() != end(); }
	/// @returns a 20-bytes SHA1 of the firmware for identification
	static inline std::span<const uint8_t, 20> buildId() { return modm::build_id(); }
	/// Clears the report
	static inline void clear() { FaultStorage::closeRead(); }
	/// Clears the report and reboots the device
	static void clearAndReboot();
};

}	// namespace modm::platform
