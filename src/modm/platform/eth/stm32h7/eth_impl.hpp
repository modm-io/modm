/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ETH_HPP
#	error	"Don't include this file directly, use 'eth.hpp' instead!"
#endif

namespace modm::platform {


inline EthernetMac::InitializationResult
EthernetMac::initialize(MediaInterface interface, uint32_t ethernetClock,
		Configuration const& configuration, uint8_t priority)
{
	if (state().txLeaseOutstanding or state().rxLeaseOutstanding)
		return {InitializationError::OutstandingLease};

	Rcc::enable<Peripheral::Eth>();
	const bool wasRunning = driverState() == DriverState::Running;
	const bool preserveHardwareCounters = driverState() != DriverState::Uninitialized;
	NVIC_DisableIRQ(ETH_IRQn);
	ETH->DMACIER = 0;
	const uint32_t activeDmaStatus = captureLiveDmaStatus(wasRunning);
	ETH->DMACSR = activeDmaStatus & detail::EthH7DmaHandledStatus;
	if ((activeDmaStatus & ETH_DMACSR_FBE) != 0)
		setDriverFailure(DriverFailure::FatalBusError);
	ETH->DMACTCR &= ~ETH_DMACTCR_ST;
	ETH->DMACRCR &= ~ETH_DMACRCR_SR;
	// Stop MAC TX/RX
	ETH->MACCR &= ~(ETH_MACCR_TE | ETH_MACCR_RE);
	// Preserve hardware diagnostics across reinitialization
	if (preserveHardwareCounters)
		accumulateHardwareCounters();
	setDriverState(DriverState::Uninitialized);
	state().driverFailure = DriverFailure::None;
	state().linkState = uint8_t(LinkState::Down);
	state().interruptEvents = 0;

	// Route Ethernet signals through the selected MII or RMII interface
	NVIC_SetPriority(ETH_IRQn, priority);
	selectMediaInterface(interface);
	state().mediaInterface = interface;
	const uint32_t pendingDmaStatus = captureLiveDmaStatus(false);
	ETH->DMACSR = pendingDmaStatus & detail::EthH7DmaHandledStatus;
	if ((pendingDmaStatus & ETH_DMACSR_FBE) != 0)
		setDriverFailure(DriverFailure::FatalBusError);
	ETH->DMAMR |= ETH_DMAMR_SWR;
	if (not modm::this_fiber::poll_for(std::chrono::microseconds{MacResetTimeoutUs},
			[]{ return (ETH->DMAMR & ETH_DMAMR_SWR) == 0; }))
		return {InitializationError::MacResetTimeout};

	// Validate the requested checksum offload against the MAC capabilities
	state().checksumOffloadEnabled = configuration.checksumMode == ChecksumMode::Hardware;
	if (state().checksumOffloadEnabled) {
		constexpr uint32_t capabilities = ETH_MACHWF0R_RXCOESEL | ETH_MACHWF0R_TXCOESEL;
		if ((ETH->MACHWF0R & capabilities) != capabilities) {
			state().checksumOffloadEnabled = false;
			return {InitializationError::UnsupportedChecksumOffload};
		}
	}

	// Select an MDIO divider for the Ethernet peripheral clock
	if (not configureMdioClock(ethernetClock))
		return {InitializationError::UnsupportedMdioClock};

	// Configure a stopped interface with fresh descriptor rings
	state().macAddress = configuration.macAddress;
	state().duplexMode = DuplexMode::Full;
	state().speed = Speed::Speed100M;
	configureMac();
	configureDma();
	initializeDescriptors();
	ETH->DMACSR = detail::EthH7DmaHandledStatus;
	state().driverFailure = DriverFailure::None;
	setDriverState(DriverState::Ready);
	state().linkState = uint8_t(LinkState::Down);
	NVIC_ClearPendingIRQ(ETH_IRQn);
	enableInterruptVector();
	return {};
}

inline void
EthernetMac::selectMediaInterface(MediaInterface interface)
{
	const uint32_t selection = interface == MediaInterface::RMII ?
			SYSCFG_PMCR_EPIS_SEL_2 : 0;
	SYSCFG->PMCR = (SYSCFG->PMCR & ~SYSCFG_PMCR_EPIS_SEL) | selection;
	(void) SYSCFG->PMCR;
}

inline bool
EthernetMac::configureMdioClock(uint32_t ethernetClock)
{
	if (ethernetClock < MdioClockMinimum or ethernetClock > MdioClockMaximum)
		return false;
	uint32_t divider;
	if (ethernetClock < MdioDiv16ClockLimit) divider = ETH_MACMDIOAR_CR_DIV16;
	else if (ethernetClock < MdioDiv26ClockLimit) divider = ETH_MACMDIOAR_CR_DIV26;
	else if (ethernetClock < MdioDiv42ClockLimit) divider = ETH_MACMDIOAR_CR_DIV42;
	else if (ethernetClock < MdioDiv62ClockLimit) divider = ETH_MACMDIOAR_CR_DIV62;
	else if (ethernetClock < MdioDiv102ClockLimit) divider = ETH_MACMDIOAR_CR_DIV102;
	else divider = ETH_MACMDIOAR_CR_DIV124;
	ETH->MACMDIOAR = (ETH->MACMDIOAR & ~ETH_MACMDIOAR_CR) | divider;
	return true;
}

inline void
EthernetMac::configureMac()
{
	uint32_t maccr = ETH_MACCR_IPG_96BIT | ETH_MACCR_CST | ETH_MACCR_ACS;
	if (state().speed == Speed::Speed100M) maccr |= ETH_MACCR_FES;
	if (state().duplexMode == DuplexMode::Full) maccr |= ETH_MACCR_DM;
	if (state().checksumOffloadEnabled) maccr |= ETH_MACCR_IPC;
	ETH->MACCR = maccr;
	ETH->MACPFR = ETH_MACPFR_PCF_BLOCKALL;
	ETH->MACHT0R = 0;
	ETH->MACHT1R = 0;
	ETH->MACVTR = 0;
	ETH->MACTFCR = 0;
	ETH->MACRFCR = 0;
	configureMacAddress();
}

inline void
EthernetMac::configureDma()
{
	// Enable store and forward queues. They buffer each complete frame before
	// DMA transmission or reception.
	ETH->MTLTQOMR |= ETH_MTLTQOMR_TSF;
	// Retain checksum error frames only when software must validate them
	uint32_t receiveQueue = ETH->MTLRQOMR | ETH_MTLRQOMR_RSF;
	if (state().checksumOffloadEnabled) receiveQueue |= ETH_MTLRQOMR_DISTCPEF;
	else receiveQueue &= ~ETH_MTLRQOMR_DISTCPEF;
	receiveQueue &= ~ETH_MTLRQOMR_FEP;
	ETH->MTLRQOMR = receiveQueue;

	// Give TX and RX equal DMA priority
	ETH->DMAMR = ETH_DMAMR_PR_1_1;
	// Use aligned fixed bursts on the system bus
	ETH->DMASBMR = ETH_DMASBMR_AAL | ETH_DMASBMR_FB;

	// Descriptors occupy one cache line each
	ETH->DMACCR = (ETH->DMACCR & ~(ETH_DMACCR_DSL | ETH_DMACCR_MSS | ETH_DMACCR_8PBL)) |
			ETH_DMACCR_DSL_128BIT;
	// Transfer packet buffers in 32 beat bursts
	ETH->DMACTCR = ETH_DMACTCR_TPBL_32PBL | ETH_DMACTCR_OSP;
	ETH->DMACRCR = ETH_DMACRCR_RPBL_32PBL |
			((DmaBufferSize << ETH_DMACRCR_RBSZ_Pos) & ETH_DMACRCR_RBSZ);

	// Disable DMA channel interrupts until start() enables the link
	ETH->DMACIER = 0;
	// Mask MAC interrupt sources because this driver only uses DMA channel interrupts
	ETH->MACIER = 0;
	// Mask unused MMC counter interrupts
	ETH->MMCTIMR = ETH_MMCTIMR_TXLPITRCIM | ETH_MMCTIMR_TXLPIUSCIM |
			ETH_MMCTIMR_TXGPKTIM | ETH_MMCTIMR_TXMCOLGPIM |
			ETH_MMCTIMR_TXSCOLGPIM;
	ETH->MMCRIMR = ETH_MMCRIMR_RXLPITRCIM | ETH_MMCRIMR_RXLPIUSCIM |
			ETH_MMCRIMR_RXUCGPIM | ETH_MMCRIMR_RXALGNERPIM |
			ETH_MMCRIMR_RXCRCERPIM;
}

inline EthernetMac::LinkUpdateError
EthernetMac::updateLinkMode(bool requireReceiveQueueIdle)
{
	const bool fullToHalf = detail::needsFullToHalfDuplexFlush(
			(ETH->MACCR & ETH_MACCR_DM) ? DuplexMode::Full : DuplexMode::Half,
			state().duplexMode);
	uint32_t macConfiguration = ETH->MACCR & ~(ETH_MACCR_FES | ETH_MACCR_DM);
	if (state().speed == Speed::Speed100M) macConfiguration |= ETH_MACCR_FES;
	if (state().duplexMode == DuplexMode::Full) macConfiguration |= ETH_MACCR_DM;

	if (fullToHalf) {
		// ES0392 Rev 15, section 2.26.10: validate the stopped receive and transmit
		// paths, then make FTQ the immediately following MMIO write after MACCR.DM
		// is cleared. RM0433 Rev 8, section 58.9.8 defines link-mode changes.
		if (requireReceiveQueueIdle and not waitForReceivePathIdle()) {
			return stopTimeoutError();
		}
		if (not waitForTransmitPathIdle()) {
			return stopTimeoutError();
		}
		const uint32_t flushRequest = ETH->MTLTQOMR | ETH_MTLTQOMR_FTQ;
		{
			// ES0392 Rev 15, section 2.26.10 requires these two precomputed
			// peripheral writes to be adjacent in both execution and MMIO order
			modm::atomic::Lock lock;
			ETH->MACCR = macConfiguration;
			ETH->MTLTQOMR = flushRequest;
		}
		if (not waitForTransmitQueueFlush()) {
			return stopTimeoutError();
		}
	}
	else {
		ETH->MACCR = macConfiguration;
	}

	configureReceiveErrorForwarding();
	return LinkUpdateError::None;
}

inline void
EthernetMac::configureReceiveErrorForwarding()
{
	uint32_t receiveQueue = ETH->MTLRQOMR;
	// ES0392 Rev 15, section 2.26.20: forward errors only in RMII
	// 10 Mbit/s so software can accept the exact false dribble+CRC signature
	if (detail::needsRmii10MWorkaround(
			state().mediaInterface, state().speed)) {
		receiveQueue |= ETH_MTLRQOMR_FEP;
	}
	else {
		receiveQueue &= ~ETH_MTLRQOMR_FEP;
	}
	ETH->MTLRQOMR = receiveQueue;
}

inline void
EthernetMac::initializeDescriptors()
{
	state().rxIndex = 0;
	state().txIndex = 0;
	state().rxTailIndex = RxDescriptorCount - 1;
	state().txLeaseOutstanding = false;
	state().rxLeaseOutstanding = false;
	for (std::size_t index = 0; index < TxDescriptorCount; ++index) {
		dmaStorage().txDescriptors[index].clear();
		cleanDCache(&dmaStorage().txDescriptors[index], sizeof(TxDmaDescriptor));
	}
	for (std::size_t index = 0; index < RxDescriptorCount; ++index)
		releaseRxDescriptor(index, false);
	__DSB();
	ETH->DMACTDRLR = TxDescriptorCount - 1;
	ETH->DMACTDLAR = dmaAddress(dmaStorage().txDescriptors);
	ETH->DMACTDTPR = dmaAddress(dmaStorage().txDescriptors);
	ETH->DMACRDRLR = RxDescriptorCount - 1;
	ETH->DMACRDLAR = dmaAddress(dmaStorage().rxDescriptors);
	ETH->DMACRDTPR = dmaAddress(&dmaStorage().rxDescriptors[state().rxTailIndex]);
}

inline void
EthernetMac::configureMacAddress()
{
	ETH->MACA0HR = (uint32_t(state().macAddress[5]) << 8) | uint32_t(state().macAddress[4]);
	ETH->MACA0LR = (uint32_t(state().macAddress[3]) << 24) | (uint32_t(state().macAddress[2]) << 16) |
			(uint32_t(state().macAddress[1]) << 8) | uint32_t(state().macAddress[0]);
	ETH->MACA1HR = ETH->MACA1LR = 0;
	ETH->MACA2HR = ETH->MACA2LR = 0;
	ETH->MACA3HR = ETH->MACA3LR = 0;
}

inline void
EthernetMac::accumulateHardwareCounters()
{
	const uint32_t missedOverflow = ETH->MTLRQMPOCR;
	state().accumulatedRxMissedPackets +=
			(missedOverflow & ETH_MTLRQMPOCR_MISPKTCNT) >> ETH_MTLRQMPOCR_MISPKTCNT_Pos;
	state().accumulatedRxOverflowPackets +=
			(missedOverflow & ETH_MTLRQMPOCR_OVFPKTCNT) >> ETH_MTLRQMPOCR_OVFPKTCNT_Pos;
	state().accumulatedRxCrcErrors += ETH->MMCRCRCEPR;
	state().accumulatedRxAlignmentErrors += ETH->MMCRAEPR;
}

inline uint32_t
EthernetMac::captureLiveDmaStatus(bool running)
{
	const uint32_t status = ETH->DMACSR;
	detail::ethH7RecordDmaStatus(status, running);
	return status;
}

inline EthernetMac::LinkUpdateError
EthernetMac::start()
{
	if (driverState() != DriverState::Ready)
		return driverState() == DriverState::Faulted ?
				faultError() : LinkUpdateError::NotInitialized;
	if (const auto error = updateLinkMode(); error != LinkUpdateError::None)
		return error;

	modm::atomic::Lock lock;
	if (latchPendingFatalBusError() or driverState() != DriverState::Ready)
		return faultError();

	// Mark the TX ring empty at the next descriptor software will fill
	ETH->DMACTDTPR = dmaAddress(&dmaStorage().txDescriptors[state().txIndex]);
	// Expose all released RX descriptors to DMA
	ETH->DMACRDTPR = dmaAddress(&dmaStorage().rxDescriptors[state().rxTailIndex]);
	ETH->DMACSR = ETH_DMACSR_TPS | ETH_DMACSR_RPS;
	ETH->DMACIER = 0;
	NVIC_ClearPendingIRQ(ETH_IRQn);
	// Start both DMA channels
	ETH->DMACTCR |= ETH_DMACTCR_ST;
	ETH->DMACRCR |= ETH_DMACRCR_SR;
	__DSB();
	// Enable MAC transmission and reception
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	// Publish the running state before enabling operational DMA interrupts
	setDriverState(DriverState::Running);
	ETH->DMACIER = OperationalDmaInterrupts;
	NVIC_EnableIRQ(ETH_IRQn);
	return LinkUpdateError::None;
}

inline EthernetMac::LinkUpdateError
EthernetMac::stop()
{
	// RM0433 Rev 8, sections 58.11.2-58.11.4: ETH_MTLRQDR and ETH_MACDR expose
	// the receive-drain state. Leave DMACRCR.SR enabled while MACCR.RE is clear
	// so frames already accepted by the MAC can drain into prepared descriptors.
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return LinkUpdateError::FatalBusError;
		if (driverState() == DriverState::Faulted)
			return faultError();
		if (state().txLeaseOutstanding or state().rxLeaseOutstanding)
			return LinkUpdateError::OutstandingLease;

		ETH->DMACIER = 0;
		NVIC_DisableIRQ(ETH_IRQn);
		setDriverState(DriverState::Stopping);
		ETH->MACCR &= ~ETH_MACCR_RE;
	}

	// ES0392 Rev 15, sections 2.26.1, 2.26.3, and 2.26.5: with OSP enabled,
	// stop producer-tail updates, wait for Suspend, clear ST, then issue FTQ
	// only after the DMA, MTL, and MAC transmit paths are fully idle
	if (not waitForTransmitDmaSuspend()) {
		return stopTimeoutError();
	}

	ETH->DMACTCR &= ~ETH_DMACTCR_ST;
	if (not waitForTransmitDmaInactive()) {
		return stopTimeoutError();
	}

	if (not waitForReceivePathIdle()) {
		return stopTimeoutError();
	}

	if (not waitForTransmitPathIdle()) {
		return stopTimeoutError();
	}

	requestTransmitQueueFlush();
	if (not waitForTransmitQueueFlush()) {
		return stopTimeoutError();
	}
	ETH->MACCR &= ~ETH_MACCR_TE;
	ETH->DMACSR = ETH_DMACSR_TPS | ETH_DMACSR_RPS;
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return LinkUpdateError::FatalBusError;
		// Ready blocks public packet operations; internally SR may remain enabled
		// while MACCR.RE is clear because no new frames can enter the receive path
		NVIC_ClearPendingIRQ(ETH_IRQn);
		setDriverState(DriverState::Ready);
		ETH->DMACIER = MediaClockUnavailableDmaInterrupts;
		NVIC_EnableIRQ(ETH_IRQn);
	}
	return LinkUpdateError::None;
}

inline EthernetMac::LinkUpdateError
EthernetMac::stopWithoutMediaClock()
{
	modm::atomic::Lock lock;
	if (latchPendingFatalBusError())
		return LinkUpdateError::FatalBusError;
	if (driverState() == DriverState::Faulted)
		return faultError();
	if (state().txLeaseOutstanding or state().rxLeaseOutstanding)
		return LinkUpdateError::OutstandingLease;

	// Work around STM32H7 errata ES0392 sections 2.26.1, 2.26.3, and 2.26.5:
	// link loss may remove the MII media clocks, so clear TE/RE but leave ST/SR
	// set until the clocks return and the required OSP stop sequence can run
	setDriverState(DriverState::Stopping);
	ETH->MACCR &= ~(ETH_MACCR_TE | ETH_MACCR_RE);
	ETH->DMACTCR |= ETH_DMACTCR_ST;
	ETH->DMACRCR |= ETH_DMACRCR_SR;
	ETH->DMACIER = MediaClockUnavailableDmaInterrupts;
	NVIC_EnableIRQ(ETH_IRQn);
	return LinkUpdateError::None;
}

inline EthernetMac::LinkUpdateError
EthernetMac::resumeWithRestoredMediaClock(LinkMode mode)
{
	// Complete the STM32H7 errata stop workaround after media clocks return.
	// Every defined MACDR state must be idle before changing mode or enabling TE/RE.
	if (not waitForMacIdle())
		return stopTimeoutError();
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return LinkUpdateError::FatalBusError;
		if (driverState() != DriverState::Stopping)
			return driverState() == DriverState::Faulted ?
					faultError() : LinkUpdateError::NotInitialized;

		ETH->DMACIER = 0;
		NVIC_DisableIRQ(ETH_IRQn);
	}

	const bool fullToHalf = detail::needsFullToHalfDuplexFlush(
			(ETH->MACCR & ETH_MACCR_DM) ? DuplexMode::Full : DuplexMode::Half,
			mode.duplex);
	if (fullToHalf and not isTransmitPathIdle())
		return resetMacDma(mode);

	state().speed = mode.speed;
	state().duplexMode = mode.duplex;
	if (const auto error = updateLinkMode(false); error != LinkUpdateError::None)
		return error;

	modm::atomic::Lock lock;
	if (latchPendingFatalBusError())
		return LinkUpdateError::FatalBusError;
	if (driverState() != DriverState::Stopping)
		return driverState() == DriverState::Faulted ?
				faultError() : LinkUpdateError::NotInitialized;
	// Speed-only and half-to-full transitions retain the DMA rings and queued
	// transmit descriptors. Only the stopped MAC is re-enabled.
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	setDriverState(DriverState::Running);
	ETH->DMACIER = OperationalDmaInterrupts;
	NVIC_EnableIRQ(ETH_IRQn);
	return LinkUpdateError::None;
}

inline EthernetMac::LinkUpdateError
EthernetMac::resetMacDma(LinkMode mode)
{
	uint32_t mdioClock;
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return LinkUpdateError::FatalBusError;
		if (driverState() != DriverState::Stopping)
			return driverState() == DriverState::Faulted ?
					faultError() : LinkUpdateError::NotInitialized;
		ETH->DMACIER = 0;
		NVIC_DisableIRQ(ETH_IRQn);
		accumulateHardwareCounters();
		const uint32_t pendingDmaStatus = captureLiveDmaStatus(false);
		if ((pendingDmaStatus & ETH_DMACSR_FBE) != 0) {
			setDriverFailure(DriverFailure::FatalBusError);
			ETH->DMACSR = pendingDmaStatus & detail::EthH7DmaHandledStatus;
			return LinkUpdateError::FatalBusError;
		}
		ETH->DMACSR = pendingDmaStatus & detail::EthH7DmaHandledStatus;
		mdioClock = ETH->MACMDIOAR & ETH_MACMDIOAR_CR;
		ETH->DMAMR |= ETH_DMAMR_SWR;
	}

	// ES0392 Rev 15, section 2.26.10: if a full-to-half transition cannot
	// atomically clear DM and request FTQ from an idle transmitter, discard
	// uncertain queued frames with an internal MAC/DMA reset
	if (not modm::this_fiber::poll_for(std::chrono::microseconds{MacResetTimeoutUs},
			[]{ return (ETH->DMAMR & ETH_DMAMR_SWR) == 0; }))
		return stopTimeoutError();

	ETH->MACMDIOAR = mdioClock;
	state().speed = mode.speed;
	state().duplexMode = mode.duplex;
	configureMac();
	configureDma();
	initializeDescriptors();
	ETH->DMACSR = detail::EthH7DmaHandledStatus;
	setDriverState(DriverState::Ready);
	return start();
}

inline bool
EthernetMac::waitForTransmitDmaSuspend()
{
	return modm::this_fiber::poll_for(std::chrono::microseconds{MacStopTimeoutUs}, [] {
		const uint32_t state = ETH->DMADSR & ETH_DMADSR_TPS;
		return state == ETH_DMADSR_TPS_SUSPENDED or state == ETH_DMADSR_TPS_STOPPED;
	});
}

inline bool
EthernetMac::waitForTransmitDmaInactive()
{
	return modm::this_fiber::poll_for(std::chrono::microseconds{MacStopTimeoutUs}, [] {
		const uint32_t state = ETH->DMADSR & ETH_DMADSR_TPS;
		return state == ETH_DMADSR_TPS_STOPPED;
	});
}

inline bool
EthernetMac::waitForReceivePathIdle()
{
	return modm::this_fiber::poll_for(std::chrono::microseconds{MacStopTimeoutUs}, [] {
		constexpr uint32_t MacReceiveState = ETH_MACDR_RFCFCSTS | ETH_MACDR_RPESTS;
		return ETH->MTLRQDR == 0 and (ETH->MACDR & MacReceiveState) == 0;
	});
}

inline bool
EthernetMac::waitForMacIdle()
{
	return modm::this_fiber::poll_for(std::chrono::microseconds{MacStopTimeoutUs}, [] {
		return (ETH->MACDR & MacDebugStateMask) == 0;
	});
}

inline bool
EthernetMac::isTransmitPathIdle()
{
	const uint32_t dmaState = ETH->DMADSR & ETH_DMADSR_TPS;
	const bool dmaInactive = dmaState == ETH_DMADSR_TPS_SUSPENDED or
			dmaState == ETH_DMADSR_TPS_STOPPED;
	return detail::canFlushTransmitQueue(
			dmaInactive, ETH->MTLTQDR == 0, (ETH->MACDR & MacDebugStateMask) == 0);
}

inline bool
EthernetMac::waitForTransmitPathIdle()
{
	return modm::this_fiber::poll_for(std::chrono::microseconds{MacStopTimeoutUs}, [] {
		return isTransmitPathIdle();
	});
}

inline void
EthernetMac::requestTransmitQueueFlush()
{
	ETH->MTLTQOMR |= ETH_MTLTQOMR_FTQ;
}

inline bool
EthernetMac::waitForTransmitQueueFlush()
{
	return modm::this_fiber::poll_for(std::chrono::microseconds{MacStopTimeoutUs}, [] {
		return (ETH->MTLTQOMR & ETH_MTLTQOMR_FTQ) == 0 and
				ETH->MTLTQDR == 0 and (ETH->MACDR & MacDebugStateMask) == 0;
	});
}

inline EthernetMac::LinkUpdateError
EthernetMac::stopTimeoutError()
{
	modm::atomic::Lock lock;
	if (latchPendingFatalBusError())
		return LinkUpdateError::FatalBusError;
	if (driverState() == DriverState::Faulted)
		return faultError();
	setDriverFailure(DriverFailure::MacStopTimeout);
	return LinkUpdateError::MacStopTimeout;
}

inline bool
EthernetMac::latchPendingFatalBusError()
{
	// Preserve a live fatal DMA fault in software so all later operations fail
	// consistently until initialize() resets the driver
	const uint32_t status = ETH->DMACSR;
	if ((status & ETH_DMACSR_FBE) == 0)
		return false;
	const uint32_t capturedStatus =
			captureLiveDmaStatus(driverState() == DriverState::Running);
	setDriverFailure(DriverFailure::FatalBusError);
	ETH->DMACSR = capturedStatus & detail::EthH7DmaHandledStatus;
	return true;
}

inline EthernetMac::LinkUpdateError
EthernetMac::faultError()
{
	return state().driverFailure == DriverFailure::MacStopTimeout ?
			LinkUpdateError::MacStopTimeout : LinkUpdateError::FatalBusError;
}

inline void
EthernetMac::setDriverFailure(DriverFailure failure)
{
	state().driverFailure = failure;
	setDriverState(DriverState::Faulted);
	ETH->DMACIER = 0;
}

inline EthernetMac::LinkStatus
EthernetMac::currentLinkStatus()
{
	LinkStatus status{LinkState(state().linkState), std::nullopt};
	if (status.state == LinkState::Up)
		status.mode = LinkMode{state().speed, state().duplexMode};
	return status;
}

inline EthernetMac::LinkUpdateResult
EthernetMac::notifyUpdatedLinkStatus(LinkStatus observed)
{
	if (driverState() == DriverState::Uninitialized)
		return {currentLinkStatus(), LinkUpdateError::NotInitialized};
	if (driverState() == DriverState::Faulted) {
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return {currentLinkStatus(), LinkUpdateError::FatalBusError};
		return {currentLinkStatus(), faultError()};
	}

	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return {currentLinkStatus(), LinkUpdateError::FatalBusError};
		if (driverState() == DriverState::Faulted)
			return {currentLinkStatus(), faultError()};
	}

	if (observed.state != LinkState::Up) {
		if (driverState() == DriverState::Running) {
			// Work around STM32H7 errata ES0392: RMII keeps its shared 50 MHz
			// REF_CLK, while MII Tx/Rx clocks may disappear on link loss
			const bool clocksContinue =
					state().mediaInterface == MediaInterface::RMII;
			const auto error = observed.state == LinkState::Down and not clocksContinue ?
					stopWithoutMediaClock() : stop();
			if (error != LinkUpdateError::None)
				return {currentLinkStatus(), error};
		}
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return {currentLinkStatus(), LinkUpdateError::FatalBusError};
		if (driverState() == DriverState::Faulted)
			return {currentLinkStatus(), faultError()};
		state().linkState = uint8_t(observed.state);
		return {observed, LinkUpdateError::None};
	}

	modm_assert(observed.mode.has_value(), "eth.link.mode",
			"An up Ethernet link requires a speed and duplex mode");
	const LinkMode mode = *observed.mode;
	const bool modeChanged = mode.speed != state().speed or mode.duplex != state().duplexMode;
	if (driverState() == DriverState::Stopping) {
		const auto error = resumeWithRestoredMediaClock(mode);
		if (error != LinkUpdateError::None)
			return {currentLinkStatus(), error};
	}
	else {
		if (driverState() == DriverState::Running and modeChanged) {
			const auto error = stop();
			if (error != LinkUpdateError::None)
				return {currentLinkStatus(), error};
		}
		if (driverState() == DriverState::Faulted)
			return {currentLinkStatus(), faultError()};
		if (driverState() == DriverState::Ready or modeChanged) {
			state().speed = mode.speed;
			state().duplexMode = mode.duplex;
			const auto error = start();
			if (error != LinkUpdateError::None)
				return {currentLinkStatus(), error};
		}
	}
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return {currentLinkStatus(), LinkUpdateError::FatalBusError};
		if (not detail::canCommitLinkUpState(driverState()))
			return {currentLinkStatus(), faultError()};
		state().linkState = uint8_t(LinkState::Up);
	}
	return {currentLinkStatus(), LinkUpdateError::None};
}

inline EthernetMac::MdioError
EthernetMac::readPhyRegister(uint8_t phyAddress, uint8_t reg, uint16_t &value)
{
	if (reg > 31) return MdioError::InvalidRegister;
	if (phyAddress > MdioPhyAddressMaximum) return MdioError::InvalidPhyAddress;
	if ((ETH->MACMDIOAR & ETH_MACMDIOAR_MB) != 0) return MdioError::Busy;
	uint32_t command = ETH->MACMDIOAR & ETH_MACMDIOAR_CR;
	command |= (uint32_t(phyAddress) << ETH_MACMDIOAR_PA_Pos) & ETH_MACMDIOAR_PA;
	command |= (uint32_t(reg) << ETH_MACMDIOAR_RDA_Pos) & ETH_MACMDIOAR_RDA;
	ETH->MACMDIOAR = command | ETH_MACMDIOAR_MOC_RD | ETH_MACMDIOAR_MB;
	if (not modm::this_fiber::poll_for(std::chrono::microseconds{MdioTimeoutUs},
			[]{ return (ETH->MACMDIOAR & ETH_MACMDIOAR_MB) == 0; }))
		return MdioError::Timeout;
	value = uint16_t(ETH->MACMDIODR & ETH_MACMDIODR_MD);
	return MdioError::None;
}

inline EthernetMac::MdioError
EthernetMac::writePhyRegister(uint8_t phyAddress, uint8_t reg, uint16_t value)
{
	if (reg > 31) return MdioError::InvalidRegister;
	if (phyAddress > MdioPhyAddressMaximum) return MdioError::InvalidPhyAddress;
	if ((ETH->MACMDIOAR & ETH_MACMDIOAR_MB) != 0) return MdioError::Busy;
	uint32_t command = ETH->MACMDIOAR & ETH_MACMDIOAR_CR;
	command |= (uint32_t(phyAddress) << ETH_MACMDIOAR_PA_Pos) & ETH_MACMDIOAR_PA;
	command |= (uint32_t(reg) << ETH_MACMDIOAR_RDA_Pos) & ETH_MACMDIOAR_RDA;
	ETH->MACMDIODR = value;
	ETH->MACMDIOAR = command | ETH_MACMDIOAR_MOC_WR | ETH_MACMDIOAR_MB;
	return modm::this_fiber::poll_for(std::chrono::microseconds{MdioTimeoutUs},
			[]{ return (ETH->MACMDIOAR & ETH_MACMDIOAR_MB) == 0; }) ?
			MdioError::None : MdioError::Timeout;
}

inline EthernetMac::TransmitBufferLease::TransmitBufferLease(TransmitBufferLease &&other) noexcept :
	index_(other.index_), length_(other.length_), error_(other.error_), active_(other.active_)
{
	other.active_ = false;
	other.error_ = TransmitError::Busy;
}

inline EthernetMac::TransmitBufferLease &
EthernetMac::TransmitBufferLease::operator=(TransmitBufferLease &&other) noexcept
{
	if (this != &other) {
		if (active_) EthernetMac::cancelTransmitLease(index_);
		index_ = other.index_;
		length_ = other.length_;
		error_ = other.error_;
		active_ = other.active_;
		other.active_ = false;
		other.error_ = TransmitError::Busy;
	}
	return *this;
}

inline EthernetMac::TransmitBufferLease::~TransmitBufferLease() noexcept
{
	if (active_) EthernetMac::cancelTransmitLease(index_);
}

inline std::span<uint8_t>
EthernetMac::TransmitBufferLease::buffer() noexcept
{
	modm_assert(active_, "eth.tx.lease", "Inactive transmit lease accessed");
	return {EthernetMac::dmaStorage().txBuffers[index_], length_};
}

inline EthernetMac::TransmitResult
EthernetMac::TransmitBufferLease::commit() noexcept
{
	modm_assert(active_, "eth.tx.lease", "Inactive transmit lease committed");
	const auto result = EthernetMac::commitTransmitLease(index_, length_);
	active_ = false;
	error_ = result.error;
	return result;
}

inline void
EthernetMac::TransmitBufferLease::cancel() noexcept
{
	if (not active_) return;
	EthernetMac::cancelTransmitLease(index_);
	active_ = false;
}

inline EthernetMac::ReceiveBufferLease::ReceiveBufferLease(ReceiveBufferLease &&other) noexcept :
	index_(other.index_), length_(other.length_), error_(other.error_),
	checksumStatus_(other.checksumStatus_), active_(other.active_)
{
	other.active_ = false;
	other.error_ = ReceiveError::NoFrameAvailable;
}

inline EthernetMac::ReceiveBufferLease &
EthernetMac::ReceiveBufferLease::operator=(ReceiveBufferLease &&other) noexcept
{
	if (this != &other) {
		if (active_) EthernetMac::releaseReceiveLease(index_);
		index_ = other.index_;
		length_ = other.length_;
		error_ = other.error_;
		checksumStatus_ = other.checksumStatus_;
		active_ = other.active_;
		other.active_ = false;
		other.error_ = ReceiveError::NoFrameAvailable;
	}
	return *this;
}

inline EthernetMac::ReceiveBufferLease::~ReceiveBufferLease() noexcept
{
	if (active_) EthernetMac::releaseReceiveLease(index_);
}

inline std::span<const uint8_t>
EthernetMac::ReceiveBufferLease::buffer() const noexcept
{
	modm_assert(active_, "eth.rx.lease", "Inactive receive lease accessed");
	return {EthernetMac::dmaStorage().rxBuffers[index_], length_};
}

inline EthernetMac::ReceiveChecksumStatus
EthernetMac::ReceiveBufferLease::checksumStatus() const noexcept
{
	modm_assert(active_, "eth.rx.lease", "Inactive receive lease checksum accessed");
	return checksumStatus_;
}

inline void
EthernetMac::ReceiveBufferLease::release() noexcept
{
	if (not active_) return;
	EthernetMac::releaseReceiveLease(index_);
	active_ = false;
}

inline EthernetMac::TransmitBufferLease
EthernetMac::acquireTransmitBuffer(std::size_t length)
{
	if (driverState() == DriverState::Uninitialized)
		return TransmitBufferLease(TransmitError::NotInitialized);
	if (driverState() == DriverState::Faulted)
		return TransmitBufferLease(TransmitError::Faulted);
	if (length == 0 or length > MaxFrameSize)
		return TransmitBufferLease(TransmitError::InvalidLength);
	if (driverState() != DriverState::Running or LinkState(state().linkState) != LinkState::Up)
		return TransmitBufferLease(TransmitError::LinkDown);
	if (state().txLeaseOutstanding)
		return TransmitBufferLease(TransmitError::Busy);

	TxDmaDescriptor &descriptor = dmaStorage().txDescriptors[state().txIndex];
	invalidateDCache(&descriptor, sizeof(descriptor));
	if (descriptor.isOwnedByDma()) {
		return TransmitBufferLease(TransmitError::Busy);
	}
	const uint32_t writebackErrors = descriptor.writebackErrors();
	if (writebackErrors != 0) {
		++state().txDescriptorErrors;
		state().hardwareTxDescriptorErrorStatus |= writebackErrors;
	}
	descriptor.clear();
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError())
			return TransmitBufferLease(TransmitError::Faulted);
		if (driverState() == DriverState::Faulted)
			return TransmitBufferLease(TransmitError::Faulted);
		if (driverState() != DriverState::Running or LinkState(state().linkState) != LinkState::Up)
			return TransmitBufferLease(TransmitError::LinkDown);
		state().txLeaseOutstanding = true;
	}
	return TransmitBufferLease(state().txIndex, length);
}

inline EthernetMac::TransmitResult
EthernetMac::commitTransmitLease(std::size_t index, std::size_t length)
{
	// Validate the lease and current link state
	modm_assert(state().txLeaseOutstanding and index == state().txIndex,
			"eth.tx.lease", "Transmit lease does not own the producer descriptor");
	TransmitError stateError = TransmitError::None;
	if (driverState() == DriverState::Uninitialized) stateError = TransmitError::NotInitialized;
	else if (driverState() == DriverState::Faulted) stateError = TransmitError::Faulted;
	else if (driverState() != DriverState::Running or LinkState(state().linkState) != LinkState::Up)
		stateError = TransmitError::LinkDown;
	if (stateError != TransmitError::None) {
		cancelTransmitLease(index);
		return {stateError};
	}
	// Reject fragmented IPv4 frames that hardware cannot checksum
	if (state().checksumOffloadEnabled and hasUnsupportedHardwareChecksumFragment(
			{dmaStorage().txBuffers[index], length})) {
		cancelTransmitLease(index);
		return {TransmitError::UnsupportedFragmentation};
	}

	// Prepare the frame and descriptor in DMA-visible memory
	TxDmaDescriptor &descriptor = dmaStorage().txDescriptors[index];
	invalidateDCache(&descriptor, sizeof(descriptor));
	if (descriptor.isOwnedByDma()) {
		cancelTransmitLease(index);
		return {TransmitError::Busy};
	}
	cleanDCache(dmaStorage().txBuffers[index], length);
	descriptor.prepare(dmaAddress(dmaStorage().txBuffers[index]), length,
			state().checksumOffloadEnabled);
	cleanDCache(&descriptor, sizeof(descriptor));
	// Recheck the driver state before releasing the descriptor to DMA
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError()) {
			state().txLeaseOutstanding = false;
			return {TransmitError::Faulted};
		}
		if (driverState() == DriverState::Faulted) {
			state().txLeaseOutstanding = false;
			return {TransmitError::Faulted};
		}
		if (driverState() != DriverState::Running or LinkState(state().linkState) != LinkState::Up) {
			state().txLeaseOutstanding = false;
			return {TransmitError::LinkDown};
		}
		descriptor.releaseToDma();
		cleanDCache(&descriptor, sizeof(descriptor));
		__DSB();
		state().txLeaseOutstanding = false;
		state().txIndex = (state().txIndex + 1) % TxDescriptorCount;
		ETH->DMACTDTPR = dmaAddress(&dmaStorage().txDescriptors[state().txIndex]);
	}
	return {};
}

inline bool
EthernetMac::hasUnsupportedHardwareChecksumFragment(std::span<const uint8_t> frame)
{
	if (frame.size() < 22)
		return false;
	const bool ipv4 = frame[12] == 0x08 and frame[13] == 0x00 and
			(frame[14] >> 4) == 4;
	const uint16_t fragment = (uint16_t(frame[20]) << 8) | frame[21];
	return ipv4 and (fragment & 0x3fff) != 0;
}

inline void
EthernetMac::cancelTransmitLease(std::size_t index)
{
	modm_assert(state().txLeaseOutstanding and index == state().txIndex,
			"eth.tx.lease", "Transmit lease does not own the producer descriptor");
	state().txLeaseOutstanding = false;
}

inline EthernetMac::TransmitResult
EthernetMac::transmit(std::span<const uint8_t> frame)
{
	auto lease = acquireTransmitBuffer(frame.size());
	if (not lease) return {lease.error()};
	std::memcpy(lease.buffer().data(), frame.data(), frame.size());
	return lease.commit();
}

inline EthernetMac::ReceiveChecksumStatus
EthernetMac::classifyReceiveChecksum(RxDmaDescriptor const &descriptor)
{
	if (not state().checksumOffloadEnabled or
			not descriptor.status().any(detail::EthH7RxDescriptor3::ReceiveStatus1Valid))
		return ReceiveChecksumStatus::NotChecked;
	const uint32_t status = descriptor.word1;
	if ((status & (Rdes1IpHeaderError | Rdes1IpPayloadError)) != 0)
		return ReceiveChecksumStatus::Invalid;
	const uint32_t payloadType = status & Rdes1PayloadTypeMask;
	const bool supportedPayload = payloadType == 1 or payloadType == 2 or payloadType == 3;
	if ((status & Rdes1IpChecksumBypassed) != 0 or
			(status & (Rdes1Ipv4Header | Rdes1Ipv6Header)) == 0 or not supportedPayload)
		return ReceiveChecksumStatus::NotChecked;
	return ReceiveChecksumStatus::Valid;
}

inline bool
EthernetMac::isValidReceiveDescriptor(RxDmaDescriptor const &descriptor)
{
	const auto status = descriptor.status();
	// ES0392 Rev 15, section 2.26.20: RMII 10 Mbit/s can falsely
	// report the exact dribble+CRC signature. No other error is accepted.
	const bool toleratedRmii10MError = detail::needsRmii10MWorkaround(
			state().mediaInterface, state().speed) and
			detail::isExactRmii10MDribbleCrcError(status.value);
	return status.none(detail::EthH7RxDescriptor3::ContextDescriptor) and
			(status.none(detail::EthH7RxDescriptor3::ErrorSummary) or toleratedRmii10MError) and
			status.all(detail::EthH7RxDescriptor3::FirstDescriptor |
					detail::EthH7RxDescriptor3::LastDescriptor);
}

inline EthernetMac::ReceiveBufferLease
EthernetMac::tryAcquireReceiveBuffer()
{
	// Validate the driver and lease state
	if (driverState() == DriverState::Uninitialized)
		return ReceiveBufferLease(ReceiveError::NotInitialized);
	if (driverState() == DriverState::Faulted)
		return ReceiveBufferLease(ReceiveError::Faulted);
	if (state().rxLeaseOutstanding) {
		return ReceiveBufferLease(ReceiveError::NoFrameAvailable);
	}

	// Scan completed descriptors, recycling invalid frames as they are found
	for (std::size_t count = 0; count < RxDescriptorCount; ++count) {
		if (driverState() == DriverState::Faulted)
			return ReceiveBufferLease(ReceiveError::Faulted);
		RxDmaDescriptor &descriptor = dmaStorage().rxDescriptors[state().rxIndex];
		invalidateDCache(&descriptor, sizeof(descriptor));
		const auto status = descriptor.status();
		if (status.any(detail::EthH7RxDescriptor3::OwnedByDma))
			return ReceiveBufferLease(ReceiveError::NoFrameAvailable);
		if (not isValidReceiveDescriptor(descriptor)) {
			if (not releaseBadRxDescriptor())
				return ReceiveBufferLease(driverState() == DriverState::Faulted ?
						ReceiveError::Faulted : ReceiveError::NoFrameAvailable);
			continue;
		}

		// Validate the frame metadata and make its buffer visible to the CPU
		const std::size_t length = descriptor.packetLength();
		if (length == 0 or length > MaxFrameSize) {
			if (not releaseBadRxDescriptor())
				return ReceiveBufferLease(driverState() == DriverState::Faulted ?
						ReceiveError::Faulted : ReceiveError::NoFrameAvailable);
			continue;
		}
		const auto checksum = classifyReceiveChecksum(descriptor);
		invalidateDCache(dmaStorage().rxBuffers[state().rxIndex], length);

		// Reserve the receive lease only while the MAC remains operational
		{
			modm::atomic::Lock lock;
			if (latchPendingFatalBusError())
				return ReceiveBufferLease(ReceiveError::Faulted);
			if (driverState() == DriverState::Faulted)
				return ReceiveBufferLease(ReceiveError::Faulted);
			if (driverState() != DriverState::Running)
				return ReceiveBufferLease(ReceiveError::NoFrameAvailable);
			state().rxLeaseOutstanding = true;
		}
		return ReceiveBufferLease(state().rxIndex, length, checksum);
	}
	return ReceiveBufferLease(ReceiveError::NoFrameAvailable);
}

inline void
EthernetMac::releaseReceiveLease(std::size_t index)
{
	modm_assert(state().rxLeaseOutstanding and index == state().rxIndex,
			"eth.rx.lease", "Receive lease does not own the consumer descriptor");
	state().rxLeaseOutstanding = false;
	if (releaseRxDescriptor(index))
		state().rxIndex = (state().rxIndex + 1) % RxDescriptorCount;
}

inline bool
EthernetMac::releaseBadRxDescriptor()
{
	if (not releaseRxDescriptor(state().rxIndex))
		return false;
	state().rxIndex = (state().rxIndex + 1) % RxDescriptorCount;
	return true;
}

inline bool
EthernetMac::releaseRxDescriptor(std::size_t index, bool resumeDma)
{
	invalidateDCache(dmaStorage().rxBuffers[index], DmaBufferSize);
	auto &descriptor = dmaStorage().rxDescriptors[index];
	descriptor.prepare(dmaAddress(dmaStorage().rxBuffers[index]));
	cleanDCache(&descriptor, sizeof(descriptor));
	if (not resumeDma) {
		descriptor.releaseToDma();
		cleanDCache(&descriptor, sizeof(descriptor));
		return true;
	}
	{
		modm::atomic::Lock lock;
		if (latchPendingFatalBusError() or driverState() != DriverState::Running)
			return false;
		descriptor.releaseToDma();
		cleanDCache(&descriptor, sizeof(descriptor));
		__DMB();
		state().rxTailIndex = index;
		ETH->DMACRDTPR = dmaAddress(&dmaStorage().rxDescriptors[index]);
	}
	resumeReceiveDma();
	return true;
}

inline void
EthernetMac::resumeReceiveDma()
{
	if (driverState() != DriverState::Running)
		return;
	modm::atomic::Lock lock;
	if (latchPendingFatalBusError() or driverState() != DriverState::Running)
		return;
	ETH->DMACSR = ETH_DMACSR_RBU | ETH_DMACSR_RPS;
	ETH->DMACRCR |= ETH_DMACRCR_SR;
	__DMB();
	ETH->DMACRDTPR = dmaAddress(&dmaStorage().rxDescriptors[state().rxTailIndex]);
}

inline void
EthernetMac::cleanDCache(void const *address, std::size_t length)
{
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
	if (address == nullptr or length == 0 or (SCB->CCR & SCB_CCR_DC_Msk) == 0)
		return;

	const auto start = cacheLineStart(reinterpret_cast<std::uintptr_t>(address));
	const auto end = cacheLineEnd(reinterpret_cast<std::uintptr_t>(address) + length);
	SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(start), static_cast<int32_t>(end - start));
#else
	(void) address;
	(void) length;
#endif
}

inline void
EthernetMac::invalidateDCache(void const *address, std::size_t length)
{
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
	if (address == nullptr or length == 0 or (SCB->CCR & SCB_CCR_DC_Msk) == 0)
		return;

	const auto start = cacheLineStart(reinterpret_cast<std::uintptr_t>(address));
	const auto end = cacheLineEnd(reinterpret_cast<std::uintptr_t>(address) + length);
	SCB_InvalidateDCache_by_Addr(reinterpret_cast<uint32_t *>(start), static_cast<int32_t>(end - start));
#else
	(void) address;
	(void) length;
#endif
}

}
