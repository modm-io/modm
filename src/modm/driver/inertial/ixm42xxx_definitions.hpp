// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IXM42XXX_DEFINITIONS_HPP
#define MODM_IXM42XXX_DEFINITIONS_HPP

#include <stdint.h>
#include <modm/architecture/interface/register.hpp>

namespace modm
{

/// @ingroup modm_driver_ixm42xxx
struct ixm42xxx
{
    enum class
    Register : uint16_t
    {
        REG_BANK_SEL = 0x76,

        // USER BANK 0 REGISTER MAP START
        // *** ORed with 0x000 to indicate user bank 0 ***
        DEVICE_CONFIG = 0x011,
        DRIVE_CONFIG = 0x013,
        INT_CONFIG = 0x014,
        FIFO_CONFIG = 0x016,

        // Temperature data registers
        TEMP_DATA1 = 0x01D,
        TEMP_DATA0 = 0x01E,

        // Accel data registers
        ACCEL_DATA_X1 = 0x01F,
        ACCEL_DATA_X0 = 0x020,
        ACCEL_DATA_Y1 = 0x021,
        ACCEL_DATA_Y0 = 0x022,
        ACCEL_DATA_Z1 = 0x023,
        ACCEL_DATA_Z0 = 0x024,

        // Gyro data registers
        GYRO_DATA_X1 = 0x025,
        GYRO_DATA_X0 = 0x026,
        GYRO_DATA_Y1 = 0x027,
        GYRO_DATA_Y0 = 0x028,
        GYRO_DATA_Z1 = 0x029,
        GYRO_DATA_Z0 = 0x02A,

        TMST_FSYNCH = 0x02B,
        TMST_FSYNCL = 0x02C,
        INT_STATUS = 0x02D,

        // FIFO data registers
        FIFO_COUNTH = 0x02E,
        FIFO_COUNTL = 0x02F,
        FIFO_DATA = 0x030,

        // Apex data register
        APEX_DATA0 = 0x031,
        APEX_DATA1 = 0x032,
        APEX_DATA2 = 0x033,
        APEX_DATA3 = 0x034,
        APEX_DATA4 = 0x035,
        APEX_DATA5 = 0x036,

        INT_STATUS2 = 0x037,
        INT_STATUS3 = 0x038,

        SIGNAL_PATH_RESET = 0x04B,

        INTF_CONFIG0 = 0x04C,
        INTF_CONFIG1 = 0x04D,

        PWR_MGMT0 = 0x04E,
        GYRO_CONFIG0 = 0x04F,
        ACCEL_CONFIG0 = 0x050,
        GYRO_CONFIG1 = 0x051,
        GYRO_ACCEL_CONFIG0 = 0x052,
        ACCEL_CONFIG1 = 0x053,

        TMST_CONFIG = 0x054,
        APEX_CONFIG0 = 0x056,
        SMD_CONFIG = 0x057,

        // FIFO configuration registers
        FIFO_CONFIG1 = 0x05F,
        FIFO_CONFIG2 = 0x060,
        FIFO_CONFIG3 = 0x061,
        FSYNC_CONFIG = 0x062,

        // Interrupt configuration registers
        INT_CONFIG0 = 0x063,
        INT_CONFIG1 = 0x064,

        // Interrupt source registers
        INT_SOURCE0 = 0x065,
        INT_SOURCE1 = 0x066,
        INT_SOURCE3 = 0x068,
        INT_SOURCE4 = 0x069,

        FIFO_LOST_PKT0 = 0x06C,
        FIFO_LOST_PKT1 = 0x06D,

        SELF_TEST_CONFIG = 0x070,
        WHO_AM_I = 0x075,
        // USER BANK 0 REGISTER MAP END

        // USER BANK 1 REGISTER MAP START
        // *** ORed with 0x100 to indicate user bank 1 ***
        SENSOR_CONFIG0 = 0x103,
        GYRO_CONFIG_STATIC2 = 0x10B,
        GYRO_CONFIG_STATIC3 = 0x10C,
        GYRO_CONFIG_STATIC4 = 0x10D,
        GYRO_CONFIG_STATIC5 = 0x10E,
        GYRO_CONFIG_STATIC6 = 0x10F,
        GYRO_CONFIG_STATIC7 = 0x110,
        GYRO_CONFIG_STATIC8 = 0x111,
        GYRO_CONFIG_STATIC9 = 0x112,
        GYRO_CONFIG_STATIC10 = 0x113,

        XG_ST_DATA = 0x15F,
        YG_ST_DATA = 0x160,
        ZG_ST_DATA = 0x161,

        TMSTVAL0 = 0x162,
        TMSTVAL1 = 0x163,
        TMSTVAL2 = 0x164,

        INTF_CONFIG4 = 0x17A,
        INTF_CONFIG5 = 0x17B,
        INTF_CONFIG6 = 0x17C,
        // USER BANK 1 REGISTER MAP END

        // USER BANK 2 REGISTER MAP START
        // *** ORed with 0x200 to indicate user bank 2 ***
        ACCEL_CONFIG_STATIC2 = 0x203,
        ACCEL_CONFIG_STATIC3 = 0x204,
        ACCEL_CONFIG_STATIC4 = 0x205,

        // Accelerometer self-test data
        XA_ST_DATA = 0x23B,
        YA_ST_DATA = 0x23C,
        ZA_ST_DATA = 0x23D,
        // USER BANK 2 REGISTER MAP END

        // USER BANK 3 REGISTER MAP START
        // *** ORed with 0x300 to indicate user bank 3 ***
        PU_PD_CONFIG1 = 0x306,
        PU_PD_CONFIG2 = 0x30E,
        // USER BANK 3 REGISTER MAP END

        // USER BANK 4 REGISTER MAP START
        // *** ORed with 0x400 to indicate user bank 4 ***
        // Apex configuration registers
        FDR_CONFIG = 0x409,
        APEX_CONFIG1 = 0x440,
        APEX_CONFIG2 = 0x441,
        APEX_CONFIG3 = 0x442,
        APEX_CONFIG4 = 0x443,
        APEX_CONFIG5 = 0x444,
        APEX_CONFIG6 = 0x445,
        APEX_CONFIG7 = 0x446,
        APEX_CONFIG8 = 0x447,
        APEX_CONFIG9 = 0x448,
        APEX_CONFIG10 = 0x449,

        // Wake on motion configuration registers
        ACCEL_WOM_X_THR = 0x44A,
        ACCEL_WOM_Y_THR = 0x44B,
        ACCEL_WOM_Z_THR = 0x44C,

        // Interupt sources registers
        INT_SOURCE6 = 0x44D,
        INT_SOURCE7 = 0x44E,
        INT_SOURCE8 = 0x44F,
        INT_SOURCE9 = 0x450,
        INT_SOURCE10 = 0x451,

        // User programable offsets registers
        OFFSET_USER0 = 0x477,
        OFFSET_USER1 = 0x478,
        OFFSET_USER2 = 0x479,
        OFFSET_USER3 = 0x47A,
        OFFSET_USER4 = 0x47B,
        OFFSET_USER5 = 0x47C,
        OFFSET_USER6 = 0x47D,
        OFFSET_USER7 = 0x47E,
        OFFSET_USER8 = 0x47F,
        // USER BANK 4 REGISTER MAP END
    };

public:

    /// REG_BANK_SEL reset value is 0x00
    enum class
    RegBankSel : uint8_t
    {
        BANK_SEL2 = Bit2,    ///< See BankSel_t
        BANK_SEL1 = Bit1,    ///< See BankSel_t
        BANK_SEL0 = Bit0,    ///< See BankSel_t
    };
    MODM_FLAGS8(RegBankSel);

    enum class
    BankSel : uint8_t
    {
        Bank0 = 0b000,
        Bank1 = 0b001,
        Bank2 = 0b010,
        Bank3 = 0b011,
        Bank4 = 0b100,
    };
    typedef modm::Configuration<RegBankSel_t, BankSel, Bit2 | Bit1 | Bit0> BankSel_t;

    // -------------------------------------------------------------------------------

    /// DEVICE_CONFIG reset value is 0x00
    enum class
    DeviceConfig : uint8_t
    {
        SPI_MODE = Bit4,            ///< SPI mode selection
        SOFT_RESET_CONFIG = Bit0,   ///< Software reset configuration
    };
    MODM_FLAGS8(DeviceConfig);

    enum class
    DataMode : uint8_t
    {
        Mode0 = 0,
        Mode1 = int(DeviceConfig::SPI_MODE),
        Mode2 = int(DeviceConfig::SPI_MODE),
        Mode3 = 0,
    };
    typedef modm::Configuration<DeviceConfig_t, DataMode, Bit4> DataMode_t;

    enum class
    SoftResetConfig : uint8_t
    {
        Normal = 0,
        Enable = int(DeviceConfig::SOFT_RESET_CONFIG),
    };
    typedef modm::Configuration<DeviceConfig_t, SoftResetConfig, Bit0> SoftResetConfig_t;

    // -------------------------------------------------------------------------------

    /// DRIVE_CONFIG reset value is 0x05
    enum class
    DriveConfig : uint8_t
    {
        I2C_SLEW_RATE2 = Bit5,  ///< See I2cSlewRate_t
        I2C_SLEW_RATE1 = Bit4,  ///< See I2cSlewRate_t
        I2C_SLEW_RATE0 = Bit3,  ///< See I2cSlewRate_t
        SPI_SLEW_RATE2 = Bit2,  ///< See SpiSlewRate_t
        SPI_SLEW_RATE1 = Bit1,  ///< See SpiSlewRate_t
        SPI_SLEW_RATE0 = Bit0,  ///< See SpiSlewRate_t
    };
    MODM_FLAGS8(DriveConfig);

    enum class
    SlewRate : uint8_t
    {
        ns20ns60 = 0,
        ns12ns36 = int(DriveConfig::SPI_SLEW_RATE0),
        ns6ns18 = int(DriveConfig::SPI_SLEW_RATE1),
        ns4ns12 = int(DriveConfig::SPI_SLEW_RATE1) | int(DriveConfig::SPI_SLEW_RATE0),
        ns2ns6 = int(DriveConfig::SPI_SLEW_RATE2),
        ns2 = int(DriveConfig::SPI_SLEW_RATE2) | int(DriveConfig::SPI_SLEW_RATE0),
    };
    typedef modm::Configuration<DriveConfig_t, SlewRate, Bit2 | Bit1 | Bit0, 3> I2cSlewRate_t;
    typedef modm::Configuration<DriveConfig_t, SlewRate, Bit2 | Bit1 | Bit0, 0> SpiSlewRate_t;

    // -------------------------------------------------------------------------------

    /// INT_CONFIG reset value is 0x00
    enum class
    IntConfig : uint8_t
    {
        INT2_MODE = Bit5,           ///< INT2 pulsed/latched interrupt mode
        INT2_DRIVE_CIRCUIT = Bit4,  ///< INT2 open drain/push pull drive circuit
        INT2_POLARITY = Bit3,       ///< INT2 active low/high interrupt polarity
        INT1_MODE = Bit2,           ///< INT1 pulsed/latched interrupt mode
        INT1_DRIVE_CIRCUIT = Bit1,  ///< INT1 open drain/push pull drive circuit
        INT1_POLARITY = Bit0,       ///< INT1 active low/high interrupt polarity
    };
    MODM_FLAGS8(IntConfig);

    // -------------------------------------------------------------------------------

    /// FIFO_CONFIG reset value is 0x00
    enum class
    FifoConfig : uint8_t
    {
        FIFO_MODE1 = Bit7,   ///< See FifoMode_t
        FIFO_MODE0 = Bit6,   ///< See FifoMode_t
    };
    MODM_FLAGS8(FifoConfig);

    enum class
    FifoMode : uint8_t
    {
        Bypass = 0,
        StreamToFifo = int(FifoConfig::FIFO_MODE0),
        StopOnFull = int(FifoConfig::FIFO_MODE1),
    };
    typedef modm::Configuration<FifoConfig_t, FifoMode, Bit7 | Bit6> FifoMode_t;

    // -------------------------------------------------------------------------------

    /// INT_STATUS reset value is 0x10
    enum class
    IntStatus : uint8_t
    {
        UI_FSYNC_INT = Bit6,    ///< UI FSYNC Interrupt, clears on read
        PLL_RDY_INT = Bit5,     ///< PLL Ready Interrupt, clears on read
        RESET_DONE_INT = Bit4,  ///< Software Reset Complete Interrupt, clears on read
        DATA_RDY_INT = Bit3,    ///< Data Ready Interrupt, clears on read
        FIFO_THS_INT = Bit2,    ///< FIFO Buffer Threshold Interrupt, clears on read
        FIFO_FULL_INT = Bit1,   ///< FIFO Buffer Full Interrupt, clears on read
        AGC_RDY_INT = Bit0,     ///< AGC Ready Interrupt, clears on read
    };
    MODM_FLAGS8(IntStatus);

    // -------------------------------------------------------------------------------

    /// INT_STATUS2 reset value is 0x00
    enum class
    IntStatus2 : uint8_t
    {
        SMD_INT = Bit3,     ///< Significant Motion Detection Interrupt, clears on read
        WOM_Z_INT = Bit2,   ///< Wake on Motion Interrupt on Z-axis, clears on read
        WOM_Y_INT = Bit1,   ///< Wake on Motion Interrupt on Y-axis, clears on read
        WOM_X_INT = Bit0,   ///< Wake on Motion Interrupt on X-axis, clears on read
    };
    MODM_FLAGS8(IntStatus2);

    // -------------------------------------------------------------------------------

    /// INT_STATUS3 reset value is 0x00
    enum class
    IntStatus3 : uint8_t
    {
        STEP_DET_INT = Bit5,        ///< Step Detection Interrupt, clears on read
        STEP_CNT_OVF_INT = Bit4,    ///< Step Count Overflow Interrupt, clears on read
        TILT_DET_INT = Bit3,        ///< Tilt Detection Interrupt, clears on read
        FF_DET_INT = Bit1,          ///< Freefall Interrupt, clears on read
        TAP_DET_INT = Bit0,         ///< Tap Detection Interrupt, clears on read
    };
    MODM_FLAGS8(IntStatus3);

    // -------------------------------------------------------------------------------

    /// SIGNAL_PATH_RESET reset value is 0x00
    enum class
    SignalPathReset : uint8_t
    {
        DMP_INIT_EN = Bit6,         ///< DMP enable/disable
        DMP_MEM_RESET_EN = Bit5,    ///< DMP memory reset enable/disable
        ABORT_AND_RESET = Bit3,     ///< Signal path reset enable/disable
        TMST_STROBE = Bit2,         ///< Strobe enable/disable
        FIFO_FLUSH = Bit1,          ///< FIFO flush enable/disable
    };
    MODM_FLAGS8(SignalPathReset);

    // -------------------------------------------------------------------------------

    /// INTF_CONFIG0 reset value is 0x30
    enum class
    IntfConfig0 : uint8_t
    {
        FIFO_HOLD_LAST_DATA_EN = Bit7,
        FIFO_COUNT_REC = Bit6,          ///< FIFO count is reported in records enable/disable
        FIFO_COUNT_ENDIAN = Bit5,       ///< FIFO count is reported in Little/Big Endian format
        SENSOR_DATA_ENDIAN = Bit4,      ///< Sensor data is reported in Little/Big Endian
        UI_SIFS_CFG1 = Bit1,            ///< See UiSifsCfg_t
        UI_SIFS_CFG0 = Bit0,            ///< See UiSifsCfg_t
    };
    MODM_FLAGS8(IntfConfig0);

    enum class
    UiSifsCfg : uint8_t
    {
        DisableSpi = int(IntfConfig0::UI_SIFS_CFG1),
        DisableI2c = int(IntfConfig0::UI_SIFS_CFG1) | int(IntfConfig0::UI_SIFS_CFG0),
    };
    typedef modm::Configuration<IntfConfig0_t, UiSifsCfg, Bit1 | Bit0> UiSifsCfg_t;

    // -------------------------------------------------------------------------------

    /// INTF_CONFIG1 reset value is 0x91
    enum class
    IntfConfig1 : uint8_t
    {
        ACCEL_LP_CLK_SEL = Bit3,    ///< Accelerometer LP mode uses Wake Up/RC oscillator clock
        RTC_MODE = Bit2,            ///< Require input RTC clock
        CLK_SEL1 = Bit1,            ///< See ClkSel_t
        CLK_SEL0 = Bit0,            ///< See ClkSel_t
    };
    MODM_FLAGS8(IntfConfig1);

    enum class
    ClkSel : uint8_t
    {
        Rc      = 0,
        Pll     = int(IntfConfig1::CLK_SEL0),
        Disable = int(IntfConfig1::CLK_SEL1) |  int(IntfConfig1::CLK_SEL0),
    };
    typedef modm::Configuration<IntfConfig1_t, ClkSel, Bit1 | Bit0> ClkSel_t;

    // -------------------------------------------------------------------------------

    /// PWR_MGMT0 reset value is 0x00
    enum class
    PwrMgmt0 : uint8_t
    {
        TEMP_DIS = Bit5,    ///< Temperature sensor is enable/disable
        IDLE = Bit4,        ///< Idle mode is enable/disable
        GYRO_MODE1 = Bit3,  ///< See GyroMode_t
        GYRO_MODE0 = Bit2,  ///< See GyroMode_t
        ACCEL_MODE1 = Bit1, ///< See AccelMode_t
        ACCEL_MODE0 = Bit0, ///< See AccelMode_t
    };
    MODM_FLAGS8(PwrMgmt0);

    enum class
    GyroMode : uint8_t
    {
        Off = 0,
        Standby  = int(PwrMgmt0::GYRO_MODE0),
        LowNoise = int(PwrMgmt0::GYRO_MODE1) | int(PwrMgmt0::GYRO_MODE0),
    };
    typedef modm::Configuration<PwrMgmt0_t, GyroMode, Bit3 | Bit2> GyroMode_t;

    enum class
    AccelMode : uint8_t
    {
        Off = 0,
        LowPower = int(PwrMgmt0::ACCEL_MODE1),
        LowNoise = int(PwrMgmt0::ACCEL_MODE1) | int(PwrMgmt0::ACCEL_MODE0),
    };
    typedef modm::Configuration<PwrMgmt0_t, AccelMode, Bit1 | Bit0> AccelMode_t;

    // -------------------------------------------------------------------------------

    /// GYRO_CONFIG0 reset value is 0x06
    enum class
    GyroConfig0 : uint8_t
    {
        GYRO_FS_SEL2 = Bit7,    ///< See GyroFs_t
        GYRO_FS_SEL1 = Bit6,    ///< See GyroFs_t
        GYRO_FS_SEL0 = Bit5,    ///< See GyroFs_t
        GYRO_ODR3 = Bit3,       ///< See GyroOdr_t
        GYRO_ODR2 = Bit2,       ///< See GyroOdr_t
        GYRO_ODR1 = Bit1,       ///< See GyroOdr_t
        GYRO_ODR0 = Bit0,       ///< See GyroOdr_t
    };
    MODM_FLAGS8(GyroConfig0);

    enum class
    GyroFs: uint8_t
    {
        dps2000 = 0b000,
        dps1000 = 0b001,
        dps500 = 0b010,
        dps250 = 0b011,
        dps125 = 0b100,
        dps62_5 = 0b101,
        dps31_25 = 0b110,
        dps15_625 = 0b111,
    };
    typedef modm::Configuration<GyroConfig0_t, GyroFs, Bit2 | Bit1 | Bit0, 5> GyroFs_t;

    enum class
    GyroOdr: uint8_t
    {
        kHz32 = 0b0001,
        kHz16 = 0b0010,
        kHz8 = 0b0011,
        kHz4 = 0b0100,
        kHz2 = 0b0101,
        kHz1 = 0b0110,
        Hz200 = 0b0111,
        Hz100 = 0b1000,
        Hz50 = 0b1001,
        Hz25 = 0b1010,
        Hz12_5 = 0b1011,
        Hz500 = 0b1111,
    };
    typedef modm::Configuration<GyroConfig0_t, GyroOdr, Bit3 | Bit2 | Bit1 | Bit0> GyroOdr_t;

    // -------------------------------------------------------------------------------

    /// ACCEL_CONFIG0 reset value is 0x06
    enum class
    AccelConfig0 : uint8_t
    {
        ACCEL_FS_SEL2 = Bit7,   ///< See AccelFs_t
        ACCEL_FS_SEL1 = Bit6,   ///< See AccelFs_t
        ACCEL_FS_SEL0 = Bit5,   ///< See AccelFs_t
        ACCEL_ODR3 = Bit3,      ///< See AccelOdr_t
        ACCEL_ODR2 = Bit2,      ///< See AccelOdr_t
        ACCEL_ODR1 = Bit1,      ///< See AccelOdr_t
        ACCEL_ODR0 = Bit0,      ///< See AccelOdr_t
    };
    MODM_FLAGS8(AccelConfig0);

    enum class
    AccelFs : uint8_t
    {
        g16 = 0b000,    ///< ±16g
        g8 = 0b001,     ///< ±8g
        g4 = 0b010,     ///< ±4g
        g2 = 0b011,     ///< ±2g
    };
    typedef modm::Configuration<AccelConfig0_t, AccelFs, Bit2 | Bit1 | Bit0, 5> AccelFs_t;

    enum class
    AccelOdr: uint8_t
    {
        kHz32 = 0b0001,     ///< 32 kHz
        kHz16 = 0b0010,     ///< 16 kHz
        kHz8 = 0b0011,      ///< 8 kHz (LN mode)
        kHz4 = 0b0100,      ///< 4 kHz (LN mode)
        kHz2 = 0b0101,      ///< 2 kHz (LN mode)
        kHz1 = 0b0110,      ///< 1 kHz (LN mode) (default)
        Hz200 = 0b0111,     ///< 200 Hz (LN or LP mode)
        Hz100 = 0b1000,     ///< 100 Hz (LN or LP mode)
        Hz50 = 0b1001,      ///< 50 Hz (LN or LP mode)
        Hz25 = 0b1010,      ///< 25 Hz (LN or LP mode)
        Hz12_5 = 0b1011,    ///< 12.5 Hz (LN or LP mode)
        Hz6_25 = 0b1100,    ///< 6.25 Hz (LP mode)
        Hz3_125 = 0b1101,   ///< 3.125 Hz (LP mode)
        Hz1_5625 = 0b1110,  ///< 1.5625 Hz (LP mode)
        Hz500 = 0b1111,     ///< 500 Hz (LN or LP mode)
    };
    typedef modm::Configuration<AccelConfig0_t, AccelOdr, Bit3 | Bit2 | Bit1 | Bit0> AccelOdr_t;

    // -------------------------------------------------------------------------------

    /// GYRO_CONFIG1 reset value is 0x16
    enum class
    GyroConfig1 : uint8_t
    {
        TEMP_FILTER_BW2 = Bit7,     ///< See TempFiltBw_t
        TEMP_FILTER_BW1 = Bit6,     ///< See TempFiltBw_t
        TEMP_FILTER_BW0 = Bit5,     ///< See TempFiltBw_t
        GYRO_UI_FILTER_ORD1 = Bit3, ///< See GyroUiFiltOrd_t
        GYRO_UI_FILTER_ORD0 = Bit2, ///< See GyroUiFiltOrd_t
        GYRO_DEC2_M2_ORD1 = Bit1,   ///< See GyroDec2M2Ord_t
        GYRO_DEC2_M2_ORD0 = Bit0,   ///< See GyroDec2M2Ord_t
    };
    MODM_FLAGS8(GyroConfig1);

    enum class
    TempFiltBw : uint8_t
    {
        Hz4000 = 0b000,
        Hz170 = 0b001,
        Hz82 = 0b010,
        Hz40 = 0b011,
        Hz20 = 0b100,
        Hz10 = 0b101,
        Hz5 = 0b110,
    };
    typedef modm::Configuration<GyroConfig1_t, TempFiltBw, Bit2 | Bit1 | Bit0, 5> TempFiltBw_t;

    enum class
    GyroUiFiltOrd : uint8_t
    {
        First = 0,
        Second = int(GyroConfig1::GYRO_UI_FILTER_ORD0),
        Third = int(GyroConfig1::GYRO_UI_FILTER_ORD1),
    };
    typedef modm::Configuration<GyroConfig1_t, GyroUiFiltOrd, Bit3 | Bit2> GyroUiFiltOrd_t;

    enum class
    GyroDec2M2Ord : uint8_t
    {
        Third = int(GyroConfig1::GYRO_DEC2_M2_ORD1),
    };
    typedef modm::Configuration<GyroConfig1_t, GyroDec2M2Ord, Bit1 | Bit0> GyroDec2M2Ord_t;

    // -------------------------------------------------------------------------------

    /// GYRO_ACCEL_CONFIG0 reset value is 0x11
    enum class
    GyroAccelCongfig0 : uint8_t
    {
        ACCEL_UI_FILTER_BW3 = Bit7, ///< See AccelUiFilterBw_t
        ACCEL_UI_FILTER_BW2 = Bit6, ///< See AccelUiFilterBw_t
        ACCEL_UI_FILTER_BW1 = Bit5, ///< See AccelUiFilterBw_t
        ACCEL_UI_FILTER_BW0 = Bit4, ///< See AccelUiFilterBw_t
        GRYO_UI_FILTER_BW3 = Bit3,  ///< See GyroUiFilterBw_t
        GRYO_UI_FILTER_BW2 = Bit2,  ///< See GyroUiFilterBw_t
        GRYO_UI_FILTER_BW1 = Bit1,  ///< See GyroUiFilterBw_t
        GRYO_UI_FILTER_BW0 = Bit0,  ///< See GyroUiFilterBw_t
    };
    MODM_FLAGS8(GyroAccelCongfig0);

    enum class
    UiFiltBw : uint8_t
    {
        /// Low Noise Mode:
        Div2 = 0,           ///< BW = ODR / 2
        Div4 = 1,           ///< BW = max(400 Hz, ODR) / 2 (default)
        Div5 = 2,           ///< BW = max(400 Hz, ODR) / 5
        Div8 = 3,           ///< BW = max(400 Hz, ODR) / 8
        Div10 = 4,          ///< BW = max(400 Hz, ODR) / 10
        Div16 = 5,          ///< BW = max(400 Hz, ODR) / 16
        Div20 = 6,          ///< BW = max(400 Hz, ODR) / 20
        Div40 = 7,          ///< BW = max(400 Hz, ODR) / 40
        LowLatency1 = 14,   ///< Dec2 runs at max(400 Hz, ODR)
        LowLatency2 = 15,   ///< Dec2 runs at max(200 Hz, 8 * ODR)

        /// Low Power Mode:
        Avg1  = 1,          ///< 1x AVG filter (default)
        Avg16 = 6,          ///< 16x AVG filter
    };
    typedef modm::Configuration<GyroAccelCongfig0_t, UiFiltBw, Bit3 | Bit2 | Bit1 | Bit0, 4> AccelUiFiltBw_t;
    typedef modm::Configuration<GyroAccelCongfig0_t, UiFiltBw, Bit3 | Bit2 | Bit1 | Bit0, 0> GyroUiFiltBw_t;

    // -------------------------------------------------------------------------------

    /// ACCEL_CONFIG1 reset value is 0x0D
    enum class
    AccelConfig1 : uint8_t
    {
        ACCEL_UI_FILTER_ORD1 = Bit4,    ///< See AccelUiFiltOrd_t
        ACCEL_UI_FILTER_ORD0 = Bit3,    ///< See AccelUiFiltOrd_t
        ACCEL_DEC2_M2_ORD1 = Bit2,      ///< See AccelDec2M2Ord_t
        ACCEL_DEC2_M2_ORD0 = Bit1,      ///< See AccelDec2M2Ord_t
    };
    MODM_FLAGS8(AccelConfig1);

    enum class
    AccelUiFiltOrd : uint8_t
    {
        First = 0,
        Second = int(AccelConfig1::ACCEL_UI_FILTER_ORD0),
        Third = int(AccelConfig1::ACCEL_UI_FILTER_ORD1),
    };
    typedef modm::Configuration<AccelConfig1_t, AccelUiFiltOrd, Bit3 | Bit2> AccelUiFiltOrd_t;

    enum class
    AccelDec2M2Ord : uint8_t
    {
        Third = int(AccelConfig1::ACCEL_DEC2_M2_ORD1),
    };
    typedef modm::Configuration<AccelConfig1_t, AccelDec2M2Ord, Bit1 | Bit0> AccelDec2M2Ord_t;

    // -------------------------------------------------------------------------------

    /// TMST_CONFIG reset value is 0x23
    enum class
    TmstConfig : uint8_t
    {
        TMST_TO_REGS_EN = Bit4, ///< Time stamp to register enable/disable
        TMST_RES = Bit3,        ///< Time stamp resolution 1 us/16 us or one RTC clock period
        TMST_DELTA_EN = Bit2,   ///< Time stamp delta enable/disable
        TMST_FSYNC_EN = Bit1,   ///< Time stamp register FSYNC enable/disable
        TMST_EN = Bit0,         ///< Time stamp register enable/disable
    };
    MODM_FLAGS8(TmstConfig);

    // -------------------------------------------------------------------------------

    /// APEX_CONFIG0 reset value is 0x82
    enum class
    ApexConfig0 : uint8_t
    {
        DMP_POWER_SAVE = Bit7,  ///< DMP power save mode enable/disable
        TAP_ENABLE = Bit6,      ///< Tap detection enable/disable
        PED_ENABLE = Bit5,      ///< Pedometer enable/disable
        TILT_ENABLE = Bit4,     ///< Tile detection enable/disable
        FF_ENABLE = Bit2,       ///< Freefall detection enable/disable
        DMP_ODR1 = Bit1,        ///< See DmpOdr_t
        DMP_ODR0 = Bit0,        ///< See DmpOdr_t
    };
    MODM_FLAGS8(ApexConfig0);

    enum class
    DmpOdr : uint8_t
    {
        Hz25 = 0,
        Hz500 = int(ApexConfig0::DMP_ODR0),
        Hz50 = int(ApexConfig0::DMP_ODR1),
        Hz100 = int(ApexConfig0::DMP_ODR1) | int(ApexConfig0::DMP_ODR0),
    };
    typedef modm::Configuration<ApexConfig0_t, DmpOdr, Bit1 | Bit0> DmpOdr_t;

    // -------------------------------------------------------------------------------

    /// SMD_CONFIG reset value is 0x00
    enum class
    SmdConfig : uint8_t
    {
        WOM_INT_MODE = Bit3,    ///< Set WoM interrupt on the OR/AND of all enabled accelerometer thresholds
        WOM_MODE = Bit2,        ///< Compare samples to initial/previous sample
        SMD_MODE1 = Bit1,       ///< See SmdMode_t
        SMD_MODE0 = Bit0,       ///< See SmdMode_t
    };
    MODM_FLAGS8(SmdConfig);

    enum class
    SmdMode : uint8_t
    {
        Disabled = 0,
        Wom = int(SmdConfig::SMD_MODE0),
        Short = int(SmdConfig::SMD_MODE1),
        Long = int(SmdConfig::SMD_MODE1) | int(SmdConfig::SMD_MODE0),
    };
    typedef modm::Configuration<SmdConfig_t, SmdMode, Bit1 | Bit0> SmdMode_t;

    // -------------------------------------------------------------------------------

    /// FIFO_CONFIG1 reset value is 0x00
    enum class
    FifoConfig1 : uint8_t
    {
        FIFO_RESUME_PARTIAL_RD = Bit6,  ///< Partial FIFO read enable/disable
        FIFO_WM_GT_TH = Bit5,           ///< FIFO watermark interrupt enable/disable
        FIFO_HIRES_EN = Bit4,           ///< FIFO extended resolution enable/disable
        FIFO_TMST_FSYNC_EN = Bit3,      ///< FIFO FSYNC enable/disable
        FIFO_TEMP_EN = Bit2,            ///< FIFO temperature data enable/disable
        FIFO_GYRO_EN = Bit1,            ///< FIFO gyro data enable/disable
        FIFO_ACCEL_EN = Bit0,           ///< FIFO acdel data enable/disable
    };
    MODM_FLAGS8(FifoConfig1);

    // -------------------------------------------------------------------------------

    /// FSYNC_CONFIG reset value is 0x10
    enum class
    FsyncConfig : uint8_t
    {
        FSYNC_UI_SEL2 = Bit6,           ///< See FsyncUiSel_t
        FSYNC_UI_SEL1 = Bit5,           ///< See FsyncUiSel_t
        FSYNC_UI_SEL0 = Bit4,           ///< See FsyncUiSel_t
        FSYNC_UI_FLAG_CLEAR_SEL = Bit1, ///< FSYNC flag is cleared when UI sensor register is updated/read lsb
        FSYNC_POLARITY = Bit0,          ///< Start from Rising/Falling edge of FSYNC pulse to measure FSYNC interval
    };
    MODM_FLAGS8(FsyncConfig);

    enum class
    FsyncUiSel : uint8_t
    {
        NoTag = 0,
        TempOut = 0b001,
        GyroXout = 0b010,
        GyroYout = 0b011,
        GyroZout = 0b100,
        AccelXout = 0b101,
        AccelYout = 0b110,
        AccelZout = 0b111,
    };
    typedef modm::Configuration<FsyncConfig_t, FsyncUiSel, Bit2 | Bit1 | Bit0, 4> FsyncUiSel_t;

    // -------------------------------------------------------------------------------

    /// INT_CONFIG0 reset value is 0x00
    enum class
    IntConfig0 : uint8_t
    {
        UI_DRDY_INT_CLEAR1 = Bit5,      ///< See UiDrdyIntClear_t
        UI_DRDY_INT_CLEAR0 = Bit4,      ///< See UiDrdyIntClear_t
        FIFO_THS_INT_CLEAR1 = Bit3,     ///< See FifoThsIntClear_t
        FIFO_THS_INT_CLEAR0 = Bit2,     ///< See FifoThsIntClear_t
        FIFO_FULL_INT_CLEAR1 = Bit1,    ///< See FifoFullIntClear_t
        FIFO_FULL_INT_CLEAR0 = Bit0,    ///< See FifoFullIntClear_t
    };
    MODM_FLAGS8(IntConfig0);

    enum class
    UiDrdyIntClear : uint8_t
    {
        StatusBitRead = 0,
        SensorRegisterRead = int(IntConfig0::UI_DRDY_INT_CLEAR1),
        Both = int(IntConfig0::UI_DRDY_INT_CLEAR1) | int(IntConfig0::UI_DRDY_INT_CLEAR0),
    };
    typedef modm::Configuration<IntConfig0_t, UiDrdyIntClear, Bit5 | Bit4> UiDrdyIntClear_t;

    enum class
    FifoThsIntClear : uint8_t
    {
        StatusBitRead = 0,
        FifoDataRead = int(IntConfig0::FIFO_THS_INT_CLEAR1),
        Both = int(IntConfig0::FIFO_THS_INT_CLEAR1) | int(IntConfig0::FIFO_THS_INT_CLEAR0),
    };
    typedef modm::Configuration<IntConfig0_t, FifoThsIntClear, Bit3 | Bit2> FifoThsIntClear_t;

    enum class
    FifoFullIntClear : uint8_t
    {
        StatusBitRead = 0,
        FifoDataRead = int(IntConfig0::FIFO_FULL_INT_CLEAR1),
        Both = int(IntConfig0::FIFO_FULL_INT_CLEAR1) | int(IntConfig0::FIFO_FULL_INT_CLEAR0),
    };
    typedef modm::Configuration<IntConfig0_t, FifoFullIntClear, Bit3 | Bit2> FifoFullIntClear_t;

    // -------------------------------------------------------------------------------

    /// INT_CONFIG1 reset value is 0x10
    enum class
    IntConfig1 : uint8_t
    {
        INT_TPULSE_DURATION = Bit6,     ///< Interrupt pulse duration is 100 us/8 us
        INT_TDASSERT_DISABLE = Bit5,    ///< De-assert duration enable/disable
        INT_ASYNC_RESET = Bit4,         ///< Change to 0 for proper INT1 and INT2 pin operation
    };
    MODM_FLAGS8(IntConfig1);

    // -------------------------------------------------------------------------------

    /// INT_SOURCE0 reset value is 0x10
    enum class
    IntSource0 : uint8_t
    {
        UI_FSYNC_INT1_EN = Bit6,    ///< UI FSYNC interrupt routed to INT1 enable/disable
        PLL_RDY_INT1_EN = Bit5,     ///< PLL ready interrupt routed to INT1 enable/disable
        REST_DONE_INT1_EN = Bit4,   ///< Reset done interrupt routed to INT1 enable/disable
        UI_DRDY_INT1_EN = Bit3,     ///< UI data ready interrupt routed to INT1 enable/disable
        FIFO_THS_INT1_EN = Bit2,    ///< FIFO threshold interrupt routed to INT1 enable/disable
        FIFO_FULL_INT1_EN = Bit1,   ///< FIFO full interrupt routed to INT1 enable/disable
        UI_AGC_RDY_INT1_EN = Bit0,  ///< UI AGC ready interrupt routed to INT1 enable/disable
    };
    MODM_FLAGS8(IntSource0);

    // -------------------------------------------------------------------------------

    /// INT_SOURCE1 reset value is 0x00
    enum class
    IntSource1 : uint8_t
    {
        I3C_PROTOCOL_ERROR_INT1_EN = Bit6,  ///< I3c protocol error interrupt routed to INT1 enable/disable
        SMD_INT1_EN = Bit3,                 ///< SMD interrupt routed to INT1 enable/disable
        WOM_Z_INT1_EN = Bit2,               ///< Z-axis WOM interrupt routed to INT1 enable/disable
        WOM_Y_INT1_EN = Bit1,               ///< Y-axis WOM interrupt routed to INT1 enable/disable
        WOM_X_INT1_EN = Bit0,               ///< X-axis WOM interrupt routed to INT1 enable/disable
    };
    MODM_FLAGS8(IntSource1);

    // -------------------------------------------------------------------------------

    /// INT_SOURCE3 reset value is 0x00
    enum class
    IntSource3 : uint8_t
    {
        UI_FSYNC_INT2_EN = Bit6,    ///< UI FSYNC interrupt routed to INT2 enable/disable
        PLL_RDY_INT2_EN = Bit5,     ///< PLL ready interrupt routed to INT2 enable/disable
        RESET_DONE_INT2_EN = Bit4,  ///< Reset done interrupt routed to INT2 enable/disable
        UI_DRDY_INT2_EN = Bit3,     ///< UI data ready interrupt routed to INT2 enable/disable
        FIFO_THS_INT2_EN = Bit2,    ///< FIFO threshold interrupt routed to INT2 enable/disable
        FIFO_FULL_INT2_EN = Bit1,   ///< FIFO full interrupt routed to INT2 enable/disable
        UI_AGC_RDY_INT2_EN = Bit0,  ///< UI AGC ready interrupt routed to INT2 enable/disable
    };
    MODM_FLAGS8(IntSource3);

    // -------------------------------------------------------------------------------

    /// INT_SOURCE4 reset value is 0x00
    enum class
    IntSource4 : uint8_t
    {
        I3C_PROTOCOL_ERROR_INT2_EN = Bit6,  ///< I3c protocol error interrupt routed to INT2 enable/disable
        SMD_INT2_EN = Bit3,                 ///< SMD interrupt routed to INT2 enable/disable
        WOM_Z_INT2_EN = Bit2,               ///< Z-axis WOM interrupt routed to INT2 enable/disable
        WOM_Y_INT2_EN = Bit1,               ///< Y-axis WOM interrupt routed to INT2 enable/disable
        WOM_X_INT2_EN = Bit0,               ///< X-axis WOM interrupt routed to INT2 enable/disable
    };
    MODM_FLAGS8(IntSource4);

    // -------------------------------------------------------------------------------

    /// SELF_TEST_CONFIG reset value is 0x00
    enum class
    SelfTestConfig : uint8_t
    {
        ACCEL_ST_POWER = Bit6,  ///< Accel self-test enable/disable
        EN_AZ_ST = Bit5,        ///< Enable Z-accel self-test
        EN_AY_ST = Bit4,        ///< Enable Y-accel self-test
        EN_AX_ST = Bit3,        ///< Enable X-accel self-test
        EN_GZ_ST = Bit2,        ///< Enable Z-gyro self-test
        EN_GY_ST = Bit1,        ///< Enable Y-gyro self-test
        EN_GX_ST = Bit0,        ///< Enable X-gyro self-test
    };
    MODM_FLAGS8(SelfTestConfig);

public:

    using Register_t = FlagsGroup<
            RegBankSel_t,
            DeviceConfig_t,
            DriveConfig_t,
            IntConfig_t,
            FifoConfig_t,
            SignalPathReset_t,
            IntfConfig0_t,
            IntfConfig1_t,
            PwrMgmt0_t,
            GyroConfig0_t,
            AccelConfig0_t,
            GyroConfig1_t,
            GyroAccelCongfig0_t,
            AccelConfig1_t,
            TmstConfig_t,
            ApexConfig0_t,
            SmdConfig_t,
            FifoConfig1_t,
            FsyncConfig_t,
            IntConfig0_t,
            IntConfig1_t,
            IntSource0_t,
            IntSource1_t,
            IntSource3_t,
            IntSource4_t,
            SelfTestConfig_t
        >;

protected:
    /// @cond
    static constexpr uint8_t
    i(Register reg) { return uint8_t(reg); }
    /// @endcond
};

} // namespace modm

#endif // MODM_IXM42XXX_DEFINITIONS_HPP