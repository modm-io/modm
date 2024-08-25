/*
 * Copyright (c) 2024, Elias H.
 * Copyright (c) 2024, Michael Jossen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DW3110_DEFINITIONS_HPP
#define MODM_DW3110_DEFINITIONS_HPP

#include <stdint.h>

#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/bit_constants.hpp>

namespace modm
{

/// @ingroup modm_driver_dw3110
struct Dw3110
{
	enum class FastCommand : uint8_t
	{
		CMD_TXRXOFF = (0x0 << 1) | 1,  //< Puts the device into IDLE state and clears any events.
		CMD_TX = (0x1 << 1) | 1,       //< Immediate start of transmission
		CMD_RX = (0x2 << 1) | 1,       //< Enable RX immediately
		CMD_DTX = (0x3 << 1) | 1,      //< Delayed TX w.r.t. DX_TIME
		CMD_DRX = (0x4 << 1) | 1,      //< Delayed RX w.r.t. DX_TIME
		CMD_DTX_TS = (0x5 << 1) | 1,   //< Delayed TX w.r.t. TX timestamp + DX_TIME
		CMD_DRX_TS = (0x6 << 1) | 1,   //< Delayed RX w.r.t. TX timestamp + DX_TIME
		CMD_DTX_RS = (0x7 << 1) | 1,   //< Delayed TX w.r.t. RX timestamp + DX_TIME
		CMD_DRX_RS = (0x8 << 1) | 1,   //< Delayed RX w.r.t. RX timestamp + DX_TIME
		CMD_DTX_REF = (0x9 << 1) | 1,  //< Delayed TX w.r.t. DREF_TIME + DX_TIME
		CMD_DRX_REF = (0xA << 1) | 1,  //< Delayed RX w.r.t. DREF_TIME + DX_TIME
		CMD_CCA_TX = (0xB << 1) | 1,   //< TX if no preamble detected
		CMD_TX_W4R =
			(0xC << 1) | 1,  //< Start TX immediately, then when TX is done, enable the receiver
		CMD_DTX_W4R = (0xD << 1) | 1,  //< Delayed TX w.r.t. DX_TIME, then enable receiver
		CMD_DTX_TS_W4R =
			(0xE << 1) | 1,  //< Delayed TX w.r.t. TX timestamp + DX_TIME, then enable receiver
		CMD_DTX_RS_W4R =
			(0xF << 1) | 1,  //< Delayed TX w.r.t. RX timestamp + DX_TIME, then enable receiver
		CMD_DTX_REF_W4R =
			(0x10 << 1) | 1,  //< Delayed TX w.r.t. DREF_TIME + DX_TIME, then enable receiver
		CMD_CCA_TX_W4R =
			(0x11 << 1) | 1,             //< TX packet if no preamble detected, then enable receiver
		CMD_CLR_IRQS = (0x12 << 1) | 1,  //< Clear all interrupt events
		CMD_DB_TOGGLE =
			(0x13 << 1) | 1,  //< Toggle double buffer pointer / notify the device that the host
							  // has finished processing the received buffer/data.
	};

	enum class SpiMode : uint8_t
	{
		FULL_READ = 0b01 << 6,
		FULL_WRITE = 0b11 << 6,
		SHORT_READ = 0b00 << 6,
		SHORT_WRITE = 0b10 << 6,
	};

	enum class Channel : uint8_t
	{
		Channel5,
		Channel9
	};

	struct OTPAddr
	{
		std::array<uint8_t, 2> or_mask;
		std::array<uint8_t, 2> and_mask;
	};

	enum class SystemState : uint8_t
	{
		OFF,
		WAKEUP,
		INIT_RC,
		IDLE_RC,
		IDLE_PLL,
		TX_WAIT,
		TX,
		RX_WAIT,
		RX,
		SLEEP,
		DEEPSLEEP,
		INVALID
	};

	enum class PreambleLength : uint8_t
	{
		Preamble_32 = 4,
		Preamble_64 = 1,
		Preamble_128 = 5,
		Preamble_256 = 9,
		Preamble_512 = 13,
		Preamble_1024 = 2,
		Preamble_1536 = 6,
		Preamble_2048 = 10,
		Preamble_4096 = 3,

	};

	enum class PreambleCode : uint8_t
	{
		Code_16Mhz_3 = 3,
		Code_16Mhz_4 = 4,
		Code_64Mhz_9 = 9,
		Code_64Mhz_10 = 10,
		Code_64Mhz_11 = 11,
		Code_64Mhz_12 = 12,
	};

	enum class StartFrameDelimiter : uint8_t
	{
		IEEE802_15_4_8 = 0,
		Decawave_8 = 1,
		Decawave_16 = 2,
		IEEE802_15_4z_8 = 3,
	};

	enum class SystemStatus : uint64_t
	{
		IRQS = (1ll << 0),
		CPLOCK = (1ll << 1),
		SPICRCE = (1ll << 2),
		AAT = (1ll << 3),
		TXFRB = (1ll << 4),
		TXPRS = (1ll << 5),
		TXPHS = (1ll << 6),
		TXFRS = (1ll << 7),
		RXPRD = (1ll << 8),
		RXSFDD = (1ll << 9),
		CIADONE = (1ll << 10),
		RXPHD = (1ll << 11),
		RXPHE = (1ll << 12),
		RXFR = (1ll << 13),
		RXFCG = (1ll << 14),
		RXFCE = (1ll << 15),
		RXFSL = (1ll << 16),
		RXFTO = (1ll << 17),
		CIAERR = (1ll << 18),
		VWARN = (1ll << 19),
		RXOVRR = (1ll << 20),
		RXPTO = (1ll << 21),
		SPIRDY = (1ll << 23),
		RCINIT = (1ll << 24),
		PLL_HILO = (1ll << 25),
		RXSTO = (1ll << 26),
		HPDWARN = (1ll << 27),
		CPERR = (1ll << 28),
		ARFE = (1ll << 29),
		RXPREJ = (1ll << 33),
		VT_DET = (1ll << 36),
		GPIOIRQ = (1ll << 37),
		AES_DONE = (1ll << 38),
		AES_ERR = (1ll << 39),
		CMD_ERR = (1ll << 40),
		SPI_OVF = (1ll << 41),
		SPI_UNF = (1ll << 42),
		SPIERR = (1ll << 43),
		CCA_FAIL = (1ll << 44),
	};
	typedef ::modm::Flags<SystemStatus, uint64_t> SystemStatus_t;
	MODM_INT_TYPE_FLAGS(SystemStatus_t);

	enum class FilterConfig : uint16_t
	{
		AllowBeacon = (1u << 0),
		AllowData = (1u << 1),
		AllowACK = (1u << 2),
		AllowMAC = (1u << 3),
		AllowReserved = (1u << 4),
		AllowMultiPurpose = (1u << 5),
		AllowFragmented = (1u << 6),
		AllowExtended = (1u << 7),
		BehaveAsPANCoordinator = (1u << 8),
		AllowImplicitBroadcast = (1u << 9),
		LE0HasPending = (1u << 10),
		LE1HasPending = (1u << 11),
		LE2HasPending = (1u << 12),
		LE3HasPending = (1u << 13),
		EnablePendingShort = (1u << 14),
		EnablePendingLong = (1u << 15),
	};
	MODM_FLAGS16(FilterConfig);

	static constexpr OTPAddr XTAL_TRIM{.or_mask = {0x13, 0x00}, .and_mask = {0x13, 0xFC}};
	static constexpr OTPAddr BIASTUNE_CAL{.or_mask = {0x0A, 0x00}, .and_mask = {0x0A, 0xFC}};
	static constexpr OTPAddr LDOTUNE_CAL_1{.or_mask = {0x04, 0x00}, .and_mask = {0x04, 0xFC}};
	static constexpr OTPAddr LDOTUNE_CAL_2{.or_mask = {0x05, 0x00}, .and_mask = {0x05, 0xFC}};

	struct RegisterBank
	{
		uint8_t addr;
	};

	static constexpr RegisterBank GEN_CFG_AES_0{.addr = 0x0};
	static constexpr RegisterBank GEN_CFG_AES_1{.addr = 0x1};
	static constexpr RegisterBank STS_CONFIG{.addr = 0x2};
	static constexpr RegisterBank RX_TUNE{.addr = 0x3};
	static constexpr RegisterBank EXT_SYNC{.addr = 0x4};
	static constexpr RegisterBank GPIO_CTRL{.addr = 0x5};
	static constexpr RegisterBank DRX{.addr = 0x6};
	static constexpr RegisterBank RF_CONF{.addr = 0x7};
	static constexpr RegisterBank RF_CAL{.addr = 0x8};
	static constexpr RegisterBank FS_CTRL{.addr = 0x9};
	static constexpr RegisterBank AON{.addr = 0xA};
	static constexpr RegisterBank OTP_IF{.addr = 0xB};
	static constexpr RegisterBank CIA_0{.addr = 0xC};
	static constexpr RegisterBank CIA_1{.addr = 0xD};
	static constexpr RegisterBank CIA_2{.addr = 0xE};
	static constexpr RegisterBank DIG_DIAG{.addr = 0xF};
	static constexpr RegisterBank PMSC_CTRL{.addr = 0x11};
	static constexpr RegisterBank RX_BUFFER_0_BANK{.addr = 0x12};
	static constexpr RegisterBank RX_BUFFER_1_BANK{.addr = 0x13};
	static constexpr RegisterBank TX_BUFFER_BANK{.addr = 0x14};
	static constexpr RegisterBank ACC_MEM_BANK{.addr = 0x15};
	static constexpr RegisterBank SCRATCH_RAM_BANK{.addr = 0x16};
	static constexpr RegisterBank AES_RAM{.addr = 0x17};
	static constexpr RegisterBank DB_DIAG{.addr = 0x18};
	static constexpr RegisterBank INDIRECT_PTR_A{.addr = 0x1D};
	static constexpr RegisterBank INDIRECT_PTR_B{.addr = 0x1E};
	static constexpr RegisterBank IN_PTR_CFG{.addr = 0x1F};

	struct Register
	{
		RegisterBank bank;
		uint8_t offset;
		uint16_t length;
	};

	// Subregisters for GEN_CFG_AES
	static constexpr Register DEV_ID{.bank = GEN_CFG_AES_0, .offset = 0x0, .length = 4};
	static constexpr Register EUI_64{.bank = GEN_CFG_AES_0, .offset = 0x4, .length = 8};
	static constexpr Register PANADR{.bank = GEN_CFG_AES_0, .offset = 0xC, .length = 4};
	static constexpr Register SYS_CFG{.bank = GEN_CFG_AES_0, .offset = 0x10, .length = 4};
	static constexpr Register FF_CFG{.bank = GEN_CFG_AES_0, .offset = 0x14, .length = 2};
	static constexpr Register SPI_RD_CRC{.bank = GEN_CFG_AES_0, .offset = 0x18, .length = 1};
	static constexpr Register SYS_TIME{.bank = GEN_CFG_AES_0, .offset = 0x1C, .length = 4};
	static constexpr Register TX_FCTRL{.bank = GEN_CFG_AES_0, .offset = 0x24, .length = 6};
	static constexpr Register DX_TIME{.bank = GEN_CFG_AES_0, .offset = 0x2C, .length = 4};
	static constexpr Register DREF_TIME{.bank = GEN_CFG_AES_0, .offset = 0x30, .length = 4};
	static constexpr Register RX_FWTO{.bank = GEN_CFG_AES_0, .offset = 0x34, .length = 3};
	static constexpr Register SYS_CTRL{.bank = GEN_CFG_AES_0, .offset = 0x38, .length = 1};
	static constexpr Register SYS_ENABLE{.bank = GEN_CFG_AES_0, .offset = 0x3C, .length = 6};
	static constexpr Register SYS_STATUS{.bank = GEN_CFG_AES_0, .offset = 0x44, .length = 6};
	static constexpr Register RX_FINFO{.bank = GEN_CFG_AES_0, .offset = 0x4C, .length = 4};
	static constexpr Register RX_TIME{.bank = GEN_CFG_AES_0, .offset = 0x64, .length = 16};
	static constexpr Register TX_TIME{.bank = GEN_CFG_AES_0, .offset = 0x74, .length = 5};
	static constexpr Register TX_RAWST{.bank = GEN_CFG_AES_1, .offset = 0x0, .length = 4};
	static constexpr Register TX_ANTD{.bank = GEN_CFG_AES_1, .offset = 0x4, .length = 2};
	static constexpr Register ACK_RESP_T{.bank = GEN_CFG_AES_1, .offset = 0x8, .length = 4};
	static constexpr Register TX_POWER{.bank = GEN_CFG_AES_1, .offset = 0xC, .length = 4};
	static constexpr Register CHAN_CTRL{.bank = GEN_CFG_AES_1, .offset = 0x14, .length = 2};
	static constexpr Register LA_PEND_01{.bank = GEN_CFG_AES_1, .offset = 0x18, .length = 4};
	static constexpr Register LA_PEND_23{.bank = GEN_CFG_AES_1, .offset = 0x1C, .length = 4};
	static constexpr Register SPI_COLLISION{.bank = GEN_CFG_AES_1, .offset = 0x20, .length = 1};
	static constexpr Register RDB_STATUS{.bank = GEN_CFG_AES_1, .offset = 0x24, .length = 1};
	static constexpr Register RDB_DIAG{.bank = GEN_CFG_AES_1, .offset = 0x28, .length = 1};
	static constexpr Register AES_CFG{.bank = GEN_CFG_AES_1, .offset = 0x30, .length = 2};
	static constexpr Register AES_IV0{.bank = GEN_CFG_AES_1, .offset = 0x34, .length = 4};
	static constexpr Register AES_IV1{.bank = GEN_CFG_AES_1, .offset = 0x38, .length = 4};
	static constexpr Register AES_IV2{.bank = GEN_CFG_AES_1, .offset = 0x3C, .length = 2};
	static constexpr Register AES_IV3{.bank = GEN_CFG_AES_1, .offset = 0x40, .length = 2};
	static constexpr Register DMA_CFG{.bank = GEN_CFG_AES_1, .offset = 0x44, .length = 8};
	static constexpr Register AES_START{.bank = GEN_CFG_AES_1, .offset = 0x4C, .length = 1};
	static constexpr Register AES_STS{.bank = GEN_CFG_AES_1, .offset = 0x50, .length = 4};
	static constexpr Register AES_KEY{.bank = GEN_CFG_AES_1, .offset = 0x54, .length = 16};

	// Subregisters for STS_CONFIG
	static constexpr Register STS_CFG{.bank = STS_CONFIG, .offset = 0x0, .length = 2};
	static constexpr Register STS_CTRL{.bank = STS_CONFIG, .offset = 0x4, .length = 1};
	static constexpr Register STS_STS{.bank = STS_CONFIG, .offset = 0x8, .length = 2};
	static constexpr Register STS_KEY{.bank = STS_CONFIG, .offset = 0xC, .length = 16};
	static constexpr Register STS_IV{.bank = STS_CONFIG, .offset = 0x1C, .length = 16};

	// Subregisters for RX_TUNE
	static constexpr Register DGC_CFG{
		.bank = RX_TUNE, .offset = 0x18, .length = 2};  // Confusing names i know
	static constexpr Register DGC_CFG0{.bank = RX_TUNE, .offset = 0x1C, .length = 4};
	static constexpr Register DGC_CFG1{.bank = RX_TUNE, .offset = 0x20, .length = 4};
	static constexpr Register DGC_LUT_0{.bank = RX_TUNE, .offset = 0x38, .length = 4};
	static constexpr Register DGC_LUT_1{.bank = RX_TUNE, .offset = 0x3C, .length = 4};
	static constexpr Register DGC_LUT_2{.bank = RX_TUNE, .offset = 0x40, .length = 4};
	static constexpr Register DGC_LUT_3{.bank = RX_TUNE, .offset = 0x44, .length = 4};
	static constexpr Register DGC_LUT_4{.bank = RX_TUNE, .offset = 0x48, .length = 4};
	static constexpr Register DGC_LUT_5{.bank = RX_TUNE, .offset = 0x4C, .length = 4};
	static constexpr Register DGC_LUT_6{.bank = RX_TUNE, .offset = 0x50, .length = 4};
	static constexpr Register DGC_DBG{.bank = RX_TUNE, .offset = 0x60, .length = 4};

	// Subregisters for EXT_SYNC
	static constexpr Register EC_CTRL{.bank = EXT_SYNC, .offset = 0x0, .length = 4};
	static constexpr Register RX_CAL{.bank = EXT_SYNC, .offset = 0xC, .length = 4};
	static constexpr Register RX_CAL_RESI{.bank = EXT_SYNC, .offset = 0x14, .length = 4};
	static constexpr Register RX_CAL_RESQ{.bank = EXT_SYNC, .offset = 0x1C, .length = 4};
	static constexpr Register RX_CAL_STS{.bank = EXT_SYNC, .offset = 0x20, .length = 1};

	// Subregisters for GPIO_CTRL
	static constexpr Register GPIO_MODE{.bank = GPIO_CTRL, .offset = 0x0, .length = 4};
	static constexpr Register GPIO_PULL_EN{.bank = GPIO_CTRL, .offset = 0x04, .length = 2};
	static constexpr Register GPIO_DIR{.bank = GPIO_CTRL, .offset = 0x08, .length = 2};
	static constexpr Register GPIO_OUT{.bank = GPIO_CTRL, .offset = 0x0C, .length = 2};
	static constexpr Register GPIO_IRQE{.bank = GPIO_CTRL, .offset = 0x10, .length = 2};
	static constexpr Register GPIO_ISTS{.bank = GPIO_CTRL, .offset = 0x14, .length = 2};
	static constexpr Register GPIO_ISEN{.bank = GPIO_CTRL, .offset = 0x18, .length = 2};
	static constexpr Register GPIO_IMODE{.bank = GPIO_CTRL, .offset = 0x1C, .length = 2};
	static constexpr Register GPIO_IBES{.bank = GPIO_CTRL, .offset = 0x20, .length = 2};
	static constexpr Register GPIO_ICLR{.bank = GPIO_CTRL, .offset = 0x24, .length = 4};
	static constexpr Register GPIO_IDBE{.bank = GPIO_CTRL, .offset = 0x28, .length = 4};
	static constexpr Register GPIO_RAW{.bank = GPIO_CTRL, .offset = 0x2C, .length = 2};

	// Subregisters for DRX
	static constexpr Register DTUNE0{.bank = DRX, .offset = 0x0, .length = 2};
	static constexpr Register RX_SFD_TOC{.bank = DRX, .offset = 0x2, .length = 2};
	static constexpr Register PRE_TOC{.bank = DRX, .offset = 0x4, .length = 2};
	static constexpr Register DTUNE3{.bank = DRX, .offset = 0xC, .length = 4};
	static constexpr Register DTUNE_5{.bank = DRX, .offset = 0x14, .length = 4};
	static constexpr Register DRX_CAR_INT{.bank = DRX, .offset = 0x29, .length = 3};

	// Subregisters for RF_CONF
	static constexpr Register RF_ENABLE{.bank = RF_CONF, .offset = 0x0, .length = 4};
	static constexpr Register RF_CTRL_MASK{.bank = RF_CONF, .offset = 0x4, .length = 4};
	static constexpr Register RF_SWITCH{.bank = RF_CONF, .offset = 0x14, .length = 4};
	static constexpr Register RF_RX_CTRL_HI{
		.bank = RF_CONF, .offset = 0x10, .length = 4};  // Undocumented
	static constexpr Register RF_TX_CTRL_1{.bank = RF_CONF, .offset = 0x1A, .length = 1};
	static constexpr Register RF_TX_CTRL_2{.bank = RF_CONF, .offset = 0x1C, .length = 4};
	static constexpr Register TX_TEST{.bank = RF_CONF, .offset = 0x28, .length = 1};
	static constexpr Register SAR_TEST{.bank = RF_CONF, .offset = 0x34, .length = 1};
	static constexpr Register LDO_TUNE{
		.bank = RF_CONF, .offset = 0x40, .length = 8};  // TODO Check if 0 before initializing
	static constexpr Register LDO_CTRL{.bank = RF_CONF, .offset = 0x48, .length = 4};
	static constexpr Register LDO_RLOAD{
		.bank = RF_CONF, .offset = 0x51, .length = 1};  // TODO check if 0x14 after boot

	// Subregisters for RF_CAL/TX_CAL
	static constexpr Register SAR_CTRL{.bank = RF_CAL, .offset = 0x0, .length = 1};
	static constexpr Register SAR_STATUS{.bank = RF_CAL, .offset = 0x4, .length = 1};
	static constexpr Register SAR_READING{.bank = RF_CAL, .offset = 0x8, .length = 3};
	static constexpr Register SAR_WAKE_RD{.bank = RF_CAL, .offset = 0xC, .length = 2};
	static constexpr Register PGC_CTRL{.bank = RF_CAL, .offset = 0x10, .length = 2};
	static constexpr Register PGC_STATUS{.bank = RF_CAL, .offset = 0x14, .length = 2};
	static constexpr Register PG_TEST{.bank = RF_CAL, .offset = 0x18, .length = 2};
	static constexpr Register PG_CAL_TARGET{.bank = RF_CAL, .offset = 0x1C, .length = 2};

	// Subregisters for FS_CTRL
	static constexpr Register PLL_CFG{.bank = FS_CTRL, .offset = 0x0, .length = 2};
	static constexpr Register PLL_CC{.bank = FS_CTRL, .offset = 0x4, .length = 1};
	static constexpr Register PLL_CAL{.bank = FS_CTRL, .offset = 0x8, .length = 2};
	static constexpr Register XTAL{.bank = FS_CTRL, .offset = 0x14, .length = 1};

	// Subregisters for AON
	static constexpr Register AON_DIG_CFG{.bank = AON, .offset = 0x0, .length = 3};
	static constexpr Register AON_CTRL{.bank = AON, .offset = 0x4, .length = 1};
	static constexpr Register AON_RDATA{.bank = AON, .offset = 0x8, .length = 1};
	static constexpr Register AON_ADDR{.bank = AON, .offset = 0xC, .length = 2};
	static constexpr Register AON_WDATA{.bank = AON, .offset = 0x10, .length = 1};
	static constexpr Register AON_CFG{.bank = AON, .offset = 0x14, .length = 1};

	// Subregisters for OTP_IF
	static constexpr Register OTP_WDATA{.bank = OTP_IF, .offset = 0x0, .length = 4};
	static constexpr Register OTP_ADDR{.bank = OTP_IF, .offset = 0x4, .length = 2};
	static constexpr Register OTP_CFG{.bank = OTP_IF, .offset = 0x8, .length = 2};
	static constexpr Register OTP_STAT{.bank = OTP_IF, .offset = 0xC, .length = 1};
	static constexpr Register OTP_RDATA{.bank = OTP_IF, .offset = 0x10, .length = 4};
	static constexpr Register OTP_SRDATA{.bank = OTP_IF, .offset = 0x14, .length = 4};

	// TODO CIA 0-3
	static constexpr Register CIA_DIAG_0{.bank = CIA_0, .offset = 0x20, .length = 4};
	static constexpr Register CIA_CONF{.bank = CIA_2, .offset = 0x0, .length = 4};
	static constexpr Register STS_CONF_1{.bank = CIA_2, .offset = 0x16, .length = 4};

	// Subregisters for DIG_DIAG
	static constexpr Register EVC_CTRL{.bank = DIG_DIAG, .offset = 0x0, .length = 1};
	static constexpr Register EVC_PHE{.bank = DIG_DIAG, .offset = 0x4, .length = 2};
	static constexpr Register EVC_RSE{.bank = DIG_DIAG, .offset = 0x6, .length = 2};
	static constexpr Register EVC_FCG{.bank = DIG_DIAG, .offset = 0x8, .length = 2};
	static constexpr Register EVC_FCE{.bank = DIG_DIAG, .offset = 0xA, .length = 2};
	static constexpr Register EVC_FFR{.bank = DIG_DIAG, .offset = 0xC, .length = 1};
	static constexpr Register EVC_OVR{.bank = DIG_DIAG, .offset = 0xE, .length = 1};
	static constexpr Register EVC_STO{.bank = DIG_DIAG, .offset = 0x10, .length = 2};
	static constexpr Register EVC_PTO{.bank = DIG_DIAG, .offset = 0x12, .length = 2};
	static constexpr Register EVC_FWTO{.bank = DIG_DIAG, .offset = 0x14, .length = 1};
	static constexpr Register EVC_TXFS{.bank = DIG_DIAG, .offset = 0x16, .length = 2};
	static constexpr Register EVC_HPW{.bank = DIG_DIAG, .offset = 0x18, .length = 1};
	static constexpr Register EVC_SWCE{.bank = DIG_DIAG, .offset = 0x1A, .length = 1};
	static constexpr Register EVC_RES1{.bank = DIG_DIAG, .offset = 0x1C, .length = 8};
	static constexpr Register DIAG_TMC{.bank = DIG_DIAG, .offset = 0x24, .length = 4};
	static constexpr Register EVC_CPQE{.bank = DIG_DIAG, .offset = 0x28, .length = 1};
	static constexpr Register EVC_VWARN{.bank = DIG_DIAG, .offset = 0x2A, .length = 1};
	static constexpr Register SPI_MODE{.bank = DIG_DIAG, .offset = 0x2C, .length = 1};
	static constexpr Register SYS_STATE{.bank = DIG_DIAG, .offset = 0x30, .length = 4};
	static constexpr Register FCMD_STAT{.bank = DIG_DIAG, .offset = 0x3C, .length = 1};
	static constexpr Register CTR_DBG{.bank = DIG_DIAG, .offset = 0x48, .length = 4};
	static constexpr Register SPICRCINIT{.bank = DIG_DIAG, .offset = 0x4C, .length = 1};

	// Subregisters for PMSC_CTRL
	static constexpr Register SOFT_RST{.bank = PMSC_CTRL, .offset = 0x0, .length = 2};
	static constexpr Register CLK_CTRL{.bank = PMSC_CTRL, .offset = 0x4, .length = 4};
	static constexpr Register SEQ_CTRL{.bank = PMSC_CTRL, .offset = 0x8, .length = 4};
	static constexpr Register TXFSEQ{.bank = PMSC_CTRL, .offset = 0x12, .length = 4};
	static constexpr Register LED_CTRL{.bank = PMSC_CTRL, .offset = 0x16, .length = 4};
	static constexpr Register RX_SNIFF{.bank = PMSC_CTRL, .offset = 0x1A, .length = 4};
	static constexpr Register BIAS_CTRL{.bank = PMSC_CTRL, .offset = 0x1F, .length = 2};

	static constexpr Register RX_BUFFER_0{.bank = RX_BUFFER_0_BANK, .offset = 0x0, .length = 1024};

	static constexpr Register RX_BUFFER_1{.bank = RX_BUFFER_1_BANK, .offset = 0x0, .length = 1024};

	static constexpr Register TX_BUFFER{.bank = TX_BUFFER_BANK, .offset = 0x0, .length = 1024};

	static constexpr Register ACC_MEM{.bank = ACC_MEM_BANK, .offset = 0x0, .length = 12288};

	static constexpr Register SCRATCH_RAM{.bank = SCRATCH_RAM_BANK, .offset = 0x0, .length = 127};

	// Subregisters for AES_RAM
	static constexpr Register AES_KEY_0{.bank = AES_RAM, .offset = 0x0, .length = 16};
	static constexpr Register AES_KEY_1{.bank = AES_RAM, .offset = 0x10, .length = 16};
	static constexpr Register AES_KEY_2{.bank = AES_RAM, .offset = 0x20, .length = 16};
	static constexpr Register AES_KEY_3{.bank = AES_RAM, .offset = 0x30, .length = 16};
	static constexpr Register AES_KEY_4{.bank = AES_RAM, .offset = 0x40, .length = 16};
	static constexpr Register AES_KEY_5{.bank = AES_RAM, .offset = 0x50, .length = 16};
	static constexpr Register AES_KEY_6{.bank = AES_RAM, .offset = 0x60, .length = 16};
	static constexpr Register AES_KEY_7{.bank = AES_RAM, .offset = 0x70, .length = 16};

	// TODO DB_DIAG

	// TODO INDIRECT_PTR_A

	// TODO INDIRECT_PTR_B

	// TODO IN_PTR_CFG

	static constexpr uint32_t SEQ_CTRL_AINIT2IDLE_mask = 0x00000010;
};

}  // namespace modm

#endif /* MODM_DW3110_DEFINITIONS_HPP */
