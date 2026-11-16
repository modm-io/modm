/*
 * Copyright (c) 2019, 2023, Raphael Lehmann
 * Copyright (c) 2021, Christopher Durand
 * Copyright (c) 2022, Rasmus Kleist Hørlyck Sørensen
 * Copyright (c) 2024, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cstdint>
#include <concepts>
#include <span>
#include <tuple>

#include <modm/math/utils/bit_constants.hpp>
#include <modm/architecture/interface/can_message.hpp>
#include <modm/architecture/interface/register.hpp>

/// @cond

namespace modm::platform::fdcan
{

// Convert message RAM element size to setting bits in RXESC/TXESC registers
template<int size>
constexpr unsigned elementSizeToSetting()
{
	constexpr std::array settings = {
		8,  // 000
		12, // 001
		16, // 010
		20, // 011
		24, // 100
		32, // 101
		48, // 110
		64  // 111
	};

	// needed for the function to be constexpr
	const auto index = [](auto& range, auto value) -> std::optional<int> {
		const auto it = std::ranges::find(range, value);
		if (it == std::end(range))
			return std::nullopt;
		return it - std::begin(range);
	};

	constexpr int headerSize = 8;
	constexpr auto setting = index(settings, size - headerSize);
	static_assert(setting, "Invalid message ram element size");

	return setting.value();
}

/// Internal configuration for MessageRam. The CAN peripheral driver is expected
/// to configure the CAN core to match these settings before using the message
/// RAM. On SAM and the STM32H7 series, these values are dynamically configurable.
/// On other STM32 FDCAN implementations, they are fixed in hardware.
struct MessageRamConfig
{
	uint32_t filterCountStandard;
	uint32_t filterCountExtended;
	uint32_t rxFifo0Elements;
	uint32_t rxFifo0ElementSizeBytes;
	uint32_t rxFifo1Elements;
	uint32_t rxFifo1ElementSizeBytes;
	uint32_t rxBufferElements;
	uint32_t rxBufferElementSizeBytes;
	uint32_t txEventFifoElements;
	uint32_t txFifoElements;
	uint32_t txFifoElementSizeBytes;
	// Triggers unused, skipped


	static constexpr uint32_t RamWordSize = 4;

	// Size of message RAM elements of each type
	constexpr uint32_t standardFilterWords() const { return 1; }
	constexpr uint32_t extendedFilterWords() const { return 2; }
	constexpr uint32_t rxFifo0ElementWords() const { return rxFifo0ElementSizeBytes/RamWordSize; }
	constexpr uint32_t rxFifo1ElementWords() const { return rxFifo1ElementSizeBytes/RamWordSize; }
	constexpr uint32_t rxBufferElementWords() const { return rxBufferElementSizeBytes/RamWordSize; }
	constexpr uint32_t txEventElementWords() const { return 2; }
	constexpr uint32_t txFifoElementWords() const { return txFifoElementSizeBytes/RamWordSize; }

	// Total size of each message RAM section
	constexpr uint32_t standardFilterListSectionWords() const { return filterCountStandard * standardFilterWords(); }
	constexpr uint32_t extendedFilterListSectionWords() const { return filterCountExtended * extendedFilterWords(); }
	constexpr uint32_t rxFifo0SectionWords() const { return rxFifo0Elements * rxFifo0ElementWords(); }
	constexpr uint32_t rxFifo1SectionWords() const { return rxFifo1Elements * rxFifo1ElementWords(); }
	constexpr uint32_t rxBufferSectionWords() const { return rxBufferElements * rxBufferElementWords(); }
	constexpr uint32_t txEventSectionWords() const { return txEventFifoElements * txEventElementWords(); }
	constexpr uint32_t txFifoSectionWords() const { return txFifoElements * txFifoElementWords(); }

	constexpr uint32_t totalSectionWords() const {
		return standardFilterListSectionWords()
			+ extendedFilterListSectionWords()
			+ rxFifo0SectionWords()
			+ rxFifo1SectionWords()
			+ rxBufferSectionWords()
			+ txEventSectionWords()
			+ txFifoSectionWords();
	}

	// Offsets of each message RAM section from the instance's assigned base
	constexpr uint32_t standardFilterListSectionOffset() const { return 0; }
	constexpr uint32_t extendedFilterListSectionOffset() const { return standardFilterListSectionOffset() + standardFilterListSectionWords(); }
	constexpr uint32_t rxFifo0SectionOffset() const { return extendedFilterListSectionOffset() + extendedFilterListSectionWords(); }
	constexpr uint32_t rxFifo1SectionOffset() const { return rxFifo0SectionOffset() + rxFifo0SectionWords(); }
	constexpr uint32_t rxBufferSectionOffset() const { return rxFifo1SectionOffset() + rxFifo1SectionWords(); }
	constexpr uint32_t txEventSectionOffset() const { return rxBufferSectionOffset() + rxBufferSectionWords(); }
	constexpr uint32_t txFifoSectionOffset() const { return txEventSectionOffset() + txEventSectionWords(); }
};

/// Internal class to manage FDCAN message ram
/// \tparam InstanceIndex index of FDCAN instance (starts at 0)
/// \tparam Config configuration of the Tx/Rx/Filter/... buffer sizes
template<uint8_t InstanceIndex, MessageRamConfig config>
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
	static constexpr MessageRamConfig Config = config;

	static constexpr uint32_t StandardFilterCount 	= config.filterCountStandard;
	static constexpr uint32_t StandardFilterSize	= config.standardFilterWords();
	static constexpr uint32_t ExtendedFilterCount	= config.filterCountExtended;
	static constexpr uint32_t ExtendedFilterSize	= config.extendedFilterWords();
	static constexpr uint32_t RxFifo0Elements		= config.rxFifo0Elements;
	static constexpr uint32_t RxFifo0ElementSize	= config.rxFifo0ElementWords();
	static constexpr uint32_t RxFifo1Elements		= config.rxFifo1Elements;
	static constexpr uint32_t RxFifo1ElementSize	= config.rxFifo1ElementWords();
	static constexpr uint32_t RxBufferElements		= config.rxBufferElements;
	static constexpr uint32_t RxBufferElementSize	= config.rxBufferElementWords();
	static constexpr uint32_t TxEventFifoElements	= config.txEventFifoElements;
	static constexpr uint32_t TxEventFifoEntrySize	= config.txEventElementWords();
	static constexpr uint32_t TxFifoElements		= config.txFifoElements;
	static constexpr uint32_t TxFifoElementSize		= config.txFifoElementWords();

	static constexpr uint32_t RamWordSize = MessageRamConfig::RamWordSize;

	// InstanceIndex is not really used (anymore), but needed to have a unique MessageRam<> type for
	// each CAN peripheral
	static inline uintptr_t RamBase;
	static void setRamBase(uintptr_t address) { RamBase = address; }
	static uintptr_t getRamBase() { return RamBase; }

	static uintptr_t FilterListStandard() { return getRamBase() + (config.standardFilterListSectionOffset() * RamWordSize); }
	static uintptr_t FilterListExtended() { return getRamBase() + (config.extendedFilterListSectionOffset() * RamWordSize); }
	static uintptr_t RxFifo0() { return getRamBase() + (config.rxFifo0SectionOffset() * RamWordSize); }
	static uintptr_t RxFifo1() { return getRamBase() + (config.rxFifo1SectionOffset() * RamWordSize); }
	static uintptr_t RxBuffer() { return getRamBase() + (config.rxBufferSectionOffset() * RamWordSize); }
	static uintptr_t TxEventFifo() { return getRamBase() + (config.txEventSectionOffset() * RamWordSize); }
	static uintptr_t TxFifo() { return getRamBase() + (config.txFifoSectionOffset() * RamWordSize); }

	static_assert(StandardFilterCount <= 128, "A maximum of 128 standard filters are allowed.");
	static_assert(ExtendedFilterCount <= 64, "A maximum of 64 standard filters are allowed.");
	static_assert(RxFifo0Elements <= 64, "A maximum of 64 Rx Fifo 0 elements are allowed.");
	static_assert(RxFifo1Elements <= 64, "A maximum of 64 Rx Fifo 1 elements are allowed.");
	static_assert(RxBufferElements <= 64, "A maximum of 64 dedicated Rx buffers are allowed.");
	static_assert(TxEventFifoElements<= 32, "A maximum of 32 Tx Event Fifo elements are allowed.");
	static_assert(TxFifoElements <= 32, "A maximum of 32 Tx Fifo elements are allowed.");

	/// \returns pointer to specified word in the message RAM, taken as relative
	/// to the start of this instance's chunk of the RAM
	static uint32_t*
	messageRamWord(uintptr_t base, uint32_t instanceRelativeWordIndex)
	{
		return reinterpret_cast<uint32_t*>(base + (instanceRelativeWordIndex * RamWordSize));
	}

	/// Address of element in RX FIFO
	struct RxFifoAddress
	{
		uint8_t fifoIndex;
		uint8_t getIndex;

		/// \returns pointer to RX fifo element
		uint32_t* ptr() const
		{
			if (fifoIndex == 0)
				return messageRamWord(RxFifo0(), getIndex * RxFifo0ElementSize);
			else
				return messageRamWord(RxFifo1(), getIndex * RxFifo1ElementSize);
		}
	};

	/// \returns pointer to element in TX queue
	static uint32_t*
	txFifoElement(uint8_t putIndex)
	{
		return messageRamWord(TxFifo(), putIndex * TxFifoElementSize);
	}

	/// \returns pointer to standard filter element
	static uint32_t*
	standardFilter(uint8_t index)
	{
		return messageRamWord(FilterListStandard(), index * StandardFilterSize);
	}

	/// \returns pointer to extended filter element
	static uint32_t*
	extendedFilter(uint8_t index)
	{
		return messageRamWord(FilterListExtended(), index * ExtendedFilterSize);
	}

public:
	static void
	zeroAllData()
	{
		for (uint32_t word = 0; word < Config.totalSectionWords(); ++word)
			*messageRamWord(RamBase, word) = 0;
	}

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

	/// Set identifier, flags and length of a message from RX element headers
	static void
	messageFromRxHeaders(modm::can::Message& message, CommonFifoHeader common, RxFifoHeader rx)
	{
		message.setExtended(bool(common & CommonFifoHeader::ExtendedId));
		message.setRemoteTransmitRequest(bool(common & CommonFifoHeader::RemoteFrame));
		const uint32_t id = CanId_t::get(common);
		message.setIdentifier(message.isExtended() ? id : (id >> 18));
		const bool fd = bool(rx & RxFifoHeader::FdFrame);
		// Classic CAN frames with a DLC above 8 carry 8 data bytes
		const uint8_t dlc = RxDlc_t::get(rx);
		message.setDataLengthCode(fd ? dlc : std::min<uint8_t>(dlc, 8));
		message.setFlexibleData(fd);
		message.setBitRateSwitching(bool(rx & RxFifoHeader::BitRateSwitching));
	}

	/// Read the common header of a TX element
	static CommonFifoHeader_t
	readTxCommonHeader(uint8_t index)
	{
		return CommonFifoHeader_t(*txFifoElement(index));
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
