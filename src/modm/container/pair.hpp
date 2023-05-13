/*
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_PAIR_HPP
#define	MODM_PAIR_HPP

#include <utility>

namespace modm
{
	/**
	 * \brief		Pair<FirstType, SecondType> is a heterogeneous pair
	 *
	 * This class holds one object of type FirstType and one object of type
	 * SecondType.
	 *
	 * It provides only the default constructors and public variables to enable
	 * usage as POD (plain old data) object. Only with this characteristic it
	 * is possible to storage objects of this type inside the flash-memory
	 * which is desirable for example for the interpolation classes.
	 *
	 * \see	modm::interpolation::Linear
	 *
	 * This constraint make a the usage rather unusual. See this example:
	 * \code
	 * // construction
	 * modm::Pair<uint8_t, uint16_t> pair = { 12, -12345 };
	 *
	 * // this typedef is necessary to be able to use
	 * // the type inside the FLASH macro
	 * typedef modm::Pair<int8_t, int8_t> MyPair;
	 *
	 * // put some values into the flash memory
	 * FLASH_STORAGE(MyPair pairs[3]) =
	 * {
	 *     { 0, 10 },
	 *     { 2, 30 },
	 *     { 5, 60 }
	 * };
	 * \endcode
	 *
	 * As this class has no constructor you need to use the following syntax
	 * to initialize an instance of this class:
	 * \code
	 *	modm::Pair<int, float> pair = { 1203, 1035.234 };
	 * \endcode
	 *
	 * \note	Even if this class provides public attributes it is recommend to use
	 * 			the access methods, because as soon as C++0x is available the
	 * 			implementation will be changed and the attributes will be made private.
	 * 			Until then just ignore that you can access them directly ;-)
	 *
	 * \author		Fabian Greif
	 * \ingroup		modm_container
	 */
	template<typename T1, typename T2>
	class Pair : public std::pair<T1, T2>
	{
	public:
		using FirstType = T1;
		using SecondType = T2;

	public:
		using std::pair<T1, T2>::pair;

		FirstType&
		getFirst()
		{
			return this->first;
		}

		const FirstType&
		getFirst() const
		{
			return this->first;
		}

		SecondType&
		getSecond()
		{
			return this->second;
		}

		const SecondType&
		getSecond() const
		{
			return this->second;
		}
	};
}

#endif	// MODM_PAIR_HPP
