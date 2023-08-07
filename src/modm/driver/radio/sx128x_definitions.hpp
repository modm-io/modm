/*
 * Copyright (c) 2023, Lasse Alexander Jensen
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SX128X_DEFINITIONS_HPP
#define MODM_SX128X_DEFINITIONS_HPP

#include <stdint.h>
#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/bit_constants.hpp>

namespace modm
{

/// @ingroup modm_driver_sx128x
struct sx128x
{
    enum class
    Opcode : uint8_t
    {
        // Status
        GetStatus = 0xC0,

        // Register Access Operations
        WriteRegister = 0x18,
        ReadRegister = 0x19,

        // Data buffer Operations
        WriteBuffer = 0x1A,
        ReadBuffer = 0x1B,

        // Radio Operation Modes
        SetSleep = 0x84,
        SetStandby = 0x80,
        SetFs = 0xC1,
        SetTx = 0x83,
        SetRx = 0x82,
        SetRxDutyCycle = 0x94,
        SetCad = 0xC5,
        SetTxContinuousWave = 0xD1,
        SetTxContinuousPreamble = 0xD2,

        // Radio Configuration
        SetPacketType = 0x8A,
        GetPacketType = 0x03,
        SetRfFrequency = 0x86,
        SetTxParams = 0x8E,
        SetCadParams = 0x88,
        SetBufferBaseAddress = 0x8F,
        SetModulationParams = 0x8B,
        SetPacketParams = 0x8C,

        // Communication Status Information
        GetRxBufferStatus = 0x17,
        GetPacketStatus = 0x1D,
        GetRssiInst = 0x1F,

        // IRQ Handling
        SetDioIrqParams = 0x8D,
        GetIrqStatus = 0x15,
        ClrIrqStatus = 0x97,

        // Settings functions
        SetRegulatorMode = 0x96,
        SetSaveContext = 0xD5,
        SetAutoFs = 0x9E,
        SetAutoTx = 0x98,
        SetLongPreamble = 0x9B,
        SetUartSpeed = 0x9D,
        SetRangingRole = 0xA3,
        SetAdvancedRanging = 0x9A,
    };

    enum class
    Register : uint16_t
    {
        RxGain = 0x891,
        ManualGainSetting = 0x895,
        LnaGainValue = 0x89E,
        LnaGainControl = 0x89F,
        SynchPeakAttenuation = 0x8C2,
        PayloadLength = 0x901,
        LoRaHeaderMode = 0x903,

        RangingRequestAddressByte3 = 0x912,
        RangingRequestAddressByte2 = 0x913,
        RangingRequestAddressByte1 = 0x914,
        RangingRequestAddressByte0 = 0x915,

        RangingDeviceAdressByte3 = 0x916,
        RangingDeviceAdressByte2 = 0x917,
        RangingDeviceAdressByte1 = 0x918,
        RangingDeviceAdressByte0 = 0x919,

        RangingFilterWindowSize = 0x91E,
        ResetRangingFilter = 0x923,
        RangingResultMux = 0x924,

        SfAdditionalConfiguration = 0x925,

        RangingCalibrationByte2 = 0x92B,
        RangingCalibrationByte1 = 0x92C,
        RangingCalibrationByte0 = 0x92D,

        RangingIdCheckLength = 0x931,
        FrequencyErrorCorrection = 0x93C,

        LoRaSynchWordByte1 = 0x944,
        LoRaSynchWordByte0 = 0x945,

        FeiByte2 = 0x954,
        FeiByte1 = 0x955,
        FeiByte0 = 0x956,

        RangingResultByte2 = 0x961,
        RangingResultByte1 = 0x962,
        RangingResultByte0 = 0x963,

        RangingRssi = 0x964,
        FreezeRangingResult = 0x97F,
        PacketPreambleSettings = 0x9C1,
        WhiteningInitialValue = 0x9C5,

        CrcPolynomialDefMsb = 0x9C6,
        CrcPolynomialDefLsb = 0x9C7,

        CrcPolynomialSeedByte2 = 0x9C7,
        CrcPolynomialSeedByte1 = 0x9C8,
        CrcPolynomialSeedByte0 = 0x9C9,

        CrcMsbInitialValue = 0x9C8,
        CrcLsbInitialValue = 0x9C9,

        SynchAddressControl = 0x9CD,

        SyncAddress1Byte4 = 0x9CE,
        SyncAddress1Byte3 = 0x9CF,
        SyncAddress1Byte2 = 0x9D0,
        SyncAddress1Byte1 = 0x9D1,
        SyncAddress1Byte0 = 0x9D2,

        SyncAddress2Byte4 = 0x9D3,
        SyncAddress2Byte3 = 0x9D4,
        SyncAddress2Byte2 = 0x9D5,
        SyncAddress2Byte1 = 0x9D6,
        SyncAddress2Byte0 = 0x9D7,

        SyncAddress3Byte4 = 0x9D8,
        SyncAddress3Byte3 = 0x9D9,
        SyncAddress3Byte2 = 0x9DA,
        SyncAddress3Byte1 = 0x9DB,
        SyncAddress3Byte0 = 0x9DC
    };

public:

    struct modm_packed
    Status
    {
        enum class
        CircuitMode : uint8_t
        {
            StdbyRc = 0x2,
            StdbyXosc = 0x3,
            Fs = 0x4,
            Rx = 0x5,
            Tx = 0x6,
        };

        enum class
        CommandStatus : uint8_t
        {
            Processed = 0x1,        /// Transceiver has successfully processed the command
            DataAvailable = 0x2,    /// Data are available to host
            Timeout = 0x3,          /// Command time-out
            ProcessingError = 0x4,  /// Command processing error
            ExecutionFailure = 0x5, /// Failure to execute command
            TxDone = 0x6,           /// Command Tx done
        };

        uint8_t: 2;
        CommandStatus commandStatus: 3;
        CircuitMode circuitMode: 3;
    };

    enum class
    SleepConfig : uint8_t
    {
        DataRamFlushed = Bit0,
        DataRamRetention = Bit1,
        DataBufferRetention = Bit2,
    };
    MODM_FLAGS8(SleepConfig);

    enum class
    StandbyConfig : uint8_t
    {
        StdbyRc = 0,    /// Device running on RC 13MHz, set STDBY_RC mode
        StdbyXosc = 1,  /// Device running on XTAL 52MHz, set STDBY_XOSC mode
    };

    enum class
    PeriodBase : uint8_t
    {
        us15_625 = 0x00,
        us62_5 = 0x01,
        ms1 = 0x02,
        ms4 = 0x03
    };

    enum class
    PacketType : uint8_t
    {
        Gfsk = 0x00,
        LoRa = 0x01,
        Ranging = 0x02,
        Flrc = 0x03,
        Ble = 0x04,
    };

    enum class
    RampTime : uint8_t
    {
        us2 = 0x00,
        us4 = 0x20,
        us6 = 0x40,
        us8 = 0x60,
        us10 = 0x80,
        us12 = 0xA0,
        us16 = 0xC0,
        us20 = 0xE0
    };

    enum class
    CadSymbolNumber : uint8_t
    {
        One = 0x00,
        Two = 0x20,
        Four = 0x40,
        Eight = 0x60,
        Sixteen = 0x80
    };

    enum class
    RegulatorMode : uint8_t
    {
        Ldo  = 0x00,
        DcDc = 0x01,
    };

    enum class
    Irq : uint16_t
    {
        TxDone = Bit0,
        RxDone = Bit1,
        SyncWordValid = Bit2,
        SyncWordError = Bit3,
        HeaderValid = Bit4,
        HeaderError = Bit5,
        CrcError = Bit6,
        RangingSlaveResponseDone = Bit7,
        RangingSlaveRequestDiscard = Bit8,
        RangingMasterResultValid = Bit9,
        RangingMasterTimeout = Bit10,
        RangingMasterRequestTimeout = Bit11,
        CadDone = Bit12,
        CadDetected = Bit13,
        RxTxTimeout = Bit14,
        PreambleDetected = Bit15,
    };
    MODM_FLAGS16(Irq);

    struct RxBufferStatus
    {
        uint8_t rxPayloadLength;
        uint8_t rxStartBufferPointer;
    };

    enum class
    UartDividerRatio : uint16_t
    {
        Bd2400      = 0x00C2,
        Bd4800      = 0x0183,
        Bd9600      = 0x0306,
        Bd14400     = 0x0489,
        Bd19200     = 0x060D,
        Bd38400     = 0x0C19,
        Bd57600     = 0x1226,
        Bd115200    = 0x244C,
        Bd460600    = 0x9120,
        Bd812490    = 0xFFFF,
    };

    enum class
    RangingRole : uint8_t
    {
        Master = 0x01,
        Slave = 0x00,
    };

    struct Gfsk
    {
        struct modm_packed
        PacketStatus
        {
            enum class
            Status : uint8_t
            {
                RxNoAck = Bit5,
                PktSent = Bit0,
            };
            MODM_FLAGS8(Status);

            enum class
            Error : uint8_t
            {
                SyncError = Bit6,
                LengthError = Bit5,
                CrcError = Bit4,
                AbortError = Bit3,
                HeaderReceived = Bit2,
                PacketCtrlBusy = Bit1,
            };
            MODM_FLAGS8(Error);

            enum class
            Sync : uint8_t
            {
                SyncAddrsCode2 = Bit2,
                SyncAddrsCode1 = Bit1,
                SyncAddrsCode0 = Bit0,
            };
            MODM_FLAGS8(Sync);

            uint8_t rfu;
            uint8_t rssiSync;
            Error_t error;
            Status_t status;
            Sync_t sync;
        };

        struct modm_packed
        PacketParams
        {
            enum class
            PreambleLength : uint8_t
            {
                Bits4  = 0x00,
                Bits8  = 0x10,
                Bits12 = 0x20,
                Bits16 = 0x30,
                Bits20 = 0x40,
                Bits24 = 0x50,
                Bits28 = 0x60,
                Bits32 = 0x70,
            };

            enum class
            SyncWordLength : uint8_t
            {
                Byte1 = 0x00,
                Byte2 = 0x02,
                Byte3 = 0x04,
                Byte4 = 0x06,
                Byte5 = 0x08,
            };

            enum class
            SyncWordMatch : uint8_t
            {
                Disable = 0x00,
                One = 0x10,
                Two = 0x20,
                OneTwo = 0x30,
                Three = 0x40,
                OneThree = 0x50,
                TwoThree = 0x60,
                OneTwoThree = 0x70,
            };

            enum class
            HeaderType : uint8_t
            {
                FixedLength = 0x00,
                VariableLength = 0x20,
            };

            enum class
            CrcLength : uint8_t
            {
                Off = 0x00,
                Byte2 = 0x10,
                Byte3 = 0x20,
                Byte4 = 0x30,
            };

            enum class
            Whitening : uint8_t
            {
                Enable = 0x00,
                Disable = 0x08,
            };

            PreambleLength preambleLength;
            SyncWordLength syncWordLength;
            SyncWordMatch syncWordMatch;
            HeaderType headerType;
            uint8_t payloadLength;
            CrcLength crcLength;
            Whitening whitening;
        };

        struct modm_packed
        ModulationParams
        {
            enum class
            BitrateBandwidth : uint8_t
            {
                Br2000Bw2400 = 0x04,
                Br1600Bw2400 = 0x28,
                Br1000Bw2400 = 0x4C,
                Br1000Bw1200 = 0x45,
                Br800Bw2400 = 0x70,
                Br800Bw1200 = 0x69,
                Br500Bw1200 = 0x8D,
                Br500Bw600 = 0x86,
                Br400Bw1200 = 0xB1,
                Br400Bw600 = 0xAA,
                Br250Bw600 = 0xCE,
                Br250Bw300 = 0xC7,
                Br124Bw300 = 0xEF,
            };

            enum class
            ModulationIndex : uint8_t
            {
                Ind0_35 = 0x00,
                Ind0_50 = 0x01,
                Ind0_75 = 0x02,
                Ind1_00 = 0x03,
                Ind1_25 = 0x04,
                Ind1_50 = 0x05,
                Ind1_75 = 0x06,
                Ind2_00 = 0x07,
                Ind2_25 = 0x08,
                Ind2_50 = 0x09,
                Ind2_75 = 0x0A,
                Ind3_00 = 0x0B,
                Ind3_25 = 0x0C,
                Ind3_50 = 0x0D,
                Ind3_75 = 0x0E,
                Ind4_00 = 0x0F,
            };

            enum class
            ModulationShaping : uint8_t
            {
                BtOff = 0x00,
                Bt1_0 = 0x10,
                Bt0_5 = 0x20,
            };

            BitrateBandwidth bitrateBandwidth;
            ModulationIndex modulationIndex;
            ModulationShaping modulationShaping;
        };
    };

    struct Ble
    {
        struct modm_packed
        PacketStatus
        {
            enum class
            Status : uint8_t
            {
                RxNoAck = Bit5,
                PktSent = Bit0,
            };
            MODM_FLAGS8(Status);

            enum class
            Error : uint8_t
            {
                SyncError = Bit6,
                LengthError = Bit5,
                CrcError = Bit4,
                AbortError = Bit3,
                HeaderReceived = Bit2,
                PacketCtrlBusy = Bit1,
            };
            MODM_FLAGS8(Error);

            enum class
            Sync : uint8_t
            {
                SyncAddrsCode2 = Bit2,
                SyncAddrsCode1 = Bit1,
                SyncAddrsCode0 = Bit0,
            };
            MODM_FLAGS8(Sync);

            uint8_t rfu;
            uint8_t rssiSync;
            Error_t error;
            Status_t status;
            Sync_t sync;
        };

        struct modm_packed
        PacketParams
        {
            enum class
            ConnectionState : uint8_t
            {
                PayloadLengthMax31Bytes = 0x00,
                PayloadLengthMax37Bytes = 0x20,
                TxTestMode = 0x40,
                PayloadLengthMax355Bytes = 0x80,
            };

            enum class
            CrcLength : uint8_t
            {
                Off = 0x00,
                Byte3 = 0x10,
            };

            enum class
            TestPayload : uint8_t
            {
                Prbs9 = 0x00,
                Eyelong10 = 0x04,
                Eyeshort10 = 0x08,
                Prbs15 = 0x0C,
                All1 = 0x10,
                All0 = 0x14,
                Eyelong01 = 0x18,
                Eyeshort01 = 0x1c,
            };

            enum class
            Whitening : uint8_t
            {
                Enable = 0x00,
                Disable = 0x08,
            };

            ConnectionState connectionState;
            CrcLength crcLength;
            TestPayload testPayload;
            Whitening whitening;
        };

        struct modm_packed
        ModulationParams
        {
            enum class
            BitrateBandwidth : uint8_t
            {
                Br2000Bw2400 = 0x04,
                Br1600Bw2400 = 0x28,
                Br1000Bw2400 = 0x4C,
                Br1000Bw1200 = 0x45,
                Br800Bw2400 = 0x70,
                Br800Bw1200 = 0x69,
                Br500Bw1200 = 0x8D,
                Br500Bw600 = 0x86,
                Br400Bw1200 = 0xB1,
                Br400Bw600 = 0xAA,
                Br250Bw600 = 0xCE,
                Br250Bw300 = 0xC7,
                Br124Bw300 = 0xEF,
            };

            enum class
            ModulationIndex : uint8_t
            {
                Ind0_35 = 0x00,
                Ind0_50 = 0x01,
                Ind0_75 = 0x02,
                Ind1_00 = 0x03,
                Ind1_25 = 0x04,
                Ind1_50 = 0x05,
                Ind1_75 = 0x06,
                Ind2_00 = 0x07,
                Ind2_25 = 0x08,
                Ind2_50 = 0x09,
                Ind2_75 = 0x0A,
                Ind3_00 = 0x0B,
                Ind3_25 = 0x0C,
                Ind3_50 = 0x0D,
                Ind3_75 = 0x0E,
                Ind4_00 = 0x0F,
            };

            enum class
            ModulationShaping : uint8_t
            {
                BtOff = 0x00,
                Bt1_0 = 0x10,
                Bt0_5 = 0x20,
            };

            BitrateBandwidth bitrateBandwidth;
            ModulationIndex modulationIndex;
            ModulationShaping modulationShaping;
        };
    };

    struct Flrc
    {
        struct modm_packed
        PacketStatus
        {
            enum class
            Status : uint8_t
            {
                RxNoAck = Bit5,
                PktSent = Bit0,
            };
            MODM_FLAGS8(Status);

            enum class
            Error : uint8_t
            {
                SyncError = Bit6,
                LengthError = Bit5,
                CrcError = Bit4,
                AbortError = Bit3,
                HeaderReceived = Bit2,
                PacketCtrlBusy = Bit1,
            };
            MODM_FLAGS8(Error);

            enum class
            Sync : uint8_t
            {
                SyncAddrsCode2 = Bit2,
                SyncAddrsCode1 = Bit1,
                SyncAddrsCode0 = Bit0,
            };
            MODM_FLAGS8(Sync);

            uint8_t rfu;
            uint8_t rssiSync;
            Error_t error;
            Status_t status;
            Sync_t sync;
        };

        struct modm_packed
        PacketParams
        {
            enum class
            PreambleLength : uint8_t
            {
                Bits4  = 0x00,
                Bits8  = 0x10,
                Bits12 = 0x20,
                Bits16 = 0x30,
                Bits20 = 0x40,
                Bits24 = 0x50,
                Bits28 = 0x60,
                Bits32 = 0x70,
            };

            enum class
            SyncWordLength : uint8_t
            {
                NoSync = 0x00,
                Bits32 = 0x02,
            };

            enum class
            SyncWordMatch : uint8_t
            {
                Disable = 0x00,
                One = 0x10,
                Two = 0x20,
                OneTwo = 0x30,
                Three = 0x40,
                OneThree = 0x50,
                TwoThree = 0x60,
                OneTwoThree = 0x70,
            };

            enum class
            PacketType : uint8_t
            {
                FixedLength = 0x00,
                VariableLength = 0x20,
            };

            enum class
            CrcLength : uint8_t
            {
                Off = 0x00,
                Byte1 = 0x10,
                Byte2 = 0x20,
            };

            enum class
            Whitening : uint8_t
            {
                Disable = 0x08,
            };

            PreambleLength preambleLength;
            SyncWordLength syncWordLength;
            SyncWordMatch syncWordMatch;
            PacketType packetType;
            uint8_t payloadLength;
            CrcLength crcLength;
            Whitening whitening;
        };

        struct modm_packed
        ModulationParams
        {
            enum class
            BitrateBandwidth : uint8_t
            {
                Br1300Bw1200 = 0x45,
                Br1000Bw1200 = 0x69,
                Br650Bw600 = 0x86,
                Br520Bw600 = 0xAA,
                Br325Bw300 = 0xC7,
                Br260Bw300 = 0xEB,
            };

            enum class
            CodingRate : uint8_t
            {
                Cr1_2 = 0x00,
                Cr3_4 = 0x02,
                Cr1_1 = 0x04,
            };

            enum class
            ModulationShaping : uint8_t
            {
                BtDis = 0x00,
                Bt1_0 = 0x10,
                Bt0_5 = 0x20,
            };

            BitrateBandwidth bitrateBandwidth;
            CodingRate codingRate;
            ModulationShaping modulationShaping;
        };
    };

    struct LoRa
    {
        struct modm_packed
        PacketStatus
        {
            uint8_t rssiSync;
            uint8_t snr;
        };

        struct modm_packed
        PacketParams
        {
            enum class
            HeaderType : uint8_t
            {
                Explicit = 0x00,
                Implicit = 0x80,
            };

            enum class
            Crc : uint8_t
            {
                Enable = 0x20,
                Disable = 0x00,
            };

            enum class
            InvertIq : uint8_t
            {
                Inverted = 0x00,
                Standard = 0x40,
            };

            uint8_t preambleLength;
            HeaderType headerType;
            uint8_t payloadLength;
            Crc crc;
            InvertIq invertIq;
        };

        struct modm_packed
        ModulationParams
        {
            enum class
            SpreadingFactor : uint8_t
            {
                Sf5 = 0x50,
                Sf6 = 0x60,
                Sf7 = 0x70,
                Sf8 = 0x80,
                Sf9 = 0x90,
                Sf10 = 0xA0,
                Sf11 = 0xB0,
                Sf12 = 0xC0,
            };

            enum class
            Bandwidth : uint8_t
            {
                Bw1600 = 0x0A,
                Bw800 = 0x18,
                Bw400 = 0x26,
                Bw200 = 0x34,
            };

            enum class
            CodingRate : uint8_t
            {
                Cr4_5 = 0x01,
                Cr4_6 = 0x02,
                Cr4_7 = 0x03,
                Cr4_8 = 0x04,
                Cr_Li_4_5 = 0x05,
                Cr_Li_4_6 = 0x06,
                Cr_Li_4_7 = 0x07,
            };

            SpreadingFactor spreadingFactor;
            Bandwidth bandwidth;
            CodingRate codingRate;
        };
    };

    struct Ranging
    {
        struct modm_packed
        PacketStatus
        {
            uint8_t rssiSync;
            uint8_t sni;
        };

        struct modm_packed
        PacketParams
        {
            enum class
            HeaderType : uint8_t
            {
                Explicit = 0x00,
                Implicit = 0x80,
            };

            enum class
            Crc : uint8_t
            {
                Enable = 0x20,
                Disable = 0x00,
            };

            enum class
            InvertIq : uint8_t
            {
                Inverted = 0x00,
                Standard = 0x40,
            };

            uint8_t preambleLength;
            HeaderType headerType;
            uint8_t payloadLength;
            Crc crc;
            InvertIq invertIq;
        };

        struct modm_packed
        ModulationParams
        {
            enum class
            SpreadingFactor : uint8_t
            {
                Sf5 = 0x50,
                Sf6 = 0x60,
                Sf7 = 0x70,
                Sf8 = 0x80,
                Sf9 = 0x90,
                Sf10 = 0xA0,
            };

            enum class
            Bandwidth : uint8_t
            {
                Bw1600 = 0x0A,
                Bw800 = 0x18,
                Bw400 = 0x26,
            };

            enum class
            CodingRate : uint8_t
            {
                Cr4_5 = 0x01,
                Cr4_6 = 0x02,
                Cr4_7 = 0x03,
                Cr4_8 = 0x04,
                Cr_Li_4_5 = 0x05,
                Cr_Li_4_6 = 0x06,
                Cr_Li_4_7 = 0x07,
            };

            SpreadingFactor spreadingFactor;
            Bandwidth bandwidth;
            CodingRate codingRate;
        };
    };

    union PacketParams
    {
        Gfsk::PacketParams gfsk;
        Flrc::PacketParams flrc;
        Ble::PacketParams ble;
        LoRa::PacketParams lora;
        Ranging::PacketParams ranging;

        PacketParams() {}
        PacketParams(Gfsk::PacketParams packetParams) : gfsk(packetParams) {}
        PacketParams(Flrc::PacketParams packetParams) : flrc(packetParams) {}
        PacketParams(Ble::PacketParams packetParams) : ble(packetParams) {}
        PacketParams(LoRa::PacketParams packetParams) : lora(packetParams) {}
        PacketParams(Ranging::PacketParams packetParams) : ranging(packetParams) {}
    };

    union PacketStatus
    {
        Gfsk::PacketStatus gfsk;
        Flrc::PacketStatus flrc;
        Ble::PacketStatus ble;
        LoRa::PacketStatus lora;
        Ranging::PacketStatus ranging;

        PacketStatus() {}
        PacketStatus(Gfsk::PacketStatus packetStatus) : gfsk(packetStatus) {}
        PacketStatus(Flrc::PacketStatus packetStatus) : flrc(packetStatus) {}
        PacketStatus(Ble::PacketStatus packetStatus) : ble(packetStatus) {}
        PacketStatus(LoRa::PacketStatus packetStatus) : lora(packetStatus) {}
        PacketStatus(Ranging::PacketStatus packetStatus) : ranging(packetStatus) {}
    };

    union ModulationParams
    {
        Gfsk::ModulationParams gfsk;
        Flrc::ModulationParams flrc;
        Ble::ModulationParams ble;
        LoRa::ModulationParams lora;
        Ranging::ModulationParams ranging;

        ModulationParams() {}
        ModulationParams(Gfsk::ModulationParams modulationParams) : gfsk(modulationParams) {}
        ModulationParams(Flrc::ModulationParams modulationParams) : flrc(modulationParams) {}
        ModulationParams(Ble::ModulationParams modulationParams) : ble(modulationParams) {}
        ModulationParams(LoRa::ModulationParams modulationParams) : lora(modulationParams) {}
        ModulationParams(Ranging::ModulationParams modulationParams) : ranging(modulationParams) {}
    };

protected:
    /// @cond
    static constexpr uint16_t
    i(Register reg) { return uint16_t(reg); }
    static constexpr uint8_t
    i(Opcode opcode) { return uint8_t(opcode); }
    static constexpr uint8_t
    i(StandbyConfig standbyConfig) { return uint8_t(standbyConfig); }
    static constexpr uint8_t
    i(PeriodBase periodBase) { return uint8_t(periodBase); }
    static constexpr uint8_t
    i(PacketType packetType) { return uint8_t(packetType); }
    static constexpr uint8_t
    i(RampTime rampTime) { return uint8_t(rampTime); }
    static constexpr uint8_t
    i(CadSymbolNumber cadSymbolNumber) { return uint8_t(cadSymbolNumber); }
    static constexpr uint8_t
    i(RegulatorMode regulatorMode) { return uint8_t(regulatorMode); }
    static constexpr uint16_t
    i(UartDividerRatio uartDividerRatio) { return uint16_t(uartDividerRatio); }
    static constexpr uint8_t
    i(RangingRole rangingRole) { return uint8_t(rangingRole); }
    /// @endcond
};

}



#endif