/*
 * Copyright (c) 2009, 2011, Georgi Grinshpun
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_COMMUNICATOR_HPP
#define	XPCC_COMMUNICATOR_HPP

#include <stdint.h>

#include "communicatable.hpp"
#include "response_callback.hpp"
#include "response_handle.hpp"
#include "dispatcher.hpp"

namespace xpcc
{
	/**
	 * \brief	A adapter class which can be obtained from a AbstractComponent.
	 *
	 * Use it to enable communication for a group of classes. The component
	 * identifier will be taken from the AbstractComponent.
	 *
	 * This class is just a forwarder to the Dispatcher like AbstractComponent
	 * it also does.
	 *
	 * \ingroup	modm_communication_xpcc
	 */
	class Communicator : public Communicatable
	{
		friend class AbstractComponent;

	private:
		/**
		 * \brief	Constructor
		 *
		 * \param	ownIdentifier	Identifier of the component, must be unique
		 * 							within the network.
		 * \param	dispatcher		Communication class use to send messages
		 */
		Communicator(const uint8_t ownIdentifier,
				Dispatcher &dispatcher);

		Communicator(const Communicator&);

		Communicator&
		operator = (const Communicator&);

	public:
		inline uint8_t
		getIdentifier() const
		{
			return this->ownIdentifier;
		}

		void
		callAction(uint8_t receiver, uint8_t actionIdentifier);

		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, ResponseCallback& responseCallback);

		template<typename T>
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data);

		template<typename T>
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data, ResponseCallback& responseCallback);


		void
		publishEvent(uint8_t eventIdentifier);

		template<typename T>
		void
		publishEvent(uint8_t eventIdentifier, const T& data);


		void
		sendResponse(const ResponseHandle& handle);

		template<typename T>
		void
		sendResponse(const ResponseHandle& handle, const T& data);

		template<typename T>
		void
		sendNegativeResponse(const ResponseHandle& handle, const T& data);

		void
		sendNegativeResponse(const ResponseHandle& handle);

	private:
		const uint8_t ownIdentifier;
		Dispatcher & dispatcher;
	};
}

#include "communicator_impl.hpp"

#endif // XPCC_COMMUNICATOR_HPP
