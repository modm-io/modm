#include <xpcc/debug/error_report.hpp>
#include <xpcc/architecture/driver/atomic/queue.hpp>

//#include "../error_code.hpp"

#include "c_can.hpp"
#include <xpcc_config.hpp>

#define LPC11C_USING_CAN_INTERRUPTS ((LPC11C_CAN_TX_BUFFER_SIZE > 0) && (LPC11C_CAN_RX_BUFFER_SIZE > 0))

// ----------------------------------------------------------------------------

#if LPC11C_USING_CAN_INTERRUPTS
xpcc::atomic::Queue<xpcc::can::Message, LPC11C_CAN_TX_BUFFER_SIZE> txQueue;
xpcc::atomic::Queue<xpcc::can::Message, LPC11C_CAN_RX_BUFFER_SIZE> rxQueue;
#endif

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
	for (uint8_t ii = 0; ii < 8; ++ii)
	{
		msg_obj.data[ii] = message.data[ii];
	}

	// Really send the message
	(*xpcc::lpc::Can::rom)->pCAND->can_transmit(&msg_obj);

	// Really necessary to call the interrupt?
	(*xpcc::lpc::Can::rom)->pCAND->isr();
}

// ----------------------------------------------------------------------------
/* Low level function
 * Called by interrupt of by getMessage to receive a message */
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
	for (uint8_t ii = 0; ii < 8; ++ii)
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
	 * CAN CANCLKDIV always 0.
	 *
	 * MAIN_CLOCK / SYSAHBCLKDIV / (CANCLKDIV_val + 1) = 48 MHz
	 *
	 * 48 MHz	 125 kbit	0x451f	OK
	 * 48 MHz	 250 kbit	0x450f 	OK
	 * 48 MHz	 500 kbit	0x4507	OK
	 * 48 MHz	1000 kbit	0x4503	OK
	 *
	 */

	/* Initialize CAN Controller */
	uint32_t ClkInitTable[2] = {
	  0x00000000UL, // CANCLKDIV
	  0x0000451fUL  // CANBT: bit timing register:
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

void
xpcc::lpc::Can::CAN_tx(uint8_t /* msg_obj_num */)
{
#if LPC11C_USING_CAN_INTERRUPTS
	// Send next from queue, if available
#endif
}

// ----------------------------------------------------------------------------

void
xpcc::lpc::Can::CAN_rx(uint8_t /* msg_obj_num */)
{
#if LPC11C_USING_CAN_INTERRUPTS
	// Move to queue
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
xpcc::lpc::Can::sendMessage(const can::Message & message)
{
	// This function is not reentrant. If one of the mailboxes is empty it
	// means that the software buffer is empty too. Therefore the mailbox
	// will stay empty and won't be taken by an interrupt.
	if (!(~(LPC_CAN->TXREQ2 & 0xff00)))
	{
		/* All Message Objects used for sending (17 to 32) are pending
		 * at the moment. */
#if LPC11C_USING_CAN_INTERRUPTS
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
	else {
		/* At least one Message Object is free, find first empty
		 * transmit Message Object from bitmask and use this
		 * Message Object to send message. */
		uint8_t messageObject = ffs(LPC_CAN->TXREQ2 & 0xff00) - 1;
		sendMessageObject(message, messageObject);
		return true;
	}
}

// ----------------------------------------------------------------------------

bool
xpcc::lpc::Can::getMessage(can::Message & message)
{
#if LPC11C_USING_CAN_INTERRUPTS
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
#if LPC11C_USING_CAN_INTERRUPTS
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
#if LPC11C_USING_CAN_INTERRUPTS
	return !txQueue.isFull();
#else
	/* Check if at least one Message Object 17 to 32
	 * is not pending. If it is not pending it is free. */
	return ( ~(LPC_CAN->TXREQ2 & 0xff00) );
#endif
}
