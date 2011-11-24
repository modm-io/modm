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

#ifndef XPCC_STM32__CAN1_HPP
#define XPCC_STM32__CAN1_HPP

#include <xpcc/driver/connectivity/can/message.hpp>

#include "device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Basic Extended CAN
		 * 
		 * The Basic Extended CAN peripheral, named bxCAN, interfaces the CAN
		 * network. It supports the CAN protocols version 2.0A and B. It has
		 * been designed to manage a high number of incoming messages
		 * efficiently with a minimum CPU load.
		 * 
		 * <h2>Filter</h2>
		 * 
		 * For connectivity line devices there are 28 filter banks,
		 * i=0 .. 27, in other devices there are 14 filter banks i = 0 ..13.
		 * 
		 * The 28 filter banks for the connectivity line devices are
		 * shared between CAN1 and CAN2.
		 * 
		 * <h2>Configuration</h2>
		 * 
		 * \code
		 * STM32_CAN_TX_BUFFER_SIZE
		 * STM32_CAN_RX_BUFFER_SIZE
		 * \endcode
		 * 
		 * @author		Fabian Greif <fabian.greif@rwth-aachen.de>
		 * @ingroup		stm32
		 */
		class Can1
		{
		public:
			enum Remap
			{
				REMAP_PA11_PA12 = AFIO_MAPR_CAN_REMAP_REMAP1,	///< CANRX mapped to PA11, CANTX mapped to PA12
				REMAP_PB8_PB9 = AFIO_MAPR_CAN_REMAP_REMAP2,	///< CANRX mapped to PB8, CANTX mapped to PB9
				REMAP_PD0_PD1 = AFIO_MAPR_CAN_REMAP_REMAP3,	///< CANRX mapped to PD0, CANTX mapped to PD1
			};
			
			/**
			 * Remap compare channels to other locations.
			 *
			 * Remap has to be done before calling initialize() otherwise
			 * the wrong pins are configured.
			 */
			static inline void
			remapPins(Remap mapping)
			{
				AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_CAN_REMAP) | mapping;
			}
			
		public:
			/**
			 * Enables the clock for the CAN controller and resets all settings
			 *
			 * \warning	Has to be called after remapPins and before any other
			 * 			function from this class!
			 */
			static bool
			initialize(can::Bitrate bitrate);
			
			/**
			 * The the operating mode.
			 * 
			 * Default after initialization is the normal mode.
			 */
			static void
			setMode(can::Mode mode);
			
		public:
			enum Fifo
			{
				// FIFO assignment
				FIFO0 = 0x0,		///< Message will be stored in Fifo 0
				FIFO1 = 0x1,		///< Message will be stored in Fifo 1
			};
			
			static const bool RTR = true;
			static const bool NO_RTR = false;
			
			static const bool EXTENDED = true;
			static const bool STANDARD = false;
			
			static inline uint32_t
			extendedFilter(uint32_t identifier, bool ide, bool rtr)
			{
				return (identifier << 3) | ((ide) ? 0x4 : 0) | ((rtr) ? 0x2 : 0);
			}
			
			static inline uint32_t
			standardFilter(uint16_t identifier, bool ide, bool rtr)
			{
				return (identifier << 21) | ((ide) ? 0x4 : 0) | ((rtr) ? 0x2 : 0);
			}
			
			/**
			 * Generate a identifier for the Dual Filter mode from an extended
			 * (29-bit) identifier.
			 * 
			 * Uses only the upper 14-bit of the 29-bit identifier!
			 */
			static inline uint16_t
			extendedSmallFilter(uint32_t identifier, bool ide, bool rtr)
			{
				uint32_t tmp;
				tmp  = (identifier >> 13) & 0xffe0;
				tmp |= (identifier >> 15) & 0x007f;
				return tmp | ((ide) ? 0x08 : 0) | ((rtr) ? 0x10 : 0);
			}
			
			/**
			 * Generate a identifier for the Dual Filter mode from a standard
			 * (11-bit) identifier.
			 */
			static inline uint16_t
			standardSmallFilter(uint16_t identifier, bool ide, bool rtr)
			{
				uint32_t tmp;
				tmp  = (identifier << 5) & 0xffe0;
				return tmp | ((ide) ? 0x08 : 0) | ((rtr) ? 0x10 : 0);
			}
			
			/**
			 * TODO
			 * 
			 * @param	bank	Number of the filter bank
			 * @param	fifo	FIFO assignment (message will be stored in Fifo 0 or Fifo 1)
			 */
			static void
			setMaskFilter(uint8_t bank, Fifo fifo, uint32_t id, uint32_t mask);
			
			/// TODO
			static void
			setIdentifierFilter(uint8_t bank, Fifo fifo, uint32_t id, uint32_t id2);
			
			/// TODO
			static void
			setSmallMaskFilter(uint8_t bank, Fifo fifo,
					uint16_t id1, uint16_t mask1,
					uint16_t id2, uint16_t mask2);
			
			/// TODO
			static void
			setSmallIdentifierFilter(uint8_t bank, Fifo fifo,
					uint16_t id1, uint16_t id2,
					uint16_t id3, uint16_t id4);
			
			/// TODO
			static void
			disableFilter(uint8_t bank);
			
			/**
			 * TODO
			 * 
			 * Only available on connectivity line devices.
			 * 
			 * @param	startBank	The start bank for the CAN2 interface (Slave)
			 * 						in the range 1 to 27.
			 */
			static void
			setCan2StartBank(uint8_t startBank);
		public:
			static bool
			isMessageAvailable();

			static bool
			getMessage(can::Message& message);

			/*
			 * The CAN controller has a free slot to send a new message.
			 * \return true if a slot is available, false otherwise
			 */
			static bool
			isReadyToSend();

			/*
			 * Send a message over the CAN.
			 * \return true if the message was send, false otherwise
			 */
			static bool
			sendMessage(const can::Message& message);

		public:
			/**
			 * Get Receive Error Counter.
			 *
			 * In case of an error during reception, this counter is
			 * incremented by 1 or by 8 depending on the error condition as
			 * defined by the CAN standard. After every successful reception
			 * the counter is decremented by 1 or reset to 120 if its value
			 * was higher than 128. When the counter value exceeds 127, the
			 * CAN controller enters the error passive state.
			 */
			static inline uint8_t
			getReceiveErrorCounter()
			{
				return (CAN1->ESR & CAN_ESR_REC) >> 24;
			}

			/**
			 * Get Transmit Error Counter.
			 *
			 */
			static inline uint8_t
			getTransmitErrorCounter()
			{
				return (CAN1->ESR & CAN_ESR_TEC) >> 16;
			}

			static can::BusState
			getBusState();
		};
	}
}

#endif	//  XPCC_STM32__CAN1_HPP
