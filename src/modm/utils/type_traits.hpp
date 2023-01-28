/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

namespace modm
{

/// @cond
namespace impl
{
	// Deduce signature from member function type
	// e.g. gives R(Args...) for R(T::*)(Args...) const
	template<typename F>
	struct mem_fn_helper;

	template<typename T, typename R, typename... Args>
	struct mem_fn_helper<R(T::*)(Args...)>
	{
		using type = R(Args...);
	};

	template<typename T, typename R, typename... Args>
	struct mem_fn_helper<R(T::*)(Args...) const>
	{
		using type = R(Args...);
	};

	template<typename T, typename R, typename... Args>
	struct mem_fn_helper<R(T::*)(Args...) &>
	{
		using type = R(Args...);
	};

	template<typename T, typename R, typename... Args>
	struct mem_fn_helper<R(T::*)(Args...) const&>
	{
		using type = R(Args...);
	};
}

template<typename F>
struct get_callable_signature;

template<typename R, typename... Args>
struct get_callable_signature<R(*)(Args...)>
{
    using type = R(Args...);
};

template<typename R, typename... Args>
struct get_callable_signature<R(Args...)>
{
    using type = R(Args...);
};

template<typename F>
    requires requires { &F::operator(); }
struct get_callable_signature<F>
{
    using type = typename impl::mem_fn_helper<decltype(&F::operator())>::type;
};
/// @endcond

/**
 * Deduce function signature type from callable
 *
 * Identity for function types
 * For functions objects the return type and parameters of
 * operator() are deduced
 *
 * e.g. deduces "void(int)" for "[&](int){}"
 *
 * @warning Not applicable to overload sets
 * @ingroup modm_utils
 */
template <typename T>
using get_callable_signature_t = typename get_callable_signature<T>::type;

}
