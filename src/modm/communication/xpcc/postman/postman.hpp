/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_POSTMAN_HPP
#define	XPCC_POSTMAN_HPP

#include "../backend/backend_interface.hpp"
#include "response.hpp"

namespace xpcc
{
	/**
	 * \brief	Postman interface
	 *
	 * The Postman class is responsible of delivering messages the the
	 * components.
	 *
	 * \ingroup	modm_communication_xpcc
	 */
	class Postman
	{
	public:
		enum DeliverInfo
		{
			OK = 0,
			ERROR,
			NO_COMPONENT,
			NO_ACTION,
			WRONG_ACTION_PARAMETER,
			NO_EVENT,
			WRONG_EVENT_PARAMETER,
			NOT_IMPLEMENTED_YET_ERROR,
		};

	public:
		virtual
		~Postman();

		virtual DeliverInfo
		deliverPacket(const Header& header, const modm::SmartPointer& payload) = 0;

		/**
		 * \brief	Check if a component is available on this board
		 *
		 * \param	component	Id of the component
		 * \return	\c true if the postman is available to delivier messages
		 * 			to the given component, \c false otherwise.
		 */
		virtual bool
		isComponentAvailable(uint8_t component) const = 0;
	};
}

#endif // XPCC_POSTMAN_HPP
