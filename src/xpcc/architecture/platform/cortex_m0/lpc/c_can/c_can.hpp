#ifndef XPCC_LPC11C__C_CAN_HPP
#define XPCC_LPC11C__C_CAN_HPP

#include <xpcc/driver/connectivity/can/message.hpp>

#define CONFIG_ENABLE_DRIVER_ROMCAN   1
#define CONFIG_ENABLE_DRIVER_ROMUSB   0
#define CONFIG_ENABLE_DRIVER_ROMPOWER 0

#include <lpc11xx/driver/rom_drivers.h>

namespace xpcc
{
namespace lpc
{
	/**
	 * \brief	The CAN bus driver for LPC11Cxx devices.
	 *
	 * The C_CAN block is available in LPC11Cxx parts only (LPC11C00 series).
	 * The LPC11C22 and LPC11C24 parts include an on-chip, high-speed transceiver.
	 * For these parts, the CAN_RXD and CAN_TXD signals are connected internally
	 * to the on-chip transceiver, and the transceiver signals are pinned out.
	 *
	 * This class uses the on-chip CAN drivers.
	 *
	 * There are 32 Message Objects in two groups of 16 Message Objects.
	 * (1 to 16 and 17 to 32).
	 *
	 * In this class the following assumption is made:
	 * Message Objects  1 to 16 are used for reception and
	 * Message Objects 17 to 32 are used for transmission.
	 *
	 * TODO Group Message Objects to a FIFO ?! p307 section 16.7.3.10.1
	 *
	 * TODO Filter
	 *
	 * TODO Configuration
	 *
	 * \author	strongly-typed
	 * \ingroup	lpc11
	 */
	class Can
	{
	public:
		static bool
		initialize(can::Bitrate  bitrate);

	public:
		static bool
		isMessageAvailable();

		static bool
		getMessage(can::Message& message);

		/**
		 * The CAN controller has a free slot to send a new message.
		 * \return true if a slot is available, false otherwise
		 */
		static bool
		isReadyToSend();

		/**
		 * Send a message over the CAN.
		 * \return true if the message was send, false otherwise
		 */
		static bool
		sendMessage(const can::Message& message);

	public:
		static constexpr
		ROM **rom = (ROM **)0x1fff1ff8;

	protected:
		static void
		CAN_rx(uint8_t msg_obj_num);

		static void
		CAN_tx(uint8_t msg_obj_num);

		static void
		CAN_error(uint32_t error_info);


	};

} // lpc namespace
} // xpcc namespace

#endif // XPCC_LPC11C__C_CAN_HPP
