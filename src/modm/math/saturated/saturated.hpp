/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_SATURATED_HPP
#define	MODM_SATURATED_HPP

#include <modm/math/utils/arithmetic_traits.hpp>

namespace modm
{
	/**
	 * \brief	Saturated arithmetics
	 *
	 * \see http://www.mikrocontroller.net/articles/AVR_Arithmetik/Saturierung
	 *
	 * \author	Fabian Greif <fabian.greif@rwth-aachen.de>
	 *
	 * \todo	extend implementation (multiplication etc.)
	 * \todo	add 16-bit datetype assembler implementations for AVRs
	 * \todo	documentation
	 * \ingroup modm_math_saturated
	 */
	template<typename T>
	class Saturated
	{
		typedef modm::SignedType<T> SignedType;
		typedef modm::WideType<SignedType> WideType;

	public:
		Saturated();

		Saturated(const T& initialValue);

		inline const T&
		getValue() const
		{
			return value;
		}

		Saturated&
		operator += (const Saturated& other);

		Saturated&
		operator -= (const Saturated& other);

		void
		absolute();

	public:
		template<typename U>
		friend Saturated<U>
		operator - (const Saturated<U>& x);

		template<typename U>
		friend Saturated<U>
		abs(const Saturated<U>& x);

		template<typename U>
		friend Saturated<U>
		operator - (const Saturated<U>& a, const Saturated<U>& b);

		template<typename U>
		friend Saturated<U>
		operator + (const Saturated<U>& a, const Saturated<U>& b);

		template<typename U>
		friend bool
		operator == (const Saturated<U>& a, const Saturated<U>& b);

		template<typename U>
		friend bool
		operator != (const Saturated<U>& a, const Saturated<U>& b);

		// TODO > >= < <=

	private:
		void
		setValue(WideType value);

		T value;
	};

	// ------------------------------------------------------------------------

	/// \brief	Invert value
	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& x);

	/// \brief	Calculate the absolute value
	template<typename U>
	Saturated<U>
	abs(const Saturated<U>& x);

	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& a, const Saturated<U>& b);

	template<typename U>
	Saturated<U>
	operator + (const Saturated<U>& a, const Saturated<U>& b);

	template<typename U>
	inline bool
	operator == (const Saturated<U>& a, const Saturated<U>& b);

	template<typename U>
	inline bool
	operator != (const Saturated<U>& a, const Saturated<U>& b);
}

#include "saturated_impl.hpp"

#endif	// MODM_SATURATED_HPP
