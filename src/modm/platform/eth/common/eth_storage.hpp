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

#include <array>
#include <cstddef>
#include <cstdint>

#include "eth_descriptor.hpp"

namespace modm::platform::stm32::dwmac
{

enum class DescriptorState : uint8_t
{
	Free,
	DmaOwned,
	Leased,
};

template<std::size_t Size>
struct alignas(CacheLineSize) PacketBuffer
{
	static constexpr std::size_t AllocatedSize =
		((Size + CacheLineSize - 1) / CacheLineSize) * CacheLineSize;

	std::array<uint8_t, AllocatedSize> data{};
};

/** Caller-owned Ethernet descriptor and packet storage. */
template<std::size_t RxCount, std::size_t TxCount, std::size_t BufferSize = 1536>
struct alignas(CacheLineSize) Storage
{
	static_assert(RxCount > 0 && RxCount <= 1024,
				  "Ethernet RX descriptor count must be between 1 and 1024");
	static_assert(TxCount > 0 && TxCount <= 1024,
				  "Ethernet TX descriptor count must be between 1 and 1024");
	static_assert(BufferSize >= 64 && BufferSize <= descriptor::tx::Buffer1LengthMask,
				  "Ethernet buffer size must fit a normal DMA descriptor");
	static_assert((BufferSize % 4) == 0,
				  "STM32 Ethernet receive buffer size must be a multiple of four bytes");

	static constexpr std::size_t RxDescriptorCount = RxCount;
	static constexpr std::size_t TxDescriptorCount = TxCount;
	static constexpr std::size_t PacketBufferSize = BufferSize;

	alignas(CacheLineSize) std::array<Descriptor, RxCount> rxDescriptors{};
	alignas(CacheLineSize) std::array<Descriptor, TxCount> txDescriptors{};
	alignas(CacheLineSize) std::array<PacketBuffer<BufferSize>, RxCount> rxBuffers{};
	alignas(CacheLineSize) std::array<PacketBuffer<BufferSize>, TxCount> txBuffers{};

	// CPU-only state deliberately lives outside the DMA-visible descriptors.
	alignas(CacheLineSize) std::array<DescriptorState, RxCount> rxState{};
	alignas(CacheLineSize) std::array<DescriptorState, TxCount> txState{};
};

}  // namespace modm::platform::stm32::dwmac
