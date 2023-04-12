/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/io/iostream.hpp>
#include <array>
#include <atomic>

/*
 * ADC example with DMA and hardware timer trigger.
 *
 * Channels 0 and 8 are sampled 5 times in a sequence. The sampling is triggered
 * by the Timer Counter Channel 0 TIOA output.
 */

using namespace modm::platform;
using namespace modm::literals;

using Ad2 = GpioA19::Ad; // channel 8, pin AD2 on board
using Ad3 = GpioD30::Ad; // channel 0, pin AD3 on board

constexpr size_t ChannelCount{2};
constexpr size_t ChannelSamples{5};
constexpr size_t TotalSamples{ChannelCount * ChannelSamples};

std::array<uint16_t, TotalSamples> data{};
std::atomic_bool samplingDone{false};

BlockTransfer prepareDmaTransfer()
{
	BlockTransfer transfer;
	transfer.setSourceAddress(Afec0::dataRegister());
	transfer.setDestinationAddress(data.data());
	transfer.setDataLength(TotalSamples);

	Dma::ChannelConfig_t config;
	Dma::TransferType_t::set(config, Dma::TransferType::Peripheral);
	Dma::PeripheralDirection_t::set(config, Dma::PeripheralDirection::PeripheralToMemory);
	// 16 bit data size
	Dma::DataWidth_t::set(config, Dma::DataWidth::HalfWord);
	// peripherals are connected to DMA AHB interface 1
	Dma::SourceBusInterface_t::set(config, Dma::BusInterface::Bus1);
	// data in RAM, all RAMs are connected to DMA AHB interface 0
	Dma::DestinationBusInterface_t::set(config, Dma::BusInterface::Bus0);
	// Auto-increment destination address after each sample
	Dma::DestinationAddressingMode_t::set(config, Dma::AddressingMode::Incrementing);
	// Transfer is triggered by Afec0 conversion
	Dma::DmaRequest_t::set(config, DmaRequests::Afec<0>::Rx);
	transfer.setConfiguration(config);

	return transfer;
}

void startSampling(DmaChannel channel)
{
	channel.startTransfer(prepareDmaTransfer());
	Afec0::enableHardwareTrigger(Afec0::Trigger::Tc0Tioa);
}

void initializeTimer()
{
	TimerChannel0::initialize();
	// 32 kHz slow clock source
	TimerChannel0::setClockSource(TimerChannel0::ClockSource::Slck);
	TimerChannel0::setWaveformMode(true);
	TimerChannel0::setWaveformSelection(TimerChannel0::WavSel::Up_Rc);

	// 32768 Hz / 3277 = ~10 Hz sampling rate, 10 total samples => cycle ~1 Hz
	TimerChannel0::setRegC(3277);
	TimerChannel0::setRegA(100);
	TimerChannel0::setTioaEffects(TimerChannel0::TioEffect::Set, TimerChannel0::TioEffect::Clear);

	TimerChannel0::enable();
	TimerChannel0::start();
}

void printSamples()
{
	MODM_LOG_INFO << "ADC Readings:\n";
	MODM_LOG_INFO << "Channel 0 (AD3): ";
	for (size_t i = 0; i < ChannelSamples; ++i) {
		MODM_LOG_INFO << data[2 * i + 0] << " ";
	}
	MODM_LOG_INFO << '\n';
	MODM_LOG_INFO << "Channel 8 (AD2): ";
	for (size_t i = 0; i < ChannelSamples; ++i) {
		MODM_LOG_INFO << data[2 * i + 1] << " ";
	}
	MODM_LOG_INFO << '\n';
}

int main()
{
	Board::initialize();
	initializeTimer();

	MODM_LOG_INFO << "ADC DMA Test\n";

	Afec0::initialize<Board::SystemClock>();
	Afec0::connect<Ad2::Ad, Ad3::Ad>();
	Afec0::setChannels(Afec0::Channel::Ch0, Afec0::Channel::Ch8);

	Dma::initialize();
	const int priority = 5;
	Dma::enableInterruptVector(priority);

	auto channel = Dma::channel(0);
	channel.enableInterrupts();
	channel.enableInterruptFlag(DmaChannel::Interrupt::EndOfBlock);

	channel.setInterruptHandler([](auto flags) {
		if (flags & DmaChannel::Interrupt::EndOfBlock) {
			Afec0::disableHardwareTrigger();
			samplingDone = true;
		}
	});

	startSampling(channel);

	while (true) {
		if (samplingDone) {
			printSamples();
			samplingDone = false;
			startSampling(channel);
		}
	}
}
