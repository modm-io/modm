/*
 * Copyright (c) 2009, Thorsten Lajewski
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

#ifndef MODM_FILTER_DEBOUNCE_HPP
#define MODM_FILTER_DEBOUNCE_HPP

#include <stdint.h>

namespace modm
{
	namespace filter
	{
		/**
		 * \brief	Debouncing binary signals
		 *
		 * \ingroup	modm_math_filter
		 */
		template<typename T = uint8_t>
		class Debounce
		{
		public:
			/**
			 * \brief	Constructor
			 *
			 * \param	maxValue	maximal value of the sum
			 * \param	lowerBound	lower bound for the schmitt-trigger
			 * \param	upperBound	upper bound for the schmitt-trigger. If
			 * 						set to zero, the value of maxValue is used.
			 */
			Debounce(const T& maxValue,
					 const T& lowerBound = 0,
					 const T& upperBound = 0);

			void
			update(bool input);

			bool
			getValue() const;

			void
			reset(const bool state);

		private:
			const T maxValue;
			T sum;

			const T lowerBound;
			const T upperBound;
			bool state;
		};
	}
}

#include "debounce_impl.hpp"

#endif // MODM_FILTER_DEBOUNCE_HPP
