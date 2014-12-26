#ifndef XPCC_LPC11C__CAN_FILTER_HPP
#define XPCC_LPC11C__CAN_FILTER_HPP

#include <xpcc/architecture/interface/can.hpp>

#include "c_can_registers.h"

namespace xpcc
{
	namespace lpc
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
		 * \ingroup		lpc
		 */
		class CanFilter
		{
		public:
			enum RemoteRequestFilter
			{
				RTR_DONT_CARE = 0,	///< Accept all messages independent from the RTR bit
				RTR_MATCH = 1,		///< Check for the RTR bit
			};

			enum RemoteRequestStatus
			{
				NO_RTR = 0,			///< Normal message
				RTR = 1,			///< Message should be a Remote Transmit Request (RTR)
			};

			// ----------------------------------------------------------------
		private:
			struct Identifier
			{
			protected:
				ALWAYS_INLINE
				Identifier(uint32_t identifier) :
					value(identifier)
				{
				}

				uint32_t value;

			public:
				ALWAYS_INLINE
				operator uint32_t () const
				{
					return value;
				}
			}; // Identifier struct

		public:
			/**
			 * Message with a 29-Bit Identifier.
			 *
			 * The bits 31 downto 16 are going to be written to ARB2 register.
			 * Bits 15 downto 0 will be written to ARB1 register.
			 */
			struct ExtendedIdentifier : public Identifier
			{
				ALWAYS_INLINE
				ExtendedIdentifier(uint32_t identifier, RemoteRequestStatus rtr = NO_RTR) :
					Identifier(identifier | (CAN_IFn_ARB2_MSGVAL << 16) | (CAN_IFn_ARB2_XTD << 16) | ((rtr) ? (CAN_IFn_ARB2_DIR << 16) : 0))
				{
				}
			};

			struct ExtendedFilterMask
			{
				ALWAYS_INLINE
				ExtendedFilterMask(uint32_t identifier, RemoteRequestFilter rtr = RTR_MATCH) :
					value(identifier | (CAN_IFn_MSK2_MXTD << 16) | ((rtr) ? (CAN_IFn_MSK2_MDIR << 16) : 0))
				{
				}

				ALWAYS_INLINE
				operator uint32_t () const
				{
					return value;
				}

			private:
				uint32_t value;

			};

		public:
		static void
		setFilter(
				ExtendedIdentifier id,
				ExtendedFilterMask mask,
				uint8_t firstMob,
				uint8_t size = 1);


		}; // CanFilter class
	} // lpc namespace
} // xpcc namespace

#endif // XPCC_LPC11C__CAN_FILTER_HPP
