#include <xpcc/debug/error_report.hpp>
#include <xpcc/architecture/driver/atomic/queue.hpp>

#include "error_code.hpp"

#include "c_can.hpp"
#include "c_can_registers.h"

#include <strings.h>	// for ffs()

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

	// Which Message Object to use, valid numbers are 0 to 31.
	// In this class 16 to 31 are used.
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
	LPC11C_ROM_CAN->pCAND->can_transmit(&msg_obj);
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
	LPC11C_ROM_CAN->pCAND->can_receive(&msg_obj);

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


#define STAT_LEC		(0x7 << 0)
#define STAT_TXOK		(1 << 3)
#define STAT_RXOK		(1 << 4)
#define STAT_EPASS		(1 << 5)
#define STAT_EWARN		(1 << 6)
#define STAT_BOFF		(1 << 7)

xpcc::lpc::Can::BusState
xpcc::lpc::Can::getBusState()
{
	if (LPC_CAN->STAT & STAT_BOFF) {
		return BusState::Off;
	}
	else if (LPC_CAN->STAT & STAT_EPASS) {
		return BusState::ErrorPassive;
	}
	else if (LPC_CAN->STAT & STAT_EWARN) {
		return BusState::ErrorWarning;
	}
	else {
		return BusState::Connected;
	}
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

	// MOBs 16 to 32 are in TXREQ2, at the *lower* bits
	if ((LPC_CAN->TXREQ2 & 0xffff) == 0x0000) {
		// All message objects empty. Otherwise the order of messages
		// is not maintained

		while (!txQueue.isEmpty())
		{
			// Still messages in the queue.

			/* At least one Message Object is free, find first empty
			 * transmit Message Object from bitmask and use this
			 * Message Object to send message. */
			uint8_t firstZero = ffs(~(LPC_CAN->TXREQ2 & 0xffff));

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

/**
 * \brief	CAN message receive callback
 *
 * Called on the interrupt level by the CAN interrupt handler when
 * a new message has been successfully received.
 */
#if LPC11C_CAN_RX_BUFFER_SIZE > 0
void
xpcc::lpc::Can::CAN_rx(uint8_t msg_obj_num)
{
	// Move received message to queue if possible

	if (rxQueue.isNotFull()) {
		xpcc::can::Message message;
		readMessageObject(message, msg_obj_num);
		if (!rxQueue.push(message)) {
			xpcc::ErrorReport::report(xpcc::lpc::CAN_RX_OVERFLOW);
		}
	}
	else {
		// Keep message in hardware buffer.
		// As soon as one element was popped from the queue this
		// driver must check if there are any messages pending in the
		// hardware MOBSs
		// .
		// Now the queue and all MOBs are full.
		// No further interrupt is generated because no message can be
		// successfully written to MOB.
		return;
	}

}
#else
void
xpcc::lpc::Can::CAN_rx(uint8_t /* msg_obj_num */)
{
}
#endif

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
XPCC_ISR(CAN) {
	LPC11C_ROM_CAN->pCAND->isr();
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::sendMessage(const can::Message & message)
{
	// This function is not reentrant. If one of the mailboxes is empty it
	// means that the software buffer is empty too. Therefore the mailbox
	// will stay empty and won't be taken by an interrupt.

	// Find a free Message Object for sending.
	// If none found push to queue if available

	uint8_t firstZero = ffs(~(LPC_CAN->TXREQ2 & 0xffff));

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

		// Check for other messages in MOBs
		// Happens if an interrupt was missed or the rxQueue got full
		// temporarily and messages were stored in the hardware FIFO
		// See Rx Interrupt for further explanation.
		while ((rxQueue.isNotFull()) && (LPC_CAN->ND1 & 0xffff)) {
			uint8_t messageObjectId = ffs(LPC_CAN->ND1 & 0xffff) - 1;
			xpcc::can::Message newMessage;
			readMessageObject(newMessage, messageObjectId);
			if (!rxQueue.push(newMessage)) {
				xpcc::ErrorReport::report(xpcc::lpc::CAN_RX_OVERFLOW);
			}
		}
		return true;
	}
#else
	// No interrupts, polling
	if (LPC_CAN->ND1 & 0xffff)
	{
		// At least one Message Object has unread data.
		uint8_t messageObject = ffs(LPC_CAN->ND1 & 0xffff) - 1;
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
	return (LPC_CAN->ND1 & 0xffff);
#endif
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::isReadyToSend()
{
#if LPC11C_CAN_TX_BUFFER_SIZE > 0
	return txQueue.isNotFull();
#else
	/* Check if at least one Message Object 17 to 32
	 * is not pending. If not all are pending at least
	 * one is free. */
	return ( (LPC_CAN->TXREQ2 & 0xffff) != 0xffff );
#endif
}
