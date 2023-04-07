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

#include "iwdg.hpp"

void
Iwdg::initialize(Iwdg::Prescaler prescaler, uint32_t reload)
{
	writeKey(writeCommand);
	IWDG->PR = (IWDG->PR & ~static_cast<uint32_t>(Iwdg::Prescaler::All)) |
			   static_cast<uint32_t>(prescaler);
	IWDG->RLR = (IWDG->RLR & ~IWDG_RLR_RL) | (reload << IWDG_RLR_RL_Pos);
	writeKey(0); // disable access to PR and RLR registers
}

void
Iwdg::enable()
{
	writeKey(enableCommand);
}

void
Iwdg::trigger()
{
	writeKey(reloadCommand);
}

void
Iwdg::writeKey(uint32_t key)
{
	IWDG->KR = (IWDG->KR & ~IWDG_KR_KEY_Msk) | ((key & IWDG_KR_KEY_Msk) << IWDG_KR_KEY_Pos);
}

Iwdg::Status
Iwdg::getStatus()
{
	const auto status = IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU);
	return static_cast<Status>(status);
};