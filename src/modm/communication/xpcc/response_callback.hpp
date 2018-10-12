/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, 2011, Georgi Grinshpun
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2014, Niklas Hauser
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

#ifndef	XPCC_RESPONSE_CALLBACK_HPP
#define	XPCC_RESPONSE_CALLBACK_HPP

#include <modm/container/smart_pointer.hpp>

#include "backend/backend_interface.hpp"
#include "communicatable.hpp"

namespace xpcc
{
	/**
	 * \brief 		Callback type, which has to be passed to communication during
	 *				actioncall in order to be able to receive a response.
	 *
	 * Is a \b Functor.
	 *
	 * \ingroup		modm_communication_xpcc
	 */
	class ResponseCallback
	{
	public:
		typedef void (Communicatable::*Function)(const Header& header, const uint8_t *type);

	public:
		ResponseCallback() : component(nullptr), function(nullptr) {}

		/**
		 * Set the method that will be called when a response is received.
		 *
		 * \param	componentObject	Pointer to a component object
		 * \param	memberFunction	Pointer to a function of the component object
		 */
		template <typename C, typename P>
		ResponseCallback(C *componentObject, void (C::*memberFunction)(const Header& header, const P* packet)) :
			component(reinterpret_cast<Communicatable *>(componentObject)),
			function(reinterpret_cast<Function>(memberFunction))/*,
			packetSize(sizeof(P))*/
		{
		}

		/**
		 * Set the method that will be called when a response is received.
		 *
		 * This specialization is to be used in case of no payload. The second
		 * parameter of the method has to be a void*.
		 *
		 * \param	componentObject	Pointer to a component object
		 * \param	memberFunction	Pointer to a function of the component object
		 */
		template <typename C>
		ResponseCallback(C *componentObject, void (C::*memberFunction)(const Header& header)) :
			component(reinterpret_cast<Communicatable *>(componentObject)),
			function(reinterpret_cast<Function>(memberFunction))/*,
			packetSize(0)*/
		{
		}

		inline bool
		isCallable() const
		{
			return component != nullptr and function != nullptr;
		}

		/// \todo check packet size?
		inline void
		call(const Header& header, const modm::SmartPointer &payload) const
		{
			if (isCallable()) {
				(component->*function)(header, payload.getPointer());
			}
			// TODO spezieller Aufruf für packetgröße = 0, funktioniert zwar
			// auch ohne ist aber extrem unschön!
		}

	protected:
		Communicatable * const component;
		Function const function;
		/*uint8_t packetSize;*/
	};

}

#endif // XPCC_RESPONSE_CALLBACK_HPP
