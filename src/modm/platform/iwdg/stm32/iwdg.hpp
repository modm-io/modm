/*
 * Copyright (c) 2023, Zühlke Engineering (Austria) GmbH
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "../device.hpp"


namespace modm::platform
{

/// @ingroup modm_platform_iwdg
class Iwdg
{
public:
	enum class
	Prescaler : uint8_t
	{
		Div4 = 0,
		Div8 = IWDG_PR_PR_0,
		Div16 = IWDG_PR_PR_1,
		Div32 = IWDG_PR_PR_1 | IWDG_PR_PR_0,
		Div64 = IWDG_PR_PR_2,
		Div128 = IWDG_PR_PR_2 | IWDG_PR_PR_0,
		Div256 = IWDG_PR_PR_2 | IWDG_PR_PR_1,
		All = IWDG_PR_PR_2 | IWDG_PR_PR_1 | IWDG_PR_PR_0,
	};

	enum class
	Status : uint8_t
	{
		None = 0,
		Prescaler = IWDG_SR_PVU,
		Reload = IWDG_SR_RVU,
		All = IWDG_SR_PVU | IWDG_SR_RVU,
	};

public:
	static inline void
	initialize(Prescaler prescaler, uint32_t reload)
	{
		writeKey(writeCommand);
		IWDG->PR = uint32_t(prescaler);
		IWDG->RLR = reload;
		writeKey(0); // disable access to PR and RLR registers
	}

	static inline void
	enable()
	{
		writeKey(enableCommand);
	}

	static inline void
	trigger()
	{
		writeKey(reloadCommand);
	}

	static inline Status
	getStatus()
	{
		return Status(IWDG->SR);
	}

private:
	static constexpr uint16_t reloadCommand = 0xAAAA;
	static constexpr uint16_t writeCommand = 0x5555;
	static constexpr uint16_t enableCommand = 0xCCCC;

	static inline void
	writeKey(uint16_t key)
	{
		IWDG->KR = key;
	}
};

}
