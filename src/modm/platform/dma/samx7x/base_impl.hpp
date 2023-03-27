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

namespace modm::platform
{

constexpr void
DmaBase::ViewBase::setDataLength(std::size_t length)
{
	uint32_t& mbr_ubc = data_[1];
	mbr_ubc = (mbr_ubc & ~0xFF'FF'FFu) | (length & 0xFF'FF'FFu);
}

constexpr std::size_t
DmaBase::ViewBase::dataLength() const
{
	const uint32_t& mbr_ubc = data_[1];
	return mbr_ubc & 0xFF'FF'FFu;
}

constexpr void
DmaBase::View0Dst::setDestinationAddress(uintptr_t address)
{
	data_[2] = address;
}

constexpr void
DmaBase::View0Dst::setDestinationAddress(volatile void* data)
{
	data_[2] = reinterpret_cast<uint32_t>(data);
}

constexpr uintptr_t
DmaBase::View0Dst::destinationAddress() const
{
	static_assert(sizeof(uint32_t) == sizeof(uintptr_t));
	return data_[2];
}

constexpr void
DmaBase::View0Src::setSourceAddress(uintptr_t address)
{
	data_[2] = address;
}

constexpr void
DmaBase::View0Src::setSourceAddress(const volatile void* data)
{
	data_[2] = reinterpret_cast<uint32_t>(data);
}

constexpr uintptr_t
DmaBase::View0Src::sourceAddress() const
{
	static_assert(sizeof(uint32_t) == sizeof(uintptr_t));
	return data_[2];
}

constexpr void
DmaBase::View1::setDestinationAddress(uintptr_t address)
{
	data_[3] = address;
}

constexpr void
DmaBase::View1::setDestinationAddress(volatile void* data)
{
	data_[3] = reinterpret_cast<uint32_t>(data);
}

constexpr uintptr_t
DmaBase::View1::destinationAddress() const
{
	static_assert(sizeof(uint32_t) == sizeof(uintptr_t));
	return data_[3];
}

constexpr void
DmaBase::View2::setConfiguration(ChannelConfig_t configuration)
{
	data_[4] = configuration.value;
}

constexpr auto
DmaBase::View2::configuration() const -> ChannelConfig_t
{
	return ChannelConfig_t(data_[4]);
}

constexpr int16_t
DmaBase::View3::sourceDataStride() const
{
	const uint32_t& mbr_ds = data_[6];
	return static_cast<int16_t>(mbr_ds & 0xFF'FFu);
}

constexpr void
DmaBase::View3::setSourceDataStride(int16_t stride)
{
	uint32_t& mbr_ds = data_[6];
	mbr_ds = (mbr_ds & ~0xFF'FFu) | static_cast<uint16_t>(stride);
}

constexpr int16_t
DmaBase::View3::destinationDataStride() const
{
	const uint32_t& mbr_ds = data_[6];
	return static_cast<int16_t>(mbr_ds >> 16);
}

constexpr void
DmaBase::View3::setDestinationDataStride(int16_t stride)
{
	uint32_t& mbr_ds = data_[6];
	mbr_ds = (mbr_ds & ~0xFF'FF'00'00u) | (static_cast<uint16_t>(stride) << 16);
}

} // namespace modm::platform
