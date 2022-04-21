/*
 * Copyright (c) 2021-2022, Raphael Lehmann
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <array>
#include <cmath>
#include <limits>
#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/driver/dac/cs43l22.hpp>
#include <modm/io/iostream.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>
#include <numbers>

#include <arm_math.h>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

using namespace Board;

template<typename T, std::size_t length>
constexpr auto computeSinTable(uint8_t cycles=1)
{
	std::array<T, length> data{};
	constexpr auto HalfOutput = std::numeric_limits<T>::max() / 2; // 16 bit full scale
	for (size_t i = 0; i < data.size(); ++i) {
		constexpr auto pi = std::numbers::pi_v<float>;
		data[i] = HalfOutput * (1 + arm_sin_f32(i * (2*pi / data.size() * cycles)));
	}
	return data;
}

constexpr std::size_t bufferSize = 960;
auto bufferA = computeSinTable<uint16_t, bufferSize>(1);
auto bufferB = computeSinTable<uint16_t, bufferSize>(2);
volatile bool bufferA_ready{true};
volatile bool bufferB_ready{true};

void
transferCompleteIrqHandler()
{
	LedGreen::reset();

	if (bufferA_ready) {
		cs43::I2sMaster::setTxBuffer(uintptr_t(bufferA.data()), bufferSize);
		bufferA_ready = false;
	}
	else if (bufferB_ready) {
		cs43::I2sMaster::setTxBuffer(uintptr_t(bufferB.data()), bufferSize);
		bufferB_ready = false;
	}
	else {
		LedRed::toggle();
		//MODM_LOG_ERROR << "No buffer ready for DMA :(" << modm::endl;
	}
	cs43::I2sMaster::startDma();

	LedGreen::set();
}

class VolumeThread : public modm::pt::Protothread
{
public:
	VolumeThread(modm::Cs43l22<cs43::I2cMaster>& audioDac) : audioDac{audioDac}
	{
		RF_CALL_BLOCKING(audioDac.setMasterVolume(volume));
	}

	bool
	update()
	{
		PT_BEGIN();
		while (true) {
			if (Board::Button::read()) {
				volume += 50;
				if (volume > modm::cs43l22::MaxVolume) {
					volume = modm::cs43l22::MinVolume;
				}
				MODM_LOG_INFO.printf("Volume: %2.1fdb\n", volume/10.f);
				PT_CALL(audioDac.setMasterVolume(volume));
				timeout.restart();
				PT_WAIT_UNTIL(timeout.isExpired());
			}
			PT_YIELD();
		}
		PT_END();
	}

private:
	modm::Cs43l22<cs43::I2cMaster>& audioDac;
	modm::cs43l22::centiBel_t volume = (modm::cs43l22::MinVolume + modm::cs43l22::MaxVolume) / 2;
	modm::ShortTimeout timeout{500ms};
};

int
main()
{
	Board::initialize();

	// initialize Uart2 for MODM_LOG_*
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	MODM_LOG_INFO << "Audio demo using CS43L22 I2S DAC on STM32F4-DSICOVERY" << modm::endl;
	MODM_LOG_INFO << "Press the 'user' button to increate volume." << modm::endl;

	Dma1::enable();

	Board::initializeCs43</*samplerate=*/48_kHz, /*tolerance=*/0.02_pct>();

	modm::Cs43l22<cs43::I2cMaster> audioDac{cs43::I2CAddress};
	if (!RF_CALL_BLOCKING(audioDac.initialize())) {
		MODM_LOG_ERROR << "Unable to initialize CS43L22 audio DAC" << modm::endl;
	}

	VolumeThread volumeThread{audioDac};

	cs43::I2sMaster::setTransferCompleteIrqHandler(transferCompleteIrqHandler);
	cs43::I2sMaster::setTxBuffer(uintptr_t(bufferA.data()), bufferSize);
	cs43::I2sMaster::start();

	modm::PeriodicTimer tmr{500ms};
	uint8_t counter{3};

	while (true)
	{
		if (!bufferA_ready) {
			bufferA = computeSinTable<uint16_t, bufferSize>(counter++);
			bufferA_ready = true;
		}
		if (!bufferB_ready) {
			bufferB = computeSinTable<uint16_t, bufferSize>(counter++);
			bufferB_ready = true;
		}
		if (counter > 100) {
			counter = 3;
		}

		if (tmr.execute()) {
			LedBlue::toggle();
		}

		if (cs43::I2sMaster::hasDmaError()) {
			MODM_LOG_ERROR << "I2S DMA Error :(" << modm::endl;
		}

		volumeThread.update();
	}

	return 0;
}
