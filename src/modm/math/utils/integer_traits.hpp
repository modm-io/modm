#pragma once

#include <limits>
#include <type_traits>

#include <cmath>
namespace modm
{
	template <int Bits>
	struct uint_t
	{
		using least = std::conditional_t<
			(Bits <= 8),uint8_t, std::conditional_t<
			(Bits <= 16), uint16_t, std::conditional_t<
			(Bits <= 32), uint32_t, std::enable_if_t<
			(Bits <= 64), uint64_t>>>>;
	};

	template<int Bits>
	using least_uint = typename modm::uint_t<Bits>::least;

	template<typename T>
	constexpr int most_digits() {
		return std::numeric_limits<T>::digits;
	}

	template<typename T, typename... Ts>
	constexpr std::enable_if_t<sizeof...(Ts), int>
	most_digits() {
		return std::max(std::numeric_limits<T>::digits, most_digits<Ts...>());
	}

	// Trait the smallest integral - signed or unsigned - fitting any Ts
	template <typename ... Ts>
	struct fits_any {
		static constexpr int most_dig = most_digits<Ts...>();

		using type = std::conditional_t<
			std::conjunction_v<std::is_unsigned<Ts>...>,
			typename uint_t<most_dig>::least,
			// An odd most_dig means: type with most digits is signed integral
			std::make_signed_t<typename uint_t<most_dig % 2 ? most_dig : most_dig * 2>::least>
		>;
	};

	template <typename ... Ts>
		using fits_any_t = typename fits_any<Ts...>::type;

	template<unsigned int N>
	struct bitmask {
		using value_type = uint_t<N>::least;
		static constexpr value_type value = std::pow(2, N) - 1;
		constexpr operator value_type() const noexcept { return value; }
	};

	template<unsigned int N>
	using bitmask_t = typename bitmask<N>::value_type;
}
