/*
 * Copyright (c) 2019, 2023, Raphael Lehmann
 * Copyright (c) 2021, Christopher Durand
 * Copyright (c) 2022, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <concepts>
#include <span>

#include <modm/math/utils/bit_constants.hpp>
#include <modm/architecture/interface/register.hpp>

/// @cond

namespace modm::platform::fdcan
{


struct MessageRamConfig
{
	uint32_t filterCountStandard;
	uint32_t filterCountExtended;
	uint32_t rxFifo0Elements;
	uint32_t rxFifo0ElementSize;
	uint32_t rxFifo1Elements;
	uint32_t rxFifo1ElementSize;
	uint32_t rxBufferElements;
	uint32_t rxBufferElementSize;
	uint32_t txEventFifoEntries;
	uint32_t txFifoElements;
	uint32_t txFifoElementSize;
};

constexpr MessageRamConfig defaultMessageRamConfig
{
	.filterCountStandard 	= 8,
	.filterCountExtended	= 8,
	.rxFifo0Elements		= 32,
	.rxFifo0ElementSize		= 2*4 + modm::can::Message::capacity,
	.rxFifo1Elements		= 32,
	.rxFifo1ElementSize		= 2*4 + modm::can::Message::capacity,
	.rxBufferElements		= 0,
	.rxBufferElementSize	= 2*4 + modm::can::Message::capacity,
	.txEventFifoEntries		= 0,
	.txFifoElements			= 32,
	.txFifoElementSize		= 2*4 + modm::can::Message::capacity,
};

/// Internal class to manage FDCAN message ram
/// \tparam InstanceIndex index of FDCAN instance (starts at 0)
/// \tparam Config configuration of the Tx/Rx/Filter/... buffer sizes
template<uint8_t InstanceIndex, MessageRamConfig config = defaultMessageRamConfig>
class MessageRam
{
public:
	/// Header common to RX and TX elements
	enum class CommonFifoHeader : uint32_t
	{
		ErrorIndicator 	= Bit31,
		ExtendedId		= Bit30,
		RemoteFrame 	= Bit29,
		// bits 28-0: can id
	};
	MODM_FLAGS32(CommonFifoHeader);
	using CanId_t = Value<CommonFifoHeader_t, 29>;

	/// TX element specific header
	enum class TxFifoHeader : uint32_t
	{
		// bit 31-24: message marker
		StoreEvents		= Bit23,
		// bit 22: reserved
		FdFrame 		= Bit21,
		BitRateSwitching	= Bit20,
		// bit 19-16: dlc
		// bit 15-0: reserved
	};
	MODM_FLAGS32(TxFifoHeader);
	using TxDlc_t = Value<TxFifoHeader_t, 4, 16>;

	/// RX element specific header
	enum class RxFifoHeader : uint32_t
	{
		NonMatchingFrame	= Bit31,
		// bit 30-24: filter index
		StoreEvents		= Bit23,
		// bit 22: reserved
		FdFrame 		= Bit21,
		BitRateSwitching	= Bit20,
		// bit 19-16: dlc
		// bit 15-0: timestamp
	};
	MODM_FLAGS32(RxFifoHeader);
	using FilterIndex_t = Value<RxFifoHeader_t, 7, 24>;
	using RxDlc_t = Value<RxFifoHeader_t, 4, 16>;
	using Timestamp_t = Value<RxFifoHeader_t, 16>;

	enum class FilterType : uint32_t
	{
		Range = 0,
		Dual = (1u << 30),
		Classic = (1u << 31)
	};

	enum class FilterConfig : uint32_t
	{
		/// skip filter
		Disabled 	= 0b000u << 27,
		/// store in RX fifo 0 if filter matches
		Fifo0		= 0b001u << 27,
		/// store in RX fifo 1 if filter matches
		Fifo1		= 0b010u << 27,
		/// reject if filter matches
		Reject		= 0b011u << 27
	};
public:
	static constexpr uint32_t StandardFilterCount 	= config.filterCountStandard;
	static constexpr uint32_t StandardFilterSize	= 1*4;
	static constexpr uint32_t ExtendedFilterCount	= config.filterCountExtended;
	static constexpr uint32_t ExtendedFilterSize	= 2*4;
	static constexpr uint32_t RxFifo0Elements		= config.rxFifo0Elements;
	static constexpr uint32_t RxFifo0ElementSize	= config.rxFifo0ElementSize;
	static constexpr uint32_t RxFifo1Elements		= config.rxFifo1Elements;
	static constexpr uint32_t RxFifo1ElementSize	= config.rxFifo1ElementSize;
	static constexpr uint32_t RxBufferElements		= config.rxBufferElements;
	static constexpr uint32_t RxBufferElementSize	= config.rxBufferElementSize;
	static constexpr uint32_t TxEventFifoEntries	= config.txEventFifoEntries;
	static constexpr uint32_t TxEventFifoEntrySize	= 2*4;
	static constexpr uint32_t TxFifoElements		= config.txFifoElements;
	static constexpr uint32_t TxFifoElementSize		= config.txFifoElementSize;

	/// Total message ram size in bytes
	static constexpr uint32_t Size =
		(StandardFilterCount * StandardFilterSize) +
		(ExtendedFilterCount * ExtendedFilterSize) +
		(RxFifo0Elements * RxFifo0ElementSize) +
		(RxFifo1Elements * RxFifo1ElementSize) +
		(RxBufferElements * RxBufferElementSize) +
		(TxEventFifoEntries * TxEventFifoEntrySize) +
		(TxFifoElements * TxFifoElementSize);

	// InstanceIndex is not really used (anymore), but needed to have a unique MessageRam<> type for
	// each CAN peripheral
	static inline uintptr_t RamBase;
	static void setRamBase(uintptr_t address) { RamBase = address; }
	static uintptr_t getRamBase() { return RamBase; }

	static constexpr uintptr_t FilterListStandardOffset = 0;
	static constexpr uintptr_t FilterListExtendedOffset = FilterListStandardOffset + (StandardFilterCount * StandardFilterSize);
	static constexpr uintptr_t RxFifo0Offset = FilterListExtendedOffset + (ExtendedFilterCount * ExtendedFilterSize);
	static constexpr uintptr_t RxFifo1Offset = RxFifo0Offset + (RxFifo0Elements * RxFifo0ElementSize);
	static constexpr uintptr_t RxBufferOffset = RxFifo1Offset + (RxFifo1Elements * RxFifo1ElementSize);
	static constexpr uintptr_t TxEventFifoOffset = RxBufferOffset + (RxBufferElements * RxBufferElementSize);
	static constexpr uintptr_t TxFifoOffset = TxEventFifoOffset + (TxEventFifoEntries * TxEventFifoEntrySize);


	static uintptr_t FilterListStandard() { return getRamBase() + FilterListStandardOffset; }
	static uintptr_t FilterListExtended() { return getRamBase() + FilterListExtendedOffset; }
	static uintptr_t RxFifo0() { return getRamBase() + RxFifo0Offset; }
	static uintptr_t RxFifo1() { return getRamBase() + RxFifo1Offset; }
	static uintptr_t RxBuffer() { return getRamBase() + RxBufferOffset; }
	static uintptr_t TxEventFifo() { return getRamBase() + TxEventFifoOffset; }
	static uintptr_t TxFifo() { return getRamBase() + TxFifoOffset; }

	/// Address of element in RX FIFO
	struct RxFifoAddress
	{
		uint8_t fifoIndex;
		uint8_t getIndex;

		/// \returns pointer to RX fifo element
		uint32_t*
		ptr() const
		{
			if (fifoIndex == 0)
				return reinterpret_cast<uint32_t*>(RxFifo0() +
												   (getIndex * RxFifo0ElementSize));
			else
				return reinterpret_cast<uint32_t*>(RxFifo1() +
												   (getIndex * RxFifo1ElementSize));
		}
	};

	/// \returns pointer to element in TX queue
	static uint32_t*
	txFifoElement(uint8_t putIndex)
	{
		return reinterpret_cast<uint32_t*>(getRamBase() + TxFifoOffset +
										   (putIndex * TxFifoElementSize));
	}

	/// \returns pointer to standard filter element
	static uint32_t*
	standardFilter(uint8_t index)
	{
		const auto address = getRamBase() + FilterListStandardOffset + (index * StandardFilterSize);
		return reinterpret_cast<uint32_t*>(address);
	}

	/// \returns pointer to extended filter element
	static uint32_t*
	extendedFilter(uint8_t index)
	{
		const auto address = getRamBase() + FilterListExtendedOffset + (index * ExtendedFilterSize);
		return reinterpret_cast<uint32_t*>(address);
	}
public:
	/// Write TX element headers to TX queue
	static void
	writeTxHeaders(uint8_t putIndex, CommonFifoHeader_t common, TxFifoHeader_t tx)
	{
		uint32_t* messageRam = txFifoElement(putIndex);
		*messageRam++ = common.value;
		*messageRam = tx.value;
	}

	/// Read RX element headers from RX fifo
	static std::tuple<CommonFifoHeader, RxFifoHeader>
	readRxHeaders(RxFifoAddress address)
	{
		const uint32_t* messageRam = address.ptr();
		const auto commonHeader = CommonFifoHeader{*messageRam++};
		const auto rxHeader = RxFifoHeader{*messageRam};
		return {commonHeader, rxHeader};
	}

	/// Read message data from RX fifo
	static void
	readData(RxFifoAddress address, std::span<uint8_t> outData)
	{
		const auto size = std::min(outData.size(), 64u);

		// + 2: skip 2x32 bit headers
		const uint32_t* messageRam = address.ptr() + 2;
		// message data must be read in 32bit accesses, memcpy on message ram does not work
		for (auto i = 0u; i < size; i += 4) {
			const uint32_t receivedData = *messageRam++;

			// copy in 32 bit words, memcpy is optimized to single store instruction
			// CAN message buffer must fit full multiples of 4 bytes
			std::memcpy(&outData[i], &receivedData, 4);
		}
	}

	/// Write message data to TX queue
	static void
	writeData(uint_fast8_t putIndex, std::span<const uint8_t> inData)
	{
		const auto size = std::min(inData.size(), 64u);

		// + 2: skip 2x32 bit headers
		uint32_t* messageRam = txFifoElement(putIndex) + 2;
		// message data must be written in 32bit accesses, memcpy to message ram does not work
		for (auto i = 0u; i < size; i += 4) {
			uint32_t outputData{};
			// copy in 32 bit words, memcpy is optimized to single store instruction
			// CAN message buffer must fit full multiples of 4 bytes
			std::memcpy(&outputData, &inData[i], 4);
			*messageRam++ = outputData;
		}
	}

	/// Construct common fifo header from CanMessage
	static CommonFifoHeader_t
	headerFromMessage(const modm::can::Message& message)
	{
		CommonFifoHeader_t header = (message.isExtended() ? CommonFifoHeader::ExtendedId : CommonFifoHeader(0))
			| (message.isRemoteTransmitRequest() ? CommonFifoHeader::RemoteFrame : CommonFifoHeader(0));

		const auto canId = message.isExtended() ? message.getIdentifier() : (message.getIdentifier() << 18);
		CanId_t::set(header, canId);

		return header;
	}

	/// Construct Tx Header from CanMessage
	static TxFifoHeader_t
	txHeaderFromMessage(const modm::can::Message& message)
	{
		TxFifoHeader_t header = (message.isFlexibleData() ? TxFifoHeader::FdFrame : TxFifoHeader(0))
			| (message.isBitRateSwitching() ? TxFifoHeader::BitRateSwitching : TxFifoHeader(0));

		const uint8_t dlc = message.getDataLengthCode();
		MessageRam::TxDlc_t::set(header, dlc);

		return header;
	}

	static void
	setStandardFilter(uint8_t index, FilterType type, FilterConfig filterConfig, uint16_t id1, uint16_t id2)
	{
		constexpr auto idMask = (1u << 11) - 1;
		*standardFilter(index) = uint32_t(type) | uint32_t(filterConfig) |
			(id2 & idMask) | ((id1 & idMask) << 16);
	}

	static void
	setExtendedFilter0(uint8_t index, FilterConfig filterConfig, uint32_t id)
	{
		constexpr auto idMask = (1u << 29) - 1;
		*extendedFilter(index) = (uint32_t(filterConfig) << 2) | (id & idMask);
	}

	static void
	setExtendedFilter1(uint8_t index, FilterType type, uint32_t id)
	{
		constexpr auto idMask = (1u << 29) - 1;
		*(extendedFilter(index) + 1) = uint32_t(type) | (id & idMask);
	}

	static void
	setStandardFilterDisabled(uint8_t index)
	{
		*standardFilter(index) = 0;
	}

	static void
	setExtendedFilterDisabled(uint8_t index)
	{
		*extendedFilter(index) = 0;
	}

};

}	// namespace modm::platform::fdcan

/// @endcond
