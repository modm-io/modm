/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SATURATED_HPP
	#error	"Don't include this file directly use 'math/saturated.hpp' instead!"
#endif

#ifdef __AVR__
	// include faster implementations written in assembler
	#include "saturated__avr_u8_impl.hpp"
	#include "saturated__avr_s8_impl.hpp"
#endif

namespace modm
{
	template<typename T>
	Saturated<T>::Saturated() :
		value()
	{
	}

	template<typename T>
	Saturated<T>::Saturated(const T& initialValue) :
		value(initialValue)
	{
	}

	template<typename T>
	void
	Saturated<T>::setValue(WideType in)
	{
		if (in > std::numeric_limits<T>::max()) {
			value = std::numeric_limits<T>::max();
		}
		else if (in < std::numeric_limits<T>::min()) {
			value = std::numeric_limits<T>::min();
		}
		else {
			value = static_cast<T>(in);
		}
	}

	template<typename T>
	Saturated<T>&
	Saturated<T>::operator += (const Saturated& other)
	{
		WideType temp = static_cast<WideType>(this->value) +
						static_cast<WideType>(other.value);
		setValue(temp);

		return *this;
	}

	template<typename T>
	Saturated<T>&
	Saturated<T>::operator -= (const Saturated& other)
	{
		WideType temp = static_cast<WideType>(value) -
						static_cast<WideType>(other.value);
		setValue(temp);

		return *this;
	}

	template<typename T>
	void
	Saturated<T>::absolute()
	{
		if (std::is_signed_v<T>)
		{
			if (value < 0) {
				value = -value;
			}
		}
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& x)
	{
		using WideType = modm::WideType<U>;

		WideType temp = - static_cast<WideType>(x.value);

		Saturated<U> result;
		result.setValue(temp);
		return result;
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	abs(const Saturated<U>& x)
	{
		Saturated<U> result(x);

		if (std::is_signed_v<U>) {
			result.absolute();
		}
		return result;
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& a, const Saturated<U>& b)
	{
		Saturated<U> t(a);
		t -= b;
		return t;
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	operator + (const Saturated<U>& a, const Saturated<U>& b)
	{
		Saturated<U> t(a);
		t += b;
		return t;
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	inline bool
	operator == (const Saturated<U>& a, const Saturated<U>& b)
	{
		return (a.value == b.value);
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	inline bool
	operator != (const Saturated<U>& a, const Saturated<U>& b)
	{
		return (a.value != b.value);
	}

	// ----------------------------------------------------------------------------

	template<>
	void
	Saturated<uint8_t>::absolute()
	{
	}

	template<>
	void
	Saturated<uint16_t>::absolute()
	{
	}

	template<>
	void
	Saturated<uint32_t>::absolute()
	{
	}
}
