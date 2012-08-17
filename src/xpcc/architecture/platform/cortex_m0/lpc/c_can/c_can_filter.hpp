#ifndef XPCC_LPC11C__CAN_FILTER_HPP
#define XPCC_LPC11C__CAN_FILTER_HPP

#include <xpcc/driver/connectivity/can/message.hpp>

#include "../device.h"

namespace xpcc
{
	namespace lpc11c
	{
		/**
		 * \brief		Reception Filters for the C_CAN module in LPC11C00 Series.
		 *
		 * The C_CAN Module has 32 Message Objects (MOBs). In the CAN class the
		 * MOBs 1 to 16 are used for transmission and the MOBs 17 to 32 for
		 * reception, respectivly.
		 *
		 * Any number of MOBs can be grouped together to form a FIFO of
		 * arbitrary length. This class can be used to configure the CAN
		 * reception filters.
		 *
		 * When using the class with XPCC the following scenarios is likely:
		 * The container consists of several components. These receive events
		 * and actions at different addresses. One filter with a small FIFO
		 * is set up for events and one or more filters with an own FIFO is
		 * set up for the actions. It is advisable to choose the identifiers
		 * of the components in the container so that one filter can match
		 * all messages for this container.
		 *
		 * Having several filters that all put their matched messages in one
		 * FIFO (like in STM32) might be a better implementation of CAN filtering.
		 *
		 * This class is intended to be used with XPCC so only filtering
		 * of Extended CAN messages (29 bit identifier) is implemented yet.
		 *
		 * \author		strongly-typed
		 * \ingroup		lpc11c
		 */
		class CanFilter
		{

		};
	}
}

#endif // XPCC_LPC11C__CAN_FILTER_HPP
