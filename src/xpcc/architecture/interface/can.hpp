// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_INTERFACE_CAN_HPP
#define XPCC_INTERFACE_CAN_HPP

#include <xpcc/architecture/interface.hpp>
#include "can_message.hpp"

/**
 * @ingroup		interface
 * @defgroup	can		Controller Area Network (CAN)
 * @brief 		Interface to the CAN-Communication BUS.
 *
 * Controller–area network (CAN or CAN-bus) is a vehicle bus standard designed
 * to allow microcontrollers and devices to communicate with each other within
 * a vehicle without a host computer.
 *
 * CAN is a message based protocol, designed specifically for automotive
 * applications but now also used in other areas such as industrial automation
 * and medical equipment.
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
	struct Bitrate
	{
		enum
		{
			kBps10  =   10000,
			kBps20  =   20000,
			kBps50  =   50000,
			kBps100 =  100000,
			kBps125 =  125000,
			kBps250 =  250000,
			kBps500 =  500000,
			MBps1   = 1000000,
		};
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
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam clockSource
	 *		the targets system clock
	 * @tparam bitrate
	 * 		the desired bitrate in Hz
	 * @tparam	tolerance
	 * 		the allowed relative tolerance for the resulting baudrate
	 */
	template< class clockSource, uint32_t bitrate = Bitrate::kBps125,
			uint16_t tolerance = Tolerance::OnePercent >
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

} // namespace xpcc

#endif // XPCC_INTERFACE_CAN_HPP
