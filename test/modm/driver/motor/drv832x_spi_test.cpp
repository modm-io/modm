/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/driver/motor/drv832x_spi.hpp>
#include <modm/debug/logger/logger.hpp>

#include <modm/platform/spi/mock/spi_master.hpp>
#include <modm/platform/gpio/unused.hpp>

#include "drv832x_spi_test.hpp"

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

void
Drv832xSpiTest::testRegister()
{
	// Some random tests
	// Todo (?): Add systematic tests
	using SpiMaster = modm::platform::SpiMasterMock;
	modm::Drv832xSpi<SpiMaster, modm::platform::GpioUnused> gateDriver;

	gateDriver.driverControl().value = 0b1111111111; // All bits set
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::ClearFaultBits));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::Brake));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::Coast));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::Pwm1Dir));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::Pwm1Com));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::PwmMode1));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::PwmMode2));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::OvertemperatureReport));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::DisableGateDriveFault));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::DisableChargePumpUVLO));

	// Reset some bits
	gateDriver.driverControl() &= ~modm::drv832xSpi::DriverControl::Coast;
	gateDriver.driverControl() &= ~modm::drv832xSpi::PwmMode_t::mask();
	gateDriver.driverControl() &= ~modm::drv832xSpi::DriverControl::DisableChargePumpUVLO;
	TEST_ASSERT_FALSE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::Coast));
	TEST_ASSERT_FALSE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::PwmMode1));
	TEST_ASSERT_FALSE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::PwmMode2));
	TEST_ASSERT_FALSE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::DisableChargePumpUVLO));
	TEST_ASSERT_FALSE(gateDriver.driverControl().any(modm::drv832xSpi::DriverControl::Coast | modm::drv832xSpi::DriverControl::PwmMode1 | modm::drv832xSpi::DriverControl::PwmMode2 | modm::drv832xSpi::DriverControl::DisableChargePumpUVLO));
	uint16_t value = gateDriver.driverControl().value;
	TEST_ASSERT_EQUALS(value, 0b0110011011);

	gateDriver.driverControl() |= modm::drv832xSpi::PwmMode_t(modm::drv832xSpi::PwmMode::PwmModeIndependent);
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::PwmMode1));
	TEST_ASSERT_TRUE(bool(gateDriver.driverControl() & modm::drv832xSpi::DriverControl::PwmMode2));

	modm::drv832xSpi::CsaControl_t csaControl(0b00011000011);
	modm::drv832xSpi::CsaGain gain = modm::drv832xSpi::CsaGain_t::get(csaControl);
	TEST_ASSERT_TRUE(gain == modm::drv832xSpi::CsaGain::Gain40);
	modm::drv832xSpi::SenseOcpLevel level = modm::drv832xSpi::SenseOcpLevel_t::get(csaControl);
	TEST_ASSERT_TRUE(level == modm::drv832xSpi::SenseOcpLevel::SenseOcp1V);
}

void
Drv832xSpiTest::testSpi()
{
	// Test Spi with MockSpiMaster
	using SpiMaster = modm::platform::SpiMasterMock;
	modm::Drv832xSpi<SpiMaster, modm::platform::GpioUnused> gateDriver;

	// Real DRV832x device has default register values, but we expect all flags zero because SpiMasterMock reads zero per default
	RF_CALL_BLOCKING(gateDriver.initialize());
	// initialize reads 14 bytes, but we don't care
	SpiMaster::clearBuffers();

	// Test csa control register write
	gateDriver.driverControl() |= modm::drv832xSpi::PwmMode_t(modm::drv832xSpi::PwmMode::PwmModeIndependent);
	// Spi transfer should not occure before commit()
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 0u);
	RF_CALL_BLOCKING(gateDriver.commit());
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 2u);
	uint8_t txBuffer[2];
	SpiMaster::popTxBuffer(txBuffer);
	uint8_t txBufferCompare[] = {(0x2 << 3), (0b11 << 5)};
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare, 2u);

	// Test csa control register write
	gateDriver.csaControl() |= modm::drv832xSpi::CsaControl::DisableOvercurrentSense | modm::drv832xSpi::CsaControl::VrefDiv2;
	RF_CALL_BLOCKING(gateDriver.commit());
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 2u);
	SpiMaster::popTxBuffer(txBuffer);
	uint8_t txBufferCompare2[] = {(0x6 << 3 | 0b1 << 1), (0b1 << 5)};
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare2, 2u);

	// Test fault status register read
	RF_CALL_BLOCKING(gateDriver.readFaultStatus1());
	TEST_ASSERT_EQUALS(SpiMaster::getTxBufferLength(), 2u);
	SpiMaster::popTxBuffer(txBuffer);
	uint8_t txBufferCompare3[] = {(0b1 << 7 | 0x0 << 3), 0x00};
	TEST_ASSERT_EQUALS_ARRAY(txBuffer, txBufferCompare3, 2u);
}

void
Drv832xSpiTest::tearDown() {
	modm::platform::SpiMasterMock::clearBuffers();
}
