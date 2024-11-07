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
#include <modm/processing/fiber.hpp>
#include <modm/processing/timer/timeout.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/accessor_flash.hpp>
#include <modm/debug/logger.hpp>
#include <modm/ui/color/brightness.hpp>

EXTERN_FLASH_STORAGE(uint8_t adns9800_firmware[3070]);

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
   * @tparam Dpi    Dots per inch (aproximately!)
   *                Range: 200 to 8200 in steps of 200
   */
  template<int Dpi>
  requires (Dpi >= 200) and (Dpi % 200 == 0) and (Dpi <= 8200)
  struct Resolution: public modm::Register8 {
    Resolution() : modm::Register8(Dpi / 200) {};
  };
  using Resolution_t = modm::Register8;

  enum class LaserControl : uint8_t {
    ForceDisable = Bit0,
    AlwaysOn = Bit2
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

  // forward declarations
  struct Data;
  struct Data_FailFlags;
  struct Data_FailFlags_Monitoring;
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
template <class SpiMaster, class Cs>
class Adns9800 : public adns9800, public modm::SpiDevice<SpiMaster> {
  // @see Default values found in datasheet P22
  ShutterConfig shutter_config{
    period_min:   4000, // 0xa00f
    period_max:  24000, // 0xc05d
    exposure_max: 20000 // 0x204e
  };

  // Adns9800 has specific deadtimes between consequtive transactions.
  // Timelocks are used to fulfill the requirements:
  modm::ShortTimeout ready_to_read{0s};
  modm::ShortTimeout ready_to_write{0s};

  template<class Callable>
  void
  readTransaction(Callable&& closure) {
    while(not ready_to_read.isExpired())
      modm::this_fiber::yield();
    while(not this->acquireMaster())
      modm::this_fiber::yield();
    Cs::reset();

    std::forward<Callable>(closure)();

    ready_to_write.restart(20us);      // tSRW: [t]ime [S]pi between [R]rite and [W]rite
    ready_to_read.restart(20us);       // tSRR: [t]ime [S]pi between [R]rite and [R]ead

    modm::delay(120ns); // tSCLK_NCS_read: Cs persistance after last SCLK for read
    Cs::set();
  }

  template<class Callable>
  void
  writeTransaction(Callable&& closure) {
    while(not ready_to_write.isExpired())
      modm::this_fiber::yield();
    while(not this->acquireMaster())
      modm::this_fiber::yield();
    Cs::reset();

    std::forward<Callable>(closure)();

    ready_to_write.restart(20us);      // tSWW [t]ime [S]pi between [W]rite and [W]rite
    ready_to_read.restart(120us);      // tSWR [t]ime [S]pi between [W]rite and [R]ead

    modm::this_fiber::sleep_for(20us); // tSCLK_NCS_write: Cs persistance after last SCLK for write
    Cs::set();
  }

  uint8_t
  readRegister(const Register reg) {
    uint8_t ret;

    readTransaction([reg, &ret]() {
      SpiMaster::transfer(static_cast<uint8_t>(reg));
      modm::this_fiber::sleep_for(100us); // tSRAD: [time] [S]pi between [R]ead [A]dress and [D]ata
      SpiMaster::transfer(nullptr, &ret, 1);
    });

    return ret;
  }

  void
  writeRegister(const Register reg, const uint8_t data) {
    writeTransaction([&]() {
      // Setting Bit7 indicates a write
      SpiMaster::transfer(static_cast<uint8_t>(reg) | Bit7);
      SpiMaster::transfer(data);
    });
  }

  void
  writeFirmware() {
    writeRegister(Register::Configuration_IV, 0x02); // Enable 3k firmware mode
    writeRegister(Register::SROM_Enable, 0x1d); // Initialize SROM
    modm::this_fiber::sleep_for(shutter_config.getOneFrameTime());
	  writeRegister(Register::SROM_Enable, 0x18); // Start SROM download

    writeTransaction([&]() {
      // Setting Bit7 indicates a write
      SpiMaster::transfer(static_cast<uint8_t>(Register::SROM_Load_Burst) | Bit7);

      accessor::Flash<uint8_t> flash(adns9800_firmware);
      for(size_t ii = 0; ii < sizeof(adns9800_firmware); ++ii)
      {
        modm::this_fiber::sleep_for(15us);
        SpiMaster::transfer(*flash++);
      }
    });
    modm::this_fiber::sleep_for(shutter_config.getOneFrameTime());

    // Additionaly, a CRC of the firmware may be requested. @see datasheet P31
  }

public:
  /// @brief Reset the device's internal state after power-loss or user invoked shutdown().
	void
  powerUp() {
    // power-up sequence @see datasheet P20
    Cs::reset();
    modm::delay(10ns);
    Cs::set();
    modm::delay(10ns);

    writeRegister(Register::Power_Up_Reset, 0x5a);
    modm::this_fiber::sleep_for(50ms);
    read<Data>(); // read and discard motion data
	}

  /// @brief Put the device into low power mode. Do not use this for power management in normal operation.
	void
  shutdown() {
		writeRegister(Register::Shutdown, 0xb6);
	}

  /// @brief     Verify presence of the device by validating various id registers.
  Verification
  verify() {
    return {
      ProductId: readRegister(Register::Product_ID) == static_cast<uint8_t>(0x33),
      ProductIdInverse: readRegister(Register::Inverse_Product_ID) == static_cast<uint8_t>(~0x33),
      RevisionId: readRegister(Register::Revision_ID) == static_cast<uint8_t>(0x03)
    };
  }

  /// @brief     Default protocoll to initialize the device
  Verification
  initialize() {
    powerUp();
    const Verification verification = verify();
    if(verification) {
      writeFirmware();
      laserEnable();
    }

    return verification;
  }

  void
  laserEnable(const bool enable = true) {
    LaserControl_t control(readRegister(Register::LASER_CTRL0));
    control.update(LaserControl::ForceDisable, !enable);
    writeRegister(Register::LASER_CTRL0, control.value);
  }

  void
  laserAlwaysOn(const bool enable = true) {
    LaserControl_t control(readRegister(Register::LASER_CTRL0));
    control.update(LaserControl::AlwaysOn, enable);
    writeRegister(Register::LASER_CTRL0, control.value);
  }

  /**
   * Enable or disable the Angle Snapping function. When enabled, actual movement ranges from ±5° from X or Y-axis,
   * it will be snapped to the closest axis. For example, if the sensor moves at ±2° from X-axis, the output motion
   * data will be snapped to X-axis at 0°.
   */
  void
  snapAngle(const bool value = true) {
    const uint8_t preserved = readRegister(Register::Snap_Angle) & ~0x80;
    writeRegister(Register::Snap_Angle, preserved | value ? 0x80 : 0x00);
  }

  void
  set(const ConfigurationII_t config) {
    writeRegister(Register::Configuration_II, config.value);
  }

  void
	set(const Resolution_t resolution) {
		writeRegister(Register::Configuration_I, resolution.value);
	}

  void
	set(ShutterConfig shutter_new) {
    if (shutter_new.isValid()) {
      shutter_config = shutter_new;

      writeRegister(Register::Frame_Period_Max_Bound_Lower, shutter_config.period_max & 0xff);
      writeRegister(Register::Frame_Period_Max_Bound_Upper, shutter_config.period_max >> 8);

      writeRegister(Register::Frame_Period_Min_Bound_Lower, shutter_config.period_min & 0xff);
      writeRegister(Register::Frame_Period_Min_Bound_Upper, shutter_config.period_min >> 8);

      writeRegister(Register::Shutter_Max_Bound_Lower, shutter_config.exposure_max & 0xff);
      writeRegister(Register::Shutter_Max_Bound_Upper, shutter_config.exposure_max >> 8);
    }
  }

  /// In fixed framerate mode (Register ConfigurationII::Fixed_FrameRate: 1), period_max selects the framerate
  void
	setFramePeriodMax(const Period period_max) {
    const Period recover = shutter_config.period_max;
    shutter_config.period_max = period_max;

    if (shutter_config.isValid()) {
      writeRegister(Register::Frame_Period_Max_Bound_Lower, shutter_config.period_max & 0xff);
      writeRegister(Register::Frame_Period_Max_Bound_Upper, shutter_config.period_max >> 8);
    } else {
      shutter_config.period_max = recover;
    }
  }

  void
	setFramePeriodMin(const Period period_min) {
    const Period recover = shutter_config.period_min;
    shutter_config.period_min = period_min;

    if (shutter_config.isValid()) {
      writeRegister(Register::Frame_Period_Min_Bound_Lower, shutter_config.period_min & 0xff);
      writeRegister(Register::Frame_Period_Min_Bound_Upper, shutter_config.period_min >> 8);
    } else {
      shutter_config.period_min = recover;
    }
  }

  void
  setExposureMax(const Period exposure_max) {
    const Period recover = shutter_config.exposure_max;
    shutter_config.exposure_max = exposure_max;

    if (shutter_config.isValid()) {
      writeRegister(Register::Shutter_Max_Bound_Lower, shutter_config.exposure_max & 0xff);
      writeRegister(Register::Shutter_Max_Bound_Upper, shutter_config.exposure_max >> 8);
    } else {
      shutter_config.exposure_max = recover;
    }
  }

  bool
  hasNewMotionData() const {
    return readRegister(Register::Motion) & Bit7;
  }

  /**
   * @brief       Read a Data packet from the sensor.
   *              Calling read() also resets the motion interrupt (Pin7)
   *
   * @tparam D    Data packet type to read. @see adns9800_data.hpp for details
   */
  template<class D>
  requires std::is_base_of_v<Data, D>
  D
  read() {
    std::array<uint8_t, D::Span::extent> buffer;

    readTransaction([this, &buffer]() {
      SpiMaster::transfer(static_cast<uint8_t>(Register::Motion_Burst));
      modm::this_fiber::sleep_for(shutter_config.getOneFrameTime());
      SpiMaster::transfer(nullptr, buffer.data(), buffer.size());
    });

    return D(buffer);
  }

  // @todo test captureFrame
  /**
   * @brief         Capture the next frame into the SROM of Adns9800. Afterwards copy it to
   *                &buffer. Since this overwrites the Firmware, you have to restore normal
   *                operation by calling initialize().
   *
   * @param  buffer Buffer to store the captured frame
   */
  void
  captureFrame(FrameBuffer& buffer) {
    readTransaction([&buffer]() {
      writeRegister(Register::Frame_Capture, 0x93);
      writeRegister(Register::Frame_Capture, 0xc5);
      modm::this_fiber::poll(readRegister(Register::Motion) & Bit0); // wait until frame is captured
      SpiMaster::transfer(nullptr, buffer.data(), buffer.size());
    });
  }
};

} // namespace modm

#include "adns9800_data.hpp"