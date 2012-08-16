// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2010, Roboterclub Aachen e.V.
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
 */
// ----------------------------------------------------------------------------

#include <xpcc/debug/logger.hpp>

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

#include "communication/identifier.hpp"
#include "communication/packets.hpp"

#include "sender.hpp"

// ----------------------------------------------------------------------------
component::Sender::Sender(uint8_t id, xpcc::Dispatcher *communication) :
	xpcc::AbstractComponent(id, communication),
	positionCallback(this, &Sender::getPositionCallback),
	timer(2000)
{
}

// ----------------------------------------------------------------------------
void
component::Sender::initialize()
{
	uint8_t mask = static_cast<uint8_t>(Adc::ChannelMask::PIO1_4);

	Adc::configurePins(mask);
	Adc::initialize();
	Adc::startConversion(mask);
}

// ----------------------------------------------------------------------------
void
component::Sender::getPositionCallback(const xpcc::Header&,
		const robot::packet::Position *parameter)
{
	XPCC_LOG_INFO << XPCC_FILE_INFO
			<< "get position callback: x=" << parameter->x
			<< ", y=" << parameter->y << xpcc::endl;
}

// ----------------------------------------------------------------------------
void
component::Sender::update()
{
	if (timer.isExpired())
	{
		XPCC_LOG_INFO << XPCC_FILE_INFO << "sender update" << xpcc::endl;
		
		robot::packet::Position xpccPosition(position, 20);
		
		this->callAction(
				robot::component::RECEIVER,
				robot::action::SET_POSITION,
				xpccPosition);
		
		this->callAction(
				robot::component::RECEIVER,
				robot::action::GET_POSITION,
				positionCallback);

		this->publishEvent(robot::event::POSITION, xpccPosition);
	}

	uint16_t val;
	if (Adc::read(val, Adc::Channel::PIO1_4)) {
		position = val;
	}


}
