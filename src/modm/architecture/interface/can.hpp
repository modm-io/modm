/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2015, 2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_CAN_HPP
#define MODM_INTERFACE_CAN_HPP

#include <modm/architecture/interface/peripheral.hpp>
#include "can_message.hpp"

namespace modm
{

/// Interface to the CAN bus
/// @ingroup modm_architecture_can
class Can : public ::modm::PeripheralDriver
{
public:
	/// Possible modes of the CAN controller
	enum class
	Mode : uint8_t
	{
		Normal		= 0b00,	///< standard operation mode
		ListenOnly	= 0b01,	///< only possible to receive messages
		LoopBack	= 0b10,	///< messages sent are immediately received by own controller
		ListenOnlyLoopBack	= 0b11,	///< combination of both modes
	};

	enum class
	BusState : uint8_t
	{
		Connected = 0,
		///< This bit is set by hardware when the warning limit has been
		///< reached (Receive Error Counter or Transmit Error Counter ≥ 96).
		ErrorWarning = 1,
		///< This bit is set by hardware when the Error Passive limit has
		///< been reached (Receive Error Counter or Transmit Error Counter > 127).
		ErrorPassive = 2,
		///< This bit is set by hardware when it enters the bus-off state.
		///< The bus-off state is entered on TEC overflow, greater than 255
		Off = 3,
	};
#ifdef __DOXYGEN__
public:
	/// Size of the receive buffer.
	static constexpr size_t RxBufferSize = 16;

	/// Size of the transmit buffer.
	static constexpr size_t TxBufferSize = 16;

public:
	/**
	 * Configures the Tx and Rx signals and connects them.
	 *
	 * @tparam	Signals
	 *		One Tx and one Rx signal are required and can be passed out-of-order.
	 */
	template< class... Signals >
	static void
	connect();

	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam SystemClock
	 * 		the currently active system clock
	 * @tparam bitrate
	 * 		the desired bitrate in Hz
	 * @tparam	tolerance
	 * 		the allowed relative tolerance for the resulting baudrate
	 */
	template< class SystemClock, bitrate_t bitrate = 125_kbps, percent_t tolerance = 1_pct >
	static void
	initialize(Mode startupMode);

	/// Returns true if a message can be retrieved by calling @c getMessage.
	static bool
	isMessageAvailable();

	/// Returns true if a message was copied into the message buffer
	static bool
	getMessage(can::Message& message);

	/**
	 * The CAN controller has a free slot to send a new message.
	 * @return true if a slot is available, false otherwise
	 */
	static bool
	isReadyToSend();

	/**
	 * Send a message over the CAN.
	 * @return true if the message was send, false otherwise
	 */
	static bool
	sendMessage(const can::Message& message);

	// Optional
	/// Get Receive Error Counter.
	static uint8_t
	getReceiveErrorCounter();

	/// Get Transmit Error Counter.
	static uint8_t
	getTransmitErrorCounter();

	static BusState
	getBusState();
#endif
};

modm::IOStream&
operator << (modm::IOStream& stream, const Can::BusState state);

} // namespace modm

#endif // MODM_INTERFACE_CAN_HPP
