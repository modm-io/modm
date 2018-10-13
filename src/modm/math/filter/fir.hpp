/*
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Kevin Läufer
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FIR_HPP
#define MODM_FIR_HPP

#include <stdint.h>

namespace modm
{
	/**
	 * \brief	A finit impulse response (FIR) filter implementation
	 *
	 * g[n] = SUM(h[k]x[n-k])
	 *
	 * \todo
	 *
	 * \author	Kevin Laeufer
	 * \ingroup modm_math_filter
	 */
	namespace filter
	{
		template<typename T, int N, int BLOCK_SIZE, signed int ScaleFactor = 1>
		class Fir
		{

		public:
			/**
			 * \param	coeff	array containing the coefficients
			 **/
			Fir(const float (&coeff)[N]);

			/**
			 * Reset the coefficients.
			 *
			 * \param	coeff	array containing the coefficients
			 **/
			void
			setCoefficients(const float (&coeff)[N]);

			/**
			 * \brief	Resets the tap buffer
			 */
			void
			reset();

			/**
			 * \brief	Appends new tap
			 */
			void
			append(const T& input);

			/**
			 * \brief	Calculates g[0]
			 *
			 */
			void
			update();

			/**
			 * \brief	Returns g[0].
			 */
			inline const T&
			getValue() const
			{
				return output;
			}

		private:
			T output;
			T taps[N+BLOCK_SIZE];
			T coefficients[N];
			int taps_index;
		};
	}
}

#include "fir_impl.hpp"

#endif // MODM_FIR_HPP
