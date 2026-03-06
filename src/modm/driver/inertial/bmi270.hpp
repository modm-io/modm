/*
 * Copyright (c) 2026, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMI270_HPP
#define MODM_BMI270_HPP

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <modm/architecture/interface/accessor.hpp>
#include <modm/math/geometry/vector3.hpp>
#include <optional>
#include <span>

#include "bmi270_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_bmi270
struct bmi270
{
	enum class AccRange : uint8_t
	{
		Range2g = 0x00,  //< +-2g
		Range4g = 0x01,  //< +-4g
		Range8g = 0x02,  //< +-8g
		Range16g = 0x03  //< +-16g
	};

	enum class GyroRange : uint8_t
	{
		Range2000dps = 0x00,  //< +-2000 deg/s
		Range1000dps = 0x01,  //< +-1000 deg/s
		Range500dps = 0x02,   //< +-500 deg/s
		Range250dps = 0x03,   //< +-250 deg/s
		Range125dps = 0x04    //< +-125 deg/s
	};

	enum class AccRate : uint8_t
	{
		Rate25Hz_Osr4 = 0x06 | 0x80,
		Rate25Hz_Normal = 0x06 | 0x20 | 0x80,
		Rate50Hz_Normal = 0x07 | 0x20 | 0x80,
		Rate100Hz_Normal = 0x08 | 0x20 | 0x80,
		Rate200Hz_Normal = 0x09 | 0x20 | 0x80,
		Rate400Hz_Normal = 0x0A | 0x20 | 0x80,
		Rate800Hz_Normal = 0x0B | 0x20 | 0x80,
		Rate1600Hz_Normal = 0x0C | 0x20 | 0x80
	};

	enum class GyroRate : uint8_t
	{
		Rate25Hz_Normal = 0x06 | 0x20 | 0x40 | 0x80,
		Rate50Hz_Normal = 0x07 | 0x20 | 0x40 | 0x80,
		Rate100Hz_Normal = 0x08 | 0x20 | 0x40 | 0x80,
		Rate200Hz_Normal = 0x09 | 0x20 | 0x40 | 0x80,
		Rate400Hz_Normal = 0x0A | 0x20 | 0x40 | 0x80,
		Rate800Hz_Normal = 0x0B | 0x20 | 0x40 | 0x80,
		Rate1600Hz_Normal = 0x0C | 0x20 | 0x40 | 0x80
	};

	enum class Status : uint8_t
	{
		AccDataReady = Bit7,
		GyroDataReady = Bit6,
		CommandReady = Bit4
	};
	MODM_FLAGS8(Status);

	enum class PowerControl : uint8_t
	{
		Auxiliary = Bit0,
		Gyroscope = Bit1,
		Accelerometer = Bit2,
		Temperature = Bit3
	};
	MODM_FLAGS8(PowerControl);

	enum class PowerConfiguration : uint8_t
	{
		AdvancedPowerSave = Bit0,
		FifoSelfWakeup = Bit1,
		FastPowerUp = Bit2
	};
	MODM_FLAGS8(PowerConfiguration);

	enum class InterruptOutputLevel : uint8_t
	{
		ActiveLow = 0x00,
		ActiveHigh = 0x01
	};

	enum class InterruptOutputType : uint8_t
	{
		PushPull = 0x00,
		OpenDrain = 0x01
	};

	enum class InterruptLatch : uint8_t
	{
		None = 0x00,
		Permanent = 0x01
	};

	enum class InternalMessage : uint8_t
	{
		NotInitialized = 0x00,
		Initialized = 0x01,
		InitError = 0x02,
		DriverError = 0x03,
		SensorError = 0x04,
		NvmError = 0x05,
		StartupError = 0x06,
		CompatibilityError = 0x07
	};

	enum class PullUpConfiguration : uint8_t
	{
		Off = 0x00,
		PullUp40k = 0x01,
		PullUp10k = 0x02,
		PullUp2k = 0x03
	};

	enum class InterfaceSpiMode : uint8_t
	{
		Spi4Wire = 0x00,
		Spi3Wire = 0x01
	};

	enum class DriveStrength : uint8_t
	{
		Level0 = 0b000,
		Level1 = 0b001,
		Level2 = 0b010,
		Level3 = 0b011,
		Level4 = 0b100,
		Level5 = 0b101,
		Level6 = 0b110,
		Level7 = 0b111
	};

	enum class Command : uint8_t
	{
		TriggerGyro = 0x02,
		ProgramNvm = 0xA0,
		FlushFifo = 0xB0,
		SoftReset = 0xB6
	};

	struct AccData
	{
		/// acceleration in milli-g
		Vector3f
		getFloat() const;

		Vector3i raw;
		AccRange range;
	};

	struct GyroData
	{
		/// angular rate in deg/s
		Vector3f
		getFloat() const;

		Vector3i raw;
		GyroRange range;
	};

	struct Data
	{
		AccData acc;
		GyroData gyro;
		uint32_t sensorTime;
	};

	struct ErrorInfo
	{
		bool fatalError;
		uint8_t internalError;
		bool fifoError;
		bool auxError;
	};

	struct SensorStatus
	{
		bool accDataReady;
		bool gyroDataReady;
		bool auxDataReady;
		bool commandReady;
		bool auxBusy;
	};

	struct InternalStatus
	{
		InternalMessage message;
		bool axesRemapError;
		bool odr50HzError;
	};

	struct InterruptStatus
	{
		bool fifoFull;
		bool fifoWatermark;
		bool error;
		bool auxDataReady;
		bool gyroDataReady;
		bool accDataReady;
	};

	struct InterruptIoControl
	{
		InterruptOutputLevel level;
		InterruptOutputType outputType;
		bool outputEnable;
		bool inputEnable;
	};

	struct InterruptMapData
	{
		bool int1FifoFull;
		bool int1FifoWatermark;
		bool int1DataReady;
		bool int1Error;
		bool int2FifoFull;
		bool int2FifoWatermark;
		bool int2DataReady;
		bool int2Error;
	};

	struct ErrorInterruptMask
	{
		bool fatalError;
		bool internalError;
		bool fifoError;
		bool auxError;
	};

	struct InternalError
	{
		bool longProcessingTime;
		bool fatalError;
		bool featureEngineDisabled;
	};

	struct GyroCrtConfig
	{
		bool running;
		bool readyForDownload;
	};

	struct InterfaceConfig
	{
		InterfaceSpiMode primarySpiMode;
		InterfaceSpiMode oisSpiMode;
		bool oisEnabled;
		bool auxEnabled;
	};

	struct DriveConfig
	{
		DriveStrength ioPadDrv1;
		bool ioPadI2cBoost1;
		DriveStrength ioPadDrv2;
		bool ioPadI2cBoost2;
	};

	struct AccOffsets
	{
		uint8_t x;
		uint8_t y;
		uint8_t z;
	};

	struct GyroOffsets
	{
		uint16_t x;
		uint16_t y;
		uint16_t z;
		bool offsetEnabled;
		bool gainEnabled;
	};

	struct GyroUserGain
	{
		int8_t x;
		int8_t y;
		int8_t z;
	};

	enum class FifoFilterData : uint8_t
	{
		Unfiltered = 0x00,
		Filtered = 0x01
	};

	struct FifoDownsampling
	{
		uint8_t gyroDownsampling;
		FifoFilterData gyroFilterData;
		uint8_t accDownsampling;
		FifoFilterData accFilterData;
	};

	enum class FifoTagInterrupt : uint8_t
	{
		Edge = 0x00,
		Level = 0x01,
		AccSaturation = 0x02,
		GyroSaturation = 0x03
	};

	struct FifoConfiguration
	{
		bool stopOnFull;
		bool timeEnable;
		FifoTagInterrupt tagInt1;
		FifoTagInterrupt tagInt2;
		bool headerEnable;
		bool auxEnable;
		bool accEnable;
		bool gyroEnable;
	};

	struct Temperature
	{
		bool valid;
		float celsius;
	};
};

/// @cond
namespace bmi270_private
{
static constexpr std::size_t ConfigurationSize{8192};
extern modm::accessor::Flash<uint8_t> configuration;
}  // namespace bmi270_private
/// @endcond

/**
 * Bosch BMI270 IMU
 *
 * The device contains an accelerometer and a gyroscope.
 *
 * @tparam Transport Transport layer (use @ref Bmi270SpiTransport or @ref Bmi270I2cTransport)
 * @ingroup modm_driver_bmi270
 */
template<Bmi270Transport Transport>
class Bmi270 : public bmi270, public Transport
{
public:
	/// @arg transportArgs Arguments to transport layer.
	template<typename... Args>
	Bmi270(Args... transportArgs);

	/// Initialize device. Call before any other member function.
	/// @return true on success, false on error
	bool
	initialize(std::span<const uint8_t> configFile = {});

	/// Soft-reset device and wait for reset completion.
	bool
	reset();

	/// Read acceleration and gyroscope data and sensor time from one burst read.
	std::optional<Data>
	readData();

	std::optional<AccData>
	readAccData();

	std::optional<GyroData>
	readGyroData();

	bool
	readAccDataReady();

	bool
	readGyroDataReady();

	bool
	readCommandReady();

	std::optional<uint8_t>
	getChipId();

	std::optional<ErrorInfo>
	getErrors();

	std::optional<SensorStatus>
	getStatus();

	std::optional<InternalStatus>
	getInternalStatus();

	std::optional<Temperature>
	getTemperature();

	std::optional<InterruptStatus>
	getInterruptStatus();

	std::optional<ErrorInterruptMask>
	getErrorInterruptMask();

	bool
	setErrorInterruptMask(ErrorInterruptMask mask);

	std::optional<InterruptIoControl>
	getInt1IoControl();

	bool
	setInt1IoControl(InterruptIoControl control);

	std::optional<InterruptIoControl>
	getInt2IoControl();

	bool
	setInt2IoControl(InterruptIoControl control);

	std::optional<InterruptLatch>
	getInterruptLatch();

	bool
	setInterruptLatch(InterruptLatch mode);

	std::optional<InterruptMapData>
	getInterruptMapData();

	bool
	setInterruptMapData(InterruptMapData map);

	std::optional<InternalError>
	getInternalError();

	std::optional<PullUpConfiguration>
	getPullUpConfiguration();

	bool
	setPullUpConfiguration(PullUpConfiguration configuration);

	std::optional<GyroCrtConfig>
	getGyroCrtConfig();

	bool
	setGyroCrtConfig(GyroCrtConfig configuration);

	std::optional<bool>
	getNvmCrtEnabled();

	bool
	setNvmCrtEnabled(bool enable);

	bool
	triggerGyroCrt();

	/// Perform component retrim (CRT) for the gyroscope.
	///
	/// If no configuration blob is passed, the built-in BMI270 configuration is used.
	bool
	doCrt(std::span<const uint8_t> configFile = {});

	std::optional<InterfaceConfig>
	getInterfaceConfig();

	bool
	setInterfaceConfig(InterfaceConfig configuration);

	std::optional<DriveConfig>
	getDriveConfig();

	bool
	setDriveConfig(DriveConfig configuration);

	std::optional<AccOffsets>
	getAccOffsets();

	bool
	setAccOffsets(AccOffsets offsets);

	std::optional<GyroOffsets>
	getGyroOffsets();

	bool
	setGyroOffsets(GyroOffsets offsets);

	/// Read compensated gyroscope user-gain values.
	std::optional<GyroUserGain>
	getGyroUserGain();

	/// Write compensated gyroscope user-gain values.
	///
	/// Values are encoded on 7 bits per axis.
	bool
	setGyroUserGain(GyroUserGain gain);

	std::optional<PowerConfiguration_t>
	getPowerConfiguration();

	bool
	setPowerConfiguration(PowerConfiguration_t configuration);

	std::optional<PowerControl_t>
	getPowerControl();

	bool
	sendCommand(Command command);

	bool
	setAccRate(AccRate rate);

	bool
	setAccRange(AccRange range);

	bool
	setGyroRate(GyroRate rate);

	bool
	setGyroRange(GyroRange range);

	/// Enable/disable sensor power domains.
	bool
	setPowerControl(PowerControl_t control);

	std::optional<uint16_t>
	getFifoLength();

	std::optional<FifoDownsampling>
	getFifoDownsampling();

	bool
	setFifoDownsampling(FifoDownsampling configuration);

	std::optional<uint16_t>
	getFifoWatermark();

	bool
	setFifoWatermark(uint16_t watermark);

	std::optional<FifoConfiguration>
	getFifoConfiguration();

	bool
	setFifoConfiguration(FifoConfiguration configuration);

	/// Read all currently available FIFO bytes into \p buffer.
	/// @return Number of bytes read on success.
	std::optional<uint16_t>
	readFifo(std::span<uint8_t> buffer);

	bool
	flushFifo();

private:
	using Register = Transport::Register;

	static constexpr std::chrono::microseconds WriteTimeout{2};
	static constexpr std::chrono::microseconds PowerModeTimeout{450};
	static constexpr std::chrono::milliseconds ResetTimeout{45};
	static constexpr std::chrono::milliseconds ConfigLoadTimeout{20};

	static constexpr uint8_t ChipId{0x24};
	static constexpr uint8_t SoftResetCommand{0xB6};
	static constexpr uint8_t FifoFlushCommand{0xB0};
	static constexpr uint8_t InitControlLoadDisabled{0x00};
	static constexpr uint8_t InitControlLoadEnabled{0x01};
	static constexpr uint8_t InternalStatusInitOk{0x01};

	bool
	checkChipId();

	bool
	uploadConfig(std::span<const uint8_t> configFile);

	bool
	uploadConfig(modm::accessor::Flash<uint8_t> configFile, std::size_t configSize);

	bool
	setAdvancedPowerSave(bool enable);

	bool
	enableSensors();

	bool
	readFeaturePage(uint8_t page, std::array<uint8_t, 16>& data);

	bool
	writeFeaturePage(uint8_t page, const std::array<uint8_t, 16>& data);

	std::optional<uint8_t>
	readRegister(Register reg);

	AccRange accRange_{AccRange::Range2g};
	GyroRange gyroRange_{GyroRange::Range2000dps};
};

}  // namespace modm

#include "bmi270_calibration_impl.hpp"
#include "bmi270_impl.hpp"

#endif  // MODM_BMI270_HPP
