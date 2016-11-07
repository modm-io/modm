/*
 * Copyright (c) 2010, 2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef ALGORITHM_BASE_HPP
#define ALGORITHM_BASE_HPP

namespace std
{
	/**
	 * @brief This does what you think it does.
	 * @ingroup sorting_algorithms
	 * @param  a  A thing of arbitrary type.
	 * @param  b  Another thing of arbitrary type.
	 * @return   The lesser of the parameters.
	 *
	 * This is the simple classic generic implementation.  It will work on
	 * temporary expressions, since they are only evaluated once, unlike a
	 * preprocessor macro.
	 */
	template<typename T>
	inline const T&
	min(const T& a, const T& b)
	{
		if (b < a)
			return b;
		else
			return a;
	}
	
	/**
	 * @brief This does what you think it does.
	 * @ingroup sorting_algorithms
	 * @param  a  A thing of arbitrary type.
	 * @param  b  Another thing of arbitrary type.
	 * @return   The greater of the parameters.
	 *
	 * This is the simple classic generic implementation.  It will work on
	 * temporary expressions, since they are only evaluated once, unlike a
	 * preprocessor macro.
	 */
	template<typename T>
	inline const T&
	max(const T& a, const T& b)
	{
		if (a < b)
			return b;
		else
			return a;
	}
	
	/**
	 * @brief This does what you think it does.
	 * @ingroup sorting_algorithms
	 * @param  a  A thing of arbitrary type.
	 * @param  b  Another thing of arbitrary type.
	 * @param  compare  A comparison functor.
	 * @return   The lesser of the parameters.
	 *
	 * This will work on temporary expressions, since they are only evaluated
	 * once, unlike a preprocessor macro.
	 */
	template<typename T, typename Compare>
	inline const T&
	min(const T& a, const T& b, Compare compare)
	{
		if (compare(b, a))
			return b;
		else
			return a;
	}
	
	/**
	 * @brief This does what you think it does.
	 * @ingroup sorting_algorithms
	 * @param  a  A thing of arbitrary type.
	 * @param  b  Another thing of arbitrary type.
	 * @param  compare  A comparison functor.
	 * @return   The greater of the parameters.
	 * 
	 * This will work on temporary expressions, since they are only evaluated
	 * once, unlike a preprocessor macro.
	 */
	template<typename T, typename Compare>
	inline const T&
	max(const T& a, const T& b, Compare compare)
	{
		if (compare(a, b))
			return b;
		else
			return a;
	}
}

#endif	// ALGORITHM_BASE_HPP
