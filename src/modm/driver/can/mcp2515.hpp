/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2015, 2017-2018, 2025, Niklas Hauser
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
#include <modm/architecture/driver/atomic.hpp>
#include <modm/architecture/interface/can.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/fiber.hpp>

#include "mcp2515_definitions.hpp"
#include "mcp2515_bit_timings.hpp"

/**
 * Restructure filter and mask bits for the MCP2515
 *
 * @code
 * const uint8_t can_filter[] =
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
 * @endcode
 *
 * Filter 0 and 1 belong to Mask 0 (Group 0) and Filter 2 to 5 to
 * Mask 1 (Group 1). You can set one group to receive only standard messages
 * by using `MCP2515_FILTER()`/`MCP2515_MASK()` for all parts. Or to receive only
 * extended frame by using `MCP2515_FILTER_EXTENDED()` and `MCP2515_MASK_EXTENDED()`.
 * But you must not mix both possibilities within one group!
 *
 * You can use one group to receive standard frames and one group to receive
 * extended frames, but you can't set one group to receive both!
 *
 * @warning	Do not use this macro for variables, only for static values known
 *          at compile-time.
 * @ingroup	modm_driver_mcp2515
 * @{
 */
/// @hideinitializer
#define MCP2515_FILTER_EXTENDED(id)	\
		(uint8_t)  ((uint32_t) (id) >> 21), \
		(uint8_t)((((uint32_t) (id) >> 13) & 0xe0) | (1<<3) | \
			(((uint32_t) (id) >> 16) & 0x3)), \
		(uint8_t)  ((uint32_t) (id) >> 8), \
		(uint8_t)  ((uint32_t) (id))

/// @hideinitializer
#define	MCP2515_FILTER(id) \
		(uint8_t)((uint32_t) id >> 3), \
		(uint8_t)((uint32_t) id << 5), \
		0, \
		0

/// @hideinitializer
#define MCP2515_MASK_EXTENDED(id) MCP2515_FILTER_EXTENDED(id)

// TODO check this would receive all frames
/// @hideinitializer
#define	MCP2515_MASK(id) MCP2515_FILTER(id)
/// @}

namespace modm
{
	/**
	 * Driver for the MPC2515 CAN controller
	 *
	 * @tparam	SpiMaster	SPI interface
	 * @tparam	Cs			Chip select pin
	 * @tparam	Int			Interrupt pin
	 *
	 * If you want to activate the internal pull-up for the INT pin you
	 * need to do this by yourself before calling the initialize method!
	 *
	 * @author	Fabian Greif
	 * @ingroup	modm_driver_mcp2515
	 */
	template < class SpiMaster, class Cs, class Int,
			   size_t TxSize=32, size_t RxSize=32 >
	class Mcp2515 : public modm::SpiDevice<SpiMaster>
	{
	public:
		using BusState = Can::BusState;
		using Mode = Can::Mode;
		static constexpr size_t RxBufferSize = RxSize;
		static constexpr size_t TxBufferSize = TxSize;

		template<frequency_t ExternalClock, bitrate_t bitrate=kbps(125)>
		bool
		initialize(Mode mode = Mode::Normal)
		{
			this->attachConfigurationHandler([]
			{
				SpiMaster::setDataMode(SpiMaster::DataMode::Mode3);
				SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
			});
			Cs::setOutput();
			Int::setInput(Int::InputType::PullUp);

			using Timings = modm::CanBitTimingMcp2515<ExternalClock, bitrate>;
			if (not initializeWithPrescaler(Timings::getPrescaler(),
					Timings::getSJW(), Timings::getProp(),
					Timings::getPS1(), Timings::getPS2()))
				return false;

			setMode(mode);
			return true;
		}

		void
		setFilter(const uint8_t filter[32]);

		void
		setMode(Mode mode);

		/// Must be called periodically to service the MCP2515
		void
		update();

	public:
		bool
		isMessageAvailable() const
		{ return rxQueue.isNotEmpty(); }

		bool
		getMessage(can::Message& message, uint8_t *filter_id=nullptr)
		{
			(void) filter_id;
			if (rxQueue.isEmpty()) return false;
			message = rxQueue.get(); rxQueue.pop();
			return true;
		}

		bool
		isReadyToSend() const
		{ return txQueue.isNotFull(); }

		bool
		sendMessage(const can::Message& message)
		{ return txQueue.push(message); }

		BusState
		getBusState() const
		{ return BusState::Connected; }

	private:
		enum
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

		bool
		writeMessage(const can::Message& message);

		bool
		readMessage(can::Message& message);

		void
		writeRegister(uint8_t address, uint8_t data);

		uint8_t
		readRegister(uint8_t address);

		void
		bitModify(uint8_t address, uint8_t mask, uint8_t data);

		uint8_t
		readStatus(uint8_t type);

		bool
		initializeWithPrescaler(uint8_t prescaler, uint8_t sjw,
								uint8_t prop_seg, uint8_t ps1, uint8_t ps2);

	private:
		modm::atomic::Queue<modm::can::Message, TxBufferSize> txQueue;
		modm::atomic::Queue<modm::can::Message, RxBufferSize> rxQueue;
	};
}

#include "mcp2515_impl.hpp"

#endif // MODM_MCP2515_HPP
