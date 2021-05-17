/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

#include <numbers>
#include <cmath>
#include <array>

using namespace Board;

constexpr auto computeSinTable(float frequency = 1.f)
{
	std::array<uint16_t, 100> data{};
	constexpr auto HalfOutput = ((1 << 12) - 1) / 2; // 12 bit dac
	for (size_t i = 0; i < data.size(); ++i) {
		constexpr auto pi = std::numbers::pi_v<float>;
		data[i] = HalfOutput * (1 + sin(i * (2 * pi * frequency / data.size())));
	}
	return data;
}

constexpr auto sinTable1 = computeSinTable(1.0f);
constexpr auto sinTable2 = computeSinTable(2.0f);

// DAC1 channel 1 on GpioA4: constantly output 20kHz sine signal in circular mode
// DAC4 channel 1 on GpioB12: switch between 20kHz and 40 kHz sine signal after DMA transfer complete

void setupDac1()
{
	using Dac = Dac1Dma;
	using DacChannel = Dac::Channel1<Dma1::Channel1>;

	Dac::connect<GpioOutputA4::Out1>();
	Dac::initialize<Board::SystemClock>();

	DacChannel::configure(sinTable1.data(), sinTable1.size(), DmaBase::CircularMode::Enabled);

	// trigger source 5: timer 4 for DAC4, see reference manual
	DacChannel::setTriggerSource(5);

	DacChannel::startDma();
	DacChannel::enableDacChannel();
}

void setupDac4()
{
	using Dac = Dac4Dma;
	using DacChannel = Dac::Channel1<Dma1::Channel2>;

	Dac::initialize<Board::SystemClock>();

	// Internal output to opamp 4
	DacChannel::setMode(DacChannel::Mode::Internal);

	DacChannel::configure(sinTable1.data(), sinTable1.size(), DmaBase::CircularMode::Disabled);

	// trigger source 5: timer 4 for DAC4, see reference manual
	DacChannel::setTriggerSource(5);

	// switch between signals when transfer completed
	static bool toggleBit = false;
	Dma1::Channel1::setTransferCompleteIrqHandler([]() {
		DacChannel::stopDma();
		toggleBit = !toggleBit;
		if (toggleBit) {
			DacChannel::setData(sinTable1.data(), sinTable1.size());
		} else {
			DacChannel::setData(sinTable2.data(), sinTable2.size());
		}
		DacChannel::startDma();
	});

	Dma1::Channel1::enableInterruptVector();
	Dma1::Channel1::enableInterrupt(Dma1::Interrupt::TransferComplete | Dma1::Interrupt::Error);

	// DAC4 channel 1 output via opamp 4 in follower mode connected to output B12
	GpioOutputB12::setAnalogInput();
	OPAMP4->CSR = OPAMP_CSR_OPAMPxEN | OPAMP_CSR_VPSEL | OPAMP_CSR_VMSEL;

	DacChannel::startDma();
	DacChannel::enableDacChannel();
}

int main()
{
	Board::initialize();
	LedD13::setOutput();

	MODM_LOG_INFO << "DAC DMA Demo" << modm::endl;

	Dma1::enable();

	setupDac1();
	setupDac4();

	// configure timer 4 as trigger for DACs
	// 170 MHz / 85 = 2 MHz => 2 Msps DAC output
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);
	Timer4::setPrescaler(1);
	Timer4::setOverflow(85 - 1);
	Timer4::applyAndReset();
	Timer4::start();

	// Enable trigger out for timer 4
	TIM4->CR2 |= TIM_CR2_MMS_1;

	while (true)
	{
		LedD13::toggle();
		modm::delay_ms(500);
	}

	return 0;
}
