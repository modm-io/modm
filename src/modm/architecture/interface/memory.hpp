/*
 * Copyright (c) 2016, 2020, Niklas Hauser
 * Copyright (c) 2017, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <new>
#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/register.hpp>

namespace modm
{

/// @ingroup modm_architecture_memory
/// @{

/// Describes access and type of dynamic memory.
enum class
MemoryTrait : uint16_t
{
	AccessSBus = Bit0,			///< Memory is accessible via System-Bus
	AccessDBus = Bit1,			///< Memory is accessible via Data-Bus
	AccessIBus = Bit2,			///< Memory is accessible via Instruction-Bus

	AccessDMA = Bit3,			///< Memory is accessible via DMA
	AccessDMA2D = Bit4,			///< Memory is accessible via 2D DMA
	AccessMDMA = Bit5,			///< Memory is accessible via MDMA

	// Bit6 - Bit12 are reserved

	TypeCoreCoupled = Bit13,	///< Memory is coupled closely to the core
	TypeNonVolatile = Bit14,	///< Memory is non-volatile (battery-backed)
	TypeExternal = Bit15,		///< Memory is external RAM
};

/// A memory can have multiple traits.
using MemoryTraits = Flags16<MemoryTrait>;
MODM_TYPE_FLAGS(MemoryTraits);

/// Fast code memory is accessible by at least the I-Bus and it is core-coupled.
/// Fallback memory on exhaustion is not core-coupled.
constexpr MemoryTraits MemoryFastCode = (MemoryTrait::AccessIBus | MemoryTrait::TypeCoreCoupled);
/// Fast data memory is accessible by at least the D-Bus and it is core coupled.
/// Fallback memory on exhaustion is not core-coupled.
constexpr MemoryTraits MemoryFastData = (MemoryTrait::AccessDBus | MemoryTrait::TypeCoreCoupled);

/// DMA-able memory is accessible by at least the S-Bus and DMA.
/// There is no fallback memory on exhaustion.
constexpr MemoryTraits MemoryDMA = (MemoryTrait::AccessSBus | MemoryTrait::AccessDMA);
/// DMA-able memory is accessible by at least the S-Bus and 2D DMA.
/// There is no fallback memory on exhaustion.
constexpr MemoryTraits MemoryDMA2D = (MemoryTrait::AccessSBus | MemoryTrait::AccessDMA2D);
/// DMA-able memory is accessible by at least the S-Bus and MDMA.
/// There is no fallback memory on exhaustion.
constexpr MemoryTraits MemoryMDMA = (MemoryTrait::AccessSBus | MemoryTrait::AccessMDMA);

/// External memory is accessible by at least the S-Bus and it is external.
/// Fallback memory on exhaustion is internal.
constexpr MemoryTraits MemoryExternal = (MemoryTrait::AccessSBus | MemoryTrait::TypeExternal);
/// Backup memory is accessible by at least the S-Bus and it is non-volatile.
/// There is no fallback memory on exhaustion.
constexpr MemoryTraits MemoryBackup = (MemoryTrait::AccessSBus | MemoryTrait::TypeNonVolatile);

/// Default memory is DMA-able.
constexpr MemoryTraits MemoryDefault = MemoryDMA;

/// @}

} // namespace modm

/// @ingroup modm_architecture_memory
/// @{

/** Request object memory with defined traits.
 * @code
 *    Foo *p = new (modm::MemoryFastData) Foo;
 * @endcode
 */
void *
operator new(std::size_t size, modm::MemoryTraits traits);

/** Request array memory with defined traits.
 * @code
 *    uint32_t *p = new (modm::MemoryFastData) uint32_t[200];
 * @endcode
 */
void *
operator new[](std::size_t size, modm::MemoryTraits traits);

/** Request object memory with defined traits and return nullptr if out-of-memory!
 * @code
 *    Foo *p = new (modm::MemoryFastData, std::nothrow) Foo;
 * @endcode
 */
void *
operator new(std::size_t size, modm::MemoryTraits traits,
             const std::nothrow_t&) noexcept;

/** Request array memory with defined traits and return nullptr if out-of-memory!
 * @code
 *    uint32_t *p = new (modm::MemoryFastData, std::nothrow) uint32_t[200];
 * @endcode
 */
void *
operator new[](std::size_t size, modm::MemoryTraits traits,
               const std::nothrow_t&) noexcept;

/// @}
