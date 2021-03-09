/*
 * Copyright (c) 2021, Henrik Hose
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_STUSB4500_HPP
#define MODM_STUSB4500_HPP

#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{

/// @ingroup modm_driver_stusb4500
struct stusb4500
{
public:
	enum class
	Register : uint8_t
	{
		RdoRegStatus = 0x91,
		// PortStatus1 = 0x0E,
		// CcStatus = 0x11,
		// TxHeaderLow = 0x51,
		// PdCommandCtrl = 0x1A,
		DpmPdoNumb = 0x70,
		DpmSnkPdo1 = 0x85,
		DpmSnkPdo2 = 0x89,
		DpmSnkPdo3 = 0x8D,
	};

	struct RdoRegStatusData {
		uint32_t MaxCurrent; /// [mA]
		uint32_t OperatingCurrent; /// [mA]
		uint8_t ObjectPos;
	};
};

/**
 * @ingroup modm_driver_stusb4500
 * @author Henrik Hose, Raphael Lehmann
 */
template<class I2cMaster>
class Stusb4500 : public stusb4500, public modm::I2cDevice< I2cMaster, 4 >
{
public:
	Stusb4500(uint8_t address=0x28):
		modm::I2cDevice<I2cMaster,4>(address)
	{};

	/**
	 * Configures a PDO.
	 *
	 * Note: According to the USB standard the first PDO (1) always has to set
	 *  voltage to 5000 [mV].
	 *
	 * @param	pdoNumber Which PDO to configure, range 1 to 3.
	 * @param	voltage	  in mV
	 * @param	current	  in mA
	 */
	modm::ResumableResult<bool>
	configurePdo(uint8_t pdoNumber, uint32_t voltage, uint32_t current)
	{
		RF_BEGIN();

		if((pdoNumber < 1) || (pdoNumber > 3))
			RF_RETURN(false);

		RF_CALL(readRegister(DpmSnkPdos[pdoNumber - 1], buffer2.data(), 4));

		current /= 10;
		voltage /= 50;

		buffer2[0] = current;
		buffer2[1] = ((current >> 8) & 0b11) | ((voltage & 0b11'1111 ) << 2);
		buffer2[2] = (buffer2[2] & 0b1111'0000) | ((voltage >> 6) & 0b1111);

		RF_END_RETURN_CALL(updateRegister(DpmSnkPdos[pdoNumber - 1], buffer2.data(), 4));
	}

	/**
	 * Sets the valid PDO.
	 *
	 * @param	pdoNumber Which PDO to set valid, range 1 to 3.
	 */
	modm::ResumableResult<bool>
	setValidPdo(uint8_t pdoNumber)
	{
		RF_BEGIN();

		if((pdoNumber < 1) || (pdoNumber > 3))
			RF_RETURN(false);

		RF_END_RETURN_CALL(updateRegister(Register::DpmPdoNumb, &pdoNumber, 1));
	}

	/**
	 * Retrieve RDO status from STUSB4500 chip.
	 *
	 * @return	RdoRegStatusData object. Typically only
	 *  RdoRegStatusData::MaxCurrent [mA] is of interest.
	 */
	modm::ResumableResult<RdoRegStatusData>
	getRdoRegStatus()
	{
		RF_BEGIN();

		RF_CALL(readRegister(Register::RdoRegStatus, buffer2.data(), 4));

		// MaxCurrent: Bits 9..0; OperatingCurrent: Bits 19..10; ObjectPos: Bits 30..28
		rdoRegStatusData.MaxCurrent = buffer2[0] | ((buffer2[1] & 0b11) << 8);
		rdoRegStatusData.MaxCurrent *= 10;
		rdoRegStatusData.OperatingCurrent = ((buffer2[1] & 0b1111'1100) >> 2) | ((buffer2[2] & 0b1111) << 6);
		rdoRegStatusData.OperatingCurrent *= 10;
		rdoRegStatusData.ObjectPos = (buffer2[3] & 0b0110'0000) >> 5;

		RF_END_RETURN(rdoRegStatusData);
	}

public:
	modm::ResumableResult<bool>
	readRegister(Register reg, uint8_t* output, size_t length)
	{
		RF_BEGIN();
		buffer[0] = uint8_t(reg);
		this->transaction.configureWriteRead(buffer.data(), 1, output, length);
		RF_END_RETURN_CALL( this->runTransaction() );
	}

	modm::ResumableResult<bool>
	updateRegister(Register reg, const uint8_t* data, size_t length)
	{
		RF_BEGIN();
		if(length > (std::tuple_size<decltype(buffer)>::value - 1))
			RF_RETURN(false);
		buffer[0] = uint8_t(reg);
		std::memcpy(&buffer[1], data, length);
		this->transaction.configureWriteRead(buffer.data(), length + 1, nullptr, 0);
		RF_END_RETURN_CALL( this->runTransaction() );
	}

private:
	static constexpr std::array DpmSnkPdos {Register::DpmSnkPdo1, Register::DpmSnkPdo2, Register::DpmSnkPdo3};
	std::array<uint8_t, 5> buffer;
	std::array<uint8_t, 4> buffer2;
	RdoRegStatusData rdoRegStatusData;
};

} // namespace modm

#endif // MODM_STUSB4500_HPP
