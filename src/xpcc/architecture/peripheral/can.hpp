// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_CAN_HPP
#define XPCC_PERIPHERAL_CAN_HPP

#include "../peripheral.hpp"

/**
 * @ingroup 	peripheral
 * @defgroup	can		Controller Area Network (CAN)
 */

namespace xpcc
{

/// @ingroup can
class Can : public ::xpcc::Peripheral
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

	/// Supported CAN bitrates; maybe different on a per device basis
	enum class
	Bitrate : uint8_t
	{
		kBps10	= 0,
		kBps20	= 1,
		kBps50	= 2,
		kBps100	= 3,
		kBps125	= 4,
		kBps250	= 5,
		kBps500	= 6,
		MBps1	= 7,
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
	/**
	 * Enables the clock for the CAN controller and resets all settings
	 *
	 * @param clockSource
	 *			nomally the SystemClock<...> that you enable on startup
	 * @param bitrate
	 * 			CAN bitrate
	 */
	template< class clockSource, Bitrate bitrate = Bitrate::kBps125>
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
#if 0
	/// Get Receive Error Counter.
	static uint8_t
	getReceiveErrorCounter();

	/// Get Transmit Error Counter.
	static uint8_t
	getTransmitErrorCounter();

	static BusState
	getBusState();
#endif // if 0
#endif
};

} // namespace xpcc

#endif // XPCC_PERIPHERAL_SPI_HPP
