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
 * DMA linked list transfer example
 *
 * Output two alternating signals on DAC output 0
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

constexpr std::array<uint16_t, 50> signal0 = computeSinTable(1.0f);
constexpr std::array<uint16_t, 50> signal1 = computeSinTable(2.0f);

using Out0 = GpioB13;

void initializeDac()
{
	Dac::connect<Out0::Ch0>();
	Dac::initialize<SystemClock, 12_MHz>();

	// Max speed mode: DAC runs at DAC clock / 12 and triggers DMA
	Dac::setChannelMode(Dac::Channel::Channel0, Dac::Mode::MaxSpeed);

	Dac::enableChannel(Dac::Channel::Channel0);
}

using ListTransfer = LinkedListTransfer<Dma::View2, Dma::View0Src>;

ListTransfer prepareTransfer()
{
	ListTransfer transfer;

	// Transfer configuration, set by first descriptor.
	// It is not overwritten by the second View0 descriptor.
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

	auto [d0, d1] = transfer.descriptors();

	// Configure first descriptor (type 2)
	// Set: configuration, source, destination, size
	d0.setConfiguration(config);
	d0.setSourceAddress(signal0.data());
	d0.setDestinationAddress(Dac::channel0DataRegister());
	d0.setDataLength(signal0.size());

	// Configure first descriptor (type 0)
	// Update source and size, other parameters remain unchanged
	d1.setSourceAddress(signal1.data());
	d1.setDataLength(signal1.size());

	// Circular mode
	// After the second transfer has finished, the sequence automatically starts
	// again with the first descriptor.
	transfer.setCircularMode(true);

	return transfer;
}

DmaChannel channel = Dma::channel(0);
// Transfer object must remain valid while transfer is active
auto transfer = prepareTransfer();

int main()
{
	Board::initialize();
	initializeDac();

	Dma::initialize();

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
