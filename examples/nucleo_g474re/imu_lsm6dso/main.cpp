/*
 * Copyright (c) 2023, Raphael Lehmann
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
#include <modm/driver/inertial/lsm6dso.hpp>
#include <optional>

using namespace Board;

using SpiMaster = SpiMaster1;
using Mosi = GpioA7;
using Miso = GpioA6;
using Sck = GpioA5;
using Cs = GpioC5;
using Int1 = GpioC4;
using ImuVdd = GpioA10;

using Lsm6dso = modm::Lsm6dso<modm::Lsm6TransportSpi<SpiMaster, Cs>>;

Lsm6dso imu{};

int
main()
{
	Board::initialize();
	Leds::setOutput();

	MODM_LOG_INFO << "LSM6DSO IMU demo on Nucleo-G474RE\n" << modm::endl;

	SpiMaster::initialize<Board::SystemClock, 5300_kHz>();
	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();
	Int1::setInput(Gpio::InputType::PullUp);

	// The imu behaves weird when the pins are temporarily unconnected (e.g.
	// while changing jumper wires) or after reprogramming the nucleo board,
	// and a software reset does not work.
	// Therefore the IMU can be powered through a GPIO pin (Vdd) which we
	// switch on and off to reboot it completely.
	ImuVdd::reset();
	ImuVdd::setOutput();
	modm::delay(10ms);
	// Power up imu
	ImuVdd::set();

	// Give the sensor some time to boot...
	modm::delay(10ms);

	MODM_LOG_INFO << "Resetting and rebooting imu (BOOT | SW_RESET)..." << modm::endl;
	// Write bits BOOT | SW_RESET (and IF_INC, because it default to 1) to reboot imu
	if(!RF_CALL_BLOCKING(imu.writeRegister(modm::lsm6dso::Register::CTRL3_C, 0b1000'0101))) {
		MODM_LOG_INFO.printf(" ... failed.");
		Board::Leds::set();
	}

	MODM_LOG_INFO << "Initializing IMU..." << modm::endl;
	RF_CALL_BLOCKING(imu.initialize());

	MODM_LOG_INFO << "Enable IMU by configuring data rate and range" << modm::endl;
	if(!RF_CALL_BLOCKING(imu.setOutputDataRateAndRange<12_Hz, 5_pct>(
				modm::lsm6dso::LinearRange::G2,
				modm::lsm6dso::AngularRange::dps125))) {
		MODM_LOG_INFO.printf(" ... failed.");
		Board::Leds::set();
	}

	// Reading CTRL1_XL register
	std::optional<uint8_t> reg = RF_CALL_BLOCKING(imu.readRegister(modm::lsm6dso::Register::CTRL1_XL));
	if (reg.has_value()) {
		MODM_LOG_INFO << "Reading CTRL1_XL register = " << *reg << modm::endl;
	}
	else {
		MODM_LOG_INFO << "Unable to CTRL1_XL register." << modm::endl;
	}

	MODM_LOG_INFO << "Enable accelerometer and gyroscope data-ready interrupts on INT1 pin" << modm::endl;
	if(!RF_CALL_BLOCKING(imu.writeRegister(modm::lsm6dso::Register::INT1_CTRL, 0b0000'0011))) {
		MODM_LOG_INFO.printf(" ... failed.");
		Board::Leds::set();
	}

	std::array<uint8_t, 14> data;

	while (true)
	{
		MODM_LOG_INFO.printf("\n\nIMU data: ");

		// Wait for rising edge
		while (Int1::read()) {}
		while (!Int1::read()) {}

		if(!RF_CALL_BLOCKING(imu.readRegisters(modm::lsm6dso::Register::OUT_TEMP_L, data))) {
			MODM_LOG_INFO.printf("failed. ");
			Board::Leds::set();
		}

		// print data anyways
		MODM_LOG_INFO.printf("TEMP=%+2.1f degC, ", (double)((static_cast<int16_t>(data[0] | data[1] << 8) / 256.f) + 25.f));
		MODM_LOG_INFO.printf("G_X=%+05i, ", static_cast<int16_t>(data[ 2] | data[ 3] << 8));
		MODM_LOG_INFO.printf("G_Y=%+05i, ", static_cast<int16_t>(data[ 4] | data[ 5] << 8));
		MODM_LOG_INFO.printf("G_Z=%+05i, ", static_cast<int16_t>(data[ 6] | data[ 7] << 8));
		MODM_LOG_INFO.printf("A_X=%+05i, ", static_cast<int16_t>(data[ 8] | data[ 9] << 8));
		MODM_LOG_INFO.printf("A_Y=%+05i, ", static_cast<int16_t>(data[10] | data[11] << 8));
		MODM_LOG_INFO.printf("A_Z=%+05i, ", static_cast<int16_t>(data[12] | data[13] << 8));

		Board::Leds::toggle();
	}

	return 0;
}
