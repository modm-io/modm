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

// DAC1 channel 1 on GpioA4: switch between 10 kHz and 20 kHz sine signal after DMA transfer complete

// The DAC, its DMA channel and its trigger source for timer 4 differ between
// device families, see the reference manuals.
#if defined STM32F4 or defined STM32F7
using MyDac = DacDma;
using DmaChannel = Dma1::Channel5;
constexpr uint8_t TriggerSource = 5;
#elif defined STM32L4
using MyDac = Dac1Dma;
using DmaChannel = Dma1::Channel3;
constexpr uint8_t TriggerSource = 5;
#else
using MyDac = Dac1Dma;
using DmaChannel = Dma1::Channel1;
constexpr uint8_t TriggerSource = 3;
#endif

void setupDac()
{
	using DacChannel = MyDac::Channel1<DmaChannel>;

	MyDac::connect<GpioOutputA4::Out1>();
	MyDac::initialize();

	DacChannel::configure(sinTable1.data(), sinTable1.size(), DmaBase::CircularMode::Disabled);

	DacChannel::setTriggerSource(TriggerSource);

	// switch between signals when transfer completed
	static bool toggleBit = false;
	DmaChannel::setTransferCompleteIrqHandler([]
	{
		DacChannel::stopDma();
		toggleBit = !toggleBit;
		if (toggleBit) {
			DacChannel::setData(sinTable1.data(), sinTable1.size());
		} else {
			DacChannel::setData(sinTable2.data(), sinTable2.size());
		}
		DacChannel::startDma();
	});

	DmaChannel::enableInterruptVector();
	DmaChannel::enableInterrupt(Dma1::InterruptEnable::TransferComplete |
									Dma1::InterruptEnable::TransferError);

	DacChannel::startDma();
	DacChannel::enableDacChannel();
}

int main()
{
	Board::initialize();
	Leds::setOutput();

	MODM_LOG_INFO << "DAC DMA Demo" << modm::endl;

	Dma1::enable();

	setupDac();

	// configure timer 4 as trigger for DACs
	// 1 MHz => 1 Msps DAC output
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);
	Timer4::setPrescaler(1);
	Timer4::setOverflow(Board::SystemClock::Timer4 / 1_MHz - 1);
	Timer4::applyAndReset();
	Timer4::start();

	// Enable trigger out for timer 4
	TIM4->CR2 |= TIM_CR2_MMS_1;

	while (true)
	{
		Leds::toggle();
		modm::delay_ms(500);
	}

	return 0;
}
