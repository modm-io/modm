/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

namespace modm::platform
{

template<int instance>
void
ExtInt<instance>::initialize(std::function<void()> handler, InputTrigger trigger,
							 bool wakeupEnabled)
{
	handlers_[instance] = handler;
	if (wakeupEnabled)
	{
		EIC->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN(1u << instance);
	} else
	{
		EIC->WAKEUP.reg &= ~EIC_WAKEUP_WAKEUPEN(1u << instance);
	}
	constexpr int sensePos = instance * EIC_CONFIG_SENSE1_Pos;
	EIC->CONFIG[instance & 0x8].reg &= EIC_CONFIG_SENSE0_Msk << sensePos;
	EIC->CONFIG[instance & 0x8].reg |= uint32_t(trigger) << sensePos;
	EIC->INTENSET.vec.EXTINT |= 1u << instance;
}

}  // namespace modm::platform
