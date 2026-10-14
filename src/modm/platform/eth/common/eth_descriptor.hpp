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

#include <cstddef>
#include <cstdint>

namespace modm::platform::stm32::dwmac
{

inline constexpr std::size_t CacheLineSize = 32;

/**
 * DMA-visible normal descriptor.
 *
 * STM32H5/H7 Ethernet DMA consumes the first four words. The remaining four
 * words make every descriptor occupy one Cortex-M7 cache line. The DMA
 * descriptor skip length must therefore be configured to 128 bits.
 */
struct alignas(CacheLineSize) Descriptor
{
	volatile uint32_t words[4]{};
	uint32_t padding[4]{};
};

static_assert(sizeof(Descriptor) == CacheLineSize);
static_assert(alignof(Descriptor) == CacheLineSize);

namespace descriptor
{

namespace tx
{
inline constexpr uint32_t InterruptOnCompletion = 1u << 31;  // TDES2
inline constexpr uint32_t Buffer1LengthMask = 0x3fffu;

inline constexpr uint32_t Own = 1u << 31;  // TDES3
inline constexpr uint32_t First = 1u << 29;
inline constexpr uint32_t Last = 1u << 28;
inline constexpr uint32_t ErrorSummary = 1u << 15;
inline constexpr uint32_t FrameLengthMask = 0x7fffu;
}  // namespace tx

namespace rx
{
inline constexpr uint32_t Own = 1u << 31;  // RDES3 read format
inline constexpr uint32_t InterruptOnCompletion = 1u << 30;
inline constexpr uint32_t Buffer2Valid = 1u << 25;
inline constexpr uint32_t Buffer1Valid = 1u << 24;

inline constexpr uint32_t Context = 1u << 30;  // RDES3 write-back format
inline constexpr uint32_t First = 1u << 29;
inline constexpr uint32_t Last = 1u << 28;
inline constexpr uint32_t CrcError = 1u << 24;
inline constexpr uint32_t GiantPacket = 1u << 23;
inline constexpr uint32_t WatchdogTimeout = 1u << 22;
inline constexpr uint32_t OverflowError = 1u << 21;
inline constexpr uint32_t ReceiveError = 1u << 20;
inline constexpr uint32_t DribbleError = 1u << 19;
inline constexpr uint32_t ErrorSummary = 1u << 15;
inline constexpr uint32_t PacketLengthMask = 0x7fffu;

inline constexpr uint32_t DetailedErrorMask =
	CrcError | GiantPacket | WatchdogTimeout | OverflowError | ReceiveError | DribbleError;
}  // namespace rx

}  // namespace descriptor

}  // namespace modm::platform::stm32::dwmac
