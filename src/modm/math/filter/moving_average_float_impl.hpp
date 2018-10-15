/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Thorsten Lajewski
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MOVING_AVERAGE_HPP
	#error	"Don't include this file directly, use 'moving_average.hpp' instead!"
#endif

namespace modm
{
	namespace filter{
		template<std::size_t N>
		class MovingAverage<double, N >
		{
		private:
			using Index = std::conditional_t<
				(N >= 256),
				uint_fast16_t,
				uint_fast8_t
			>;

		public:

			MovingAverage(const double& initialValue = 0);

			/// Append new value
			void
			update(const double& input);

			/// Get filtered value
			double
			getValue() const;

		private:
			Index index;
			double buffer[N];
			double sum;
		};
	}
}

//---------------------------------------------------------------------------------

template<std::size_t N>
modm::filter::MovingAverage<double, N>::MovingAverage(const double& initialValue) :
	index(0), sum(N * initialValue)
{
	for (Index i = 0; i < N; ++i) {
		buffer[i] = initialValue;
	}
}


//---------------------------------------------------------------------------------
template<std::size_t N>
void
modm::filter::MovingAverage<double, N>::update(const double& input){
	buffer[index] = input;

	index++;
	if (index >= N) {
		index = 0;
	}

	sum = 0;
	for (Index i = 0; i < N; ++i) {
		sum+= buffer[i];
	}

}

// ------------------------------------------------------------------------------
template<std::size_t N>
double
modm::filter::MovingAverage<double, N>::getValue() const
{
	return (sum / N);
}
