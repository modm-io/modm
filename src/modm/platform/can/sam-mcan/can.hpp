/*
 * Copyright (c) 2019, 2023, Raphael Lehmann
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <optional>

#include <modm/architecture/interface/can.hpp>
#include <modm/architecture/interface/can_filter.hpp>
#include <modm/platform/gpio/connector.hpp>

#include "can_bit_timings.hpp"

#include "message_ram.hpp"

namespace modm::platform
{

using McanErrorCallback = void (*)();

extern McanErrorCallback mcan0ErrorCallback;
extern McanErrorCallback mcan1ErrorCallback;

/**
 * @brief		MCAN (CAN with Flexible Data-Rate)
 *
 * @author		Raphael Lehmann <raphael@rleh.de>
 * @author		Christopher Durand <christopher.durand@rwth-aachen.de>
 * @ingroup		modm_platform_can
 */
template<uint8_t id, fdcan::MessageRamConfig mrc>
class McanDriver : public ::modm::Can
{
public:
	enum class
	Mode : uint32_t
	{
		Normal,
		Restricted,
		Monitoring,
		Sleep,
		TestExternalLoopback,
		TestInternalLoopback,

		// for compatibility with bxCAN:
		ListenOnly = Monitoring, // no acknowledging
		LoopBack = TestInternalLoopback,

	};

private:
	static auto*
	Regs()
	{
		if constexpr (id == 0)
		{
			return MCAN0;
		}
#ifdef MCAN1
		else if constexpr (id == 1)
		{
			return MCAN1;
		}
#endif
		else
		{
			return nullptr;
		}
	}

	// helper for static_assert
	template<class T = void>
	static constexpr bool
	always_false_v = false;

	using MessageRam = fdcan::MessageRam<id, mrc>;
	static_assert(MessageRam::StandardFilterCount <= 128, "A maximum of 128 standard filters are allowed.");
	static_assert(MessageRam::ExtendedFilterCount <= 64, "A maximum of 64 standard filters are allowed.");
	static_assert(MessageRam::RxFifo0Elements <= 64, "A maximum of 64 Rx Fifo 0 elements are allowed.");
	static_assert(MessageRam::RxFifo1Elements <= 64, "A maximum of 64 Rx Fifo 1 elements are allowed.");
	static_assert(MessageRam::RxBufferElements <= 64, "A maximum of 64 dedicated Rx buffers are allowed.");
	static_assert(MessageRam::TxEventFifoEntries <= 32, "A maximum of 32 Tx Event Fifo elements are allowed.");
	static_assert(MessageRam::TxFifoElements <= 32, "A maximum of 32 Tx Fifo elements are allowed.");
	static_assert(MessageRam::Size <= 4352*4, "Max message ram size is 4352 words.");

	static inline std::array<uint32_t, MessageRam::Size/4> modm_aligned(4)
	messageRamMemory{};

	struct RxMessage
	{
		modm::can::Message message;
		uint8_t filter_id;
		uint16_t timestamp;
	};

	static void
	initializeWithPrescaler(CanBitTimingConfiguration standardTimings,
							std::optional<CanBitTimingConfiguration> fdDataTimings,
							uint32_t interruptPriority, Mode startupMode, bool overwriteOnOverrun);

public:
	// Expose template parameters to be checked by e.g. drivers or application
	static constexpr size_t RxBufferSize = std::min(MessageRam::RxFifo0Elements, MessageRam::RxFifo1Elements);
	static constexpr size_t TxBufferSize = MessageRam::TxFifoElements;

	template<class... Pins>
	static void
	connect(modm::platform::InputType inputType = modm::platform::InputType::Floating)
	{
		using RxPin = GetPin_t<PeripheralPin::Rx, Pins...>;
		using TxPin = GetPin_t<PeripheralPin::Tx, Pins...>;

		using Peripheral = typename Peripherals::Mcan<id>;

		if constexpr (!std::is_void_v<RxPin>) {
			RxPin::configure(inputType);
			using RxConnector = typename RxPin::template Connector<Peripheral, typename Peripheral::template Canrx<id>>;
			RxConnector::connect();
		}

		if constexpr (!std::is_void_v<TxPin>) {
			using TxConnector = typename TxPin::template Connector<Peripheral, typename Peripheral::template Cantx<id>>;
			TxConnector::connect();
		}
	}

	/**
	 * Enables the clock for the CAN controller and resets all settings
	 *
	 * \tparam SystemClock
	 * 			System clock struct with an MCAN member containing
	 * 			the clock speed supplied to the peripheral.
	 * 			\warning	The CAN subsystem prescaler can be configured using
	 * 						the RCC module and must be taken into account in
	 * 						the provided clock speed value.
	 * \tparam bitrate
	 * 			Nominal CAN bitrate
	 * \tparam tolerance
	 * 			Maximum relative deviation between requested and resulting
	 * 			CAN bitrates. If the tolerance is exceeded a compile-time
	 * 			assertion will be triggered.
	 * \tparam fastDataBitrate
	 * 			CAN bitrate for data in FD frames with bit rate switching
	 * 			Set to 0 to disable CANFD support.
	 *
	 * \param interruptPriority
	 * 			Not used in this driver: Interrupt vector priority (0=highest to 15=lowest)
	 * \param startupMode
	 * 			Mode of operation set after initialization
	 * 			\see Mcan<id>::Mode
	 * \param overwriteOnOverrun
	 * 			Once a receive FIFO is full the next incoming message
	 * 			will overwrite the previous one if \c true otherwise
	 * 			the incoming message will be discarded
	 *
	 * \warning	Has to called after connect(), but before any
	 * 			other function from this class!
	 */
	template<
		class SystemClock,
		bitrate_t bitrate=kbps(125),
		percent_t tolerance=pct(1),
		bitrate_t fastDataBitrate=0 // 0: MCAN mode disabled
	>
	static inline void
	initialize(uint32_t interruptPriority, Mode startupMode = Mode::Normal,
				bool overwriteOnOverrun = true)
	{
		using Timings = CanBitTiming<
			SystemClock::Mcan,
			bitrate,
			9, 8, 7, 7
		>;
		Timings::template assertBitrateInTolerance<tolerance>();

		std::optional<CanBitTimingConfiguration> fastDataTimings = std::nullopt;

		if constexpr (fastDataBitrate != 0) {
			using DataTimings = CanBitTiming<
				SystemClock::Mcan,
				fastDataBitrate,
				5, 5, 4, 3
			>;
			DataTimings::template assertBitrateInTolerance<tolerance>();

			fastDataTimings = DataTimings::getBitTimings();
		}

		return initializeWithPrescaler(
			Timings::getBitTimings(),
			fastDataTimings,
			interruptPriority,
			startupMode,
			overwriteOnOverrun);
	}

	/**
	 * Set the operating mode.
	 *
	 * Default after initialization is the normal mode.
	 */
	static void
	setMode(Mode mode);

	static void
	setAutomaticRetransmission(bool retransmission);

public:
	// Can interface methods
	static bool
	isMessageAvailable();

	static bool
	getMessage(can::Message& message, uint8_t *filter_id=nullptr, uint16_t *timestamp=nullptr);

	static bool
	isReadyToSend();

	static bool
	sendMessage(const can::Message& message);

public:
	// Can filter configuration

	using FilterConfig = MessageRam::FilterConfig;

	/// Set standard filter with id and mask
	/// \param standardIndex Standard filter index 0..27
	/// \returns true if filter index is valid
	static bool
	setStandardFilter(uint8_t standardIndex, FilterConfig config,
		modm::can::StandardIdentifier id_,
		modm::can::StandardMask mask);

	/// Set standard filter with dual ids
	/// Matches on any of both specified ids
	/// \param standardIndex Standard filter index 0..27
	/// \returns true if filter index is valid
	static bool
	setStandardFilter(uint8_t standardIndex, FilterConfig config,
		modm::can::StandardIdentifier id0,
		modm::can::StandardIdentifier id1);

	/// Set standard range filter
	/// Matches the inclusive range between both specified ids
	/// \param standardIndex Standard filter index 0..27
	/// \returns true if filter index is valid
	static bool
	setStandardRangeFilter(uint8_t standardIndex, FilterConfig config,
		modm::can::StandardIdentifier first,
		modm::can::StandardIdentifier last);

	/// Set extended filter with id and mask
	/// \param extendedIndex Extended filter index 0..7
	/// \returns true if filter index is valid
	static bool
	setExtendedFilter(uint8_t extendedIndex, FilterConfig config,
		modm::can::ExtendedIdentifier id_,
		modm::can::ExtendedMask mask);

	/// Set standard filter with dual ids
	/// Matches on any of both specified ids
	/// \param extendedIndex Extended filter index 0..7
	/// \returns true if filter index is valid
	static bool
	setExtendedFilter(uint8_t extendedIndex, FilterConfig config,
		modm::can::ExtendedIdentifier id0,
		modm::can::ExtendedIdentifier id1);

	/// Set standard range filter
	/// Matches the inclusive range between both specified ids
	/// \param extendedIndex Extended filter index 0..7
	/// \returns true if filter index is valid
	static bool
	setExtendedRangeFilter(uint8_t extendedIndex, FilterConfig config,
		modm::can::ExtendedIdentifier first,
		modm::can::ExtendedIdentifier last);

	/// Disable all standard filters, receive no standard frames
	static void
	clearStandardFilters();

	/// Disable all extended filters, receive no extended frames
	static void
	clearExtendedFilters();

public:
	// Extended Functionality
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
		return ((Regs()->MCAN_ECR >> 8) & 0x7F);
	}

	/**
	 * Get Transmit Error Counter.
	 *
	 */
	static inline uint8_t
	getTransmitErrorCounter()
	{
		return (Regs()->MCAN_ECR & 0xFF);
	}

	static BusState
	getBusState();

	/**
	 * Set the error interrupt callback.
	 *
	 * It will be called on the following events:
	 * - The MCAN peripheral enters ERROR_PASSIVE or BUS_OFF state
	 * - The error counter exceeds the warning limit
	 *
	 * To disable the interrupt set the callback to nullptr.
	 */
	static void
	setErrorInterruptCallback(McanErrorCallback callback)
	{
		if constexpr (id == 0) {
			modm::platform::mcan0ErrorCallback = callback;
		}
		else {
			modm::platform::mcan1ErrorCallback = callback;
		}
		if(callback) {
			Regs()->MCAN_IE |=  (MCAN_IE_BOE | MCAN_IE_EPE | MCAN_IE_EWE);
		} else {
			Regs()->MCAN_IE &= ~(MCAN_IE_BOE | MCAN_IE_EPE | MCAN_IE_EWE);
		}
	}

	static McanErrorCallback
	getErrorInterruptCallback()
	{
		if constexpr (id == 0) {
			return modm::platform::mcan0ErrorCallback;
		}
		else {
			return modm::platform::mcan1ErrorCallback;
		}
	}

	static uint16_t
	getCurrentTimestamp()
	{
		return Regs()->MCAN_TSCV;
	}

private:
	static void
	configureMode(Mode mode);

	static void
	configureInterrupts(uint32_t interruptPriority);

	struct EnterInitMode
	{
		EnterInitMode()
		{
			Regs()->MCAN_CCCR |= MCAN_CCCR_INIT;
			int deadlockPreventer = 10'000; // max ~10ms
			while (((Regs()->MCAN_CCCR & MCAN_CCCR_INIT) == 0) and (deadlockPreventer-- > 0)) {
				using namespace std::literals;
				modm::delay_us(1);
				// Wait until the initialization mode is entered.
			}
			modm_assert(deadlockPreventer > 0, "can.init", "timeout expired");
			Regs()->MCAN_CCCR |= MCAN_CCCR_CCE;
		}

		~EnterInitMode()
		{
			// Switch to normal operation, automatically clears CCE flag
			Regs()->MCAN_CCCR &= ~MCAN_CCCR_INIT;
		}

		EnterInitMode(const EnterInitMode&) = delete;
		EnterInitMode& operator=(const EnterInitMode&) = delete;
	};

private:
	static bool
	isHardwareTxQueueFull();

	static bool
	rxFifo0HasMessage();

	static bool
	rxFifo1HasMessage();

	static void
	acknowledgeRxFifoRead(uint8_t fifoIndex, uint8_t getIndex);

	static uint8_t
	retrieveRxFifoGetIndex(uint8_t fifoIndex);

	static uint8_t
	retrieveTxFifoPutIndex();

	static void
	readMsg(modm::can::Message& message, uint8_t fifoIndex, uint8_t* filter_id, uint16_t *timestamp);

	static bool
	sendMsg(const modm::can::Message& message);
};

}	// namespace modm::platform

#include "can_impl.hpp"
