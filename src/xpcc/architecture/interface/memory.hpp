// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MEMORY_HPP
#define XPCC_MEMORY_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/interface/register.hpp>

/**
 * @ingroup		interface
 * @defgroup	memory_traits	Memory traits
 *
 * Traits of dynamic memory.
 *
 * @see		driver
 * @author	Niklas Hauser
 */

namespace xpcc
{

/// Describes access and type of dynamic memory.
/// @ingroup memory_traits
enum class
MemoryTrait : uint16_t
{
	AccessSBus = Bit0,			///< Memory is accessible via System-Bus
	AccessDBus = Bit1,			///< Memory is accessible via Data-Bus
	AccessIBus = Bit2,			///< Memory is accessible via Instruction-Bus

	AccessDMA = Bit3,			///< Memory is accessible via DMA
	AccessDMA2D = Bit4,			///< Memory is accessible via 2D DMA

	// Bit5 - Bit12 are reserved

	TypeCoreCoupled = Bit13,	///< Memory is coupled closely to the core
	TypeNonVolatile = Bit14,	///< Memory is non-volatile (battery-backed)
	TypeExternal = Bit15,		///< Memory is external RAM
};

/// A memory can have multiple traits.
/// @ingroup memory_traits
using MemoryTraits = Flags16<MemoryTrait>;
XPCC_TYPE_FLAGS(MemoryTraits);

/// Fast code memory is accessible by at least the I-Bus and it is core-coupled.
/// Fallback memory on exhaustion is not core-coupled.
/// @ingroup memory_traits
constexpr MemoryTraits MemoryFastCode = (MemoryTrait::AccessIBus | MemoryTrait::TypeCoreCoupled);
/// Fast data memory is accessible by at least the D-Bus and it is core coupled.
/// Fallback memory on exhaustion is not core-coupled.
/// @ingroup memory_traits
constexpr MemoryTraits MemoryFastData = (MemoryTrait::AccessDBus | MemoryTrait::TypeCoreCoupled);

/// DMA-able memory is accessible by at least the S-Bus and DMA.
/// There is no fallback memory on exhaustion.
/// @ingroup memory_traits
constexpr MemoryTraits MemoryDMA = (MemoryTrait::AccessSBus | MemoryTrait::AccessDMA);
/// DMA-able memory is accessible by at least the S-Bus and 2D DMA.
/// There is no fallback memory on exhaustion.
/// @ingroup memory_traits
constexpr MemoryTraits MemoryDMA2D = (MemoryTrait::AccessSBus | MemoryTrait::AccessDMA2D);

/// External memory is accessible by at least the S-Bus and it is external.
/// Fallback memory on exhaustion is internal.
/// @ingroup memory_traits
constexpr MemoryTraits MemoryExternal = (MemoryTrait::AccessSBus | MemoryTrait::TypeExternal);
/// Backup memory is accessible by at least the S-Bus and it is non-volatile.
/// There is no fallback memory on exhaustion.
/// @ingroup memory_traits
constexpr MemoryTraits MemoryBackup = (MemoryTrait::AccessSBus | MemoryTrait::TypeNonVolatile);

/// Default memory is DMA-able.
/// @ingroup memory_traits
constexpr MemoryTraits MemoryDefault = MemoryDMA;

} // namespace xpcc

#endif // XPCC_MEMORY_HPP
