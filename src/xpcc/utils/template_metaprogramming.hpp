// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2007-2009 by Mike Sharov <msharov@users.sourceforge.net>
//
// This implementation is adapted from the Loki library, distributed under
// the MIT license with Copyright (c) 2001 by Andrei Alexandrescu.

#ifndef XPCC_TM__TEMPLATE_METAPROGRAMMING_HPP
#define XPCC_TM__TEMPLATE_METAPROGRAMMING_HPP

#include "macros.hpp"

/**
 * \ingroup		utils
 * \defgroup	tm	Template Metaprogramming
 * 
 * Helper classes for some basic template metaprogramming.
 * 
 * \see		http://en.wikipedia.org/wiki/Template_metaprogramming
 */

namespace xpcc
{
	namespace tm
	{
		/**
		 * \brief	An empty type useful as a placeholder.
		 * \ingroup	tm
		 */
		class NullType
		{
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Selects type Result = (flag == true) ? T : U
		 * \ingroup	tm
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
		 * \ingroup	tm
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
		 * \ingroup	tm
		 */
		template <typename T, typename U>
		class Conversion
		{
			typedef char UT;
			typedef short TT;
			static UT Test (U);
			static TT Test (...);
			static T MakeT (void);
		public:
#ifdef __DOXYGEN__
			enum {
				exists = false,
				existsBothWays = false,
				isSameType = false
			};
#else
			enum {
				exists = (sizeof(UT) == sizeof(Test(MakeT()))),
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
		 * \c SuperSubclass<T,U>::value is \c true when \c U is derived from \c T, or when \c U is \c T
		 * 
		 * \ingroup	tm
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
		 * \ingroup	tm
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
		
		template <int x> struct static_assert_test
		{
		};
		
		/**
		 * \brief		Static assert support
		 *
		 * Inspired by the BOOST_STATIC_ASSERT macro.
		 * 
		 * \see	http://www.boost.org/doc/libs/1_43_0/doc/html/boost_staticassert.html
		 * \ingroup	tm
		 */
#ifdef __DOXYGEN__
		#define	XPCC__STATIC_ASSERT(condition)
#else
		#define	XPCC__STATIC_ASSERT(condition) \
			typedef ::xpcc::tm::static_assert_test<\
				sizeof(::xpcc::tm::STATIC_ASSERTION_FAILURE< (bool) (condition) >) >\
					CONCAT(static_assert_typedef_, __LINE__)
#endif
	}
}

#endif // XPCC_TM__TEMPLATE_METAPROGRAMMING_HPP

