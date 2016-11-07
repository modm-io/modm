/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

namespace xpcc
{
	namespace filter
	{
		template <typename T>
		class Median<T, 9>
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
			T buffer[9];
			T sorted[9];
		};
	}
}

// ----------------------------------------------------------------------------
template <typename T>
xpcc::filter::Median<T, 9>::Median(const T& initialValue) :
	index(0)
{
	for (uint_fast8_t i = 0; i < 9; ++i) {
		buffer[i] = initialValue;
		sorted[i] = initialValue;
	}
}

template <typename T>
void
xpcc::filter::Median<T, 9>::append(const T& input)
{
	// Remove the old value from the sorted set and replace it with the
	// new input. This way the array remains mostly sorted.
	T valueToRemove = this->buffer[index];
	for (uint_fast8_t i = 0; i < 9; ++i) {
		if (this->sorted[i] == valueToRemove) {
			this->sorted[i] = input;
			break;
		}
	}
	
	this->buffer[index] = input;
	if (++index >= 9) {
		index = 0;
	}
}

template <typename T>
void
xpcc::filter::Median<T, 9>::update()
{
	XPCC_MEDIAN__SORT(sorted[1], sorted[2]);
	XPCC_MEDIAN__SORT(sorted[4], sorted[5]);
	XPCC_MEDIAN__SORT(sorted[7], sorted[8]);
	XPCC_MEDIAN__SORT(sorted[0], sorted[1]);
	XPCC_MEDIAN__SORT(sorted[3], sorted[4]);
	XPCC_MEDIAN__SORT(sorted[6], sorted[7]);
	XPCC_MEDIAN__SORT(sorted[1], sorted[2]);
	XPCC_MEDIAN__SORT(sorted[4], sorted[5]);
	XPCC_MEDIAN__SORT(sorted[7], sorted[8]);
	XPCC_MEDIAN__SORT(sorted[0], sorted[3]);
	XPCC_MEDIAN__SORT(sorted[5], sorted[8]);
	XPCC_MEDIAN__SORT(sorted[4], sorted[7]);
	XPCC_MEDIAN__SORT(sorted[3], sorted[6]);
	XPCC_MEDIAN__SORT(sorted[1], sorted[4]);
	XPCC_MEDIAN__SORT(sorted[2], sorted[5]);
	XPCC_MEDIAN__SORT(sorted[4], sorted[7]);
	XPCC_MEDIAN__SORT(sorted[4], sorted[2]);
	XPCC_MEDIAN__SORT(sorted[6], sorted[4]);
	XPCC_MEDIAN__SORT(sorted[4], sorted[2]);
}

template <typename T>
const T
xpcc::filter::Median<T, 9>::getValue() const
{
	return sorted[4];
}
