/*
 * Copyright (c) 2020, Niklas Hauser
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

namespace modm::amnb
{

/// @ingroup modm_communication_amnb
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

/// @ingroup modm_communication_amnb
struct Listener
{
	inline
	Listener(int command, void(*listener)(uint8_t))
	:	command(command), callback((void*)listener),
		redirect([](const Message &msg, void *cb)
		{
			reinterpret_cast<decltype(listener)>(cb)(msg.address());
		})
	{}

	template<typename T>
	Listener(int command, void(*listener)(uint8_t, const T&))
	:	command(command), callback((void*)listener),
		redirect([](const Message &msg, void* cb)
		{
			if (const T* arg = msg.get<T>(); arg and msg.length() == sizeof(T))
				reinterpret_cast<decltype(listener)>(cb)(msg.address(), *arg);
		})
	{}

	inline
	Listener(int command, void(*listener)(uint8_t, const uint8_t*, size_t))
	:	command(command), callback((void*)listener),
		redirect([](const Message &msg, void* cb)
		{
			if (const uint8_t* arg = msg.get<uint8_t>(); arg)
				reinterpret_cast<decltype(listener)>(cb)(msg.address(), arg, msg.length());
		})
	{}

protected:
	const uint8_t command;
	void *const callback;
	void (*const redirect)(const Message &msg, void *cb);

	inline
	void call(const Message &msg) const
	{
		redirect(msg, callback);
	}

	template< size_t, size_t > friend class Node;
};

/// @ingroup modm_communication_amnb
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

/// @ingroup modm_communication_amnb
template< typename T >
inline Response
ErrorResponse(T error)
{
	Response res(error);
	res.msg.setType(Type::UserError);
	return res;
}

/// @ingroup modm_communication_amnb
struct Action
{
	inline
	Action(int command, Response(*action)())
	:	command(command), callback((void*)action),
		redirect([](const Message &msg, void *cb) -> Message
		{
			if (msg.length() == 0)
				return reinterpret_cast<decltype(action)>(cb)().msg;
			return Response(Error::WrongArgumentSize).msg;
		})
	{}

	template<typename T>
	Action(int command, Response(*action)(const T&))
	:	command(command), callback((void*)action),
		redirect([](const Message &msg, void* cb) -> Message
		{
			const T* arg = msg.get<T>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			if (msg.length() < sizeof(T))
				return Response(Error::WrongArgumentSize).msg;
			return reinterpret_cast<decltype(action)>(cb)(*arg).msg;
		})
	{}

	template<typename T>
	Action(int command, Response(*action)(const uint8_t*, size_t))
	:	command(command), callback((void*)action),
		redirect([](const Message &msg, void* cb) -> Message
		{
			const uint8_t* arg = msg.get<uint8_t>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			return reinterpret_cast<decltype(action)>(cb)(arg, msg.length()).msg;
		})
	{}

	inline
	Action(int command, void(*action)())
	:	command(command), callback((void*)action),
		redirect([](const Message &msg, void *cb) -> Message
		{
			if (msg.length() == 0) {
				reinterpret_cast<decltype(action)>(cb)();
				return Response().msg;
			}
			return Response(Error::WrongArgumentSize).msg;
		})
	{}

	template<typename T>
	Action(int command, void(*action)(const T&))
	:	command(command), callback((void*)action),
		redirect([](const Message &msg, void* cb) -> Message
		{
			const T* arg = msg.get<T>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			if (msg.length() < sizeof(T))
				return Response(Error::WrongArgumentSize).msg;
			reinterpret_cast<decltype(action)>(cb)(*arg);
			return Response().msg;
		})
	{}

	template<typename T>
	Action(int command, void(*action)(const uint8_t*, size_t))
	:	command(command), callback((void*)action),
		redirect([](const Message &msg, void* cb) -> Message
		{
			const uint8_t* arg = msg.get<uint8_t>();
			if (arg == nullptr)
				return Response(Error::ResponseAllocationFailed).msg;
			reinterpret_cast<decltype(action)>(cb)(arg, msg.length());
			return Response().msg;
		})
	{}

protected:
	const uint8_t command;
	void *const callback;
	Message (*const redirect)(const Message &msg, void *cb);

	inline
	Message call(const Message &msg) const
	{
		return redirect(msg, callback);
	}

	template< size_t, size_t > friend class Node;
};

/// @ingroup modm_communication_amnb
template< class ReturnType = void, class ErrorType = void >
struct Result
{
	Result() = default;

	bool
	hasError() const { return syserr != Error::Ok; }

	Error
	error() const { return syserr; }

	const ErrorType*
	resultError() const { return errval; }

	const ReturnType*
	result() const { return retval; }

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

	Error
	error() const { return syserr; }

	const Error*
	resultError() const { return &syserr; }

	const ReturnType*
	result() const { return retval; }

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

	Error
	error() const { return syserr; }

	const Error*
	resultError() const { return &syserr; }

	const Error*
	result() const { return &syserr; }

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

}

#if MODM_HAS_IOSTREAM
#include <modm/io/iostream.hpp>

namespace modm
{

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

