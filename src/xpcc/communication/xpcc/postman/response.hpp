/*
 * Copyright (c) 2015, Niklas Hauser
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_RESPONSE_HPP
#define	XPCC_RESPONSE_HPP

#include "../backend/backend_interface.hpp"
#include <xpcc/processing/resumable.hpp>

namespace xpcc
{

/**
 * Response to an action call.
 *
 * @ingroup	xpcc_comm
 */
enum class
Response : bool
{
	Negative = false,
	Positive = true
};

/**
 * Action Response with optional return data.
 *
 * @ingroup	xpcc_comm
 */
template< typename T = void>
class ActionResult
{
public:
	ActionResult()
	:	response(Response::Negative)
	{}

	ActionResult(Response response)
	:	response(response)
	{}

	ActionResult(T data)
	:	response(Response::Positive), data(data)
	{}

	Response response;
	T data;
};

template<>
class ActionResult<void>
{
public:
	ActionResult()
	:	response(Response::Negative)
	{}

	ActionResult(Response response)
	:	response(response)
	{}

	Response response;
};

template < typename T >
using ActionResponse = xpcc::ResumableResult< xpcc::ActionResult< T > >;

}	// namespace xpcc

#endif // XPCC_RESPONSE_HPP
