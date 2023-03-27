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

constexpr uintptr_t
BlockTransfer::sourceAddress() const
{
	return csa;
}

constexpr void
BlockTransfer::setSourceAddress(uintptr_t address)
{
	csa = address;
}

inline void
BlockTransfer::setSourceAddress(const volatile void* address)
{
	csa = reinterpret_cast<uintptr_t>(address);
}

constexpr uintptr_t
BlockTransfer::destinationAddress() const
{
	return cda;
}

constexpr void
BlockTransfer::setDestinationAddress(uintptr_t address)
{
	cda = address;
}

inline void
BlockTransfer::setDestinationAddress(volatile void* address)
{
	cda = reinterpret_cast<uintptr_t>(address);
}

constexpr void
BlockTransfer::setDataLength(std::size_t length)
{
	cubc = length;
}

constexpr std::size_t
BlockTransfer::dataLength() const
{
	return cubc;
}

constexpr DmaBase::ChannelConfig_t
BlockTransfer::configuration() const
{
	return ChannelConfig_t(cc);
}

constexpr void
BlockTransfer::setConfiguration(ChannelConfig_t config)
{
	cc = config.value;
}

constexpr int16_t
BlockTransfer::sourceDataStride() const
{
	return static_cast<int16_t>(cds_msp & 0xFF'FF);
}

constexpr void
BlockTransfer::setSourceDataStride(int16_t stride)
{
	cds_msp = (cds_msp & ~0xFF'FF) | static_cast<uint16_t>(stride);
}

constexpr int16_t
BlockTransfer::destinationDataStride() const
{
	return static_cast<int16_t>((cds_msp >> 16) & 0xFF'FF);
}

constexpr void
BlockTransfer::setDestinationDataStride(int16_t stride)
{
	cds_msp = (cds_msp & ~0xFF'FF'00'00) | (static_cast<uint16_t>(stride) << 16);
}

namespace detail
{
	using View0Src = DmaBase::View0Src;
	using View0Dst = DmaBase::View0Dst;

	template<typename Descriptor, bool endList>
	consteval uint32_t controlFlags()
	{
		// enable descriptor fetch
		constexpr uint32_t nde = endList ? 0u : XDMAC_CNDC_NDE;
		// update source address
		constexpr uint32_t ndsup = XDMAC_CNDC_NDSUP;
		// update destination address
		constexpr uint32_t nddup = XDMAC_CNDC_NDDUP;

		// View0 descriptors contain either the source or destination address, not both.
		if constexpr (std::is_same_v<Descriptor, View0Src>) {
			return nde | ndsup | static_cast<uint32_t>(Descriptor::Type);
		} else if constexpr (std::is_same_v<Descriptor, View0Dst>) {
			return nde | nddup | static_cast<uint32_t>(Descriptor::Type);
		} else {
			return nde | ndsup | nddup | static_cast<uint32_t>(Descriptor::Type);
		}
	}

	template<size_t index, typename T>
	struct SetupList;

	template<size_t index, typename... Ts>
	struct SetupList<index, std::tuple<Ts...>>
	{
		// descriptor format for all types:
		// 0: MBR_NDA (next descriptor address)
		// 1: MBR_UBC (control block)
		// ...

		static constexpr void
		setup(std::tuple<typename Ts::Data...>& data)
		{
			setFlags(data);
			setAddress(data);
		}

		static constexpr void
		setFlags(std::tuple<typename Ts::Data...>& data)
		{
			if constexpr (index < sizeof...(Ts)) {
				// Wrap index to point last element to first
				// Setting NDE on the last element will enable circular transfers
				constexpr auto nextIndex = (index + 1) % sizeof...(Ts);
				using NextType = std::tuple_element_t<nextIndex, std::tuple<Ts...>>;

				// Bits of CNDC register are shifted by 24 bits in descriptor MBR_UBC
				// and named differently (NDSUP is NSEN, NDDUP is NDEN)
				// | 28 27 |  26  |  25  |  24 |
				// | NVIEW | NDEN | NSEN | NDE |
				uint32_t& mbr_ubc = std::get<index>(data)[1];
				const uint32_t mask = 0b11'1'1'1u << 24;
				const auto pos = 24;

				// disable circular mode by default
				constexpr bool endList = (nextIndex == 0);
				mbr_ubc = (mbr_ubc & ~mask) | (controlFlags<NextType, endList>() << pos);

				SetupList<index + 1, std::tuple<Ts...>>::setFlags(data);
			}
		}

		static constexpr void
		setAddress(std::tuple<typename Ts::Data...>& data)
		{
			if constexpr (index < sizeof...(Ts)) {
				constexpr auto nextIndex = (index + 1) % sizeof...(Ts);

				// write start address of descriptor at (index + 1)
				// to mbr_nda field of descriptor at index
				uint32_t& mbr_nda = std::get<index>(data)[0];
				uint32_t* const nextAddress = std::get<nextIndex>(data).data();
				mbr_nda = reinterpret_cast<uintptr_t>(nextAddress);

				SetupList<index + 1, std::tuple<Ts...>>::setAddress(data);
			}
		}
	};
}

template<typename... Descriptors>
constexpr LinkedListTransfer<Descriptors...>::LinkedListTransfer()
{
	detail::SetupList<0, std::tuple<Descriptors...>>::setup(descriptors_);
}

template<std::size_t index, typename... Descriptors>
auto descriptor(LinkedListTransfer<Descriptors...>& transfer)
{
	using T = std::tuple_element_t<index, std::tuple<Descriptors...>>;
	uint32_t* const data = std::get<index>(transfer.descriptors_).data();
	return T{data};
}

template<typename... Descriptors>
auto LinkedListTransfer<Descriptors...>::descriptors() -> std::tuple<Descriptors...>
{
	return [this]<std::size_t... I>(std::index_sequence<I...>) {
		return std::tuple<Descriptors...>{std::get<I>(descriptors_).data()...};
	}(std::make_index_sequence<sizeof...(Descriptors)>{});
}

template<typename... Descriptors>
void LinkedListTransfer<Descriptors...>::setCircularMode(bool circular)
{
	// The list is constructed in a way that the last descriptor always points at the first.
	// Circular mode is configured by enabling next descriptor fetch for the last element.
	uint32_t* const last = std::get<sizeof...(Descriptors) - 1>(descriptors_).data();
	uint32_t& mbr_ubc = last[1];
	const uint32_t nde = 1u << 24;

	if (circular) {
		mbr_ubc |= nde;
	} else {
		mbr_ubc &= ~nde;
	}
}

} // namespace modm::platform
