#ifndef XPCC_LPC11C__C_CAN_HPP
#define XPCC_LPC11C__C_CAN_HPP

#include <xpcc/communication/can/message.hpp>
#include <xpcc/architecture/peripheral/can.hpp>

#define CONFIG_ENABLE_DRIVER_ROMCAN   1
#define CONFIG_ENABLE_DRIVER_ROMUSB   0
#define CONFIG_ENABLE_DRIVER_ROMPOWER 0

#include <lpc11xx/driver/rom_drivers.h>

#include "../../clock/common/common_clock.hpp"

// at least one queue, so activate interrupts
#define LPC11C_USING_CAN_INTERRUPTS ((LPC11C_CAN_TX_BUFFER_SIZE > 0) || (LPC11C_CAN_RX_BUFFER_SIZE > 0))

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
 * \ingroup	lpc
 */
class Can : public ::xpcc::Can
{
public:
	enum class
	Bitrate : int
	{
		kBps10	= xpcc::clock::Frequency::kHz10,
		kBps20	= xpcc::clock::Frequency::kHz20,
		kBps50	= xpcc::clock::Frequency::kHz50,
		kBps100	= xpcc::clock::Frequency::kHz100,
		kBps125	= xpcc::clock::Frequency::kHz125,
		kBps250	= xpcc::clock::Frequency::kHz250,
		kBps500	= xpcc::clock::Frequency::kHz500,
		MBps1	= xpcc::clock::Frequency::MHz1,
	};

public:

	template< class clockSource, Bitrate bitrate = Bitrate::kBps125 >
	static inline void
	initialize()
	{

		/**
		 * MAIN_CLOCK / SYSAHBCLKDIV should be 48 MHz.
		 *
		 * 48 MHz / (canclkdiv + 1) / (prescaler + 1) / 12 = bitrate
		 *
		 * 48 MHz	 125 kbit	0x451f	OK
		 * 48 MHz	 250 kbit	0x450f 	OK
		 * 48 MHz	 500 kbit	0x4507	OK
		 * 48 MHz	1000 kbit	0x4503	OK
		 *
		 */

		static_assert(clockSource::Can == xpcc::clock::Frequency::MHz48,
				"Other main clocks than 48 MHz are not yet supported.");

		uint16_t prescaler;
		uint32_t canclkdiv = 0;
		switch (bitrate)
		{
			case Bitrate::kBps10:	prescaler =  39; canclkdiv = 9; break;
			case Bitrate::kBps20:	prescaler =  39; canclkdiv = 4; break;
			case Bitrate::kBps50:	prescaler =   7; canclkdiv = 9; break;
			case Bitrate::kBps100:	prescaler =  39; break;
			case Bitrate::kBps125:	prescaler =  31; break;
			case Bitrate::kBps250:	prescaler =  15; break;
			case Bitrate::kBps500:	prescaler =   7; break;
			case Bitrate::MBps1:	prescaler =   3; break;
			default: prescaler = 31; break;		// 125 kbps
		}

		/* Initialize CAN Controller */
		uint32_t ClkInitTable[2] = {
				canclkdiv, // CANCLKDIV
				0x00004500UL | prescaler  // CANBT: bit timing register:
		};

	// FIXME: turn into parameter
	#if LPC11C_USING_CAN_INTERRUPTS
		/* Interrupts enabled */
		(*rom)->pCAND->init_can(&ClkInitTable[0], true);
	#else
		/* Interrupts disabled */
		(*rom)->pCAND->init_can(&ClkInitTable[0], false);
	#endif

		/* Configure the CAN callback functions */
		CAN_CALLBACKS callbacks = {
		   CAN_rx,
		   CAN_tx,
		   CAN_error,
		   0,
		   0,
		   0,
		   0,
		   0,
		};
		(*rom)->pCAND->config_calb(&callbacks);


		// Use only Message Objects 1 to 16 for reception.
		// use setFilter of CanFilter to setup message reception.

		/* Always enable the CAN Interrupt. */
		NVIC_EnableIRQ(CAN_IRQn);
	}

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

	static BusState
	getBusState();

public:
	static constexpr
	ROM **rom = (ROM **) 0x1fff1ff8;

protected:
	/**
	 * Callback functions for the on-chip CAN driver library.
	 * They are registered in initialize() and called from the
	 * interrupt handler of the on-chip CAN driver (pCAND->isr())
	 */

	/**
	 * \brief	Receive Callback
	 *
	 * The CAN message received callback function is called on the
	 * interrupt level by the CAN interrupt handler when a message was
	 * received.
	 *
	 * \param	msg_obj_num	Message Object that contains a new message
	 */
	static void
	CAN_rx(uint8_t msg_obj_num);

	/**
	 * \brief	Transmit Callback
	 *
	 * Called on the interrupt level by the CAN interrupt handler after a
	 * message has been successfully transmitted on the bus.
	 *
	 * \param	msg_obj_num	Message Object that was transmitted
	 */
	static void
	CAN_tx(uint8_t msg_obj_num);

	/**
	 * \brief	Error Callback
	 *
	 * Called on the interrupt level by the CAN interrupt handler after a
	 * message has been successfully transmitted on the bus.
	 *
	 * \param	error_info	Error bitmask
	 */
	static void
	CAN_error(uint32_t error_info);
};

} // lpc namespace
} // xpcc namespace

#endif // XPCC_LPC11C__C_CAN_HPP
