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
inline constexpr uint32_t InterruptOnCompletion = uint32_t(1) << 31;  // TDES2
inline constexpr uint32_t Buffer1LengthMask = 0x3fffu;

inline constexpr uint32_t Own = uint32_t(1) << 31;  // TDES3
inline constexpr uint32_t First = uint32_t(1) << 29;
inline constexpr uint32_t Last = uint32_t(1) << 28;
inline constexpr uint32_t ErrorSummary = uint32_t(1) << 15;
inline constexpr uint32_t FrameLengthMask = 0x7fffu;
}  // namespace tx

namespace rx
{
inline constexpr uint32_t Own = uint32_t(1) << 31;  // RDES3 read format
inline constexpr uint32_t InterruptOnCompletion = uint32_t(1) << 30;
inline constexpr uint32_t Buffer2Valid = uint32_t(1) << 25;
inline constexpr uint32_t Buffer1Valid = uint32_t(1) << 24;

inline constexpr uint32_t Context = uint32_t(1) << 30;  // RDES3 write-back format
inline constexpr uint32_t First = uint32_t(1) << 29;
inline constexpr uint32_t Last = uint32_t(1) << 28;
inline constexpr uint32_t CrcError = uint32_t(1) << 24;
inline constexpr uint32_t GiantPacket = uint32_t(1) << 23;
inline constexpr uint32_t WatchdogTimeout = uint32_t(1) << 22;
inline constexpr uint32_t OverflowError = uint32_t(1) << 21;
inline constexpr uint32_t ReceiveError = uint32_t(1) << 20;
inline constexpr uint32_t DribbleError = uint32_t(1) << 19;
inline constexpr uint32_t ErrorSummary = uint32_t(1) << 15;
inline constexpr uint32_t PacketLengthMask = 0x7fffu;

inline constexpr uint32_t DetailedErrorMask =
	CrcError | GiantPacket | WatchdogTimeout | OverflowError | ReceiveError | DribbleError;
}  // namespace rx

}  // namespace descriptor

}  // namespace modm::platform::stm32::dwmac
