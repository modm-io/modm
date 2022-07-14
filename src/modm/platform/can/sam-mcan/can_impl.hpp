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

#include <modm/architecture/interface/assert.hpp>
#include <modm/platform/clock/clockgen.hpp>

#include "can.hpp"

namespace modm::platform {

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::isHardwareTxQueueFull()
{
	return ((Regs()->MCAN_TXFQS & MCAN_TXFQS_TFQF) != 0);
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::rxFifo0HasMessage()
{
	return ((Regs()->MCAN_RXF0S & MCAN_RXF0S_F0FL_Msk) > 0);
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::rxFifo1HasMessage()
{
	return ((Regs()->MCAN_RXF1S & MCAN_RXF1S_F1FL_Msk) > 0);
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::acknowledgeRxFifoRead(uint8_t fifoIndex, uint8_t getIndex)
{
	if (fifoIndex == 0) {
		Regs()->MCAN_RXF0A = getIndex;
	}
	else {
		Regs()->MCAN_RXF1A = getIndex;
	}
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
uint8_t
McanDriver<id, mrc>::retrieveRxFifoGetIndex(uint8_t fifoIndex)
{
	if (fifoIndex == 0) {
		return ((Regs()->MCAN_RXF0S & MCAN_RXF0S_F0GI_Msk) >> MCAN_RXF0S_F0GI_Pos);
	} else {
		return ((Regs()->MCAN_RXF1S & MCAN_RXF1S_F1GI_Msk) >> MCAN_RXF1S_F1GI_Pos);
	}
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
uint8_t
McanDriver<id, mrc>::retrieveTxFifoPutIndex()
{
	return ((Regs()->MCAN_TXFQS & MCAN_TXFQS_TFQPI_Msk) >> MCAN_TXFQS_TFQPI_Pos);
}

// Internal function to receive a message from an RX Fifo.
// Called by RX interrupt or by getMessage()
template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::readMsg(modm::can::Message& message, uint8_t fifoIndex, uint8_t* filter_id, uint16_t *timestamp)
{
	using CommonHeader = MessageRam::CommonFifoHeader;
	using RxFifoAddress = MessageRam::RxFifoAddress;

	// retrieve index of next frame in RX fifo
	const uint8_t getIndex = retrieveRxFifoGetIndex(fifoIndex);
	const RxFifoAddress address = {fifoIndex, getIndex};

	const auto [commonHeader, rxHeader] = MessageRam::readRxHeaders(address);

	message.setExtended(bool(commonHeader & CommonHeader::ExtendedId));
	message.setRemoteTransmitRequest(bool(commonHeader & CommonHeader::RemoteFrame));
	const auto canid = MessageRam::CanId_t::get(commonHeader);
	if(message.isExtended()) {
		message.setIdentifier(canid);
	} else {
		message.setIdentifier(canid >> 18);
	}

	if (filter_id != nullptr) {
		*filter_id = MessageRam::FilterIndex_t::get(rxHeader);
	}

	if (timestamp != nullptr) {
		*timestamp = MessageRam::Timestamp_t::get(rxHeader);
	}

	const uint8_t dlcValue = MessageRam::RxDlc_t::get(rxHeader);
	message.setDataLengthCode(dlcValue);

	// required for optimization in MessageRam::readData()
	static_assert((std::size(decltype(message.data){}) % 4) == 0);

	MessageRam::readData(address, {&message.data[0], message.getLength()});
	acknowledgeRxFifoRead(fifoIndex, getIndex);
}

// Internal function to send a CAN message.
// called by sendMessage and by TX Interrupt.
template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::sendMsg(const modm::can::Message& message)
{
	if (!McanDriver<id, mrc>::isReadyToSend()) {
		return false;
	}

	const uint8_t putIndex = retrieveTxFifoPutIndex();
	const auto commonHeader = MessageRam::headerFromMessage(message);
	const auto txHeader = MessageRam::txHeaderFromMessage(message);
	MessageRam::writeTxHeaders(putIndex, commonHeader, txHeader);

	// required for optimization in MessageRam::readData()
	static_assert((std::size(decltype(message.data){}) % 4) == 0);

	MessageRam::writeData(putIndex, {&message.data[0], message.getLength()});

	// Make sure data is written to RAM before transfer is started.
	// Otherwise messages are corrupted.
	__DMB();

	// Activate the corresponding transmission request
	Regs()->MCAN_TXBAR = (1u << putIndex);

	return true;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::initializeWithPrescaler(
		CanBitTimingConfiguration standardTimings,
		std::optional<CanBitTimingConfiguration> fdDataTimings,
		uint32_t interruptPriority, Mode startupMode,
		bool overwriteOnOverrun)
{
	if constexpr (id == 0) {
		ClockGen::enable<ClockPeripheral::Can0>();
	}
#ifdef MCAN1
	else if constexpr (id == 1) {
		ClockGen::enable<ClockPeripheral::Can1>();
	}
#endif

	EnterInitMode init;

	MessageRam::setRamBase(uintptr_t(&messageRamMemory[0]));

	// Configure message ram upper 16 bits
	if constexpr (id == 0) {
		MATRIX->CCFG_CAN0 = (MATRIX->CCFG_CAN0 & ~CCFG_CAN0_CAN0DMABA_Msk) |
							CCFG_CAN0_CAN0DMABA((MessageRam::getRamBase() >> 16));
	}
#ifdef MCAN1
	else if constexpr (id == 1) {
		MATRIX->CCFG_SYSIO = (MATRIX->CCFG_SYSIO & ~CCFG_SYSIO_CAN1DMABA_Msk) |
							 CCFG_SYSIO_CAN1DMABA((MessageRam::getRamBase() >> 16));
	}
#endif

	// Configure nominal bitrate
	Regs()->MCAN_NBTP =
		((standardTimings.sjw - 1) << MCAN_NBTP_NSJW_Pos) |
		((standardTimings.bs2 - 1) << MCAN_NBTP_NTSEG2_Pos) |
		((standardTimings.bs1 - 1) << MCAN_NBTP_NTSEG1_Pos) |
		((standardTimings.prescaler - 1) << MCAN_NBTP_NBRP_Pos);

	if(fdDataTimings) {
		// Configure FD mode fast data bitrate
		const auto& timings = *fdDataTimings;
		Regs()->MCAN_DBTP =
			((timings.sjw - 1) << MCAN_DBTP_DSJW_Pos) |
			((timings.bs2 - 1) << MCAN_DBTP_DTSEG2_Pos) |
			((timings.bs1 - 1) << MCAN_DBTP_DTSEG1_Pos) |
			((timings.prescaler - 1) << MCAN_DBTP_DBRP_Pos)/* |
			MCAN_DBTP_TDC*/; // enable "Transceiver Delay Compensation"
	}

	// Timestamp: FDCAN internal counter with prescaler=1
	// In CAN FD mode the internal timestamp counter TCP does not provide a constant time
	// base due to the different CAN bit times between arbitration phase and data phase.
	Regs()->MCAN_TSCC = (1 << MCAN_TSCC_TSS_Pos);

	// Fifo configuration
	Regs()->MCAN_RXF0C = (overwriteOnOverrun ? MCAN_RXF0C_F0OM : 0) |
						 MCAN_RXF0C_F0S(MessageRam::RxFifo0Elements) |
						 MCAN_RXF0C_F0SA(MessageRam::RxFifo0() >> 2);
	Regs()->MCAN_RXF1C = (overwriteOnOverrun ? MCAN_RXF1C_F1OM : 0) |
						 MCAN_RXF1C_F1S(MessageRam::RxFifo1Elements) |
						 MCAN_RXF1C_F1SA(MessageRam::RxFifo1() >> 2);

	// Buffer / FIFO Element Size Configuration

	Regs()->MCAN_RXESC = 0;

	if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 8 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_8_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 12 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_12_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 16 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_16_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 20 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_20_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 24 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_24_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 32 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_32_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 48 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_48_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo0ElementSize - 2*4) == 64 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F0DS_64_BYTE;
	}
	else {
		static_assert(always_false_v<>, "Invalid MessageRam::RxFifo0ElementSize value");
	}

	if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 8 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_8_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 12 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_12_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 16 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_16_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 20 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_20_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 24 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_24_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 32 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_32_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 48 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_48_BYTE;
	}
	else if constexpr ((MessageRam::RxFifo1ElementSize - 2*4) == 64 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_F1DS_64_BYTE;
	}
	else {
		static_assert(always_false_v<>, "Invalid MessageRam::RxFifo1ElementSize value");
	}

	if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 8 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_8_BYTE;
	}
	else if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 12 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_12_BYTE;
	}
	else if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 16 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_16_BYTE;
	}
	else if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 20 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_20_BYTE;
	}
	else if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 24 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_24_BYTE;
	}
	else if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 32 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_32_BYTE;
	}
	else if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 48 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_48_BYTE;
	}
	else if constexpr ((MessageRam::RxBufferElementSize - 2*4) == 64 ) {
		Regs()->MCAN_RXESC |= MCAN_RXESC_RBDS_64_BYTE;
	}
	else {
		static_assert(always_false_v<>, "Invalid MessageRam::RxBufferElementSize value");
	}

	if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 8 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_8_BYTE;
	}
	else if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 12 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_12_BYTE;
	}
	else if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 16 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_16_BYTE;
	}
	else if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 20 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_20_BYTE;
	}
	else if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 24 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_24_BYTE;
	}
	else if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 32 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_32_BYTE;
	}
	else if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 48 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_48_BYTE;
	}
	else if constexpr ((MessageRam::TxFifoElementSize - 2*4) == 64 ) {
		Regs()->MCAN_TXESC = MCAN_TXESC_TBDS_64_BYTE;
	}
	else {
		static_assert(always_false_v<>, "Invalid MessageRam::TxFifoElementSize value");
	}

	// Configure dedicated Rx buffer address
	Regs()->MCAN_RXBC = MCAN_RXBC_RBSA(MessageRam::RxBuffer() >> 2);

	// Filter configuration
	Regs()->MCAN_SIDFC = MCAN_SIDFC_LSS(MessageRam::StandardFilterCount) |
						 MCAN_SIDFC_FLSSA(MessageRam::FilterListStandard() >> 2);
	Regs()->MCAN_XIDFC = MCAN_XIDFC_LSE(MessageRam::ExtendedFilterCount) |
						 MCAN_XIDFC_FLESA(MessageRam::FilterListExtended() >> 2);

	// reject non-matching frames, allow remote frames
	Regs()->MCAN_GFC = MCAN_GFC_ANFE(0b11) | MCAN_GFC_ANFS(0b11);

	// Tx buffer: queue mode, no dedicated transmit buffers
	Regs()->MCAN_TXBC = MCAN_TXBC_TFQM | MCAN_TXBC_TFQS(MessageRam::TxFifoElements) |
						MCAN_TXBC_NDTB(0) | MCAN_TXBC_TBSA(MessageRam::TxFifo() >> 2);

	Regs()->MCAN_TXEFC = MCAN_TXEFC_EFS(MessageRam::TxEventFifoEntries) |
						 MCAN_TXEFC_EFSA(MessageRam::TxEventFifo() >> 2);

	// Enable bit rate switching and CANFD frame format
	if(fdDataTimings) {
		Regs()->MCAN_CCCR |= MCAN_CCCR_BRSE | MCAN_CCCR_FDOE;
	} else {
		Regs()->MCAN_CCCR &= ~(MCAN_CCCR_BRSE | MCAN_CCCR_FDOE);
	}

	configureInterrupts(interruptPriority);

	configureMode(startupMode);
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::setMode(Mode mode)
{
	EnterInitMode init;
	configureMode(mode);
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::configureMode(Mode mode)
{
	// Reset all mode register bits
	Regs()->MCAN_TEST = 0;
	Regs()->MCAN_CCCR &= ~(MCAN_CCCR_ASM | MCAN_CCCR_MON | MCAN_CCCR_CSR | MCAN_CCCR_TEST);

	// set mode
	switch(mode) {
		case Mode::Normal:
			break;
		case Mode::Restricted:
			Regs()->MCAN_CCCR |= MCAN_CCCR_ASM;
			break;
		case Mode::Monitoring:
			Regs()->MCAN_CCCR |= MCAN_CCCR_MON;
			break;
		case Mode::Sleep:
			Regs()->MCAN_CCCR |= MCAN_CCCR_CSR;
			break;
		case Mode::TestExternalLoopback:
			Regs()->MCAN_CCCR |= MCAN_CCCR_TEST;
			Regs()->MCAN_TEST = MCAN_TEST_LBCK;
			break;
		case Mode::TestInternalLoopback:
			Regs()->MCAN_CCCR |= MCAN_CCCR_TEST | MCAN_CCCR_MON;
			Regs()->MCAN_TEST = MCAN_TEST_LBCK;
			break;
	}
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::setAutomaticRetransmission(bool retransmission)
{
	if (retransmission) {
		// Enable retransmission
		Regs()->MCAN_CCCR &= ~MCAN_CCCR_DAR;
	} else {
		// Disable retransmission
		Regs()->MCAN_CCCR |= MCAN_CCCR_DAR;
	}
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::isMessageAvailable()
{
	return rxFifo0HasMessage() || rxFifo1HasMessage();
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::getMessage(can::Message& message, uint8_t *filter_id, uint16_t *timestamp)
{
	if (rxFifo0HasMessage()) {
		readMsg(message, 0, filter_id, timestamp);
		return true;
	} else if (rxFifo1HasMessage()) {
		readMsg(message, 1, filter_id, timestamp);
		return true;
	}
	return false;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::isReadyToSend()
{
	return !isHardwareTxQueueFull();
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::sendMessage(const can::Message& message)
{
	return sendMsg(message);
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
McanDriver<id, mrc>::BusState
McanDriver<id, mrc>::getBusState()
{
	if (Regs()->MCAN_PSR & MCAN_PSR_BO) {
		return BusState::Off;
	}
	else if (Regs()->MCAN_PSR & MCAN_PSR_EP) {
		return BusState::ErrorPassive;
	}
	else if (Regs()->MCAN_PSR & MCAN_PSR_EW) {
		return BusState::ErrorWarning;
	}
	else {
		return BusState::Connected;
	}
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::setStandardFilter(
	uint8_t standardIndex, FilterConfig config,
	modm::can::StandardIdentifier id_,
	modm::can::StandardMask mask)
{
	if (standardIndex >= MessageRam::StandardFilterCount) {
		return false;
	}

	MessageRam::setStandardFilter(standardIndex,
		MessageRam::FilterType::Classic,
		config, uint16_t(id_), uint16_t(mask));

	return true;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::setStandardFilter(
	uint8_t standardIndex, FilterConfig config,
	modm::can::StandardIdentifier id0,
	modm::can::StandardIdentifier id1)
{
	if (standardIndex >= MessageRam::StandardFilterCount) {
		return false;
	}

	MessageRam::setStandardFilter(standardIndex,
		MessageRam::FilterType::Dual,
		config, uint16_t(id0), uint16_t(id1));

	return true;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::setStandardRangeFilter(
	uint8_t standardIndex, FilterConfig config,
	modm::can::StandardIdentifier first,
	modm::can::StandardIdentifier last)
{
	if (standardIndex >= MessageRam::StandardFilterCount) {
		return false;
	}

	MessageRam::setStandardFilter(standardIndex,
		MessageRam::FilterType::Range,
		config, uint16_t(first), uint16_t(last));

	return true;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::setExtendedFilter(
	uint8_t extendedIndex, FilterConfig config,
	modm::can::ExtendedIdentifier id_,
	modm::can::ExtendedMask mask)
{
	if (extendedIndex >= MessageRam::ExtendedFilterCount) {
		return false;
	}

	MessageRam::setExtendedFilterDisabled(extendedIndex);

	MessageRam::setExtendedFilter1(extendedIndex,
		MessageRam::FilterType::Classic, uint32_t(mask));
	MessageRam::setExtendedFilter0(extendedIndex,
		config, uint32_t(id_));

	return true;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::setExtendedFilter(
	uint8_t extendedIndex, FilterConfig config,
	modm::can::ExtendedIdentifier id0,
	modm::can::ExtendedIdentifier id1)
{
	if (extendedIndex >= MessageRam::ExtendedFilterCount) {
		return false;
	}

	MessageRam::setExtendedFilterDisabled(extendedIndex);

	MessageRam::setExtendedFilter1(extendedIndex,
		MessageRam::FilterType::Dual, uint32_t(id1));
	MessageRam::setExtendedFilter0(extendedIndex,
		config, uint32_t(id0));

	return true;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
bool
McanDriver<id, mrc>::setExtendedRangeFilter(
	uint8_t extendedIndex, FilterConfig config,
	modm::can::ExtendedIdentifier first,
	modm::can::ExtendedIdentifier last)
{
	if (extendedIndex >= MessageRam::ExtendedFilterCount) {
		return false;
	}

	MessageRam::setExtendedFilterDisabled(extendedIndex);

	MessageRam::setExtendedFilter1(extendedIndex,
		MessageRam::FilterType::Range, uint32_t(last));
	MessageRam::setExtendedFilter0(extendedIndex,
		config, uint32_t(first));

	return true;
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::clearStandardFilters()
{
	for (unsigned i = 0; i < MessageRam::StandardFilterCount; ++i) {
		MessageRam::setStandardFilterDisabled(i);
	}
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::clearExtendedFilters()
{
	for (unsigned i = 0; i < MessageRam::ExtendedFilterCount; ++i) {
		MessageRam::setExtendedFilterDisabled(i);
	}
}

template<uint8_t id, fdcan::MessageRamConfig mrc>
void
McanDriver<id, mrc>::configureInterrupts(uint32_t interruptPriority)
{
	if constexpr (id == 0) {
		NVIC_SetPriority(MCAN0_INT0_IRQn, interruptPriority);
		NVIC_EnableIRQ(MCAN0_INT0_IRQn);
	}
#ifdef MCAN1
	else if constexpr (id == 1) {
		NVIC_SetPriority(MCAN1_INT0_IRQn, interruptPriority);
		NVIC_EnableIRQ(MCAN1_INT0_IRQn);
	}
#endif
}

}
