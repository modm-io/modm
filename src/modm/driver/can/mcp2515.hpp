/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2015, 2017, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP2515_HPP
#define MODM_MCP2515_HPP

#include <stdint.h>
#include <modm/architecture/interface/accessor.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/can.hpp>
#include <modm/debug/logger.hpp>

#include "mcp2515_definitions.hpp"

/**
 * \defgroup	mcp2515	MCP2515
 * \ingroup		driver_can
 */

/**
 * \name	Restructure filter and mask bits for the MCP2515
 *
 * \code
 * FLASH_STORAGE(uint8_t can_filter[]) =
 * {
 * 		MCP2515_FILTER_EXTENDED(0),	// Filter 0
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 1
 *
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 2
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 3
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 4
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 5
 *
 *  	MCP2515_MASK_EXTENDED(0),	// Mask 0
 * 		MCP2515_MASK_EXTENDED(0),	// Mask 1
 * };
 * \endcode
 *
 * Filter 0 and 1 belong to Mask 0 (Group 0) and Filter 2 to 5 to
 * Mask 1 (Group 1). You can set one group to receive only standard messages
 * by using MCP2515_FILTER()/MCP2515_MASK() for all parts. Or to receive only
 * extended frame by using MCP2515_FILTER_EXTENDED() and MCP2515_MASK_EXTENDED().
 * But you must not mix both possibilities within one group!
 *
 * You can use one group to receive standard frames and one group to receive
 * extended frames, but you can't set one group to receive both!
 *
 * \warning	Do not use this macro for variables, only for static values
 *			known at compile-time.
 * \ingroup	mcp2515
 */
//\{
#if defined(__DOXYGEN__)

	#define	MCP2515_FILTER_EXTENDED(id)
	#define	MCP2515_FILTER(id)

	#define	MCP2515_MASK_EXTENDED(id)
	#define	MCP2515_MASK(id)

#else

	#define MCP2515_FILTER_EXTENDED(id)	\
			(uint8_t)  ((uint32_t) (id) >> 21), \
			(uint8_t)((((uint32_t) (id) >> 13) & 0xe0) | (1<<3) | \
				(((uint32_t) (id) >> 16) & 0x3)), \
			(uint8_t)  ((uint32_t) (id) >> 8), \
			(uint8_t)  ((uint32_t) (id))

	#define	MCP2515_FILTER(id) \
			(uint8_t)((uint32_t) id >> 3), \
			(uint8_t)((uint32_t) id << 5), \
			0, \
			0

	#define MCP2515_MASK_EXTENDED(id)	\
			(uint8_t)  ((uint32_t) (id) >> 21), \
			(uint8_t)((((uint32_t) (id) >> 13) & 0xe0) | (1<<3) | \
				(((uint32_t) (id) >> 16) & 0x3)), \
			(uint8_t)  ((uint32_t) (id) >> 8), \
			(uint8_t)  ((uint32_t) (id))

	// TODO check this would receive all frames
	#define	MCP2515_MASK(id) \
			(uint8_t)((uint32_t) id >> 3), \
			(uint8_t)((uint32_t) id << 5), \
			0, \
			0
#endif
//\}

namespace modm
{
	/**
	 *
	 * \brief	Driver for the MPC2515 CAN controller
	 *
	 * \tparam	SPI		SPI interface
	 * \tparam	CS		Chip select pin
	 * \tparam	INT		Interrupt pin
	 *
	 * If you want to activate the internal pull-up for the INT pin you
	 * need to do this by yourself before calling the initialize method!
	 *
	 * \author	Fabian Greif
	 * \ingroup	mcp2515
	 */
	template < typename SPI,
			   typename CS,
			   typename INT >
    class Mcp2515 : public ::modm::Can
	{
	public:
		template< int32_t ExternalClockFrequency,
			   uint32_t bitrate = Bitrate::kBps125,
			   uint16_t tolerance = Tolerance::OnePercent >
		static inline bool
		initialize();

	private:
		static inline bool
		initializeWithPrescaler(
			uint8_t prescaler,
			uint8_t sjw,
			uint8_t prop_seg,
			uint8_t ps1,
			uint8_t ps2);

	public:
		static void
		setFilter(accessor::Flash<uint8_t> filter);

		static void
		setMode(Can::Mode mode);


		static inline bool
		isMessageAvailable();

		static bool
		getMessage(can::Message& message);

		/*
		 * The CAN controller has a free slot to send a new message.
		 *
		 * \return true if a slot is available, false otherwise
		 */
		static inline bool
		isReadyToSend();

		/*
		 * Send a message over the CAN.
		 *
		 * \return true if the message was send, false otherwise
		 */
		static bool
		sendMessage(const can::Message& message);

    public:
        // Extended Functionality

        /*
         * Fixme: Empty implementation, required by connector
         */
        static BusState
        getBusState() {
            return BusState::Connected;
        }

	protected:
		enum SpiCommand
		{
			RESET = 0xC0,
			READ = 0x03,
			READ_RX = 0x90,
			WRITE = 0x02,
			WRITE_TX = 0x40,
			RTS	= 0x80,
			READ_STATUS = 0xA0,
			RX_STATUS = 0xB0,
			BIT_MODIFY = 0x05
		};

		static void
		writeRegister(uint8_t address, uint8_t data);

		static uint8_t
		readRegister(uint8_t address);

		static void
		bitModify(uint8_t address, uint8_t mask, uint8_t data);

		static uint8_t
		readStatus(uint8_t type);

		static inline void
		writeIdentifier(const uint32_t& identifier, bool isExtendedFrame);

		static inline bool
		readIdentifier(uint32_t& identifier);

	protected:
		static SPI spi;
		static CS chipSelect;
		static INT interruptPin;
	};
}

#include "mcp2515_impl.hpp"

#endif // MODM_MCP2515_HPP
