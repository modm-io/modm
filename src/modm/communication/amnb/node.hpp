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

#include "interface.hpp"
#include "handler.hpp"
#include <modm/processing.hpp>
#include <modm/container.hpp>

namespace modm::amnb
{

/// @author	Niklas Hauser
/// @ingroup modm_communication_amnb
template < size_t TxBufferSize = 2, size_t MaxHeapAllocation = 0 >
class Node : public modm::Resumable<6>
{
	static_assert(2 <= TxBufferSize, "TxBuffer must have at least two messages!");
public:
	Node(Device &device, uint8_t address): interface(device), address(address)
	{ setSeed(); }

	template< size_t Actions >
	Node(Device &device, uint8_t address, Action (&actions)[Actions])
	:	interface(device), actionList(actions), actionCount(Actions), address(address)
	{
		static_assert(Actions <= 0xff, "Actions list must be smaller than 255!");
		setSeed();
	}

	template< size_t Listeners >
	Node(Device &device, uint8_t address, Listener (&listeners)[Listeners])
	:	interface(device), listenerList(listeners), listenerCount(Listeners), address(address)
	{
		static_assert(Listeners <= 0xff, "Listeners list must be smaller than 255!");
		setSeed();
	}

	template< size_t Actions, size_t Listeners >
	Node(Device &device, uint8_t address, Action (&actions)[Actions], Listener (&listeners)[Listeners])
	:	interface(device), actionList(actions), listenerList(listeners),
		actionCount(Actions), listenerCount(Listeners), address(address)
	{
		static_assert(Actions <= 0xff, "Actions list must be smaller than 255!");
		static_assert(Listeners <= 0xff, "Listeners list must be smaller than 255!");
		setSeed();
	}

	void
	setAddress(uint8_t address)
	{
		this->address = address;
	}

	void
	setSeed(uint16_t seed)
	{ lfsr = seed; }

public:
	bool
	broadcast(uint8_t command)
	{
		if (tx_queue.isFull()) return false;
		return tx_queue.push(std::move(Message(address, command, Type::Broadcast)));
	}
	bool
	broadcast(uint8_t command, const uint8_t *data, size_t length)
	{
		if (tx_queue.isFull()) return false;
		Message msg(address, command, length, Type::Broadcast);
		uint8_t* t = msg.get<uint8_t>();
		if (t == nullptr) return false;
		std::memcpy(t, data, length);
		return tx_queue.push(std::move(msg));
	}
	template< typename T >
	bool
	broadcast(uint8_t command, const T &argument)
	{
		return broadcast(command, (const uint8_t *)&argument, sizeof(T));
	}

	template< class ReturnType = void, class ErrorType = void >
	modm::ResumableResult< Result<ReturnType, ErrorType> >
	request(uint8_t from, uint8_t command)
	{
		RF_BEGIN(0);
		request_msg = Message(from, command, Type::Request);
		RF_CALL(request());
		RF_END_RETURN(request_msg);
	}

	template< class ReturnType = void, class ErrorType = void >
	modm::ResumableResult< Result<ReturnType, ErrorType> >
	request(uint8_t from, uint8_t command, const uint8_t *data, size_t length)
	{
		RF_BEGIN(1);
		{
			request_msg = Message(from, command, length, Type::Request);
			uint8_t* arg = request_msg.get<uint8_t>();
			if (arg == nullptr) {
				request_msg = Response(Error::RequestAllocationFailed).msg;
				RF_RETURN(request_msg);
			}
			std::memcpy(arg, data, length);
		}
		RF_CALL(request());
		RF_END_RETURN(request_msg);
	}

	template< class ReturnType = void, class ErrorType = void, class T >
	modm::ResumableResult< Result<ReturnType, ErrorType> >
	request(uint8_t from, uint8_t command, const T &argument)
	{
		return request<ReturnType, ErrorType>(from, command, (uint8_t*)&argument, sizeof(T));
	}

public:
	void
	update()
	{
		transmit();
		receive();
	}

protected:
	modm::ResumableResult<void>
	transmit()
	{
		RF_BEGIN(2);
		while(1)
		{
			if (not tx_queue.isEmpty())
			{
				RF_WAIT_WHILE(isResumableRunning(3));
				RF_CALL(send(tx_queue.get()));
				tx_queue.pop();
			}
			RF_YIELD();
		}
		RF_END();
	}

	modm::ResumableResult<void>
	send(Message &msg)
	{
		RF_BEGIN(3);

		msg.setValid();
		tx_counter = std::min(MIN_TX_TRIES, uint8_t(msg.command() >> (8 - PRIORITY_BITS)));
		while(1)
		{
			while (interface.isMediumBusy())
			{
				RF_WAIT_WHILE(interface.isMediumBusy());
				reschedule(RESCHEDULE_MASK_SHORT);
				RF_WAIT_UNTIL(tx_timer.isExpired());
			}

			if (RF_CALL(interface.transmit(&msg)) == InterfaceStatus::Ok)
				break;

			if (--tx_counter == 0) break;

			// a collision or other write issue occurred
			RF_WAIT_WHILE(interface.isMediumBusy());
			reschedule(RESCHEDULE_MASK_LONG);
			RF_WAIT_UNTIL(tx_timer.isExpired());
		}
		RF_END();
	}

	modm::ResumableResult<void>
	request()
	{
		RF_BEGIN(4);

		RF_WAIT_WHILE(isResumableRunning(3));
		response_status = ResponseStatus::Waiting;
		RF_CALL(send(request_msg));

		response_timer.restart(1s);
		RF_WAIT_UNTIL((response_status == ResponseStatus::Received) or response_timer.isExpired());
		response_status = ResponseStatus::NotWaiting;

		if (response_timer.isExpired()) {
			request_msg = Message(address, request_msg.command(), 1, Type::Error);
			*request_msg.get<Error>() = Error::RequestTimeout;
		}

		RF_END();
	}

	modm::ResumableResult<void>
	receive()
	{
		RF_BEGIN(5);
		while(1)
		{
			rx_msg.deallocate();	// deallocates previous message
			if (RF_CALL(interface.receiveHeader(&rx_msg)) == InterfaceStatus::Ok)
			{
				// Check lists if we are interested in this message
				is_rx_msg_for_us = handleRxMessage(false);
				// Receive the message data, only allocate if it's for us
				if (RF_CALL(interface.receiveData(&rx_msg, is_rx_msg_for_us)) == InterfaceStatus::Ok)
				{
					// Only handle message *with* data if it's for us
					if (is_rx_msg_for_us) handleRxMessage(true);
				}
			}
			RF_YIELD();
		}
		RF_END();
	}

protected:
	bool
	handleRxMessage(bool complete)
	{
		switch(rx_msg.type())
		{
			case Type::Broadcast:
				for (size_t ii=0; ii<listenerCount; ii++)
				{
					auto& listener = listenerList[ii];
					if (rx_msg.command() == listener.command) {
						if (complete) listener.call(rx_msg);
						else return true;
					}
				}
				break;

			case Type::Request:
				if (rx_msg.address() == address)
				{
					for (size_t ii=0; ii<actionCount; ii++)
					{
						auto& action = actionList[ii];
						if (rx_msg.command() == action.command)
						{
							if (complete)
							{
								auto msg = action.call(rx_msg);
								msg.setAddress(address);
								msg.setCommand(action.command);
								tx_queue.push(std::move(msg));
							}
							return true;
						}
					}
					Message msg(address, rx_msg.command(), 1, Type::Error);
					*msg.get<Error>() = Error::NoAction;
					tx_queue.push(std::move(msg));
				}
				break;

			default:
				if (response_status == ResponseStatus::Waiting)
				{
					if (complete and
					    request_msg.address() == rx_msg.address() and
						request_msg.command() == rx_msg.command())
					{
						request_msg = std::move(rx_msg);
						response_status = ResponseStatus::Received;
					}
					return true;
				}
		}
		return false;
	}

	void
	setSeed()
	{ lfsr = address << 8 | (address + 1); }

	void
	reschedule(uint8_t mask)
	{
		lfsr ^= lfsr >> 7;
		lfsr ^= lfsr << 9;
		lfsr ^= lfsr >> 13;

		const uint16_t priority = ((1u << PRIORITY_BITS) - 1 - tx_counter) >> 3;
		const uint16_t delay = (lfsr & ((1ul << mask) - 1)) | (priority << mask);
		tx_timer.restart(std::chrono::microseconds(delay));
	}

protected:
	Interface<MaxHeapAllocation> interface;

	Action *const actionList{nullptr};
	Listener *const listenerList{nullptr};

	modm::ShortPreciseTimeout tx_timer;
	modm::ShortTimeout response_timer;

	modm::BoundedQueue<Message, TxBufferSize> tx_queue;
	Message request_msg;
	Message rx_msg;

	uint16_t lfsr;
	const uint8_t actionCount{0};
	const uint8_t listenerCount{0};
	uint8_t address;

	uint8_t tx_counter;
	bool is_rx_msg_for_us;

	enum class ResponseStatus : uint8_t
	{
		NotWaiting = 0,
		Waiting,
		Received,
	}
	response_status{ResponseStatus::NotWaiting};

	static constexpr uint8_t MIN_TX_TRIES{20};
	static constexpr uint8_t PRIORITY_BITS{6};
	static constexpr uint8_t RESCHEDULE_MASK_SHORT{7};
	static constexpr uint8_t RESCHEDULE_MASK_LONG{11};
};

}
