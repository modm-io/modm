/*
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "clocks.hpp"

#include <hardware/structs/xosc.h>

#include "../device.hpp"

// CMSIS Core compliance
constinit uint32_t modm_fastdata SystemCoreClock(modm::platform::ClockControl::BootFrequency);

namespace modm::platform
{
constinit uint16_t modm_fastdata delay_fcpu_MHz(computeDelayMhz(ClockControl::BootFrequency));
constinit uint16_t modm_fastdata delay_ns_per_loop(computeDelayNsPerLoop(ClockControl::BootFrequency));

static uint32_t configured_freq[CLK_COUNT];

void
ClockControl::enableExternalCrystal(uint32_t freq, uint16_t delay_multiplier)
{
	// Assumes 1-15 MHz input, checked above.
	xosc_hw->ctrl = XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ;

	uint32_t delay = ((((freq / 1000) + 128) / 256) * delay_multiplier);

	// Set xosc startup delay
	xosc_hw->startup = delay;

	// Set the enable bit now that we have set freq range and startup delay
	hw_set_bits(&xosc_hw->ctrl, XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB);

	// Wait for XOSC to be stable
	while (!(xosc_hw->status & XOSC_STATUS_STABLE_BITS)) __NOP();
}

void
ClockControl::configureImpl(Clock clk, uint32_t src, uint32_t auxsrc, uint32_t div, uint32_t freq)
{
	clock_hw_t *clock = &clocks_hw->clk[uint32_t(clk)];
	// If increasing divisor, set divisor before source. Otherwise set source
	// before divisor. This avoids a momentary overspeed when e.g. switching
	// to a faster source and increasing divisor to compensate.
	if (div > clock->div) clock->div = div;
	// If switching a glitchless slice to an aux source, switch
	// away from aux *first* to avoid passing glitches when changing aux mux.
	// Assume (!!!) glitchless source 0 is no faster than the aux source.
	if (src == CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX)
	{
		hw_clear_bits(&clock->ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
		while (!(clock->selected & 1u)) __NOP();
	}
	// If no glitchless mux, cleanly stop the clock to avoid glitches
	// propagating when changing aux mux. Note it would be a really bad idea
	// to do this on one of the glitchless clocks.
	else
	{
		// Disable clock. On clk_ref and clk_sys this does nothing,
		// all other clocks have the ENABLE bit in the same position.
		hw_clear_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);
		if (configured_freq[uint32_t(clk)] > 0)
		{
			// Delay for 3 cycles of the target clock, for ENABLE propagation.
			// Note XOSC_COUNT is not helpful here because XOSC is not
			// necessarily running, nor is timer... so, 3 cycles per loop:
			uint32_t delay_cyc = configured_freq[clk_sys] / configured_freq[uint32_t(clk)] + 1;
			asm volatile(
				".syntax unified \n\t"
				"1: \n\t"
				"subs %0, #1 \n\t"
				"bne 1b"
				: "+r"(delay_cyc));
		}
	}

	// Set aux mux first, and then glitchless mux
	hw_write_masked(&clock->ctrl, (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB),
					CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS);

	hw_write_masked(&clock->ctrl, src << CLOCKS_CLK_REF_CTRL_SRC_LSB,
						CLOCKS_CLK_REF_CTRL_SRC_BITS);
	while (!(clock->selected & (1u << src))) __NOP();

	// Enable clock. On clk_ref and clk_sys this does nothing,
	// all other clocks have the ENABLE bit in the same position.
	hw_set_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);

	// Now that the source is configured, we can trust that the user-supplied
	// divisor is a safe value.
	clock->div = div;
	// Store the configured frequency
	configured_freq[uint32_t(clk)] = freq;
}

void
ClockControl::configureImpl(Clock clk, uint32_t auxsrc, uint32_t div, uint32_t freq)
{

	clock_hw_t *clock = &clocks_hw->clk[uint32_t(clk)];
	// If increasing divisor, set divisor before source. Otherwise set source
	// before divisor. This avoids a momentary overspeed when e.g. switching
	// to a faster source and increasing divisor to compensate.
	if (div > clock->div) clock->div = div;
	// Disable clock. On clk_ref and clk_sys this does nothing,
	// all other clocks have the ENABLE bit in the same position.
	hw_clear_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);
	if (configured_freq[uint32_t(clk)] > 0)
	{
		// Delay for 3 cycles of the target clock, for ENABLE propagation.
		// Note XOSC_COUNT is not helpful here because XOSC is not
		// necessarily running, nor is timer... so, 3 cycles per loop:
		uint32_t delay_cyc = configured_freq[clk_sys] / configured_freq[uint32_t(clk)] + 1;
		asm volatile(
			".syntax unified \n\t"
			"1: \n\t"
			"subs %0, #1 \n\t"
			"bne 1b"
			: "+r"(delay_cyc));
	}

	// Set aux mux
	hw_write_masked(&clock->ctrl, (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB),
					CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS);

	// Enable clock. On clk_ref and clk_sys this does nothing,
	// all other clocks have the ENABLE bit in the same position.
	hw_set_bits(&clock->ctrl, CLOCKS_CLK_GPOUT0_CTRL_ENABLE_BITS);

	// Now that the source is configured, we can trust that the user-supplied
	// divisor is a safe value.
	clock->div = div;
	// Store the configured frequency
	configured_freq[uint32_t(clk)] = freq;
}
}  // namespace modm::platform
