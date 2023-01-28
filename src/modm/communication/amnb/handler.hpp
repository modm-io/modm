/*
 * Copyright (c) 2020, Niklas Hauser
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "message.hpp"
#include <modm/processing.hpp>
#include <modm/utils/inplace_any.hpp>
#include <modm/utils/inplace_function.hpp>
#include <modm/utils/type_traits.hpp>
#include <functional>

#ifndef MODM_AMNB_HANDLER_STORAGE
/// @ingroup modm_communication_amnb
#define MODM_AMNB_HANDLER_STORAGE sizeof(void*)
#endif

namespace modm::amnb
{
/// @ingroup modm_communication_amnb
/// @{

enum class Error : uint8_t
{
	Ok = 0,

	RequestTimeout = 1,
	WrongArgumentSize = 2,
	NoAction = 3,
	ResponseAllocationFailed = 4,
	RequestAllocationFailed = 5,
	UserError = 6,
	Unknown = 7,
};

struct Listener
{
	template <typename L, typename F = get_callable_signature_t<std::remove_cvref_t<L>>>
	Listener(int command, L&& listener)
	:	command(command), callback(std::in_place_type<function<F>>, std::forward<L>(listener)),
		redirect(getRedirect(static_cast<F*>(nullptr)))
	{}

protected:
	template<class Signature>
	using function = modm::inplace_function<Signature, MODM_AMNB_HANDLER_STORAGE, alignof(void*)>;
	using Storage = modm::inplace_any<MODM_AMNB_HANDLER_STORAGE + sizeof(void*)>;
	using Redirect = void(const Message&, Storage*);

	auto getRedirect(void(*)(uint8_t))
	{
		return [](const Message &msg, Storage *cb) {
			(*any_cast<function<void(uint8_t)>>(cb))(msg.address());
		};
	}

	template<typename T>
	auto getRedirect(void(*)(uint8_t, const T&))
	{
		return [](const Message &msg, Storage *cb) {
			if (const T* arg = msg.get<T>(); arg and msg.length() == sizeof(T))
				(*any_cast<function<void(uint8_t, const T&)>>(cb))(msg.address(), *arg);
		};
	}

	auto getRedirect(void(*)(uint8_t, const uint8_t*, size_t))
	{
		return [](const Message &msg, Storage *cb) {
			if (const uint8_t* arg = msg.get<uint8_t>(); arg)
				(*any_cast<function<void(uint8_t, const uint8_t*, size_t)>>(cb))(msg.address(), arg, msg.length());
		};
	}

	const uint8_t command;
	Storage callback;
	Redirect *const redirect;
	inline void call(const Message &msg) { redirect(msg, &callback); }
	template< size_t, size_t > friend class Node;
};

struct Response
{
	inline
	Response()
	:	msg(0, 0, Type::Response) {}

	template<typename T>
	Response(T retval)
	:	msg(0, 0, sizeof(T), Type::Response)
	{
		if (T* arg = msg.get<T>(); arg) *arg = retval;
		else {
			msg = Message(0, 0, 1, Type::Error);
			*msg.get<Error>() = Error::ResponseAllocationFailed;
		}
	}

	Message msg;
};

template< typename T >
inline Response
ErrorResponse(T error)
{
	Response res(error);
	res.msg.setType(Type::UserError);
	return res;
}

struct Action
{
	template <typename A, typename F = get_callable_signature_t<std::remove_cvref_t<A>>>
	Action(int command, A&& action)
	:	command(command), callback(std::in_place_type<function<F>>, std::forward<A>(action)),
		redirect(getRedirect(static_cast<F*>(nullptr)))
	{}

protected:
	template<class Signature>
	using function = modm::inplace_function<Signature, MODM_AMNB_HANDLER_STORAGE, alignof(void*)>;
	using Storage = modm::inplace_any<MODM_AMNB_HANDLER_STORAGE + sizeof(void*)>;
	using Redirect = Message(const Message &msg, Storage *cb);

	auto getRedirect(Response(*)())
	{
		return [](const Message &msg, Storage *cb) -> Message {
			if (msg.length() == 0)
				return (*any_cast<function<Response()>>(cb))().msg;
			return Response(Error::WrongArgumentSize).msg;
		};
	}

	template<typename T>
	auto getRedirect(Response(*)(const T&))
	{
		return [](const Message &msg, Storage *cb) -> Message
		{
			const T* arg = msg.get<T>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			if (msg.length() < sizeof(T))
				return Response(Error::WrongArgumentSize).msg;
			return (*any_cast<function<Response(const T&)>>(cb))(*arg).msg;
		};
	}

	auto getRedirect(Response(*)(const uint8_t*, size_t))
	{
		return [](const Message &msg, Storage *cb) -> Message
		{
			const uint8_t* arg = msg.get<uint8_t>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			return (*any_cast<function<Response(const uint8_t*, size_t)>>(cb))(arg, msg.length()).msg;
		};
	}

	auto getRedirect(void(*)())
	{
		return [](const Message &msg, Storage *cb) -> Message
		{
			if (msg.length() == 0) {
				(*any_cast<function<void()>>(cb))();
				return Response().msg;
			}
			return Response(Error::WrongArgumentSize).msg;
		};
	}

	template<typename T>
	auto getRedirect(void(*)(const T&))
	{
		return [](const Message &msg, Storage *cb) -> Message
		{
			const T* arg = msg.get<T>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			if (msg.length() < sizeof(T))
				return Response(Error::WrongArgumentSize).msg;
			(*any_cast<function<void(const T&)>>(cb))(*arg);
			return Response().msg;
		};
	}

	auto getRedirect(void(*)(const uint8_t*, size_t))
	{
		return [](const Message &msg, Storage *cb) -> Message
		{
			const uint8_t* arg = msg.get<uint8_t>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			(*any_cast<function<void(const uint8_t*, size_t)>>(cb))(arg, msg.length());
			return Response().msg;
		};
	}

	const uint8_t command;
	Storage callback;
	Redirect *const redirect;

	inline Message call(const Message &msg) { return redirect(msg, &callback); }
	template< size_t, size_t > friend class Node;
};

template< class ReturnType = void, class ErrorType = void >
struct Result
{
	Result() = default;

	bool
	hasError() const { return syserr != Error::Ok; }

	bool
	hasUserError() const { return syserr == Error::UserError; }

	Error
	error() const { return syserr; }

	const ErrorType*
	userError() const { return errval; }

	const ReturnType*
	result() const { return retval; }

	explicit
	operator bool() const { return not hasError(); }

	const ReturnType&
	operator *() const { return *result(); }

protected:
	Result(Message &msg)
	{
		switch(msg.type())
		{
			case Type::Response:
				if (retval = msg.get<ReturnType>(); not retval)
					syserr = Error::ResponseAllocationFailed;
				break;
			case Type::UserError:
				if (errval = msg.get<ErrorType>(); errval)
					syserr = Error::UserError;
				else
					syserr = Error::ResponseAllocationFailed;
				break;
			case Type::Error:
				syserr = *msg.get<Error>();
				break;
			default:
				syserr = Error::Unknown;
		}
	}
	union {
		const ReturnType *retval{nullptr};
		const ErrorType *errval;
	};
	Error syserr{Error::Ok};

	template< size_t, size_t > friend class Node;
};

/// @cond
template< class ReturnType>
struct Result<ReturnType, void>
{
	Result() = default;

	bool
	hasError() const { return syserr != Error::Ok; }

	bool
	hasUserError() const { return syserr == Error::UserError; }

	Error
	error() const { return syserr; }

	const Error*
	userError() const { return &syserr; }

	const ReturnType*
	result() const { return retval; }

	explicit
	operator bool() const { return not hasError(); }

	const ReturnType&
	operator *() const { return *result(); }

protected:
	Result(Message &msg)
	{
		switch(msg.type())
		{
			case Type::Response:
				retval = msg.get<ReturnType>();
				if (not retval) syserr = Error::ResponseAllocationFailed;
				break;
			case Type::Error:
				syserr = *msg.get<Error>();
				break;
			case Type::UserError:
				syserr = Error::UserError;
				msg.deallocate();
				break;
			default:
				syserr = Error::Unknown;
		}
	}
	const ReturnType *retval{nullptr};
	Error syserr{Error::Ok};

	template< size_t, size_t > friend class Node;
};

template<>
struct Result<void, void>
{
	Result() = default;

	bool
	hasError() const { return syserr != Error::Ok; }

	bool
	hasUserError() const { return syserr == Error::UserError; }

	Error
	error() const { return syserr; }

	const Error*
	userError() const { return &syserr; }

	const Error*
	result() const { return &syserr; }

	explicit
	operator bool() const { return not hasError(); }

protected:
	Result(Message &msg)
	{
		switch(msg.type())
		{
			case Type::Response:
				break;
			case Type::Error:
				syserr = *msg.get<Error>();
				break;
			case Type::UserError:
				syserr = Error::UserError;
				break;
			default:
				syserr = Error::Unknown;
		}
		msg.deallocate();
	}
	Error syserr{Error::Ok};

	template< size_t, size_t > friend class Node;
};
/// @endcond
/// @}

}

#if MODM_HAS_IOSTREAM
#include <modm/io/iostream.hpp>

namespace modm
{

/// @ingroup modm_communication_amnb
inline modm::IOStream&
operator << (modm::IOStream& s, const modm::amnb::Error error)
{
	using namespace modm::amnb;
	switch(error)
	{
		case Error::Ok:							s << "Ok";							break;
		case Error::NoAction:					s << "NoAction";					break;
		case Error::RequestTimeout:				s << "RequestTimeout";				break;
		case Error::WrongArgumentSize:			s << "WrongArgumentSize";			break;
		case Error::RequestAllocationFailed:	s << "RequestAllocationFailed";		break;
		case Error::ResponseAllocationFailed:	s << "ResponseAllocationFailed";	break;
		case Error::UserError:					s << "UserError";					break;
		case Error::Unknown:					s << "Unknown";						break;
	}
	return s;
}

} // modm namespace
#endif

