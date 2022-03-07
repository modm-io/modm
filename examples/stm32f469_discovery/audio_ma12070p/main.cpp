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
#include <modm/driver/dac/ma12070p.hpp>
#include <modm/platform/i2c/i2c_master_1.hpp>
#include <modm/platform/i2s/i2s_master_2.hpp>
#include <numbers>
#include <optional>

using namespace Board;
using namespace modm::literals;

// I²C pins shared with touchscreen:
using Scl = GpioB8;
using Sda = GpioB9;
using I2cMaster = modm::platform::I2cMaster1;
using Ma12070p = modm::Ma12070p<I2cMaster>;

using I2sWs = GpioOutputB12;
using I2sMck = GpioOutputC6;
using I2sCk = GpioOutputB13;
using I2sData = GpioOutputC1;
using DmaTx = Dma1::Channel4;
using I2sMaster = modm::platform::I2sMaster2<DmaTx>;


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

constexpr uint8_t ma12070pAddressI2c = 0x20;
Ma12070p ma12070p{ma12070pAddressI2c};

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

int
main()
{
	Board::initialize();
	I2sSystemClock::enable();
	Board::initializeTouchscreen(); // for I2c
	Dma1::enable();
	Dma2::enable();

	MODM_LOG_INFO << "Audio MA12070P demo on ST Discovery F469NI" << modm::endl;

	while (!RF_CALL_BLOCKING(ma12070p.initialize())) {
		MODM_LOG_ERROR << "Unable to initialize MA12070P" << modm::endl;
	}


	modm::ma12070p::I2sAndVlpConfig config {
		.pcmWordFormat		= modm::ma12070p::PcmWordFormat::RightJustifed16b,
		.clockPolarity		= modm::ma12070p::ClockPolarity::FallingEdge,
		.frameSize			= modm::ma12070p::FrameSize::Bits32,
		.wordSelectPolarity	= modm::ma12070p::WordSelectPolarity::High,
		.rightLeftOrder		= modm::ma12070p::RightLeftOrder::LeftFirst,
		.useVlp				= true,
		.useLimiter			= true,
	};
	while (!RF_CALL_BLOCKING(ma12070p.configureI2sAndVlp(config))) {
		MODM_LOG_ERROR << "Unable to configure I2S ansd VLP settings of MA12070P" << modm::endl;
	}

	while (!RF_CALL_BLOCKING(ma12070p.setMasterVolume(-20_q_db))) {
		MODM_LOG_ERROR << "Unable to set master volume of MA12070P" << modm::endl;
	}

	std::optional<modm::ma12070p::VlpMonitor_t> vlpMonitor;
	std::optional<modm::ma12070p::ErrorRegister_t> errorRegister;

	I2sMaster::connect<I2sMck::Mck, I2sCk::Ck, I2sWs::Ws, I2sData::Sd>();
	I2sMaster::initialize<I2sSystemClock, 48_kHz, 1_pct>(
		I2sMaster::BitDepth::SixteenWithChannel16,
		I2sMaster::MasterClockOutput::Enabled,
		I2sMaster::I2sStandard::Philips);

	I2sMaster::setTransferCompleteIrqHandler(transferCompleteIrqHandler);
	I2sMaster::setTxBuffer(uintptr_t(bufferA.data()), bufferSize);
	I2sMaster::start();

	while (true)
	{
		vlpMonitor = RF_CALL_BLOCKING(ma12070p.readVlpMonitor());
		if(!vlpMonitor) {
			MODM_LOG_ERROR << "Unable to read VLP monitor register" << modm::endl;
		}
		else {
			MODM_LOG_ERROR << *vlpMonitor << modm::endl;
		}
		errorRegister = RF_CALL_BLOCKING(ma12070p.readAccumulatedErrors());
		if(!errorRegister) {
			MODM_LOG_ERROR << "Unable to read accumulated error register" << modm::endl;
		}
		else {
			MODM_LOG_ERROR << *errorRegister << modm::endl;
		}

		if (I2sMaster::hasDmaError()) {
			MODM_LOG_ERROR << "I2S DMA Error :(" << modm::endl;
		}
	}

	return 0;
}
