/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <cmath>
#include <numbers>

using namespace Board;

/*
 * DMA block transfer example
 *
 * Output sine signal on DAC output 0
 */

constexpr auto computeSinTable(float frequency = 1.f)
{
	std::array<uint16_t, 50> data{};
	constexpr auto HalfOutput = Dac::MaxOutput / 2.f;
	for (size_t i = 0; i < data.size(); ++i) {
		constexpr auto pi = std::numbers::pi_v<float>;
		data[i] = HalfOutput * (1 + sin(i * (2 * pi * frequency / data.size())));
	}
	return data;
}

constexpr std::array<uint16_t, 50> sinTable = computeSinTable(1.0f);

using Out0 = GpioB13;

void initializeDac()
{
	Dac::connect<Out0::Ch0>();
	Dac::initialize<SystemClock, 12_MHz>();

	// Max speed mode: DAC runs at DAC clock / 12 and triggers DMA
	Dac::setChannelMode(Dac::Channel::Channel0, Dac::Mode::MaxSpeed);

	Dac::enableChannel(Dac::Channel::Channel0);
}

BlockTransfer prepareTransfer()
{
	BlockTransfer transfer;
	transfer.setSourceAddress(sinTable.data());
	transfer.setDestinationAddress(Dac::channel0DataRegister());
	transfer.setDataLength(sinTable.size());

	Dma::ChannelConfig_t config;
	Dma::TransferType_t::set(config, Dma::TransferType::Peripheral);
	Dma::PeripheralDirection_t::set(config, Dma::PeripheralDirection::MemoryToPeripheral);
	// 16 bit data size
	Dma::DataWidth_t::set(config, Dma::DataWidth::HalfWord);
	// const data in flash, flash is connected to DMA AHB interface 1
	Dma::SourceBusInterface_t::set(config, Dma::BusInterface::Bus1);
	// peripherals are connected to DMA AHB interface 1
	Dma::DestinationBusInterface_t::set(config, Dma::BusInterface::Bus1);
	// Auto-increment source address after each sample
	Dma::SourceAddressingMode_t::set(config, Dma::AddressingMode::Incrementing);
	// Transfer is triggered by DAC channel 0 request
	Dma::DmaRequest_t::set(config, DmaRequests::Dacc::Ch0Tx);
	transfer.setConfiguration(config);

	return transfer;
}

DmaChannel channel = Dma::channel(0);
const auto transfer = prepareTransfer();

int main()
{
	Board::initialize();
	initializeDac();

	Dma::initialize();
	const int priority = 5;
	Dma::enableInterruptVector(priority);

	channel.enableInterrupts();
	channel.enableInterruptFlag(DmaChannel::Interrupt::EndOfBlock);

	channel.setInterruptHandler([](auto flags) {
		if (flags & DmaChannel::Interrupt::EndOfBlock) {
			// transfer completed, restart
			channel.startTransfer(transfer);
		}
	});

	channel.startTransfer(transfer);

	while (true)
	{
		Led0::toggle();
		Led1::toggle();
		modm::delay(500ms);

		MODM_LOG_INFO << "ping" << modm::endl;
	}

	return 0;
}
