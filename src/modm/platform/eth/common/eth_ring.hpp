/*
 * Copyright (c) 2026, Luca Feggi
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
#include <cstddef>
#include <cstdint>
#include <modm/architecture/interface/ethernet.hpp>
#include <optional>
#include <span>

#include "eth_storage.hpp"

namespace modm::platform::stm32::dwmac
{

/** Cache policy for DMA storage mapped as non-cacheable memory. */
struct NonCacheablePolicy
{
	static void
	clean(const void*, std::size_t)
	{}
	static void
	invalidate(void*, std::size_t)
	{}
	static void
	cleanInvalidate(void*, std::size_t)
	{}
	static void
	dataMemoryBarrier()
	{}
	static void
	dataSyncBarrier()
	{}
};

struct RingDiagnostics
{
	uint32_t transmittedFrames{0};
	uint32_t transmitErrors{0};
	uint32_t transmitBusy{0};
	uint32_t receivedFrames{0};
	uint32_t receiveErrors{0};
	uint32_t droppedFrames{0};
	uint32_t descriptorStarvation{0};
};

/**
 * Owns the software state transitions of a single-channel DWMAC descriptor
 * ring. Hardware register access intentionally remains in the platform driver.
 */
template<typename Storage, typename CachePolicy = NonCacheablePolicy>
class DescriptorRing
{
public:
	class RxFrame
	{
	public:
		RxFrame() = default;
		RxFrame(const RxFrame&) = delete;
		RxFrame&
		operator=(const RxFrame&) = delete;

		RxFrame(RxFrame&& other) noexcept
			: ring_(other.ring_), index_(other.index_), length_(other.length_)
		{
			other.ring_ = nullptr;
			other.index_ = 0;
			other.length_ = 0;
		}

		RxFrame&
		operator=(RxFrame&& other) noexcept
		{
			if (this != &other)
			{
				release();
				ring_ = other.ring_;
				index_ = other.index_;
				length_ = other.length_;
				other.ring_ = nullptr;
				other.index_ = 0;
				other.length_ = 0;
			}
			return *this;
		}

		~RxFrame() { release(); }

		std::span<const uint8_t>
		data() const
		{
			if (ring_ == nullptr) return {};
			return {ring_->storage_.rxBuffers[index_].data.data(), length_};
		}

		std::size_t
		size() const
		{ return length_; }
		explicit
		operator bool() const
		{ return ring_ != nullptr; }

		void
		release()
		{
			if (ring_ != nullptr)
			{
				ring_->releaseRx(index_);
				ring_ = nullptr;
				length_ = 0;
			}
		}

	private:
		friend class DescriptorRing;

		RxFrame(DescriptorRing* ring, std::size_t index, std::size_t length)
			: ring_(ring), index_(index), length_(length)
		{}

		DescriptorRing* ring_{nullptr};
		std::size_t index_{0};
		std::size_t length_{0};
	};

public:
	explicit DescriptorRing(Storage& storage) : storage_(storage) {}

	template<typename AddressEncoder>
	void
	initialize(AddressEncoder&& encodeAddress, bool resetDiagnostics = true)
	{
		txProducer_ = 0;
		txReclaim_ = 0;
		rxConsumer_ = 0;
		rxLeaseActive_ = false;
		rxTailPending_ = false;
		if (resetDiagnostics) diagnostics_ = {};

		for (std::size_t index = 0; index < Storage::TxDescriptorCount; ++index)
		{
			auto& descriptor = storage_.txDescriptors[index];
			descriptor.words[0] = encodeAddress(storage_.txBuffers[index].data.data());
			descriptor.words[1] = 0;
			descriptor.words[2] = 0;
			descriptor.words[3] = 0;
			storage_.txState[index] = DescriptorState::Free;
			CachePolicy::clean(&descriptor, sizeof(descriptor));
		}

		for (std::size_t index = 0; index < Storage::RxDescriptorCount; ++index)
		{
			auto& descriptor = storage_.rxDescriptors[index];
			CachePolicy::cleanInvalidate(storage_.rxBuffers[index].data.data(),
										 storage_.rxBuffers[index].data.size());
			descriptor.words[0] = encodeAddress(storage_.rxBuffers[index].data.data());
			descriptor.words[1] = 0;
			descriptor.words[2] = 0;
			descriptor.words[3] = descriptor::rx::Own | descriptor::rx::InterruptOnCompletion |
								  descriptor::rx::Buffer1Valid;
			storage_.rxState[index] = DescriptorState::DmaOwned;
			CachePolicy::clean(&descriptor, sizeof(descriptor));
		}
		CachePolicy::dataSyncBarrier();
	}

	modm::ethernet::TransmitResult
	tryTransmit(std::span<const uint8_t> frame, bool started, bool linkUp, bool dmaError)
	{
		using modm::ethernet::TransmitResult;

		if (dmaError) return TransmitResult::DmaError;
		if (!started) return TransmitResult::NotStarted;
		if (!linkUp) return TransmitResult::LinkDown;
		if (frame.empty()) return TransmitResult::InvalidLength;
		if (frame.size() > Storage::PacketBufferSize ||
			frame.size() > modm::ethernet::MaximumFrameSize)
		{
			return TransmitResult::FrameTooLarge;
		}

		reclaimTransmit();
		auto& descriptor = storage_.txDescriptors[txProducer_];
		if (storage_.txState[txProducer_] != DescriptorState::Free)
		{
			++diagnostics_.transmitBusy;
			return TransmitResult::Busy;
		}

		auto& buffer = storage_.txBuffers[txProducer_].data;
		std::copy(frame.begin(), frame.end(), buffer.begin());
		CachePolicy::clean(buffer.data(), frame.size());

		descriptor.words[1] = 0;
		descriptor.words[2] =
			descriptor::tx::InterruptOnCompletion |
			(static_cast<uint32_t>(frame.size()) & descriptor::tx::Buffer1LengthMask);
		descriptor.words[3] =
			descriptor::tx::First | descriptor::tx::Last |
			(static_cast<uint32_t>(frame.size()) & descriptor::tx::FrameLengthMask);
		storage_.txState[txProducer_] = DescriptorState::DmaOwned;
		CachePolicy::dataMemoryBarrier();
		descriptor.words[3] |= descriptor::tx::Own;
		CachePolicy::clean(&descriptor, sizeof(descriptor));
		CachePolicy::dataSyncBarrier();

		txProducer_ = nextTx(txProducer_);
		return TransmitResult::Ok;
	}

	void
	reclaimTransmit()
	{
		for (std::size_t count = 0; count < Storage::TxDescriptorCount; ++count)
		{
			if (storage_.txState[txReclaim_] != DescriptorState::DmaOwned) return;

			auto& descriptor = storage_.txDescriptors[txReclaim_];
			CachePolicy::invalidate(&descriptor, sizeof(descriptor));
			CachePolicy::dataMemoryBarrier();
			const uint32_t status = descriptor.words[3];
			if ((status & descriptor::tx::Own) != 0) return;

			if ((status & descriptor::tx::ErrorSummary) != 0)
			{
				++diagnostics_.transmitErrors;
			} else
			{
				++diagnostics_.transmittedFrames;
			}
			storage_.txState[txReclaim_] = DescriptorState::Free;
			txReclaim_ = nextTx(txReclaim_);
		}
	}

	std::optional<RxFrame>
	receive()
	{
		// A single outstanding lease guarantees in-order descriptor recycling.
		if (rxLeaseActive_) return std::nullopt;

		for (std::size_t count = 0; count < Storage::RxDescriptorCount; ++count)
		{
			auto& descriptor = storage_.rxDescriptors[rxConsumer_];
			CachePolicy::invalidate(&descriptor, sizeof(descriptor));
			CachePolicy::dataMemoryBarrier();
			const uint32_t status = descriptor.words[3];
			if ((status & descriptor::rx::Own) != 0) return std::nullopt;

			const std::size_t index = rxConsumer_;
			rxConsumer_ = nextRx(rxConsumer_);
			const std::size_t length = status & descriptor::rx::PacketLengthMask;
			const bool complete = (status & (descriptor::rx::First | descriptor::rx::Last)) ==
								  (descriptor::rx::First | descriptor::rx::Last);
			const bool context = (status & descriptor::rx::Context) != 0;
			const bool error = (status & descriptor::rx::ErrorSummary) != 0;
			const bool acceptedErratum =
				acceptRmii10MbitErratum_ && error &&
				(status & (descriptor::rx::CrcError | descriptor::rx::DribbleError)) ==
					(descriptor::rx::CrcError | descriptor::rx::DribbleError) &&
				(status & (descriptor::rx::DetailedErrorMask &
						   ~(descriptor::rx::CrcError | descriptor::rx::DribbleError))) == 0;

			if (!complete || context || length == 0 || length > Storage::PacketBufferSize ||
				length > modm::ethernet::MaximumFrameSize || (error && !acceptedErratum))
			{
				++diagnostics_.receiveErrors;
				++diagnostics_.droppedFrames;
				recycleRx(index);
				continue;
			}

			CachePolicy::invalidate(storage_.rxBuffers[index].data.data(), length);
			CachePolicy::dataMemoryBarrier();
			storage_.rxState[index] = DescriptorState::Leased;
			rxLeaseActive_ = true;
			++diagnostics_.receivedFrames;
			return RxFrame(this, index, length);
		}

		++diagnostics_.descriptorStarvation;
		return std::nullopt;
	}

	void
	setRmii10MbitErratumWorkaround(bool enable)
	{ acceptRmii10MbitErratum_ = enable; }

	Descriptor*
	txTailDescriptor()
	{ return &storage_.txDescriptors[txProducer_]; }
	Descriptor*
	initialRxTailDescriptor()
	{ return &storage_.rxDescriptors[Storage::RxDescriptorCount - 1]; }

	Descriptor*
	takeRxTailDescriptor()
	{
		if (!rxTailPending_) return nullptr;
		rxTailPending_ = false;
		return &storage_.rxDescriptors[rxTailIndex_];
	}

	const RingDiagnostics&
	diagnostics() const
	{ return diagnostics_; }

	bool
	hasRxLease() const
	{ return rxLeaseActive_; }

private:
	static constexpr std::size_t
	nextRx(std::size_t index)
	{ return (index + 1) % Storage::RxDescriptorCount; }

	static constexpr std::size_t
	nextTx(std::size_t index)
	{ return (index + 1) % Storage::TxDescriptorCount; }

	void
	releaseRx(std::size_t index)
	{
		if (storage_.rxState[index] != DescriptorState::Leased) return;
		recycleRx(index);
		rxLeaseActive_ = false;
	}

	void
	recycleRx(std::size_t index)
	{
		auto& descriptor = storage_.rxDescriptors[index];
		CachePolicy::invalidate(storage_.rxBuffers[index].data.data(),
								storage_.rxBuffers[index].data.size());
		descriptor.words[1] = 0;
		descriptor.words[2] = 0;
		descriptor.words[3] = descriptor::rx::InterruptOnCompletion | descriptor::rx::Buffer1Valid;
		storage_.rxState[index] = DescriptorState::DmaOwned;
		CachePolicy::dataMemoryBarrier();
		descriptor.words[3] |= descriptor::rx::Own;
		CachePolicy::clean(&descriptor, sizeof(descriptor));
		CachePolicy::dataSyncBarrier();
		rxTailIndex_ = index;
		rxTailPending_ = true;
	}

private:
	Storage& storage_;
	std::size_t txProducer_{0};
	std::size_t txReclaim_{0};
	std::size_t rxConsumer_{0};
	std::size_t rxTailIndex_{0};
	bool rxLeaseActive_{false};
	bool rxTailPending_{false};
	bool acceptRmii10MbitErratum_{false};
	RingDiagnostics diagnostics_{};
};

}  // namespace modm::platform::stm32::dwmac
