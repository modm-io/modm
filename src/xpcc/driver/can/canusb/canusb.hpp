#ifndef XPCC__CAN_USB_HPP
#define XPCC__CAN_USB_HPP

namespace xpcc
{
	class CanUsb
	{
		CanUsb();
		
		~CanUsb();
		
		static bool open();
		
		static bool close();
		
		static inline bool
		isMessageAvailable();
		
		static bool
		getMessage(Can::Message& message);
		
		/*
		 * The CAN controller has a free slot to send a new message.
		 *
		 * \return true if a slot is available, false otherwise
		 */
		static inline bool
		isReadyToSend()
		{
			return true;
		}
		
		/*
		 * Send a message over the CAN.
		 *
		 * \return true if the message was send, false otherwise
		 */
		static bool
		sendMessage(const Can::Message& message);	
	};
}
#endif // XPCC__CAN_USB_HPP
