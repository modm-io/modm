/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <cstddef>
#include <iterator>

namespace modm::platform
{

/// @cond
class FaultStorage
{
public:
	static size_t openRead();
	static void closeRead();

	class Iterator
	{
		friend class FaultStorage;
		const void* table;
		const uint8_t* pos;
	public:
		using iterator_category = std::output_iterator_tag;
		using value_type = const uint8_t;
		using difference_type = std::ptrdiff_t;
		using pointer = const uint8_t*;
		using reference = const uint8_t&;

		explicit Iterator(const void *table, const uint8_t* pos);
		Iterator(const Iterator&) = default;
		Iterator& operator=(const Iterator&) = default;

		uint8_t operator*() const;
		Iterator& operator++();
		Iterator operator++(int);

		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;
	};

	static Iterator begin();
	static Iterator end();

public:
	void openWrite();
	void closeWrite();
	bool write(const void* data, size_t element_size, size_t elements=1);

private:
	bool write(const uint8_t *data, size_t size);
	Iterator current{begin()};
};
/// @endcond

}	// namespace modm::platform

