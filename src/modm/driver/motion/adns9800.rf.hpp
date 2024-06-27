/*
 * Copyright (c) 2024, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/platform.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/math/geometry/vector2.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/accessor_flash.hpp>

EXTERN_FLASH_STORAGE(uint8_t adns9800_firmware[3070]);

#define ADNS9800_RF_SLEEP_FOR(time) timeout.restart(time); \
  RF_WAIT_UNTIL(timeout.isExpired());

namespace modm {

/// @ingroup modm_driver_adns9800
struct adns9800 {
  /// @cond
  enum class Register : uint8_t {
    Product_ID = 0x00,
    Revision_ID = 0x01,
    Motion = 0x02,
    Delta_X_L = 0x03,
    Delta_X_H = 0x04,
    Delta_Y_L = 0x05,
    Delta_Y_H = 0x06,
    SQUAL = 0x07, // Surface Qaulity
    Pixel_Sum = 0x08,
    Maximum_Pixel = 0x09,
    Minimum_Pixel = 0x0a,
    Shutter_Lower = 0x0b,
    Shutter_Upper = 0x0c,
    Frame_Period_Lower = 0x0d,
    Frame_Period_Upper = 0x0e,
    Configuration_I = 0x0f, // Resolution
    Configuration_II = 0x10,
    Frame_Capture = 0x12,
    SROM_Enable = 0x13,
    Run_Downshift = 0x14,
    Rest1_Rate = 0x15,
    Rest1_Downshift = 0x16,
    Rest2_Rate = 0x17,
    Rest2_Downshift = 0x18,
    Rest3_Rate = 0x19,
    Frame_Period_Max_Bound_Lower = 0x1a,
    Frame_Period_Max_Bound_Upper = 0x1b,
    Frame_Period_Min_Bound_Lower = 0x1c,
    Frame_Period_Min_Bound_Upper = 0x1d,
    Shutter_Max_Bound_Lower = 0x1e,
    Shutter_Max_Bound_Upper = 0x1f,
    LASER_CTRL0 = 0x20, // Laser Control
    Observation = 0x24,
    Data_Out_Lower = 0x25,
    Data_Out_Upper = 0x26,
    SROM_ID = 0x2a,
    Lift_Detection_Thr = 0x2e,
    Configuration_V = 0x2f,
    Configuration_IV = 0x39,
    Power_Up_Reset = 0x3a,
    Shutdown = 0x3b,
    Snap_Angle = 0x42,
    Inverse_Product_ID = 0x3f,
    Motion_Burst = 0x50,
    SROM_Load_Burst = 0x62,
    Pixel_Burst = 0x64,
  };
  /// @endcond

  /**
   * @tparam Cpi    Counts per inch
   *                Allowed values: 200 to 8200 in steps of 200
   */
  template<int Cpi>
  requires (Cpi >= 200) and (Cpi % 200 == 0) and (Cpi <= 8200)
  struct Resolution: public modm::Register8 {
    Resolution() : modm::Register8(Cpi / 200) {};
  };
  using Resolution_t = modm::Register8;

  enum class LaserControl : uint8_t {
    ForceDisable = Bit0,
    AlwaysOn = Bit2,
  };
  MODM_FLAGS8(LaserControl);

  enum class ConfigurationII : uint8_t {
    Cpi_Reporting_Mode = Bit2,
    Fixed_FrameRate = Bit3,
    Disable_AGC = Bit4,
    Rest_Enable = Bit5,
    Force_Rest0 = Bit6,
    Force_Rest1 = Bit7,
  };
  MODM_FLAGS8(ConfigurationII);

  // Time periods as ticks of Adns9800 running at 50MHz
  using Period = uint16_t;
  using Duration = std::chrono::duration<Period, std::ratio<1, 50_MHz>>;
  using Delta = int16_t;

  using Pixel = modm::color::BrightnessT<uint8_t>;
  static constexpr size_t pixel_count = 30 * 30;
  using FrameBuffer = std::array<Pixel, pixel_count>;

  /**
   * @brief                ShutterConfig boundaries which may be selected by the automatic frame rate control.
   *                       In "constant Framerate mode" period_max determines the framerate.
   *
   * @param period_min     Frame period min bound -> max allowed fps
   * @param period_max     Frame period max bound -> min allowed fps.
   * @param exposure_max   Exposure period max bound -> max exposure time
   *
   * @ingroup modm_driver_adns9800
   */
  struct ShutterConfig {
    Period period_min;
    Period period_max;
    Period exposure_max;

    Duration getOneFrameTime() const {
      return Duration(exposure_max);
    }

    /// @cond
    // Validate before write: There's no internal protection against malformed combinations.
    bool isValid() {
      return modm_assert_continue_ignore(period_max >= period_min + exposure_max, "adns9800.shutter",
      "Invalid shutter configuration: !(period_max >= period_min + exposure_max)");
    }
    /// @endcond
  };

  /// @brief IC verification results containing success flags
  struct Verification {
    bool ProductId: 1;
    bool ProductIdInverse: 1;
    bool RevisionId: 1;

    operator bool() const {
      return ProductId and ProductIdInverse and RevisionId;
    }
  };

  using Data = modm::Vector<int16_t, 2>;
};

/**
 * ADNS9800 Laser Motion Sensor
 *
 * @tparam  SpiMaster
 * @tparam  Cs
 *
 * @author	Thomas Sommer
 * @ingroup	modm_driver_adns9800
 */
template <typename SpiMaster, typename Cs>
class Adns9800 : public adns9800, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<3> {
  Data &data;
  uint8_t buffer[6];

  // defaults from datasheet P22
  Shutter shutter{
    period_min:   4000, // 0xa00f
    period_max:  24000, // 0xc05d
    exposure_max: 20000 // 0x204e
  };

  modm::ShortTimeout timeout_next_read{0s}, timeout_next_write{0s};

  modm::ResumableResult<void>
  readTransacionEnd() {
    RF_BEGIN();

    timeout_next_read.restart(20us);
    timeout_next_write.restart(20us);

    modm::sleep(120ns);
    Cs::set();

    RF_END();
  }

  modm::ResumableResult<void>
  writeTransactionEnd() {
    RF_BEGIN();

    timeout_next_read.restart(120us);
    timeout_next_write.restart(20us);

    ADNS9800_RF_SLEEP_FOR(20us);
    Cs::set();

    RF_END();
  }

  modm::ResumableResult<uint8_t>
  readRegister(Register reg) {
    RF_BEGIN();

    RF_WAIT_UNTIL(timeout_next_read.isExpired());
    RF_WAIT_UNTIL(this->acquireMaster());

    Cs::reset();
    RF_CALL(SpiMaster::transfer(static_cast<uint8_t>(reg) & ~Bit7)); // Bit7(MSBit) = 0 indicates a read
    ADNS9800_RF_SLEEP_FOR(100us); // tSRAD [time] [S]pi between [R]ead [A]dress and [D]ata
    RF_CALL(SpiMaster::transfer(nullptr, buffer, 1));
    RF_CALL(readTransacionEnd());

    RF_END_RETURN(buffer[0]);
  }

  modm::ResumableResult<void>
  writeRegister(Register const reg, uint8_t const data) {
    RF_BEGIN();

    RF_WAIT_UNTIL(timeout_next_write.isExpired());
    RF_WAIT_UNTIL(this->acquireMaster());

    Cs::reset();
    RF_CALL(SpiMaster::transfer(static_cast<uint8_t>(reg) | Bit7)); // Bit7(MSBit) = 1 indicates a write
    RF_CALL(SpiMaster::transfer(data));
    RF_END_RETURN_CALL(writeTransactionEnd());
  }

  modm::ResumableResult<void>
  writeFirmware() {
    RF_BEGIN();

    RF_CALL(writeRegister(Register::Configuration_IV, 0x02)); // set 3k firmware mode
    RF_CALL(writeRegister(Register::SROM_Enable, 0x1d)); // Initialize SROM

    ADNS9800_RF_SLEEP_FOR(shutter_config.getOneFrameTime());

	  RF_CALL(writeRegister(Register::SROM_Enable, 0x18)); // Initiate SROM download

    RF_WAIT_UNTIL(timeout_next_write.isExpired());
    RF_WAIT_UNTIL(this->acquireMaster());

    Cs::reset();
    RF_CALL(SpiMaster::transfer(static_cast<uint8_t>(Register::SROM_Load_Burst) | Bit7)); // Bit7(MSBit) = 1 indicates a write
    accessor::Flash<uint8_t> flash(adns9800_firmware);
    for(size_t ii = 0; ii < sizeof(adns9800_firmware); ++ii)
    {
      ADNS9800_RF_SLEEP_FOR(15us);
      RF_CALL(SpiMaster::transfer(*flash++));
    }
    // @warning Datasheets says that there's 160us waittime
    // after firmware write until next read, not default tSWR (120us)
    RF_CALL(writeTransactionEnd());

    RF_END();
  }

public:
	Adns9800(Data &data) : data(data)
  {}

  modm::ResumableResult<void>
	powerUp() {
    RF_BEGIN();

    // @see power-up sequence, datasheet page 20
    RF_CALL(writeRegister(Register::Power_Up_Reset, 0x5a));
    ADNS9800_RF_SLEEP_FOR(50ms);
    // flush motion data
    RF_CALL(read());
    data.x = data.y = 0;

    RF_END();
	}

  // Run powerUp() to deassert shutdown mode
  modm::ResumableResult<void>
	shutdown() {
    RF_BEGIN();
		RF_END_RETURN_CALL(writeRegister(Register::Shutdown, 0xb6));
	}

  modm::ResumableResult<void>
  initialize() {
    RF_BEGIN();

    RF_CALL(powerUp());
    RF_CALL(verify());
    RF_CALL(writeFirmware());
    ADNS9800_RF_SLEEP_FOR(10ms);

    RF_END();
  }

  ResumableResult<Verification>
  verify() {
    RF_BEGIN();

    Verification verification;
    verification.ProductId = RF_CALL(readRegister(Register::Product_ID)) == uint8_t(0x33);
    verification.ProductIdInverse = RF_CALL(readRegister(Register::Inverse_Product_ID)) == uint8_t(~0x33);
    verification.RevisionId = RF_CALL(readRegister(Register::Revision_ID)) == uint8_t(0x03);

    RF_END_RETURN(verification);
  }

  modm::ResumableResult<void>
  set(const ConfigurationII_t config) {
    RF_BEGIN();

    RF_END_RETURN_CALL(writeRegister(Register::Configuration_II, config.value));
  }

  modm::ResumableResult<void>
  set(LaserControl_t config) {
    RF_BEGIN();

    uint8_t preserved = RF_CALL(readRegister(Register::LASER_CTRL0)) & 0xf0;
    RF_END_RETURN_CALL(writeRegister(Register::LASER_CTRL0, preserved | config.value));
  }

  modm::ResumableResult<void>
	set(Resolution_t resolution) {
    RF_BEGIN();

		RF_END_RETURN_CALL(writeRegister(Register::Configuration_I, resolution.value));
	}

  /**
   * Enable or disable the Angle Snapping function. When enabled, actual movement ranges from ±5° from X or Y-axis,
   * it will be snapped to the closest axis. For example, if the sensor moves at ±2° from X-axis, the output motion
   * data will be snapped to X-axis at 0°.
   */
  modm::ResumableResult<void>
  setSnapAngle(const bool value = true) {
    RF_BEGIN();

    const uint8_t preserved = RF_CALL(readRegister(Register::Snap_Angle)) & ~0x80;
    RF_END_RETURN_CALL(writeRegister(Register::Snap_Angle, preserved | value ? 0x80 : 0x00));
  }

  modm::ResumableResult<void>
	set(Shutter shutter_new) {
    RF_BEGIN();

    if (shutter_new.isValid()) {
      shutter = shutter_new;

      // @todo use SpiMaster 16bit-transfer @see #690
      // @todo use bulk write
      RF_CALL(writeRegister(Register::Frame_Period_Max_Bound_Lower, shutter.period_max & 0xff));
      RF_CALL(writeRegister(Register::Frame_Period_Max_Bound_Upper, shutter.period_max >> 8));

      RF_CALL(writeRegister(Register::Frame_Period_Min_Bound_Lower, shutter.period_min & 0xff));
      RF_CALL(writeRegister(Register::Frame_Period_Min_Bound_Upper, shutter.period_min >> 8));

      RF_CALL(writeRegister(Register::Shutter_Max_Bound_Lower, shutter.exposure_max & 0xff));
      RF_CALL(writeRegister(Register::Shutter_Max_Bound_Upper, shutter.exposure_max >> 8));

    }

    RF_END();
  }

  /// In fixed framerate mode (Register ConfigurationII::Fixed_FrameRate: 1), period_max selects the framerate
  modm::ResumableResult<void>
	setFramePeriodMax(const uint16_t period_max) {
    RF_BEGIN();

    const uint16_t period_max_recover = period_max;
    shutter.period_max = period_max;

    if (shutter.isValid()) {
      // @todo use SpiMaster 16 bit transfer @see #690
      RF_CALL(writeRegister(Register::Frame_Period_Max_Bound_Lower, shutter.period_max & 0xff));
      RF_CALL(writeRegister(Register::Frame_Period_Max_Bound_Upper, shutter.period_max >> 8));

    } else {
      shutter.period_max = period_max_recover;
    }

    RF_END();
  }

  modm::ResumableResult<void>
	setFramePeriodMin(const uint16_t period_min) {
    RF_BEGIN();

    const uint16_t period_min_recover = period_min;
    shutter.period_min = period_min;

    if (shutter.isValid()) {
      // @todo use SpiMaster 16 bit transfer @see #690
      RF_CALL(writeRegister(Register::Frame_Period_Min_Bound_Lower, shutter.period_min & 0xff));
      RF_CALL(writeRegister(Register::Frame_Period_Min_Bound_Upper, shutter.period_min >> 8));

    } else {
      shutter.period_min = period_min_recover;
    }

    RF_END();
  }

  modm::ResumableResult<void>
  setExposureMax(const uint16_t exposure_max) {
    RF_BEGIN();

    const uint16_t exposure_max_recover = exposure_max;
    shutter.exposure_max = exposure_max;

    if (shutter.isValid()) {
      // @todo use SpiMaster 16 bit transfer @see #690
      RF_CALL(writeRegister(Register::exposure_max_Bound_Lower, shutter.exposure_max & 0xff));
      RF_CALL(writeRegister(Register::exposure_max_Bound_Upper, shutter.exposure_max >> 8));

    } else {
      shutter.exposure_max = exposure_max_recover;
    }

    RF_END();
  }

  modm::ResumableResult<bool>
  hasNewMotionData() {
    RF_BEGIN();

    const uint8_t motion_reg = RF_CALL(readRegister(Register::Motion));

    RF_END_RETURN(motion_reg & Bit7);
  }

  /**
   * Read the latest motion delta from the sensor and update data with it content
   * Reading the motion delta also reloads the Motion interrupt (Pin7)
   */
  modm::ResumableResult<void>
  read() {
    RF_BEGIN();

    RF_WAIT_UNTIL(timeout_next_read.isExpired());
    RF_WAIT_UNTIL(this->acquireMaster());

    Cs::reset();
    RF_CALL(SpiMaster::transfer(static_cast<uint8_t>(Register::Motion_Burst)));
    ADNS9800_RF_SLEEP_FOR(100us); // tSRAD [time] [S]pi between [R]ead [A]dress and [D]ata
    RF_CALL(SpiMaster::transfer(nullptr, buffer, 6));
    data.x = buffer[3] << 8 | buffer[2];
    data.y = buffer[5] << 8 | buffer[4];
    RF_END_RETURN_CALL(readTransacionEnd());
  }

  // @todo test captureFrame
  /**
   * @brief         Capture the next frame into the SROM of Adns9800. Afterwards copy it to
   *                &buffer. Since this overwrites the Firmware, you have to restore normal
   *                operation by calling initialize().
   *
   * @param  buffer Buffer to store the captured frame
   */
  modm::ResumableResult<void>
  captureFrame(FrameBuffer &buffer) {
    RF_BEGIN();

    RF_WAIT_UNTIL(timeout_next_read.isExpired());
    RF_WAIT_UNTIL(this->acquireMaster());

    Cs::reset();
    RF_CALL(writeRegister(Register::Frame_Capture, 0x93));
    RF_CALL(writeRegister(Register::Frame_Capture, 0xc5));
    RF_WAIT_UNTIL(readRegister(Register::Motion) & Bit0); // wait until frame is captured
    RF_CALL(SpiMaster::transfer(nullptr, buffer.data(), buffer.size()));

    RF_END_RETURN_CALL(readTransacionEnd());
  }
};

} // namespace modm