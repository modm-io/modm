#include <xpcc/debug/error_report.hpp>
#include <xpcc/architecture/driver/atomic/queue.hpp>

#include "../error_code.hpp"

#include "c_can.hpp"
#include <xpcc_config.hpp>

// at least one queue, so activate interrupts
#define LPC11C_USING_CAN_INTERRUPTS ((LPC11C_CAN_TX_BUFFER_SIZE > 0) || (LPC11C_CAN_RX_BUFFER_SIZE > 0))

// ----------------------------------------------------------------------------
namespace xpcc
{
namespace lpc
{
#if LPC11C_CAN_TX_BUFFER_SIZE > 0
xpcc::atomic::Queue<xpcc::can::Message, LPC11C_CAN_TX_BUFFER_SIZE> txQueue;
#endif
#if LPC11C_CAN_RX_BUFFER_SIZE > 0
xpcc::atomic::Queue<xpcc::can::Message, LPC11C_CAN_RX_BUFFER_SIZE> rxQueue;
#endif
}
}

// ----------------------------------------------------------------------------
/* Low level function:
 * Use a Message Object to send a message */
static void
sendMessageObject(const xpcc::can::Message & message, uint8_t messageObjectId)
{
	// NXP's data structure for sending CAN messages
	CAN_MSG_OBJ msg_obj;

	// Which Message Object to use
	msg_obj.msgobj  = messageObjectId;

	// Mode and Identifier
	msg_obj.mode_id = message.identifier | CAN_MSGOBJ_STD;

	if (message.isExtended()) {
		msg_obj.mode_id |= CAN_MSGOBJ_EXT;
	}

	if (message.isRemoteTransmitRequest()) {
		msg_obj.mode_id |= CAN_MSGOBJ_RTR;
	}

	// Mask unused for transmission
	msg_obj.mask    = 0x0;

	// Set up the DLC
	msg_obj.dlc     = message.getLength();

	// Copy the data.
	// TODO reinterpret_cast or memcpy
	for (uint8_t ii = 0; ii < msg_obj.dlc; ++ii) {
		msg_obj.data[ii] = message.data[ii];
	}

	// Really send the message
	(*xpcc::lpc::Can::rom)->pCAND->can_transmit(&msg_obj);
}

// ----------------------------------------------------------------------------
/* Low level function:
 * Called by interrupt or by getMessage to receive a message */
static void
readMessageObject(xpcc::can::Message & message, uint8_t messageObjectId)
{
	// NXP's data structure for sending CAN messages
	CAN_MSG_OBJ msg_obj;

	// Which Message Object to receive
	msg_obj.msgobj = messageObjectId;

	// Read Message Object from CAN controller
	(*xpcc::lpc::Can::rom)->pCAND->can_receive(&msg_obj);

	// Is extended Identifier?
	if (msg_obj.mode_id & CAN_MSGOBJ_EXT) {
		message.setExtended();
		// The lower 29 bits are the extended identifier
		message.identifier = msg_obj.mode_id & 0x1fffffff;
	}
	else {
		message.setExtended(false);
		// The lower 11 bits are the standard identifier
		message.identifier = msg_obj.mode_id & 0x7ff;
	}

	// Is RTR ?
	message.setRemoteTransmitRequest( (msg_obj.mode_id & CAN_MSGOBJ_RTR) );

	// Get DLC
	message.length = msg_obj.dlc;

	// Copy Data
	// TODO Use memcpy or reinterpret_cast
	for (uint8_t ii = 0; ii < message.length; ++ii)
	{
		message.data[ii] = msg_obj.data[ii];
	}
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::initialize(can::Bitrate bitrate)
{
	// TODO variable baud rate configuration

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

	XPCC__STATIC_ASSERT(F_CPU == 48000000UL,
			"Other main clocks than 48 MHz are not yet supported.");

	uint16_t prescaler;
	uint32_t canclkdiv = 0;
	switch (bitrate)
	{
		case can::BITRATE_10_KBPS:	prescaler =  39; canclkdiv = 9; break;
		case can::BITRATE_20_KBPS:	prescaler =  39; canclkdiv = 4; break;
		case can::BITRATE_50_KBPS:	prescaler =   7; canclkdiv = 9; break;
		case can::BITRATE_100_KBPS:	prescaler =  39; break;
		case can::BITRATE_125_KBPS:	prescaler =  31; break;
		case can::BITRATE_250_KBPS:	prescaler =  15; break;
		case can::BITRATE_500_KBPS:	prescaler =   7; break;
		case can::BITRATE_1_MBPS:	prescaler =   3; break;
		default: prescaler = 31; break;		// 125 kbps
	}

	/* Initialize CAN Controller */
	uint32_t ClkInitTable[2] = {
			canclkdiv, // CANCLKDIV
			0x00004500UL | prescaler  // CANBT: bit timing register:
	};

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

	// NXP's data structure for CAN messages
	CAN_MSG_OBJ msg_obj;

	// Use Message Objects 1 to 16 for reception
	// TODO Use Message Objects 1 to 16
	msg_obj.msgobj = 1;
	msg_obj.mode_id = CAN_MSGOBJ_EXT; // extended
	msg_obj.mask = CAN_MSGOBJ_EXT;	// get all extended frames.
	// TODO Check if standard frames are received.

	// Configure
	(*rom)->pCAND->config_rxmsgobj(&msg_obj);

	/* Always enable the CAN Interrupt. */
	NVIC_EnableIRQ(CAN_IRQn);

	return true;
}

// ----------------------------------------------------------------------------

/**
 * \brief	CAN message transmit callback
 *
 * Called on the interrupt level by the CAN interrupt handler after a message
 * has been successfully transmitted on the bus.
 *
 */
void
xpcc::lpc::Can::CAN_tx(uint8_t /* msg_obj_num */)
{
#if LPC11C_CAN_TX_BUFFER_SIZE > 0
	// Send next from queue, if available

	if ((LPC_CAN->TXREQ2 & 0x0000ffff) == 0x0000) {
		// All message objects empty. Otherwise the order of messages
		// is not maintained

		while (!txQueue.isEmpty())
		{
			// Still messages in the queue.

			/* At least one Message Object is free, find first empty
			 * transmit Message Object from bitmask and use this
			 * Message Object to send message. */
			uint8_t firstZero = ffs(~(LPC_CAN->TXREQ2 & 0x0000ffff));

			if (firstZero == 17) {
				// no empty message object found
				// Actually, this should not happen because the interrupt
				// is called when a message was sent successfully.
				// It may happen if a message was sent from a message object
				// that was not intended for sending in this class. Then,
				// an interrupt may occurr but no of the used message
				// objects got freed.
				break;
			}
			else {
				// Send the Message Object. See sendMessage for messageObjectId
				// calculation.
				uint8_t messageObjectId = (firstZero - 1) + 16;

				sendMessageObject(txQueue.get(), messageObjectId);
				txQueue.pop();
			}
		}
	}
#endif
}

// ----------------------------------------------------------------------------

void
xpcc::lpc::Can::CAN_rx(uint8_t /* msg_obj_num */)
{
#if LPC11C_CAN_RX_BUFFER_SIZE > 0
	// Move received message to queue
#endif
}

// ----------------------------------------------------------------------------

void
xpcc::lpc::Can::CAN_error(uint32_t /* error_info */)
{
	// TODO Do some error handling, use ErrorReporter.
}

// ----------------------------------------------------------------------------

/*	CAN interrupt handler */
/*	The CAN interrupt handler must be provided by the user application.
	It's function is to call the isr() API located in the ROM */
extern "C" void
CAN_IRQHandler(void) {
	(*xpcc::lpc::Can::rom)->pCAND->isr();
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::sendMessage(/*const*/ can::Message & message)
{
	// This function is not reentrant. If one of the mailboxes is empty it
	// means that the software buffer is empty too. Therefore the mailbox
	// will stay empty and won't be taken by an interrupt.

	// Find a free Message Object for sending.
	// If none found push to queue if available

	uint8_t firstZero = ffs(~(LPC_CAN->TXREQ2 & 0x0000ffff));

	if (firstZero == 17)
	{
		/* All Message Objects used for sending (17 to 32) are pending
		 * at the moment. */
#if LPC11C_CAN_TX_BUFFER_SIZE > 0
		if (!txQueue.push(message)) {
			xpcc::ErrorReport::report(xpcc::lpc::CAN_TX_OVERFLOW);
			// All Message Objects are full and no space left in software buffer.
			return false;
		}
		// All Message Objects are full but message was stored in software buffer.
		return true;
#else
		// All Message Objects are full and no software buffers are available.
		return false;
#endif
	}
	else
	{
		/* At least one Message Object is free, find first empty
		 * transmit Message Object from bitmask and use this
		 * Message Object to send message. */

		// find the first 0 in TXREQ2 which means the slot is empty.
		//   The ffs counts the bit number from 1 (LSB) to 32 (MSB)
		//   The lower 16 bits of TXREQ2 correspond to MessageObjects 32 to 16
		//   with messageObjectIds 31 to 15.
		uint8_t messageObjectId = (firstZero - 1) + 16;

		// note wich messageObject was used
		message.data[0] = LPC_CAN->TXREQ2 >> 24;
		message.data[1] = LPC_CAN->TXREQ2 >> 16;
		message.data[2] = LPC_CAN->TXREQ2 >>  8;
		message.data[3] = LPC_CAN->TXREQ2 >>  0;


		message.data[6] = messageObjectId;

		sendMessageObject(message, messageObjectId);
		return true;
	}
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::getMessage(can::Message & message)
{
#if LPC11C_CAN_RX_BUFFER_SIZE > 0
	if (rxQueue.isEmpty())
	{
		// no message in the receive buffer
		return false;
	}
	else {
		memcpy(&message, &rxQueue.get(), sizeof(message));
		rxQueue.pop();
		return true;
	}
#else
	// No interrupts, polling
	if (LPC_CAN->ND1 & 0x00ff)
	{
		// At least one Message Object has unread data.
		uint8_t messageObject = ffs(LPC_CAN->ND1 & 0x00ff) - 1;
		readMessageObject(message, messageObject);
		return true;
	} else {
		// No message available
		return false;
	}
#endif
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::isMessageAvailable()
{
#if LPC11C_CAN_RX_BUFFER_SIZE > 0
	return !rxQueue.isEmpty();
#else
	/* Check if new data is available in the Message
	 * Objects 1 to 16. */
	return (LPC_CAN->ND1 & 0x00ff);
#endif
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::isReadyToSend()
{
#if LPC11C_CAN_TX_BUFFER_SIZE > 0
	return !txQueue.isFull();
#else
	/* Check if at least one Message Object 17 to 32
	 * is not pending. If it is not pending it is free. */
	return ( ~(LPC_CAN->TXREQ2 & 0xff00) );
#endif
}
