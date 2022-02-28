/*
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/dac/ma12070p.hpp>
#include <modm/platform/i2c/i2c_master_1.hpp>
#include <optional>

using namespace Board;
using namespace modm::literals;

using I2cMaster = modm::platform::I2cMaster1;
using Ma12070p = modm::Ma12070p<I2cMaster>;

constexpr uint8_t ma12070pAddressI2c = 0x20;
Ma12070p ma12070p{ma12070pAddressI2c};

int
main()
{
	Board::initialize();
	Board::initializeTouchscreen(); // for I2c

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
	}

	return 0;
}
