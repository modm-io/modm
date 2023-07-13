/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2020, Mike Wolfram
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32H7_SPI_BASE_HPP
#define MODM_STM32H7_SPI_BASE_HPP

#include <cstdint>
#include "../device.hpp"
#include <modm/architecture/interface/register.hpp>

namespace modm::platform
{

/**
 * Base class for the SPI classes
 *
 * Provides common definitions that do not depend on the specific SPI instance.
 *
 * @ingroup	modm_platform_spi
 */
class SpiBase
{
public:
	enum class
	Interrupt : uint32_t
	{
		RxPacketAvailable		= SPI_IER_RXPIE,
		TxPacketSpaceAvailable	= SPI_IER_TXPIE,
		DuplexPacket			= SPI_IER_DXPIE,
		EndOfTransfer			= SPI_IER_EOTIE,
		TxTransferFilled 		= SPI_IER_TXTFIE,
		Underrun 				= SPI_IER_UDRIE,
		Overrun 				= SPI_IER_OVRIE,
		CrcError 				= SPI_IER_CRCEIE,
		TiFrameError			= SPI_IER_TIFREIE,
		ModeFault				= SPI_IER_MODFIE,
		Reload					= SPI_IER_TSERFIE
	};
	MODM_FLAGS32(Interrupt);

	enum class
	StatusFlag : uint32_t
	{
		RxPacketAvailable		= SPI_SR_RXP,
		TxPacketSpaceAvailable	= SPI_SR_TXP,
		DuplexPacket			= SPI_SR_DXP,
		EndOfTransfer			= SPI_SR_EOT,
		TxTransferFilled 		= SPI_SR_TXTF,
		Underrun 				= SPI_SR_UDR,
		Overrun 				= SPI_SR_OVR,
		CrcError 				= SPI_SR_CRCE,
		TiFrameError			= SPI_SR_TIFRE,
		ModeFault				= SPI_SR_MODF,
		Reload					= SPI_SR_TSERF,
		Suspension				= SPI_SR_SUSP,
		TxTransferComplete		= SPI_SR_TXC,
		RxFifoLevel1			= SPI_SR_RXPLVL_1,
		RxFifoLevel0			= SPI_SR_RXPLVL_0,
		RxFifoWordNotEmpty		= SPI_SR_RXWNE
	};
	MODM_FLAGS32(StatusFlag);

	enum class
	MasterSelection : uint32_t
	{
		Slave 	= 0,
		Master 	= SPI_CFG2_MASTER,
		Mask 	= Master,
	};

	enum class
	DataMode : uint32_t
	{
		Mode0 = 0b00,			///< clock normal,   sample on rising  edge
		Mode1 = SPI_CFG2_CPHA,	///< clock normal,   sample on falling edge
		Mode2 = SPI_CFG2_CPOL,	///< clock inverted, sample on falling edge
								///  clock inverted, sample on rising  edge
		Mode3 = SPI_CFG2_CPOL | SPI_CFG2_CPHA,
		Mask = Mode3
	};

	enum class
	DataOrder : uint32_t
	{
		MsbFirst = 0b0,
		LsbFirst = SPI_CFG2_LSBFRST,
		Mask = LsbFirst,
	};

	enum class
	Prescaler : uint32_t
	{
		Div2 	= 0,
		Div4 	= SPI_CFG1_MBR_0,
		Div8 	= SPI_CFG1_MBR_1,
		Div16 	= SPI_CFG1_MBR_1 | SPI_CFG1_MBR_0,
		Div32 	= SPI_CFG1_MBR_2,
		Div64 	= SPI_CFG1_MBR_2 | SPI_CFG1_MBR_0,
		Div128 	= SPI_CFG1_MBR_2 | SPI_CFG1_MBR_1,
		Div256 	= SPI_CFG1_MBR_2 | SPI_CFG1_MBR_1 | SPI_CFG1_MBR_0
	};

	enum class
	DataSize : uint32_t
	{
		Bit4 = 3,
		Bit5 = 4,
		Bit6 = 5,
		Bit7 = 6,
		Bit8 = 7,
		Bit9 = 8,
		Bit10 = 9,
		Bit11 = 10,
		Bit12 = 11,
		Bit13 = 12,
		Bit14 = 13,
		Bit15 = 14,
		Bit16 = 15,
		Bit17 = 16,
		Bit18 = 17,
		Bit19 = 18,
		Bit20 = 19,
		Bit21 = 20,
		Bit22 = 21,
		Bit23 = 22,
		Bit24 = 23,
		Bit25 = 24,
		Bit26 = 25,
		Bit27 = 26,
		Bit28 = 27,
		Bit29 = 28,
		Bit30 = 29,
		Bit31 = 30,
		Bit32 = 31
	};
	static_assert(SPI_CFG1_DSIZE_Pos == 0);

	enum class DmaMode : uint32_t
	{
		None = 0,
		Tx = SPI_CFG1_TXDMAEN,
		Rx = SPI_CFG1_RXDMAEN,
		Mask = SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN
	};
	MODM_FLAGS32(DmaMode);

	enum class DuplexMode : uint32_t
	{
		FullDuplex = 0,
		TransmitOnly = SPI_CFG2_COMM_0,
		ReceiveOnly = SPI_CFG2_COMM_1,
		HalfDuplex = SPI_CFG2_COMM_1 | SPI_CFG2_COMM_0,
		Mask = HalfDuplex
	};

	enum class SlaveSelectMode
	{
		HardwareGpio,
		Software = SPI_CFG2_SSM
	};

	enum class SlaveSelectPolarity
	{
		ActiveLow = 0,
		ActiveHigh = SPI_CFG2_SSIOP
	};

	enum class CrcInit : uint32_t
	{
		AllZeros = 0,
		AllOnes = SPI_CR1_TCRCINI | SPI_CR1_RCRCINI,
		Mask = AllOnes
	};
};

constexpr auto
operator<=>(SpiBase::DataSize s0, SpiBase::DataSize s1)
{
    const auto v0 = static_cast<uint32_t>(s0);
    const auto v1 = static_cast<uint32_t>(s1);
    if (v0 < v1) {
        return std::strong_ordering::less;
    } else if (v0 > v1) {
        return std::strong_ordering::greater;
    } else {
        return std::strong_ordering::equal;
    }
}

} // namespace modm::platform

#endif // MODM_STM32H7_SPI_BASE_HPP
