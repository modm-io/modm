/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/platform/dma/dma.hpp>
#include <algorithm>
#include <numeric>

using namespace modm::platform;

// Copies memory once and then in circular mode, then transfers SPI data with
// DMA. Without a wire from MOSI to MISO, MISO reads the internal pull-up.
// The results are logged, if the board has no logger read them with a debugger.
std::array<uint32_t, 64> source, destination;
volatile uint32_t completions{0};
// 0: running, 1: copy ok, 2: circular ok, 3: SPI ok, negative: failed step
volatile int32_t result{0};

static bool
copy(DmaBase::CircularMode circular)
{
	using Copy = Gpdma1::Channel0;
	std::ranges::fill(destination, 0);
	completions = 0;
	Copy::configure(DmaBase::DataTransferDirection::MemoryToMemory,
			DmaBase::MemoryDataSize::Word, DmaBase::PeripheralDataSize::Word,
			DmaBase::MemoryIncrementMode::Increment, DmaBase::PeripheralIncrementMode::Increment,
			DmaBase::Priority::Medium, circular);
	Copy::setPeripheralAddress(uintptr_t(source.data()));
	Copy::setMemoryAddress(uintptr_t(destination.data()));
	Copy::setDataLength(source.size());
	Copy::setTransferCompleteIrqHandler([] { completions = completions + 1; });
	Copy::enableInterrupt(DmaBase::InterruptEnable::TransferComplete);
	Copy::enableInterruptVector();
	Copy::start();

	const uint32_t expected = (circular == DmaBase::CircularMode::Enabled) ? 10 : 1;
	const auto start = modm::Clock::now();
	while (completions < expected and modm::Clock::now() - start < 100ms) ;
	Copy::stop();
	return completions >= expected and destination == source;
}

#if __has_include(<modm/platform/spi/spi_master_1_dma.hpp>)
static bool
spi()
{
	using Spi = SpiMaster1_Dma<Gpdma1::Channel1, Gpdma1::Channel2>;
	using Sck = GpioA5;
	using Miso = GpioA6;
	using Mosi = GpioA7;
	Spi::connect<Sck::Sck, Miso::Miso, Mosi::Mosi>();
	Miso::configure(Gpio::InputType::PullUp);
	Spi::initialize<Board::SystemClock, 1_MHz, modm::pct(50)>();

	std::array<uint8_t, 100> tx, rx;
	std::iota(tx.begin(), tx.end(), 0);
	rx.fill(0);
	Spi::transfer(tx.data(), rx.data(), rx.size());
	if (std::ranges::all_of(rx, [](uint8_t b) { return b == 0xff; })) return true;
	// A wire from MOSI to MISO loops the data back
	return rx == tx;
}
#else
static bool spi() { return true; }
#endif

int
main()
{
	Board::initialize();
	Gpdma1::enable();
	std::iota(source.begin(), source.end(), 0x1234'0000);

	result = copy(DmaBase::CircularMode::Disabled) ? 1 : -1;
	if (result > 0) result = copy(DmaBase::CircularMode::Enabled) ? 2 : -2;
	if (result > 0) result = spi() ? 3 : -3;
#ifdef MODM_BOARD_HAS_LOGGER
	MODM_LOG_INFO << "GPDMA test result: " << int(result) << modm::endl;
#endif

	while (true)
	{
		Board::Leds::toggle();
		modm::delay(result > 0 ? 500ms : 100ms);
	}
	return 0;
}
