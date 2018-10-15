/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FILTER_RAMP_HPP
#define MODM_FILTER_RAMP_HPP

namespace modm
{
	namespace filter
	{
		/**
		 * \brief	Ramp
		 *
		 * The output value is incremented or decremented at every call to update
		 * until target has been reached by increment or decrement.
		 *
		 * The final value is always set to target after several calls to update.
		 *
		 * Example:
		 * \todo	example
		 *
		 * \todo	documentation
		 * \ingroup	modm_math_filter
		 */
		template<typename T>
		class Ramp
		{
		public:
			/**
			 * \brief	Create a ramp generator
			 *
			 * \param	increment		Step size for positive direction
			 * \param	decrement		Step size for the negative direction.
			 * 							<b>Needs to be positive!</b>
			 * \param	initialValue	Starting value
			 *
			 * \warning	Both values for increment and decrement needs to
			 * 			be positive!
			 */
			Ramp(const T& increment,
				 const T& decrement,
				 const T& initialValue = T());

			inline void
			setTarget(const T& target);

			/// Calculate the next step
			void
			update();

			inline void
			reset(const T& value = T());

			inline const T&
			getValue() const;

			inline bool
			isTargetReached() const;

		private:
			T target;
			T value;
			bool targetReached;

			T increment;
			T decrement;
		};
	}
}

#include "ramp_impl.hpp"

#endif // MODM_FILTER_RAMP_HPP
