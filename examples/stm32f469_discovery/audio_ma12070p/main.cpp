/*
 * Copyright (c) 2022, Raphael Lehmann
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <arm_math.h>
#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/driver/dac/ma12070p.hpp>
#include <modm/platform/gpio/inverted.hpp>
#include <modm/platform/i2c/bitbang_i2c_master.hpp>
#include <modm/platform/i2s/i2s_master_2.hpp>
#include <numbers>
#include <optional>

using namespace Board;
using namespace modm::literals;

// // I²C pins shared with touchscreen:
// using Scl = GpioB8;
// using Sda = GpioB9;
// using I2cMaster = modm::platform::I2cMaster1;

using Scl = Board::D3;
using Sda = Board::D4;
using I2cMaster = modm::platform::BitBangI2cMaster<Scl, Sda>;
using Ma12070p = modm::Ma12070p<I2cMaster>;

using I2sWs = GpioOutputB12;
using I2sMck = GpioOutputC6;
using I2sCk = GpioOutputB13;
using I2sData = GpioOutputC1;
using DmaTx = Dma1::Channel4;
using I2sMaster = modm::platform::I2sMaster2<DmaTx>;

using Enable	= GpioInverted<Board::D0>;
using Mute		= GpioInverted<Board::D1>;

struct I2sSystemClock
{
	static constexpr uint32_t I2sPll = 86_MHz;

	static void
	enable()
	{
		const Rcc::PllI2sFactors pllI2sFactors{
			.pllN = 258,	// 1 MHz * N=258 -> 258 MHz
			.pllR = 3		// 258 MHz / R=3 -> 86 MHz
		};
		Rcc::enablePllI2s(pllI2sFactors);
	}
};

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
		I2sMaster::setTxBuffer(uintptr_t(bufferA.data()), bufferSize);
		bufferA_ready = false;
	}
	else if (bufferB_ready) {
		I2sMaster::setTxBuffer(uintptr_t(bufferB.data()), bufferSize);
		bufferB_ready = false;
	}
	else {
		LedRed::toggle();
		//MODM_LOG_ERROR << "No buffer ready for DMA :(" << modm::endl;
	}
	I2sMaster::startDma();

	LedGreen::set();
}

class Ma12070pThread : public modm::pt::Protothread
{
public:
	Ma12070pThread() : ma12070p{ma12070pAddressI2c}
	{}

	bool
	update()
	{
		PT_BEGIN();

		Enable::set();
		timeout.restart();
		PT_WAIT_UNTIL(timeout.isExpired());

		/// initialize() is actually empty
		// while (!PT_CALL(ma12070p.initialize())) {
		// 	MODM_LOG_ERROR << "Unable to initialize MA12070P" << modm::endl;
		// }

		MODM_LOG_INFO << "Configuring I2S and VLP settings of MA12070P..." << modm::endl;
		while (!PT_CALL(ma12070p.configureI2sAndVlp(config))) {
			MODM_LOG_ERROR << "Unable to configure I2S and VLP settings of MA12070P" << modm::endl;
			timeout.restart();
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		MODM_LOG_INFO.printf("Setting MA12070P volume to %2.1fdb...\n", modm::ma12070p::quarterDecibelToFloat(volume));
		while (!PT_CALL(ma12070p.setMasterVolume(volume))) {
			MODM_LOG_ERROR << "Unable to set master volume of MA12070P" << modm::endl;
			timeout.restart();
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		MODM_LOG_INFO << "Unmute..." << modm::endl;
		Mute::reset();

		while (true) {
			if (Board::Button::read()) {
				volume += 3_q_db;
				if (volume > modm::ma12070p::MaxVolume) {
					volume = modm::ma12070p::MinVolume;
				}
				MODM_LOG_INFO.printf("Volume: %2.1fdb\n", modm::ma12070p::quarterDecibelToFloat(volume));
				if (!PT_CALL(ma12070p.setMasterVolume(volume))) {
					MODM_LOG_ERROR << "Unable to set master volume of MA12070P" << modm::endl;
				}
				timeout.restart();
				PT_WAIT_UNTIL(timeout.isExpired());
			}

			if (timer.execute()) {
				vlpMonitor = PT_CALL(ma12070p.readVlpMonitor());
				if(!vlpMonitor) {
					MODM_LOG_ERROR << "Unable to read VLP monitor register" << modm::endl;
				}
				else {
					MODM_LOG_ERROR << *vlpMonitor << modm::endl;
				}
				errorRegister = PT_CALL(ma12070p.readAccumulatedErrors());
				if(!errorRegister) {
					MODM_LOG_ERROR << "Unable to read accumulated error register" << modm::endl;
				}
				else {
					MODM_LOG_ERROR << *errorRegister << modm::endl;
				}
			}

			PT_YIELD();
		}
		PT_END();
	}

private:
	static constexpr uint8_t ma12070pAddressI2c = 0x20;
	static constexpr modm::ma12070p::I2sAndVlpConfig config = {
		.pcmWordFormat		= modm::ma12070p::PcmWordFormat::RightJustifed16b,
		.clockPolarity		= modm::ma12070p::ClockPolarity::FallingEdge,
		.frameSize			= modm::ma12070p::FrameSize::Bits32,
		.wordSelectPolarity	= modm::ma12070p::WordSelectPolarity::High,
		.rightLeftOrder		= modm::ma12070p::RightLeftOrder::LeftFirst,
		.useVlp				= false,
		.useLimiter			= false,
	};

	Ma12070p ma12070p;
	std::optional<modm::ma12070p::VlpMonitor_t> vlpMonitor;
	std::optional<modm::ma12070p::ErrorRegister_t> errorRegister;
	modm::ma12070p::quarter_decibel_t volume = modm::ma12070p::MinVolume;

	modm::ShortTimeout timeout{500ms};
	modm::ShortPeriodicTimer timer{1s};
};

int
main()
{
	Board::initialize();
	I2sSystemClock::enable();
	I2cMaster::connect<Scl::BitBang, Sda::BitBang>(I2cMaster::PullUps::Internal);
	I2cMaster::initialize<SystemClock, 10_kHz>();
	Dma1::enable();
	Dma2::enable();
	Enable::setOutput();
	Enable::reset();
	Mute::setOutput();
	Mute::set();


	MODM_LOG_INFO << "Audio MA12070P demo on ST Discovery F469NI" << modm::endl;


	I2sMaster::connect<I2sMck::Mck, I2sCk::Ck, I2sWs::Ws, I2sData::Sd>();
	I2sMaster::initialize<I2sSystemClock, 48_kHz, 1_pct>(
		I2sMaster::BitDepth::SixteenWithChannel16,
		I2sMaster::MasterClockOutput::Enabled,
		I2sMaster::I2sStandard::Philips);
	I2sMaster::setTransferCompleteIrqHandler(transferCompleteIrqHandler);
	I2sMaster::setTxBuffer(uintptr_t(bufferA.data()), bufferSize);
	I2sMaster::start();

	Ma12070pThread ma12070pThread{};

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

		ma12070pThread.update();

		if (I2sMaster::hasDmaError()) {
			MODM_LOG_ERROR << "I2S DMA Error :(" << modm::endl;
		}

		if (tmr.execute()) {
			LedBlue::toggle();
		}
	}

	return 0;
}
