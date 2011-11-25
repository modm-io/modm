// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__CAN_FILTER_HPP
#define XPCC_STM32__CAN_FILTER_HPP

#include <xpcc/driver/connectivity/can/message.hpp>

#include "device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Filters for the Basic Extended CAN Module
		 * 
		 * @author		Fabian Greif <fabian.greif@rwth-aachen.de>
		 * @ingroup		stm32
		 */
		class CanFilter
		{
			// internal constants
			static const uint32_t MASK_MODE	= 0x00;
			static const uint32_t LIST_MODE = 0x02;
			
			static const uint32_t DUAL_MODE = 0x00;
			static const uint32_t SINGLE_MODE = 0x04;
			
		public:
			enum Fifo
			{
				// FIFO assignment
				FIFO0 = 0x0,		///< Message will be stored in Fifo 0
				FIFO1 = 0x1,		///< Message will be stored in Fifo 1
			};
			
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
			};
			
		public:
			/**
			 * Message with a 29-Bit Identifier.
			 */
			struct ExtendedIdentifier : public Identifier
			{
				ALWAYS_INLINE
				ExtendedIdentifier(uint32_t identifier, RemoteRequestStatus rtr = NO_RTR) :
					Identifier((identifier << 3) | 0x4 | ((rtr) ? 0x2 : 0))
				{
				}
			};
			
			/**
			 * 29-Bit Mask.
			 */
			struct ExtendedFilterMask
			{
				ALWAYS_INLINE
				ExtendedFilterMask(uint32_t identifier, RemoteRequestFilter rtr = RTR_MATCH) :
					value((identifier << 3) | 0x4 | ((rtr) ? 0x2 : 0))
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
			
			/**
			 * Message with a 11-Bit Identifier.
			 */
			struct StandardIdentifier : public Identifier
			{
				ALWAYS_INLINE
				StandardIdentifier(uint16_t identifier, RemoteRequestStatus rtr = NO_RTR) :
					Identifier((identifier << 21) | ((rtr) ? 0x2 : 0))
				{
				}
			};
			
			/**
			 * 11-Bit Mask.
			 */
			struct StandardFilterMask
			{
				ALWAYS_INLINE
				StandardFilterMask(uint16_t identifier, RemoteRequestFilter rtr = RTR_MATCH) :
					value((identifier << 21) | ((rtr) ? 0x2 : 0))
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
			
			// ----------------------------------------------------------------
		private:
			struct IdentifierShort
			{
			protected:
				ALWAYS_INLINE
				IdentifierShort(uint16_t identifier) :
					value(identifier)
				{
				}
				
				uint16_t value;
				
			public:
				ALWAYS_INLINE
				operator uint16_t () const
				{
					return value;
				}
			};
			
		public:
			/**
			 * Generate a identifier for the Dual Filter mode from an extended
			 * (29-bit) identifier.
			 * 
			 * @warning	Uses only the upper 14-bit of the 29-bit identifier!
			 * 			The lower 15-bit will be don't care! 
			 */
			struct ExtendedIdentifierShort : public IdentifierShort
			{
				ALWAYS_INLINE
				ExtendedIdentifierShort(uint32_t identifier, RemoteRequestStatus rtr = NO_RTR) :
					IdentifierShort(((identifier >> 13) & 0xffe0) |
							((identifier >> 15) & 0x007f) |
							0x08 |
							((rtr) ? 0x10 : 0))
				{
				}
			};
			
			/**
			 * Generates a mask for an 29-bit identifier.
			 * 
			 * @warning	Uses only the upper 14-bit of the 29-bit mask!
			 * 			The lower 15-bit will be don't care! 
			 */
			struct ExtendedFilterMaskShort
			{
				ALWAYS_INLINE
				ExtendedFilterMaskShort(uint32_t identifier, RemoteRequestFilter rtr = RTR_MATCH) :
					value(((identifier >> 13) & 0xffe0) |
							((identifier >> 15) & 0x007f) |
							0x08 |
							((rtr) ? 0x10 : 0))
				{
				}
				
				ALWAYS_INLINE
				operator uint16_t () const
				{
					return value;
				}
				
			private:
				uint16_t value;
			};
			
			/**
			 * Generate a identifier for the Dual Filter mode from a standard
			 * (11-bit) identifier.
			 */
			struct StandardIdentifierShort : public IdentifierShort
			{
				ALWAYS_INLINE
				StandardIdentifierShort(uint16_t identifier, RemoteRequestStatus rtr = NO_RTR) :
					IdentifierShort(((identifier << 5) & 0xffe0) | ((rtr) ? 0x10 : 0))
				{
				}
			};
			
			struct StandardFilterMaskShort
			{
				ALWAYS_INLINE
				StandardFilterMaskShort(uint16_t identifier, RemoteRequestFilter rtr = RTR_MATCH) :
					value(((identifier << 5) & 0xffe0) | ((rtr) ? 0x10 : 0))
				{
				}
				
				ALWAYS_INLINE
				operator uint16_t () const
				{
					return value;
				}
				
			private:
				uint16_t value;
			};
			
			// ----------------------------------------------------------------
			/**
			 * Use bank as masked filter for extended messages.
			 * 
			 * @param	bank	Number of the filter bank
			 * @param	fifo	FIFO assignment (message will be stored in Fifo 0 or Fifo 1)
			 * 
			 * @param	id		29-bit identifier
			 * @param	mask	29-bit mask
			 */
			static ALWAYS_INLINE void
			setFilter(uint8_t bank, Fifo fifo, ExtendedIdentifier id, ExtendedFilterMask mask)
			{
				setFilterBase(bank, fifo | MASK_MODE | SINGLE_MODE, id, mask);
			}
			
			/**
			 * Use bank as masked filter for standard messages.
			 * 
			 * @param	bank	Number of the filter bank
			 * @param	fifo	FIFO assignment (message will be stored in Fifo 0 or Fifo 1)
			 * 
			 * @param	id		11-bit identifier
			 * @param	mask	11-bit mask
			 */
			static ALWAYS_INLINE void
			setFilter(uint8_t bank, Fifo fifo, StandardIdentifier id, StandardFilterMask mask)
			{
				setFilterBase(bank, fifo | MASK_MODE | SINGLE_MODE, id, mask);
			}
			
			/**
			 * Use bank as identifier filter for standard or extended messages.
			 * 
			 * The message have to match the given identifier exactly!
			 * 
			 * @param	bank	Number of the filter bank
			 * @param	fifo	FIFO assignment (message will be stored in Fifo 0 or Fifo 1)
			 * 
			 * @param	id		11 or 29-bit identifier
			 * @param	mask	11 or 29-bit mask
			 */
			static ALWAYS_INLINE void
			setIdentifierFilter(uint8_t bank, Fifo fifo, Identifier id, Identifier id2)
			{
				setFilterBase(bank, fifo | LIST_MODE | SINGLE_MODE, id, id2);
			}
			
			// ----------------------------------------------------------------
			static ALWAYS_INLINE void
			setFilterShort(uint8_t bank, Fifo fifo,
					StandardIdentifierShort id1, StandardFilterMaskShort mask1,
					StandardIdentifierShort id2, StandardFilterMaskShort mask2)
			{
				setFilterBase(bank, fifo | MASK_MODE | DUAL_MODE,
						id1 | ((uint32_t) mask1 << 16),
						id2 | ((uint32_t) mask2 << 16));
			}
			
			static ALWAYS_INLINE void
			setFilterShort(uint8_t bank, Fifo fifo,
					StandardIdentifierShort id1, StandardFilterMaskShort mask1,
					ExtendedIdentifierShort id2, ExtendedFilterMaskShort mask2)
			{
				setFilterBase(bank, fifo | MASK_MODE | DUAL_MODE,
						id1 | ((uint32_t) mask1 << 16),
						id2 | ((uint32_t) mask2 << 16));
			}
		
			static ALWAYS_INLINE void
			setFilterShort(uint8_t bank, Fifo fifo,
					ExtendedIdentifierShort id1, ExtendedFilterMaskShort mask1,
					StandardIdentifierShort id2, StandardFilterMaskShort mask2)
			{
				setFilterBase(bank, fifo | MASK_MODE | DUAL_MODE,
						id1 | ((uint32_t) mask1 << 16),
						id2 | ((uint32_t) mask2 << 16));
			}
			
			static ALWAYS_INLINE void
			setFilterShort(uint8_t bank, Fifo fifo,
					ExtendedIdentifierShort id1, ExtendedFilterMaskShort mask1,
					ExtendedIdentifierShort id2, ExtendedFilterMaskShort mask2)
			{
				setFilterBase(bank, fifo | MASK_MODE | DUAL_MODE,
						id1 | ((uint32_t) mask1 << 16),
						id2 | ((uint32_t) mask2 << 16));
			}
			
			/**
			 * Use bank as identifier filter for standard or extended messages.
			 * 
			 * The message have to match the given identifier exactly! For
			 * extended messages only the upper 14-bit are check. The lower
			 * 15-bit are don't care!
			 * 
			 * @param	bank	Number of the filter bank
			 * @param	fifo	FIFO assignment (message will be stored in Fifo 0 or Fifo 1)
			 * 
			 * @param	id		11 or 29-bit identifier
			 * @param	mask	11 or 29-bit mask
			 */
			static ALWAYS_INLINE void
			setIdentifierFilterShort(uint8_t bank, Fifo fifo,
					IdentifierShort id1, IdentifierShort id2,
					IdentifierShort id3, IdentifierShort id4)
			{
				setFilterBase(bank, fifo | LIST_MODE | DUAL_MODE,
						id1 | ((uint32_t) id2 << 16),
						id3 | ((uint32_t) id4 << 16));
			}
			
			/**
			 * Disable a filter bank.
			 */
			static void
			disableFilter(uint8_t bank);
			
			/**
			 * Set the first filter bank for CAN2.
			 * 
			 * All banks lower than this value belong to CAN1 then.
			 * Only available on connectivity line devices.
			 * 
			 * @param	startBank	The start bank for the CAN2 interface (Slave)
			 * 						in the range 1 to 27.
			 */
			static void
			setStartFilterBankForCan2(uint8_t startBank);
			
		private:
			static void
			setFilterBase(uint8_t bank, uint32_t flags, uint32_t fr1, uint32_t fr2);
		};
	}
}

#endif	//  XPCC_STM32__CAN_FILTER_HPP
