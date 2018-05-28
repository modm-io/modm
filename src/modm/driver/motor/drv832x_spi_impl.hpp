// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DRV832X_SPI_HPP
	#error	"Don't include this file directly, use 'drv832x_spi.hpp' instead!"
#endif
#include "drv832x_spi.hpp"

// ----------------------------------------------------------------------------
template < class SpiMaster, class Cs >
modm::Drv832xSpi<SpiMaster, Cs>::Drv832xSpi()
{
	this->attachConfigurationHandler([]() {
		SpiMaster::setDataMode(SpiMaster::DataMode::Mode1);
		SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
	});
	Cs::setOutput(modm::Gpio::High);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::initialize()
{
	return readAll();
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readAll()
{
	RF_BEGIN();
	RF_CALL(readFaultStatus1());
	RF_CALL(readVgsStatus2());
	RF_CALL(readDriverControl());
	RF_CALL(readGateDriveHS());
	RF_CALL(readGateDriveLS());
	RF_CALL(readOcpControl());
	RF_CALL(readCsaControl());
	RF_END();
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readFaultStatus1()
{
	return readData(Register::FaultStatus1, _faultStatus1.value);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readVgsStatus2()
{
	return readData(Register::VgsStatus2, _vgsStatus2.value);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readDriverControl()
{
	return readData(Register::DriverControl, _driverControl.value);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readGateDriveHS()
{
	return readData(Register::GateDriveHS, _gateDriveHS.value);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readGateDriveLS()
{
	return readData(Register::GateDriveLS, _gateDriveLS.value);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readOcpControl()
{
	return readData(Register::OcpControl, _ocpControl.value);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readCsaControl()
{
	return readData(Register::CsaControl, _csaControl.value);
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::commit()
{
	RF_BEGIN();
	if(accessBitmap & 0b0000100) {
		RF_CALL(writeData(Register::DriverControl, _driverControl.value));
	}
	if(accessBitmap & 0b0001000) {
		RF_CALL(writeData(Register::GateDriveHS, _gateDriveHS.value));
	}
	if(accessBitmap & 0b0010000) {
		RF_CALL(writeData(Register::GateDriveLS, _gateDriveLS.value));
	}
	if(accessBitmap & 0b0100000) {
		RF_CALL(writeData(Register::OcpControl, _ocpControl.value));
	}
	if(accessBitmap & 0b1000000) {
		RF_CALL(writeData(Register::CsaControl, _csaControl.value));
	}
	accessBitmap = 0;
	RF_END();
}


template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::writeData(Register address, uint16_t data)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	inBuffer[0] = 0;
	inBuffer[1] = 0;

	static constexpr uint8_t writeBit = (0 << 7); // 0 = write

	outBuffer[0] = writeBit | (static_cast<uint8_t>(address) << 3) | ((data >> 8) & 0b111);
	outBuffer[1] = data & 0xff;

	RF_CALL(SpiMaster::transfer(outBuffer, inBuffer, 2));

	if (this->releaseMaster()) {
		Cs::set();
	}

	RF_END();
}

template < class SpiMaster, class Cs >
modm::ResumableResult<void>
modm::Drv832xSpi<SpiMaster, Cs>::readData(Register address, uint16_t& data)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	inBuffer[0] = 0x00;
	inBuffer[1] = 0x00;

	static constexpr uint8_t writeBit = (1 << 7); // 1 = read

	outBuffer[0] = writeBit | (static_cast<uint8_t>(address) << 3);
	outBuffer[1] = 0;

	RF_CALL(SpiMaster::transfer(outBuffer, inBuffer, 2));

	if (this->releaseMaster()) {
		Cs::set();
	}

	data = static_cast<uint16_t>(inBuffer[1]) | (static_cast<uint16_t>(inBuffer[0] & 0b111) << 8);
	RF_END();
}
