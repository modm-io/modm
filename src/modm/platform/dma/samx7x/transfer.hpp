/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SAMX7X_DMA_TRANSFER_HPP
#define MODM_SAMX7X_DMA_TRANSFER_HPP

#include <cstddef>
#include <cstdint>
#include <tuple>

#include <modm/platform/dma/base.hpp>

namespace modm::platform
{

class DmaChannel;

/**
 * Simple transfer of one block of data
 *
 * All parameters are initialized to zero on construction.
 *
 * @author Christopher Durand
 * @ingroup modm_platform_dma
 */
class BlockTransfer : public DmaBase
{
public:
	constexpr uintptr_t
	sourceAddress() const;

	constexpr void
	setSourceAddress(uintptr_t address);

	inline void
	setSourceAddress(const volatile void* address);

	constexpr uintptr_t
	destinationAddress() const;

	constexpr void
	setDestinationAddress(uintptr_t address);

	inline void
	setDestinationAddress(volatile void* address);

	constexpr void
	setDataLength(std::size_t length);

	constexpr std::size_t
	dataLength() const;

	constexpr ChannelConfig_t
	configuration() const;

	constexpr void
	setConfiguration(ChannelConfig_t config);

	constexpr int16_t
	sourceDataStride() const;

	constexpr void
	setSourceDataStride(int16_t stride);

	constexpr int16_t
	destinationDataStride() const;

	constexpr void
	setDestinationDataStride(int16_t stride);

	uintptr_t csa{};
	uintptr_t cda{};
	uint32_t cc{};
	uint32_t cubc{};
	uint32_t cds_msp{};
};

/**
 * Linked list mode DMA transfer
 *
 * Transfer of multiple independent blocks configured by a linked list of descriptors.
 * The list data structure is automatically managed by this class.
 *
 * Each block operation is configured by one descriptor which can be of one of 5
 * different types. The descriptor types are passed as template arguments.
 *
 * Usually not all parameters have to be reconfigured for each block. To save memory
 * the hardware provides 4 different sizes of descriptors with type 3 being
 * the largest (9 32-bit words) and type 0 being the smallest (3 32-bit words).
 * They contain different subsets of the channel configuration.
 *
 * Descriptor types are defined in @file base.hpp.
 *
 * "View3" descriptors define all channel parameters (configuration, source address,
 * destination address, configuration and data stride settings).
 * "View2" descriptors omit data striding parameters. "View1" descriptors only configure
 * length and addresses. If only one of source or destination address is changed for
 * a block the smallest "View0" descriptor can be used.
 *
 * To guarantee complete initialization of all channel parameters the first descriptor
 * must be of type 2 or 3.
 *
 * Circular operation is supported which automatically restarts from the first block
 * after completion. It can be enabled with setCircularMode(true).
 *
 * @author Christopher Durand
 * @ingroup modm_platform_dma
 */
template<typename... Descriptors>
class LinkedListTransfer : public DmaBase
{
public:
	static_assert(sizeof...(Descriptors) > 0);

	using First = std::tuple_element_t<0, std::tuple<Descriptors...>>;
	static_assert(std::is_same_v<First, View2> || std::is_same_v<First, View3>,
		"The first descriptor must be of type 2 or 3");

	constexpr LinkedListTransfer();

	template<std::size_t index, typename... Ds>
	friend auto descriptor(LinkedListTransfer<Ds...>& transfer);

	/// Return tuple of accessors for all descriptors
	/// All descriptor data is stored inside the transfer object.
	std::tuple<Descriptors...> descriptors();

	/// Enable circular mode, off by default.
	/// If enabled the block transfer automatically restarts with the first block after completion
	void setCircularMode(bool circular);

private:
	friend DmaChannel;

	std::tuple<typename Descriptors::Data...> descriptors_{};
	static_assert((sizeof(typename Descriptors::Data) + ...) == sizeof(descriptors_));
};

/// Get descriptor accessor with descriptor index
template<std::size_t index, typename... Descriptors>
auto descriptor(LinkedListTransfer<Descriptors...>& transfer);


} // namespace modm::platform

#include "transfer_impl.hpp"

#endif // MODM_SAMX7X_DMA_TRANSFER_HPP
