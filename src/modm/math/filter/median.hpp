/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FILTER_MEDIAN_HPP
#define MODM_FILTER_MEDIAN_HPP

#include <stdint.h>

namespace modm
{
	namespace filter
	{
		/**
		 * \brief	Median filter
		 *
		 * Calculates the median of a input set. Useful for eliminating spikes
		 * from the input. Adds a group delay of N/2 ticks for the signal.
		 *
		 * Implementation are available for N = 3, 5, 7 and 9. To find
		 * the median the signal values will be partly sorted, but only as much
		 * as needed to find the median.
		 *
		 * \code
		 * // create a new filter for five samples
		 * modm::filter::Median<uint8_t, 5> filter;
		 *
		 * // append new signal values
		 * filter.append(10);
		 * filter.append(10);
		 * filter.append(20);
		 *
		 * // calculate the median
		 * filter.update();
		 *
		 * output = filter.getValue();
		 * \endcode
		 *
		 * \tparam	T	Input type
		 * \tparam	N	Number of samples
		 *
		 * \ingroup	modm_math_filter
		 */
		template<typename T, int N>
		class Median
		{
		public:
			/**
			 * \brief	Constructor
			 *
			 * \param	initialValue	Value will be set for the complete
			 * 							input buffer.
			 */
			Median(const T& initialValue = 0);

			/// Append new value
			void
			append(const T& input);

			/// calculate median
			void
			udpate();

			/// Get median value
			const T
			getValue() const;
		};
	}
}

#include "median_impl.hpp"

#endif // MODM_FILTER_MEDIAN_HPP
