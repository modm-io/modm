/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_MATH_OPERATOR_HPP
#define	MODM_MATH_OPERATOR_HPP

#include <cmath>
#include <stdint.h>

#include <modm/architecture/utils.hpp>

extern "C" uint16_t modm__sqrt32(uint32_t a);

namespace modm
{
	namespace math
	{
		/**
		 * \brief	Fast AVR integer square root assembly routines
		 *
		 * Square root calculation based on a implementation by Ruud v Gessel.
		 * The maximum execution time is 310 clock cycles (inclusive CALL+RET)
		 *
		 * \see		<a href="http://www.mikrocontroller.net/articles/AVR_Arithmetik#avr-gcc_Implementierung_.2832_Bit.29" target="_blank">
		 * 			Article on microcontroller.net</a>
		 * \see		<a href="http://members.chello.nl/j.beentjes3/Ruud/sqrt32avr.htm" target="_blank">
		 * 			Original implementation</a>
		 *
		 * \ingroup	modm_math_utils
		 */
		inline uint16_t
		sqrt(uint32_t a)
		{
			return modm__sqrt32(a);
		}

		/**
		 * \brief	unsigned 16bit x 16bit = 32bit multiplication
		 *
		 * \see		AVR201
		 * \ingroup	modm_math_utils
		 */
		inline uint32_t
		mul(uint16_t a, uint16_t b);

		/**
		 * \brief	signed 16bit x 16bit = 32bit multiplication
		 *
		 * \see		AVR201
		 * \ingroup	modm_math_utils
		 */
		inline int32_t
		mul(int16_t a, int16_t b);

		/**
		 * \brief	Signed multiply accumulate of two 16bits numbers with
		 * 			a 32bits result
		 *
		 * \code
		 * result += a * b;
		 * \endcode
		 *
		 * \see		AVR201
		 * \ingroup	modm_math_utils
		 */
		inline int32_t
		mac(int32_t result, int16_t a, int16_t b);
	}
}

#include "operator_impl.hpp"

#endif	// MODM_MATH_OPERATOR_HPP
