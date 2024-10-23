/*
 * Copyright (c) 2026, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/board.hpp>

/* from https://github.com/raspberrypi/pico-examples/blob/master/pio/ws2812/ws2812.pio */

/* .program ws2812 */
namespace ws2812
{
	using namespace modm::platform::pio;

	// constants WS2812
	static constexpr uint32_t T0H = 350; // ns
	static constexpr uint32_t T0L = 800;
	static constexpr uint32_t T1H = 700;
	static constexpr uint32_t T1L = 600;

	// constants WS2812B
	// static constexpr uint32_t T0H = 400; // ns
	// static constexpr uint32_t T0L = 850;
	// static constexpr uint32_t T1H = 850;
	// static constexpr uint32_t T1L = 400;

	static constexpr uint32_t TimeScale = 50;

	static constexpr uint32_t TH_Common = T0H/TimeScale;
	static constexpr uint32_t TH_Add = (T1H-T0H)/TimeScale;
	static constexpr uint32_t TL_Common = T1L/TimeScale;
	static constexpr uint32_t TL_Add = (T0L-T1L)/TimeScale;

	// labels
	struct bitloop {};
	struct do_one {};
	struct do_zero {};

	/*
	.side_set 1
	.wrap_target
	bitloop:
	    out x, 1       side 0 [T3 - 1] ; Side-set still takes place when instruction stalls
	    jmp !x do_zero side 1 [T1 - 1] ; Branch on the bit we shifted out. Positive pulse
	do_one:
	    jmp  bitloop   side 1 [T2 - 1] ; Continue driving high, for a long pulse
	do_zero:
	    nop            side 0 [T2 - 1] ; Or drive low, for a short pulse
	.wrap
	*/

	// PIO program
	static constexpr auto program = PIOProgram::begin()
		.sideset<1>
		.wrapTarget<>
		.label<bitloop>
			.instr(pio::Out.x<1>				.side<0>.delay<TL_Common-1>)
			.instr(pio::Jmp.not_x.to<do_zero>	.side<1>.delay<TH_Common-1>)
		.label<do_one>
			.instr(pio::Jmp.to<bitloop>			.side<1>.delay<TH_Add-1>)
		.label<do_zero>
			.instr(pio::Nop						.side<0>.delay<TL_Add-1>)
		.wrap<>
	.end();

	/*
	pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
    */
	// initialization code
	template <typename SystemClock,typename PIO,typename SM,typename DataGpio>
	static constexpr void init(uint16_t offset) {
		PIO::template connect<typename DataGpio::Pad>();
		SM::template addOutput<DataGpio>();

		SM::config(offset,program)
			.template setSidesetPins<DataGpio>()
			.template setOutShift<false,true,24>()
			.setFifoJoinTx()
			.template setFrequency<SystemClock,1000000000/TimeScale>()
			.init(offset+program.getOffset<bitloop>());
		SM::setEnabled(true);
	}

}
