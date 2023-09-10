#pragma once

#include <algorithm>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/endianness.hpp>

template<typename I2cMaster>
class Qmc5883l;

struct Qmc5883lRegisters
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class Register : uint8_t
	{
		DataX_Lsb = 0x00,
		DataX_Msb = 0x01,
		DataY_Lsb = 0x02,
		DataY_Msb = 0x03,
		DataZ_Lsb = 0x04,
		DataZ_Msb = 0x05,
		Status = 0x06,
		Tout_Lsb = 0x07,
		Tout_Msb = 0x08,
		Control1 = 0x09,
		Control2 = 0x0a,
		SetReset = 0x0b,
	};
	/// @endcond
public:
	enum class Status : uint8_t
	{
		DOR = modm::Bit2,
		OVL = modm::Bit1,
		DRDY = modm::Bit0,
	};
	MODM_FLAGS8(Status);

public:
	enum class Control1 : uint8_t
	{
		OSR1 = modm::Bit7,
		OSR0 = modm::Bit6,
		OversampleRate_Mask = OSR1 | OSR0,

		RNG1 = modm::Bit5,
		RNG0 = modm::Bit4,
		FullScale_Mask = RNG1 | RNG0,

		ODR1 = modm::Bit3,
		ODR0 = modm::Bit2,
		OutputDataRate_Mask = ODR1 | ODR0,

		MODE1 = modm::Bit1,
		MODE0 = modm::Bit0,
		Mode_Mask = MODE1 | MODE0,
	};
	MODM_FLAGS8(Control1);

	enum class Control2 : uint8_t
	{
		SOFT_RST = modm::Bit7,
		ROL_PNT = modm::Bit6,
		INT_ENB = modm::Bit0,
	};
	MODM_FLAGS8(Control2);

public:
	enum class OversampleRate : uint8_t
	{
		_512 = 0,
		_256 = int(Control1::OSR0),
		_128 = int(Control1::OSR1),
		_64 = int(Control1::OSR0) | int(Control1::OSR1),
	};

	MODM_FLAGS_CONFIG(Control1, OversampleRate);

	enum class FullScale : uint8_t
	{
		_2G = 0,
		_8G = int(Control1::RNG0),
	};

	MODM_FLAGS_CONFIG(Control1, FullScale);

	enum class OutputDataRate : uint8_t
	{
		_10Hz = 0,
		_50Hz = int(Control1::ODR0),
		_100Hz = int(Control1::ODR1),
		_200Hz = int(Control1::ODR0) | int(Control1::ODR1),
	};

	MODM_FLAGS_CONFIG(Control1, OutputDataRate);

	enum class Mode : uint8_t
	{
		StandBy = 0,
		Continious = int(Control1::MODE0),
	};

	MODM_FLAGS_CONFIG(Control1, Mode);

public:
	struct modm_packed Data
	{
		template<class I2cMaster>
		friend class Qmc5883l;

	protected:
		uint8_t data[7];

		template<Register R, uint8_t Offset = uint8_t(R)>
		int16_t inline getWord()
		{
			static_assert(Offset < sizeof data);
			const auto value = reinterpret_cast<int16_t *>(data + Offset);
			return modm::fromLittleEndian(*value);
		}
	};
};

template<class I2cMaster>
class Qmc5883l : public Qmc5883lRegisters, public modm::I2cDevice<I2cMaster>
{
	/// @cond
	Data &data;
	/// @endcond
	uint8_t buffer[sizeof data.data];

	modm::ResumableResult<bool>
	writeRegister(Register reg, uint8_t value)
	{
		RF_BEGIN();

		buffer[0] = uint8_t(reg);
		buffer[1] = value;
		this->transaction.configureWrite(buffer, 2);

		RF_END_RETURN_CALL(this->runTransaction());
	}

public:
	Qmc5883l(Data &data, uint8_t address = 0x0d) : modm::I2cDevice<I2cMaster>(address), data(data)
	{}

	auto x() { return data.getWord<Register::DataX_Lsb>(); }

	auto y() { return data.getWord<Register::DataY_Lsb>(); }

	auto z() { return data.getWord<Register::DataZ_Lsb>(); }

	auto status() { return Status_t(data.data[uint8_t(Register::Status)]); }

public:
	modm::ResumableResult<bool>
	initialize()
	{
		// Per datasheet:
		// wihtout any additional explanations recommended to set to 0x01.
		return writeRegister(Register::SetReset, 0x01);
	}

	modm::ResumableResult<bool>
	configure(Mode_t mode, Control1_t control)
	{
		control |= mode;
		return writeRegister(Register::Control1, control.value);
	}

	modm::ResumableResult<bool>
	configure(Control2_t control)
	{
		return writeRegister(Register::Control2, control.value);
	}

	modm::ResumableResult<bool>
	readData()
	{
		RF_BEGIN();

		buffer[0] = uint8_t(Register::DataX_Lsb);
		this->transaction.configureWriteRead(buffer, 1, buffer, sizeof buffer);

		if (RF_CALL(this->runTransaction()))
		{
			std::copy_n(buffer, sizeof data.data, data.data);
			RF_RETURN(true);
		}

		RF_END_RETURN(false);
	}
};
