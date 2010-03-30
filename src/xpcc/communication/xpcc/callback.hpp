// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__CALLBACK_HPP
#define	XPCC__CALLBACK_HPP

#include <xpcc/data_structure/smart_pointer.hpp>
#include "abstract_component.hpp"
#include "backend/backend_interface.hpp"

namespace xpcc
{
	/**
	 * \brief 		Callback type, which has to be passed to communication during
	 *				actioncall in order to be able to receive a response.
	 * 
	 * Is a \b Functor.
	 * 
	 * \ingroup		communication
	 */
	class Callback
	{
	public:
		typedef void (AbstractComponent::*Function)(const Header& header, const uint8_t *type);

	public:
		Callback();


		/**
		 * Set the method that will be called when a response is received.
		 *
		 * \param	component	Pointer to a component object
		 * \param	function	Pointer to a function of the component object
		 */
		template <typename C, typename P>
		inline void
		init( C *component, void (C::*function)(const Header& header, const P* packet)  )
		{
			this->component = static_cast<AbstractComponent*>( component );
			this->function = reinterpret_cast<Function>(function);
			this->packetSize = sizeof( P );
		}
		

		/// \todo control packet size?
		inline void
		call(const Header& header, const SmartPointer &payload) const
		{
			if (component != 0) {
				(component->*function)(header, payload.getPointer());
			}
		}
		
	protected:
		AbstractComponent *component;
		Function function;
		uint8_t packetSize;
	};

}

#endif // XPCC__CALLBACK_HPP
