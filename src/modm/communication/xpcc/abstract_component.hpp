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

#ifndef	XPCC_ABSTRACT_COMPONENT_HPP
#define	XPCC_ABSTRACT_COMPONENT_HPP

#include <stdint.h>

#include "dispatcher.hpp"
#include "communicatable.hpp"
#include "communicator.hpp"
#include "response_handle.hpp"

namespace xpcc
{
	/**
	 * \brief	Abstract base class for a component
	 *
	 * This class give to components an interface to xpcc communication, but
	 * effectively it forwards all requests to the internal created Communicator,
	 * which is bound to this AbstractComponent.
	 *
	 * TODO why do we use the communicator here?
	 *
	 * It is a good convention to give to a component which extends
	 * xpcc::AbstractComponent an update() method which is to be called
	 * during the main loop.
	 *
	 * The programmer is always encouraged to bundle special functionality in
	 * separate classes called Tasks. These Tasks will usually not be
	 * complete components but want send messages in the name of a parent
	 * component. Use xpcc::CommunicatableTask for such type of Tasks.
	 *
	 * \see		modm::Task
	 * \see		xpcc::Communicatable
	 *
	 * \ingroup	modm_communication_xpcc
	 */
	class AbstractComponent : public Communicatable
	{
	public:
		/**
		 * \brief	Constructor
		 *
		 * \param	ownIdentifier	Identifier of the component, must be unique
		 * 							within the network.
		 * \param	communication	Communication class used to send messages
		 */
		AbstractComponent(const uint8_t ownIdentifier,
				Dispatcher &communication) :
			communicator(ownIdentifier, communication)
		{
		}

#ifdef __DOXYGEN__
		/**
		 * \brief	Update method
		 *
		 * A component can feature an update method which might be called on
		 * every cycle of the main loop.
		 *
		 * This method is used in our robot to implement the behaviour of
		 * the component. All other methods set some flags which are then
		 * evaluated by a statemachine here.
		 */
		void
		update(...);
#endif

		inline xpcc::Communicator *
		getCommunicator();

	protected:
		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier);

		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier,
				ResponseCallback& responseCallback);

		template<typename T>
		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data);

		template<typename T>
		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data,
				ResponseCallback& responseCallback);


		inline void
		publishEvent(uint8_t eventIdentifier);

		template<typename T>
		inline void
		publishEvent(uint8_t eventIdentifier, const T& data);


		inline void
		sendResponse(const ResponseHandle& handle);

		template<typename T>
		inline void
		sendResponse(const ResponseHandle& handle, const T& data);

		template<typename T>
		inline void
		sendNegativeResponse(const ResponseHandle& handle, const T& data);

		inline void
		sendNegativeResponse(const ResponseHandle& handle);

		/*inline operator xpcc::Communicator* ()
		{
			return &this->communicator;
		}*/

	private:
		Communicator communicator;
	};
}

#include "abstract_component_impl.hpp"

#endif // XPCC_ABSTRACT_COMPONENT_HPP
