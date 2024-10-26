/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
using namespace Board;

/* from https://github.com/raspberrypi/pico-examples/blob/master/pio/ws2812/ws2812.pio */

// constants WS2812
// static constexpr uint32_t T0H = 350; // ns
// static constexpr uint32_t T0L = 800;
// static constexpr uint32_t T1H = 700;
// static constexpr uint32_t T1L = 600;

// constants WS2812B
static constexpr uint32_t T0H = 400; // ns
static constexpr uint32_t T0L = 850;
static constexpr uint32_t T1H = 850;
static constexpr uint32_t T1L = 400;

static constexpr uint32_t TimeScale = 50;

static constexpr uint32_t TH_Common = T0H/TimeScale;
static constexpr uint32_t TH_Add = (T1H-T0H)/TimeScale;
static constexpr uint32_t TL_Common = T1L/TimeScale;
static constexpr uint32_t TL_Add = (T0L-T1L)/TimeScale;

static constexpr uint32_t T3 = 12;//

// labels
struct bitloop {};
struct do_one {};
struct do_zero {};


/*
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
static constexpr auto pio_prog = PIOProgram::begin()
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


struct HW
{
	using PIO = modm::platform::Pio0;
	using PIO_SM = PIO::StateMachine<0>;
	using DataGpio = modm::platform::GpioOutput23;
};


int
main()
{


	Board::initialize();

	HW::DataGpio::setOutput(Gpio::OutputType::PushPull, Gpio::SlewRate::Fast);
	HW::DataGpio::setDriveStrength(Gpio::DriveStrength::mA_12);

	auto pio_prog_offset = HW::PIO::addProgram(pio_prog);

	HW::PIO::connect<HW::DataGpio::Pad>();

	HW::PIO_SM::addOutput<HW::DataGpio>();

	HW::PIO_SM::config()
		.setup(pio_prog_offset,pio_prog)
		.setSidesetPins<HW::DataGpio,1,false,false>()
		.setFifoJoinTx()
		.setOutShift<false,true,24>()
		.setFrequency<Board::SystemClock,1000000000/TimeScale>()
		.init(pio_prog_offset+pio_prog.getOffset<bitloop>());

	HW::PIO_SM::setEnabled(true);

	constexpr auto delay_val = 5ms;

	while (true)
	{
		uint32_t clr = 0;
		while (clr!=0xff0000) {
			clr = clr + 0x010000;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x000000) {
			clr = clr - 0x010000;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x00ff00) {
			clr = clr + 0x000100;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x000000) {
			clr = clr - 0x000100;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x0000ff) {
			clr = clr + 0x000001;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x000000) {
			clr = clr - 0x000001;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}

	}

	return 0;
}
