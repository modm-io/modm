/*
 * Copyright (c) 2022-2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#define MODM_LSM6DSO_HPP

#include <array>
#include <optional>
#include <span>
#include <utility>
#include <modm/processing/resumable.hpp>
#include <modm/math/units.hpp>

// LSM6DSO uses same I2C/SPI transport as the older LIS3 sensors
#include "lis3_transport.hpp"

namespace modm
{

template<class I2cMaster>
using Lsm6TransportI2c = modm::Lis3TransportI2c<I2cMaster>;

template<class SpiMaster, class Cs>
using Lsm6TransportSpi = modm::Lis3TransportSpi<SpiMaster, Cs>;

/// @ingroup modm_driver_lsm6dso
struct lsm6dso
{
	enum class
	Register : uint8_t
	{
		FUNC_CFG_ACCESS = 0x01,
		PIN_CTRL = 0x02,
		FIFO_CTRL1 = 0x07,
		FIFO_CTRL2 = 0x08,
		FIFO_CTRL3 = 0x09,
		FIFO_CTRL4 = 0x0A,
		COUNTER_BDR_REG1 = 0x0B,
		COUNTER_BDR_REG2 = 0x0C,
		INT1_CTRL = 0x0D,
		INT2_CTRL = 0x0E,
		WHO_AM_I = 0x0F,
		CTRL1_XL = 0x10,
		CTRL2_G = 0x11,
		CTRL3_C = 0x12,
		CTRL4_C = 0x13,
		CTRL5_C = 0x14,
		CTRL6_C = 0x15,
		CTRL7_G = 0x16,
		CTRL8_XL = 0x17,
		CTRL9_XL = 0x18,
		CTRL10_C = 0x19,
		ALL_INT_SRC = 0x1A,
		WAKE_UP_SRC = 0x1B,
		TAP_SRC = 0x1C,
		D6D_SRC = 0x1D,
		STATUS_REG = 0x1E,
		STATUS_SPIAux = 0x1E,
		OUT_TEMP_L = 0x20,
		OUT_TEMP_H = 0x21,
		OUTX_L_G = 0x22,
		OUTX_H_G = 0x23,
		OUTY_L_G = 0x24,
		OUTY_H_G = 0x25,
		OUTZ_L_G = 0x26,
		OUTZ_H_G = 0x27,
		OUTX_L_A = 0x28,
		OUTX_H_A = 0x29,
		OUTY_L_A = 0x2A,
		OUTY_H_A = 0x2B,
		OUTZ_L_A = 0x2C,
		OUTZ_H_A = 0x2D,
		EMB_FUNC_STATUS_MAINPAGE = 0x35,
		FSM_STATUS_A_MAINPAGE = 0x36,
		FSM_STATUS_B_MAINPAGE = 0x37,
		STATUS_MASTER_MAINPAGE = 0x39,
		FIFO_STATUS1 = 0x3A,
		FIFO_STATUS2 = 0x3B,
		TIMESTAMP0 = 0x40,
		TIMESTAMP1 = 0x41,
		TIMESTAMP2 = 0x42,
		TIMESTAMP3 = 0x43,
		TAP_CFG0 = 0x56,
		TAP_CFG1 = 0x57,
		TAP_CFG2 = 0x58,
		TAP_THS_6D = 0x59,
		INT_DUR2 = 0x5A,
		WAKE_UP_THS = 0x5B,
		WAKE_UP_DUR = 0x5C,
		FREE_FALL = 0x5D,
		MD1_CFG = 0x5E,
		MD2_CFG = 0x5F,
		I3C_BUS_AVB = 0x62,
		INTERNAL_FREQ_FINE = 0x63,
		INT_OIS = 0x6F,
		CTRL1_OIS = 0x70,
		CTRL2_OIS = 0x71,
		CTRL3_OIS = 0x72,
		X_OFS_USR = 0x73,
		Y_OFS_USR = 0x74,
		Z_OFS_USR = 0x75,
		FIFO_DATA_OUT_TAG = 0x78,
		FIFO_DATA_OUT_X_L = 0x79,
		FIFO_DATA_OUT_X_H = 0x7A,
		FIFO_DATA_OUT_Y_L = 0x7B,
		FIFO_DATA_OUT_Y_H = 0x7C,
		FIFO_DATA_OUT_Z_L = 0x7D,
		FIFO_DATA_OUT_Z_H = 0x7E,
	};

	enum class
	LinearRange : uint8_t
	{
		// assumes that XL_FS_MODE==0 in register CTRL8_XL
		G2  = (0b00 << 2),
		G4  = (0b10 << 2),
		G8  = (0b11 << 2),
		G16 = (0b01 << 2),
	};

	enum class
	AngularRange : uint8_t
	{
		dps125  = (0b001 << 1),
		dps250  = (0b000 << 1),
		dps500  = (0b010 << 1),
		dps1000 = (0b100 << 1),
		dps2000 = (0b110 << 1),
	};

protected:
	/// @cond
	static constexpr uint8_t Ctrl1XlOutputDataRateMask = (0b1111 << 4);
	static constexpr uint8_t Ctrl1XlLinearRangeMask = (0b11 << 2);
	static constexpr uint8_t Ctrl2GOutputDataRateMask = (0b1111 << 4);
	static constexpr uint8_t Ctrl2GAngularRangeMask = (0b111 << 1);

	template<frequency_t frequency>
	static constexpr std::pair<float, uint8_t>
	getClosestOdr()
	{
		/*
		```python3
		available_freqs = [12.5, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660]
		for i in range(len(available_freqs) - 1):
			l = available_freqs[i]
			u = available_freqs[i+1]
			print(f'else if constexpr (frequency < {round(2*l*u/(l+u))}) return ...')
		```
		*/
		if constexpr (frequency == 0) { return std::make_pair(0.0/*Hz*/, 0b0000); }
		else if constexpr (frequency < 17) { return std::make_pair(12.5/*Hz*/, 0b0001); }
		else if constexpr (frequency < 35) { return std::make_pair(26/*Hz*/, 0b0010); }
		else if constexpr (frequency < 69) { return std::make_pair(52/*Hz*/, 0b0011); }
		else if constexpr (frequency < 139) { return std::make_pair(104/*Hz*/, 0b0100); }
		else if constexpr (frequency < 277) { return std::make_pair(208/*Hz*/, 0b0101); }
		else if constexpr (frequency < 555) { return std::make_pair(416/*Hz*/, 0b0110); }
		else if constexpr (frequency < 1109) { return std::make_pair(833/*Hz*/, 0b0111); }
		else if constexpr (frequency < 2216) { return std::make_pair(1660/*Hz*/, 0b1000); }
		else if constexpr (frequency < 4440) { return std::make_pair(3330/*Hz*/, 0b1001); }
		else { return std::make_pair(6660/*Hz*/, 0b1010); }
	}

	static constexpr uint8_t
	i(Register v) { return uint8_t(v); }
	static constexpr uint8_t
	i(LinearRange v) { return uint8_t(v); }
	static constexpr uint8_t
	i(AngularRange v) { return uint8_t(v); }
	/// @endcond
}; // struct lsm6dso

/**
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 * @see Lsm6TransportI2c
 * @see Lsm6TransportSpi
 *
 * @ingroup modm_driver_lsm6dso
 * @author	Raphael Lehmann
 */
template < class Transport >
class Lsm6dso : public lsm6dso, public Transport
{
public:
	/**
	 * @brief Constructor
	 *
	 * For I2c this also sets the address to 0x6a (alternative: 0x6b with pin
	 * SA0 high).
	 */
	explicit Lsm6dso(uint8_t address=0x6a);

	/**
	 * @brief Initializes the device and driver
	 *
	 * SPI/I2C must be initialized by the user!
	 *
	 * @return False in case of any error
	 */
	modm::ResumableResult<bool>
	initialize();

	/**
	 * @brief Read a single register.
	 *
	 * @param reg The register to be read
	 * @return The register value in case of a read access, or std::nullopt if
	 * an error occured
	 */
	modm::ResumableResult<std::optional<uint8_t>>
	readRegister(Register reg);

	/**
	 * @brief Read multiple consecutive registers
	 *
	 * @param reg The first register to be read
	 * @param values Results of the read operations
	 * @return False in case of any error
	 */
	modm::ResumableResult<bool>
	readRegisters(Register reg, std::span<uint8_t> values);

	/**
	 * @brief Write a single register.
	 *
	 * @param reg The register to be written
	 * @param value The value to be written to the register.
	 * @return False in case of any error
	 */
	modm::ResumableResult<bool>
	writeRegister(Register reg, uint8_t value);

	/**
	 * @brief Set the desired output data rate from 12.5Hz to 6.7kHz
	 *
	 * Use 0 Hz to disable the IMU (power down).
	 *
	 * @tparam frequency: desired frequency
	 * @tparam tolerance: acceptable tolerance, default 1%
	 * @return False in case of any error
	 */
	template<frequency_t frequency, percent_t tolerance=pct(1)>
	modm::ResumableResult<bool>
	setOutputDataRate();

	/**
	 * @brief Set the measurement range
	 *
	 * @param lr Linear acceleration measurement range (±2G, ±4G, ±8G, ±16G)
	 * @param ar Angular rate measurement range (±125dps, ±250dps, ±500dps,
	 * ±1000dps, ±2000dps)
	 * @return False in case of any error
	 */
	modm::ResumableResult<bool>
	setRange(LinearRange lr, AngularRange ar);

	/**
	 * @brief Set the desired output data rate from 12.5Hz to 6.7kHz and the
	 * measurement range simultanously
	 *
	 * Use frequency==0 to disable the IMU (power down).
	 *
	 * @tparam frequency: desired frequency
	 * @tparam tolerance: acceptable tolerance, default 1%
	 * @param lr Linear acceleration measurement range (±2G, ±4G, ±8G, ±16G)
	 * @param ar Angular rate measurement range (±125dps, ±250dps, ±500dps,
	 * ±1000dps, ±2000dps)
	 * @return False in case of any error
	 */
	template<frequency_t frequency, percent_t tolerance=pct(1)>
	modm::ResumableResult<bool>
	setOutputDataRateAndRange(LinearRange lr, AngularRange ar);

private:
	static constexpr std::size_t bufferSize = 2;
	std::array<uint8_t, bufferSize> buffer;
};

} // namespace modm

#include "lsm6dso_impl.hpp"
