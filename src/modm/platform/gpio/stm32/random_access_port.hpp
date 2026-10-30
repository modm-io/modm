/*
* Copyright (c) 2021, Christopher Durand
*
* This file is part of the modm project.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_RANDOM_ACCESS_PORT_HPP
#define MODM_STM32_RANDOM_ACCESS_PORT_HPP

#include <array>
#include <iterator>
#include <algorithm>

#include "software_port.hpp"
#include "gpio_accessor.hpp"

namespace modm::platform
{

/**
* Extended software Gpio port with single pin random access. It inherits all
* functionality from SoftwareGpioPort with the additional feature to set and
* read single Gpios with a runtime index.
*
* It conforms to the std::ranges::random_access_range concept.
*
* Example:
* @code
*   using Port = RandomAccessPort<GpioA1, GpioB4, GpioC8>;
*   Port::setOutput();
*   Port::set(2); // set pin with index 2 => A1
*
*   Port port;
*   // set pin C8
*   GpioAccessor pin = port[0];
*   pin.set();
*
*   auto reversedPins = port | std::views::reverse;
*   for(auto pin : reversedPins) {
*      pin.toggle();
*   }
* @endcode
*
* @tparam Gpios	Up to 32 Gpios, ordered MSB to LSB
*
* @author	Christopher Durand
* @ingroup	modm_platform_gpio
*/
template<typename... Gpios>
class RandomAccessPort : public SoftwareGpioPort<Gpios...>
{
public:
	using Index = int_fast8_t;
	static constexpr inline size_t Size{sizeof...(Gpios)};

private:
	static constexpr inline std::array<GpioAccessor, Size> GpioPins = [](){
		auto gpios = makeGpioArray<Gpios...>();
		// Reverse pin order to match SoftwareGpioPort indexing
		std::reverse(gpios.begin(), gpios.end());
		return gpios;
	}();

	friend class GpioIterator;

public:
	class GpioIterator
	{
	private:
		Index index_;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = Index;
		using value_type = GpioAccessor;
		using reference = const GpioAccessor&;

		constexpr GpioIterator() = default;
		constexpr explicit GpioIterator(Index index) : index_{index}
		{}

		constexpr inline GpioIterator&
		operator+=(difference_type diff) { index_ += diff; return *this; }

		constexpr inline GpioIterator&
		operator-=(difference_type diff) { index_ -= diff; return *this; }

		constexpr inline reference
		operator*() const { return RandomAccessPort::GpioPins[index_]; }

		constexpr inline reference
		operator[](difference_type diff) const { return RandomAccessPort::GpioPins[index_ + diff]; }

		constexpr inline GpioIterator&
		operator++() { ++index_; return *this; }

		constexpr inline GpioIterator&
		operator--() { --index_; return *this; }

		constexpr inline GpioIterator
		operator++(int) { GpioIterator tmp{*this}; ++index_; return tmp; }

		constexpr inline GpioIterator
		operator--(int) { GpioIterator tmp{*this}; --index_; return tmp; }

		constexpr inline difference_type
		operator-(const GpioIterator& rhs) const { return index_ - rhs.index_; }

		constexpr inline GpioIterator
		operator+(difference_type diff) const { return GpioIterator{index_ + diff}; }

		constexpr inline GpioIterator
		operator-(difference_type diff) const { return GpioIterator{index_ - diff}; }

		friend constexpr inline GpioIterator
		operator+(difference_type lhs, const GpioIterator& rhs) { return GpioIterator{lhs + rhs.index_}; }

		friend constexpr inline GpioIterator
		operator-(difference_type lhs, const GpioIterator& rhs) { return GpioIterator{lhs - rhs.index_}; }

		constexpr inline auto
		operator<=>(const GpioIterator& rhs) const = default;
	};
	static_assert(std::random_access_iterator<GpioIterator>);

	constexpr GpioIterator
	begin() const { return GpioIterator{0}; }

	constexpr GpioIterator
	end() const { return GpioIterator{Size}; }

	constexpr const GpioAccessor&
	operator[](Index index) const
	{
		return GpioPins[index];
	}

	static void set(Index index, bool enable)
	{
		GpioPins[index].set(enable);
	}

	static void set(Index index)
	{
		GpioPins[index].set();
	}

	static void reset(Index index)
	{
		GpioPins[index].reset();
	}

	static void toggle(Index index)
	{
		GpioPins[index].toggle();
	}

	static bool read(Index index)
	{
		return GpioPins[index].read();
	}

	static bool isSet(Index index)
	{
		return GpioPins[index].isSet();
	}
};

}

#endif // MODM_STM32_RANDOM_ACCESS_PORT_HPP
