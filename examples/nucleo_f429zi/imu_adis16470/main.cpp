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
#include <modm/platform/spi/spi_master_1.hpp>
#include <modm/driver/inertial/adis16470.hpp>
#include <optional>

using namespace Board;

using SpiMaster = SpiMaster1;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
using Cs = GpioA4;
using Dr = GpioB9;
using Rst = GpioB8;

using Adis16470 = modm::Adis16470<SpiMaster, Cs>;

Adis16470 imu{};

int
main()
{
	Board::initialize();
	Leds::setOutput();

	MODM_LOG_INFO << "ADIS16470 IMU demo on Nucleo-F429ZI\n" << modm::endl;

	SpiMaster::initialize<Board::SystemClock, 650_kHz>();
	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();

	Rst::setOutput();
	Dr::setInput(Gpio::InputType::PullUp);

	// Reset ADS816x chip
	Rst::reset();
	modm::delay(10ms);
	Rst::set();

	// Wait start-up time
	modm::delay(252ms);

	MODM_LOG_INFO << "Initializing IMU..." << modm::endl;
	RF_CALL_BLOCKING(imu.initialize());

	// Software reset
	RF_CALL_BLOCKING(imu.writeGlobCmd(modm::adis16470::GlobCmd::SoftwareReset));
	// Wait start-up time after reset
	modm::delay(252ms);

	// Trigger self test
	RF_CALL_BLOCKING(imu.writeGlobCmd(modm::adis16470::GlobCmd::SensorSelfTest));
	// Wait for self test to complete
	modm::delay(14ms);

	modm::adis16470::DiagStat_t diagStat = RF_CALL_BLOCKING(imu.readDiagStat());
	MODM_LOG_INFO << "Reading DIAG_STAT register = " << diagStat << modm::endl;

	modm::delay(1ms);
	auto result = RF_CALL_BLOCKING(imu.readRegister(modm::adis16470::Register::PROD_ID));
	if (result.has_value()) {
		MODM_LOG_INFO << "ADIS16470 product id = " << *result << " (should be 16470==0x4056)" << modm::endl;
	}
	else {
		MODM_LOG_INFO << "Unable to reag PROD_ID register" << modm::endl;
	}

	modm::adis16470::MscCtrl_t mscCtrl = RF_CALL_BLOCKING(imu.readMscCtrl());
	MODM_LOG_INFO << "Reading MSC_CTRL register = " << mscCtrl << modm::endl;

	RF_CALL_BLOCKING(imu.setDataOutputFrequency<10_Hz>());

	RF_CALL_BLOCKING(imu.writeRegister(modm::adis16470::Register::FILT_CTRL, 8)); // N_B = 2^8 = 64

	mscCtrl = modm::adis16470::MscCtrl::DrPolarity | modm::adis16470::MscCtrl::PointOfPercussionAlign;
	MODM_LOG_INFO << "Writing MSC_CTRL register = " << mscCtrl << modm::endl;
	RF_CALL_BLOCKING(imu.writeMscCtrl(mscCtrl));

	diagStat = RF_CALL_BLOCKING(imu.readDiagStat());
	MODM_LOG_INFO << "Reading DIAG_STAT register = " << diagStat << modm::endl;

	mscCtrl = RF_CALL_BLOCKING(imu.readMscCtrl());
	MODM_LOG_INFO << "Reading MSC_CTRL register = " << mscCtrl << modm::endl;

	// Writing and reading USER_SCR1 register
	std::optional<uint16_t> scr1 = RF_CALL_BLOCKING(imu.readRegister(modm::adis16470::Register::USER_SCR1));
	if (scr1.has_value()) {
		MODM_LOG_INFO << "Reading USER_SCR1 register = " << *scr1 << modm::endl;
	}
	else {
		MODM_LOG_INFO << "Unable to USER_SCR1 register." << modm::endl;
	}
	MODM_LOG_INFO << "Writing USER_SCR1 register = 42" << modm::endl;
	RF_CALL_BLOCKING(imu.writeRegister(modm::adis16470::Register::USER_SCR1, 42));
	scr1 = RF_CALL_BLOCKING(imu.readRegister(modm::adis16470::Register::USER_SCR1));
	if (scr1.has_value()) {
		MODM_LOG_INFO << "Reading USER_SCR1 register = " << *scr1 << modm::endl;
	}
	else {
		MODM_LOG_INFO << "Unable to USER_SCR1 register." << modm::endl;
	}

	std::array<uint16_t, 11> data;

	while (true)
	{
		MODM_LOG_INFO.printf("\nIMU data: ");

		while (!Dr::read()) {}
		while (Dr::read()) {}

		if( !RF_CALL_BLOCKING(imu.readRegisterBurst(data))) {
			MODM_LOG_INFO.printf("checksum mismatch! ");
			Board::LedRed::toggle();
		}

		// print data anyways
		MODM_LOG_INFO.printf("DIAG_STAT=%05u, ", data[1]);
		MODM_LOG_INFO.printf("X_GYRO_OUT=%+05i, ", static_cast<int16_t>(data[2]));
		MODM_LOG_INFO.printf("Y_GYRO_OUT=%+05i, ", static_cast<int16_t>(data[3]));
		MODM_LOG_INFO.printf("Z_GYRO_OUT=%+05i, ", static_cast<int16_t>(data[4]));
		MODM_LOG_INFO.printf("X_ACCL_OUT=%+05i, ", static_cast<int16_t>(data[5]));
		MODM_LOG_INFO.printf("Y_ACCL_OUT=%+05i, ", static_cast<int16_t>(data[6]));
		MODM_LOG_INFO.printf("Z_ACCL_OUT=%+05i, ", static_cast<int16_t>(data[7]));
		MODM_LOG_INFO.printf("TEMP_OUT=%+05i (1/10 C), ", data[8]);
		MODM_LOG_INFO.printf("DATA_CNTR=%05u, ", data[9]);
		MODM_LOG_INFO.printf("checksum=%05u", data[10]);

		Board::LedGreen::toggle();
	}

	return 0;
}
