/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cstring>

// ----------------------------------------------------------------------------
namespace modm
{
	namespace filter
	{
		template <typename T>
		class Median<T, 3>
		{
		public:
			Median(const T& initialValue = 0);

			void
			append(const T& input);

			void
			update();

			const T
			getValue() const;

		private:
			uint_fast8_t index;
			T buffer[3];
			T sorted[3];
		};
	}
}

// ----------------------------------------------------------------------------
template <typename T>
modm::filter::Median<T, 3>::Median(const T& initialValue) :
	index(0)
{
	for (uint_fast8_t i = 0; i < 3; ++i) {
		buffer[i] = initialValue;
		sorted[i] = initialValue;
	}
}

template <typename T>
void
modm::filter::Median<T, 3>::append(const T& input)
{
	this->buffer[index] = input;
	if (++index >= 3) {
		index = 0;
	}
}

template <typename T>
void
modm::filter::Median<T, 3>::update()
{
	// for small sample sizes it is the fastest way to just copy the
	// buffer and sort it afterwards
	std::memcpy((void *) sorted, (const void * const) buffer, sizeof(sorted));

	MODM_MEDIAN_SORT(sorted[0], sorted[1]);
	MODM_MEDIAN_SORT(sorted[1], sorted[2]);
	MODM_MEDIAN_SORT(sorted[0], sorted[1]);
}

template <typename T>
const T
modm::filter::Median<T, 3>::getValue() const
{
	return sorted[1];
}
