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

#include <CrashCatcher.h>
#include "fault.hpp"
#include <modm/platform/device.hpp>

modm_weak void modm_hardfault_entry() {}

namespace modm::platform
{
static FaultStorage fault_storage;

void
FaultReporter::clearAndReboot()
{
	clear();
	NVIC_SystemReset();
}

}

extern "C" void
CrashCatcher_DumpStart(const CrashCatcherInfo *)
{
	modm_hardfault_entry();
	modm::platform::fault_storage.openWrite();
}

extern "C" void
CrashCatcher_DumpMemory(const void* pvMemory,
						CrashCatcherElementSizes elementSize,
						size_t elementCount)
{
	modm::platform::fault_storage.write(pvMemory, size_t(elementSize), elementCount);
}

extern "C" CrashCatcherReturnCodes
CrashCatcher_DumpEnd(void)
{
	modm::platform::fault_storage.closeWrite();
	NVIC_SystemReset();
	return CRASH_CATCHER_EXIT;
}
