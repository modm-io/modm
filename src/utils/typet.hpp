// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2007-2009 by Mike Sharov <msharov@users.sourceforge.net>
//
// This implementation is adapted from the Loki library, distributed under
// the MIT license with Copyright (c) 2001 by Andrei Alexandrescu.

#ifndef XPCC__TYPET_HPP
#define XPCC__TYPET_HPP

namespace xpcc {
/// Template metaprogramming tools
namespace tm {

/// An empty type useful as a placeholder.
class NullType { };

/// Converts an integer to a type.
template <int v> struct Int2Type { enum { value = v }; };

/// Converts an type to a unique empty type.
template <typename T> struct Type2Type { typedef T OriginalType; };

/// Selects type Result = flag ? T : U
template <bool flag, typename T, typename U>
struct Select { typedef T Result; };
template <typename T, typename U>
struct Select<false, T, U> { typedef U Result; };

/// IsSameType<T,U>::value is true when T=U
template <typename T, typename U>
struct IsSameType { enum { value = false }; };
template <typename T>
struct IsSameType<T,T> { enum { value = true }; };

/// \brief Checks for conversion possibilities between T and U
/// Conversion<T,U>::exists is true if T is convertible to U
/// Conversion<T,U>::exists2Way is true if U is also convertible to T
/// Conversion<T,U>::sameType is true if U is T
template <typename T, typename U>
class Conversion {
    typedef char UT;
    typedef short TT;
    static UT Test (U);
    static TT Test (...);
    static T MakeT (void);
public:
    enum {
	exists = sizeof(UT) == sizeof(Test(MakeT())),
	exists2Way = exists && Conversion<U,T>::exists,
	sameType = false
    };
};
template <typename T>
struct Conversion<T, T> { enum { exists = true, exists2Way = true, sameType = true }; };
template <typename T>
struct Conversion<void, T> { enum { exists = false, exists2Way = false, sameType = false }; };
template <typename T>
struct Conversion<T, void> { enum { exists = false, exists2Way = false, sameType = false }; };
template <>
struct Conversion<void, void> { enum { exists = true, exists2Way = true, sameType = true }; };

/// SuperSubclass<T,U>::value is true when U is derived from T, or when U is T
template <typename T, typename U>
struct SuperSubclass {
    enum { value = (::xpcc::tm::Conversion<const volatile U*, const volatile T*>::exists &&
		    !::xpcc::tm::Conversion<const volatile T*, const volatile void*>::sameType) };
    enum { dontUseWithIncompleteTypes = sizeof(T)==sizeof(U) };	// Dummy enum to make sure that both classes are fully defined.
};
template <>
struct SuperSubclass<void, void> { enum { value = false }; };
template <typename U>
struct SuperSubclass<void, U> {
    enum { value = false };
    enum { dontUseWithIncompleteTypes = 0==sizeof(U) };
};
template <typename T>
struct SuperSubclass<T, void> {
    enum { value = false };
    enum { dontUseWithIncompleteTypes = 0==sizeof(T) };
};

/// SuperSubclassStrict<T,U>::value is true when U is derived from T
template <typename T, typename U>
struct SuperSubclassStrict {
    enum { value = SuperSubclass<T,U>::value &&
		    !::xpcc::tm::Conversion<const volatile T*, const volatile U*>::sameType };
};

// static assert support
template <bool> struct CompileTimeError;
template <> struct CompileTimeError<true> {};
#define static_assert(cond,msg)	{ ::ustl::tm::CompileTimeError<!!(cond)> ERROR_##msg; (void) ERROR_##msg; }

} // namespace tm
} // namespace ustl

#endif // XPCC__TYPET_HPP

