/*
 * Copyright (c) 2019, Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "clock.hpp"

#define PRCI_HFROSC_CONF *((volatile uint32_t *)0x10008000)
#define PRCI_HFXOSC_CONF *((volatile uint32_t *)0x10008004)
#define PRCI_PLL_CONF    *((volatile uint32_t *)0x10008008)

void
modm::platform::PRCI::enableInternalOscillator(uint8_t divider,uint8_t trim)
{
	uint32_t confValue = (trim & 0x1F);
	confValue = confValue << 16;
	confValue |= (divider & 0x1F);
	confValue |= 1 << 30;
	PRCI_HFROSC_CONF = confValue;

	//wait for the oscillator to come ready
	while((PRCI_HFROSC_CONF & (1<<30)) == 0){};
}

void
modm::platform::PRCI::disableInternalOscillator(void)
{
	uint32_t confValue = PRCI_HFROSC_CONF;
	confValue &= ~(1 << 30);
	PRCI_HFROSC_CONF = confValue;
}

void
modm::platform::PRCI::enableExternalOscillator(void)
{
	PRCI_HFXOSC_CONF = (1 << 30);
	//wait for the Oscillator to come ready
	while((PRCI_HFXOSC_CONF & (1<<30)) == 0){};
}

void
modm::platform::PRCI::disableExternalOscillator(void)
{
	PRCI_HFXOSC_CONF = 0;
}

void
modm::platform::PRCI::setupPll(PllSource src,
							   bool bypass,
							   PllPrediv pll_r,
							   uint8_t multiplier,
							   PllPostDiv pll_q)
{
	uint32_t config = PRCI_PLL_CONF;
	//turn off the pll output first and enable the bypass
	config &= ~(1<<16);
	config |= (1<<18);
	PRCI_PLL_CONF = config;

	//configure the new pll settings
	config = (static_cast<uint32_t>(pll_r));
	config |= (static_cast<uint32_t>((multiplier/2)-1) & 0x3F ) <<4;
	config |= (static_cast<uint32_t>(pll_q)) << 10;
	//enable the pll
	config |= (1<<16);
	if(src == PllSource::ExternalOscillator)
	{
		config |= (1<<17);
	}
	if(bypass)
	{
		config |= (1<<18);
	}
	// write the new config;
	PRCI_PLL_CONF = config;
	
	//wait until we got a lock
	while((PRCI_PLL_CONF & (1<<31)) == 0){}
}

void
modm::platform::PRCI::disablePll()
{
	uint32_t config = PRCI_PLL_CONF;
	//turn off the pll output first
	config &= ~(1<<16);
	PRCI_PLL_CONF = config;
}