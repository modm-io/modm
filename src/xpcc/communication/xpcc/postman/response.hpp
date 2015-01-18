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
class ActionResponse
{
public:
	ActionResponse()
	:	response(Response::Negative)
	{}

	ActionResponse(Response response)
	:	response(response)
	{}

	ActionResponse(T data)
	:	response(Response::Positive), data(data)
	{}

	Response response;
	T data;
};

template<>
class ActionResponse<void>
{
public:
	ActionResponse()
	:	response(Response::Negative)
	{}

	ActionResponse(Response response)
	:	response(response)
	{}

	Response response;
};

}	// namespace xpcc

#endif // XPCC_RESPONSE_HPP
