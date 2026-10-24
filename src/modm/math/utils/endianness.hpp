/*
 * Copyright (c) 2013-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MATH_ENDIANNESS_HPP
#define MODM_MATH_ENDIANNESS_HPP

#include <bit>
#include <stdint.h>

namespace modm
{

/// @ingroup modm_math_utils
/// @{

/// checks if current architecture is big endian
[[deprecated("use std::endian::native == std::endian::big")]] // DEPRECATED: 2027q4
constexpr bool
isBigEndian()
{
	return std::endian::native == std::endian::big;
}

/// checks if current architecture is little endian
[[deprecated("use std::endian::native == std::endian::little")]] // DEPRECATED: 2027q4
constexpr bool
isLittleEndian()
{
	return std::endian::native == std::endian::little;
}

// MARK: - to host
constexpr uint16_t
fromLittleEndian(uint16_t value)
{
	if constexpr (std::endian::native == std::endian::little) return value;
	else return std::byteswap(value);
}

constexpr int16_t
fromLittleEndian(int16_t value)
{
	return int16_t(fromLittleEndian(uint16_t(value)));
}

constexpr uint32_t
fromLittleEndian(uint32_t value)
{
	if constexpr (std::endian::native == std::endian::little) return value;
	else return std::byteswap(value);
}

constexpr int32_t
fromLittleEndian(int32_t value)
{
	return int32_t(fromLittleEndian(uint32_t(value)));
}

constexpr uint16_t
fromBigEndian(uint16_t value)
{
	if constexpr (std::endian::native == std::endian::big) return value;
	else return std::byteswap(value);
}

constexpr int16_t
fromBigEndian(int16_t value)
{
	return int16_t(fromBigEndian(uint16_t(value)));
}

constexpr uint32_t
fromBigEndian(uint32_t value)
{
	if constexpr (std::endian::native == std::endian::big) return value;
	else return std::byteswap(value);
}

constexpr int32_t
fromBigEndian(int32_t value)
{
	return int32_t(fromBigEndian(uint32_t(value)));
}

// MARK: - from host
constexpr uint16_t
toLittleEndian(uint16_t value)
{
	if constexpr (std::endian::native == std::endian::little) return value;
	else return std::byteswap(value);
}

constexpr int16_t
toLittleEndian(int16_t value)
{
	return int16_t(toLittleEndian(uint16_t(value)));
}

constexpr uint32_t
toLittleEndian(uint32_t value)
{
	if constexpr (std::endian::native == std::endian::little) return value;
	else return std::byteswap(value);
}

constexpr int32_t
toLittleEndian(int32_t value)
{
	return int32_t(toLittleEndian(uint32_t(value)));
}

constexpr uint16_t
toBigEndian(uint16_t value)
{
	if constexpr (std::endian::native == std::endian::big) return value;
	else return std::byteswap(value);
}

constexpr int16_t
toBigEndian(int16_t value)
{
	return int16_t(toBigEndian(uint16_t(value)));
}

constexpr uint32_t
toBigEndian(uint32_t value)
{
	if constexpr (std::endian::native == std::endian::big) return value;
	else return std::byteswap(value);
}

constexpr int32_t
toBigEndian(int32_t value)
{
	return int32_t(toBigEndian(uint32_t(value)));
}

template <typename T>
struct BigEndian
{
	T raw;

	constexpr BigEndian() : raw() {}
	constexpr BigEndian(T value) : raw(toBigEndian(value)) {}
	constexpr operator T() const { return fromBigEndian(raw); }
};

template <typename T>
struct LittleEndian
{
	T raw;

	constexpr LittleEndian() : raw() {}
	constexpr LittleEndian(T value) : raw(toLittleEndian(value)) {}
	constexpr operator T() const { return fromLittleEndian(raw); }
};

/// @}

} // namespace modm

#endif // MODM_MATH_ENDIANNESS_HPP
