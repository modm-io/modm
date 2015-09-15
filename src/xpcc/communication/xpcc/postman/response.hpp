// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
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
