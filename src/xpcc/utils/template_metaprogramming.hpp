/*
 * Copyright (c) 2010-2012, Fabian Greif
 * Copyright (c) 2013, 2016, Niklas Hauser
 * Copyright (c) 2013, David Hebbeker
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
// This implementation is adapted from the Loki library, distributed under
// the MIT license with Copyright (c) 2001 by Andrei Alexandrescu.

#ifndef XPCC_TMP__TEMPLATE_METAPROGRAMMING_HPP
#define XPCC_TMP__TEMPLATE_METAPROGRAMMING_HPP

#include <modm/architecture/utils.hpp>

/**
 * \ingroup		utils
 * \defgroup	tmp		Template Metaprogramming
 * 
 * Helper classes for some basic template metaprogramming. Template
 * metaprogramming is a programming technique in which templates are used by a
 * compiler to generate temporary source code, which is merged by the compiler
 * with the rest of the source code and then compiled.
 * 
 * \see		http://en.wikipedia.org/wiki/Template_metaprogramming
 */

namespace xpcc
{
	/**
	 * \brief	Template Metaprogramming
	 * \ingroup	tmp
	 */
	namespace tmp
	{
		/**
		 * \brief	An empty type useful as a placeholder.
		 * \ingroup	tmp
		 */
		class NullType
		{
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Selects type Result = (flag == true) ? T : U
		 * \ingroup	tmp
		 */
		template <bool flag, typename T, typename U>
		struct Select
		{
			typedef T Result;
		};
		
		template <typename T, typename U>
		struct Select<false, T, U>
		{
			typedef U Result;
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	SameType<T,U>::value is true when T=U
		 * \ingroup	tmp
		 */
		template <typename T, typename U>
		struct SameType
		{
			enum {
				value = false
			};
		};
		
		template <typename T>
		struct SameType<T, T>
		{
			enum {
				value = true
			};
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief Checks for conversion possibilities between T and U
		 * 
		 * - \c Conversion<T,U>::exists is \c true if \c T is convertible to \c U
		 * - \c Conversion<T,U>::existsBothWays is \c true if \c U is also convertible to \c T
		 * - \c Conversion<T,U>::isSameType is \c true if \c U is \c T
		 *
		 * \ingroup	tmp
		 */
		template <typename T, typename U>
		class Conversion
		{
#ifdef __DOXYGEN__
		public:
			enum {
				exists = false,
				existsBothWays = false,
				isSameType = false
			};
#else
			// Only works if sizeof(char) != sizeof(int)
			typedef char ConversionPossible;
			typedef int ConversionNotPossible;
			
			static ConversionPossible Test(U);
			static ConversionNotPossible Test(...);
			
			static T CreateT(void);
			
		public:
			enum {
				exists = (sizeof(ConversionPossible) == sizeof(Test(CreateT()))),
				existsBothWays = exists && Conversion<U, T>::exists,
				isSameType = false
			};
#endif
		};
		
		template <typename T>
		struct Conversion<T, T>
		{
			enum {
				exists = true,
				existsBothWays = true,
				isSameType = true
			};
		};
		
		template <typename T>
		struct Conversion<void, T>
		{
			enum {
				exists = false,
				existsBothWays = false,
				isSameType = false
			};
		};
		
		template <typename T>
		struct Conversion<T, void>
		{
			enum {
				exists = false,
				existsBothWays = false,
				isSameType = false
			};
		};
		
		template <>
		struct Conversion<void, void>
		{
			enum {
				exists = true,
				existsBothWays = true,
				isSameType = true
			};
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Check if U is derived from T or if U is T
		 * 
		 * \c SuperSubclass<T,U>::value is \c true when \c U is derived 
		 * from \c T, or when \c U is \c T.
		 * 
		 * \ingroup	tmp
		 */
		template <typename T, typename U>
		struct SuperSubclass
		{
			enum {
				value = (Conversion<const volatile U*, const volatile T*>::exists &&
						!Conversion<const volatile T*, const volatile void*>::isSameType)
			};
			// Dummy enum to make sure that both classes are fully defined.
			enum {
				dontUseWithIncompleteTypes = (sizeof(T) == sizeof(U))
			};
		};
		
		template <>
		struct SuperSubclass<void, void>
		{
			enum {
				value = false
			};
		};
		
		template <typename U>
		struct SuperSubclass<void, U>
		{
			enum {
				value = false
			};
			enum {
				dontUseWithIncompleteTypes = (0 == sizeof(U))
			};
		};
		
		template <typename T>
		struct SuperSubclass<T, void>
		{
			enum {
				value = false
			};
			enum {
				dontUseWithIncompleteTypes = (0 == sizeof(T))
			};
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Check if U is derived from T
		 * 
		 * \c SuperSubclassStrict<T,U>::value is \c true when \c U is derived from \c T
		 * 
		 * \ingroup	tmp
		 */
		template <typename T, typename U>
		struct SuperSubclassStrict
		{
			enum {
				value = SuperSubclass<T,U>::value &&
					   !Conversion<const volatile T*, const volatile U*>::isSameType
			};
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	These templates are a set of tools to allow a function
		 * 			template or a class template specialization to include or
		 * 			exclude itself from a set of matching functions or specializations
		 * 			based on properties of its template arguments.
		 * 
		 * Example:
		 * \code
		 * struct FloatingPointTraits
		 * {
		 *     template <typename T>
		 *     static typename tm::EnableIfCondition<sizeof(T) == 4, T>::type
		 *     max()
		 *     {
		 *         return 3.40282e+38;
		 *     }
		 *     
		 *     template <typename T>
		 *     static typename tm::EnableIfCondition<sizeof(T) == 8, T>::type
		 *     max()
		 *     {
		 *         return 1.79769e+308;
		 *     }
		 * };
		 * 
		 * // Usage:
		 * FloatingPointTraits::max<float>();
		 * FloatingPointTraits::max<double>();
		 * \endcode
		 * 
		 * \ingroup	tmp
		 */
		template <bool B, class T = void>
		struct EnableIfCondition
		{
			typedef T type;
		};
		
		/**
		 * \internal
		 */
		template <class T>
		struct EnableIfCondition<false, T>
		{
		};
		
		/**
		 * \ingroup	tmp
		 */
		template <class Conditional, class T = void>
		struct EnableIf : public EnableIfCondition< Conditional::value, T >
		{
		};
		
		// --------------------------------------------------------------------
		// helper classes for the static assert macro
		template <bool x>
		struct STATIC_ASSERTION_FAILURE;
		
		template <>
		struct STATIC_ASSERTION_FAILURE<true>
		{
			enum {
				value = 1
			};
		};
		
		template <int x>
		struct static_assert_test
		{
		};
		
		/**
		 * \brief		Static assert support
		 *
		 * Inspired by the BOOST_STATIC_ASSERT macro.
		 * 
		 * \see	http://www.boost.org/doc/libs/1_43_0/doc/html/boost_staticassert.html
		 * \ingroup	tmp
		 * \deprecated Use static_assert(bool_constexpr, string) (since C++11) instead.
		 */
#ifdef __DOXYGEN__
		#define	XPCC__STATIC_ASSERT(condition, msg)
#else
		// automatically use C++11 if possible
#	if __cplusplus < 201103L
		#define	XPCC__STATIC_ASSERT(condition, msg) 					\
			typedef ::xpcc::tmp::static_assert_test<					\
				sizeof(::xpcc::tmp::STATIC_ASSERTION_FAILURE< (bool) (condition) >) >\
					XPCC_CONCAT(static_assert_typedef_, __LINE__)
#	else
		#define	XPCC__STATIC_ASSERT(condition, msg) 					\
			static_assert(condition, msg)
#	endif
#endif
	}
}

#endif // XPCC_TMP__TEMPLATE_METAPROGRAMMING_HPP

