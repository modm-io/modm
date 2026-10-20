/*
 * Copyright (c) 2026, Luca Feggi
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/delay.hpp>
#include <modm/platform/clock/rcc.hpp>

namespace modm::platform
{

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
template<class SystemClock>
EthernetInitResult
Eth<Storage, Interface, CachePolicy>::initialize(Storage& storage,
												 const EthernetMacAddress& macAddress,
												 uint8_t interruptPriority,
												 uint32_t resetTimeoutMicroseconds,
												 uint32_t mdioTimeoutMicroseconds)
{
	if (initialized_) return EthernetInitResult::AlreadyInitialized;
	constexpr uint32_t Hclk = SystemClock::Ahb;
	if (Hclk < 1'000'000u || Hclk > 310'000'000u) return EthernetInitResult::InvalidClock;
	if (!validStorage(storage)) return EthernetInitResult::InvalidStorage;
	if (!claimOwnership(&ownershipToken_)) return EthernetInitResult::AlreadyInitialized;

	initialized_ = false;
	started_ = false;
	restartAfterRecovery_ = false;
	dmaError_ = false;
	storage_ = &storage;
	macAddress_ = macAddress;
	linkState_ = {};
	internalLoopback_ = false;
	hclk_ = Hclk;
	mdioClockRange_ = mdioClockRange(Hclk);
	resetTimeoutMicroseconds_ = resetTimeoutMicroseconds;
	mdioTimeoutMicroseconds_ = mdioTimeoutMicroseconds;
	diagnostics_ = {};
	clearEvents();

	Rcc::enable<Peripheral::Eth>();
	stm32::dwmac::EthernetHardware::template selectMediaInterface<Interface>();
	if (!softwareReset(resetTimeoutMicroseconds_))
	{
		dmaError_ = true;
		diagnostics_.recoveryRequired = true;
		storage_ = nullptr;
		Rcc::disable<Peripheral::Eth>();
		releaseOwnership(&ownershipToken_);
		return EthernetInitResult::ResetTimeout;
	}

	ring_.emplace(storage);
	ring_->initialize(stm32::dwmac::EthernetHardware::dmaAddress);
	configurePeripheral();

	NVIC_DisableIRQ(ETH_IRQn);
	NVIC_ClearPendingIRQ(ETH_IRQn);
	NVIC_SetPriority(ETH_IRQn, interruptPriority);
	initialized_ = true;
	return EthernetInitResult::Ok;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
void
Eth<Storage, Interface, CachePolicy>::start()
{
	if (!initialized_ || started_ || dmaError_) return;
	NVIC_DisableIRQ(ETH_IRQn);
	clearEvents();

	constexpr uint32_t DmaStatusMask = ETH_DMACSR_NIS | ETH_DMACSR_AIS | ETH_DMACSR_CDE |
									   ETH_DMACSR_FBE | ETH_DMACSR_ETI | ETH_DMACSR_RWT |
									   ETH_DMACSR_RPS | ETH_DMACSR_RBU | ETH_DMACSR_RI |
									   ETH_DMACSR_TBU | ETH_DMACSR_TPS | ETH_DMACSR_TI;
	ETH->DMACSR = DmaStatusMask;
	NVIC_ClearPendingIRQ(ETH_IRQn);
	ETH->DMACIER = ETH_DMACIER_NIE | ETH_DMACIER_AIE | ETH_DMACIER_CDEE | ETH_DMACIER_FBEE |
				   ETH_DMACIER_RWTE | ETH_DMACIER_RSE | ETH_DMACIER_RBUE | ETH_DMACIER_RIE |
				   ETH_DMACIER_TBUE | ETH_DMACIER_TXSE | ETH_DMACIER_TIE;
	ETH->MTLQICSR =
		ETH_MTLQICSR_RXOIE | ETH_MTLQICSR_TXUIE | ETH_MTLQICSR_RXOVFIS | ETH_MTLQICSR_TXUNFIS;

	ETH->DMACTCR |= ETH_DMACTCR_ST;
	ETH->DMACRCR |= ETH_DMACRCR_SR;
	ETH->MTLTQOMR |= ETH_MTLTQOMR_FTQ;
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	started_ = true;
	NVIC_EnableIRQ(ETH_IRQn);
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
void
Eth<Storage, Interface, CachePolicy>::stop()
{
	if (!initialized_) return;
	NVIC_DisableIRQ(ETH_IRQn);
	ETH->DMACIER = 0;
	ETH->MTLQICSR = ETH_MTLQICSR_RXOVFIS | ETH_MTLQICSR_TXUNFIS;
	ETH->DMACTCR &= ~ETH_DMACTCR_ST;
	ETH->DMACRCR &= ~ETH_DMACRCR_SR;
	ETH->MACCR &= ~(ETH_MACCR_RE | ETH_MACCR_TE);
	ETH->MTLTQOMR |= ETH_MTLTQOMR_FTQ;
	constexpr uint32_t DmaStatusMask = ETH_DMACSR_NIS | ETH_DMACSR_AIS | ETH_DMACSR_CDE |
									   ETH_DMACSR_FBE | ETH_DMACSR_ETI | ETH_DMACSR_RWT |
									   ETH_DMACSR_RPS | ETH_DMACSR_RBU | ETH_DMACSR_RI |
									   ETH_DMACSR_TBU | ETH_DMACSR_TPS | ETH_DMACSR_TI;
	ETH->DMACSR = DmaStatusMask;
	started_ = false;
	restartAfterRecovery_ = false;
	clearEvents();
	NVIC_ClearPendingIRQ(ETH_IRQn);
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
ethernet::TransmitResult
Eth<Storage, Interface, CachePolicy>::tryTransmit(std::span<const uint8_t> frame)
{
	if (!initialized_ || !ring_) return ethernet::TransmitResult::DmaError;
	const auto result =
		ring_->tryTransmit(frame, started_, linkState_.up || internalLoopback_, dmaError_);
	if (result == ethernet::TransmitResult::Ok)
	{
		ETH->DMACTDTPR = stm32::dwmac::EthernetHardware::dmaAddress(ring_->txTailDescriptor());
	}
	return result;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
std::optional<typename Eth<Storage, Interface, CachePolicy>::RxFrame>
Eth<Storage, Interface, CachePolicy>::receive()
{
	if (!initialized_ || !ring_ || dmaError_) return std::nullopt;
	updateRxTail();
	return ring_->receive();
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
EthernetPollResult
Eth<Storage, Interface, CachePolicy>::poll()
{
	if (!initialized_ || !ring_) return EthernetPollResult::RecoveryRequired;

	ring_->reclaimTransmit();
	updateRxTail();
	const uint32_t dmaEvents = takeDmaEvents();
	const uint32_t mtlEvents = takeMtlEvents();
	const uint32_t interruptCount = takeInterruptCount();
	if (dmaEvents == 0 && mtlEvents == 0 && interruptCount == 0)
		return dmaError_ ? EthernetPollResult::RecoveryRequired : EthernetPollResult::Idle;

	diagnostics_.interruptCount += interruptCount;
	diagnostics_.lastDmaStatus = dmaEvents;
	diagnostics_.lastMtlStatus = mtlEvents;
	if ((dmaEvents & ETH_DMACSR_AIS) != 0) ++diagnostics_.abnormalInterrupts;

	if ((dmaEvents & ETH_DMACSR_RBU) != 0)
	{
		++diagnostics_.receiveBufferUnavailable;
		if (started_)
		{
			const uint32_t tail = ETH->DMACRDTPR;
			ETH->DMACRDTPR = tail;
		}
	}
	if ((dmaEvents & ETH_DMACSR_RWT) != 0) ++diagnostics_.receiveWatchdogTimeouts;
	if ((dmaEvents & ETH_DMACSR_RPS) != 0)
	{
		++diagnostics_.receiveProcessStopped;
		if (started_)
		{
			ETH->DMACRCR |= ETH_DMACRCR_SR;
			const uint32_t tail = ETH->DMACRDTPR;
			ETH->DMACRDTPR = tail;
		}
	}
	if ((dmaEvents & ETH_DMACSR_TPS) != 0)
	{
		++diagnostics_.transmitProcessStopped;
		if (started_)
		{
			ETH->DMACTCR |= ETH_DMACTCR_ST;
			ETH->DMACTDTPR = stm32::dwmac::EthernetHardware::dmaAddress(ring_->txTailDescriptor());
		}
	}
	if ((dmaEvents & ETH_DMACSR_TBU) != 0) ++diagnostics_.transmitBufferUnavailable;
	if ((mtlEvents & ETH_MTLQICSR_TXUNFIS) != 0) ++diagnostics_.transmitUnderflows;
	if ((mtlEvents & ETH_MTLQICSR_RXOVFIS) != 0) ++diagnostics_.receiveOverflows;
	if ((dmaEvents & ETH_DMACSR_FBE) != 0) ++diagnostics_.fatalBusErrors;
	if ((dmaEvents & ETH_DMACSR_CDE) != 0) ++diagnostics_.contextDescriptorErrors;

	if ((dmaEvents & (ETH_DMACSR_FBE | ETH_DMACSR_CDE)) != 0)
	{
		const bool restart = started_;
		dmaError_ = true;
		diagnostics_.recoveryRequired = true;
		stop();
		restartAfterRecovery_ = restart;
		return EthernetPollResult::RecoveryRequired;
	}
	return EthernetPollResult::Activity;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
EthernetRecoveryResult
Eth<Storage, Interface, CachePolicy>::recover()
{
	if (!initialized_ || !ring_ || storage_ == nullptr)
		return EthernetRecoveryResult::NotInitialized;
	if (ring_->hasRxLease()) return EthernetRecoveryResult::RxFrameOutstanding;

	const bool restart = started_ || restartAfterRecovery_;
	NVIC_DisableIRQ(ETH_IRQn);
	NVIC_ClearPendingIRQ(ETH_IRQn);
	stop();
	restartAfterRecovery_ = restart;
	Rcc::disable<Peripheral::Eth>();
	Rcc::enable<Peripheral::Eth>();
	stm32::dwmac::EthernetHardware::template selectMediaInterface<Interface>();
	if (!softwareReset(resetTimeoutMicroseconds_))
	{
		dmaError_ = true;
		diagnostics_.recoveryRequired = true;
		return EthernetRecoveryResult::ResetTimeout;
	}

	ring_->initialize(stm32::dwmac::EthernetHardware::dmaAddress, false);
	configurePeripheral();
	dmaError_ = false;
	restartAfterRecovery_ = false;
	diagnostics_.recoveryRequired = false;
	++diagnostics_.recoveries;
	clearEvents();
	applyLinkState();
	applyInternalLoopback();
	if (restart) start();
	return EthernetRecoveryResult::Ok;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
EthernetConfigResult
Eth<Storage, Interface, CachePolicy>::setLinkState(const ethernet::LinkState& state)
{
	if (!initialized_ || !ring_) return EthernetConfigResult::NotInitialized;
	if (state == linkState_) return EthernetConfigResult::Ok;
	if (started_) return EthernetConfigResult::MacRunning;
	linkState_ = state;
	applyLinkState();
	return EthernetConfigResult::Ok;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
void
Eth<Storage, Interface, CachePolicy>::applyLinkState()
{
	uint32_t configuration = ETH->MACCR & ~(ETH_MACCR_FES | ETH_MACCR_DM);
	if (linkState_.speed == ethernet::LinkSpeed::Mbit100) configuration |= ETH_MACCR_FES;
	if (linkState_.duplex == ethernet::DuplexMode::Full) configuration |= ETH_MACCR_DM;
	ETH->MACCR = configuration;

	const bool rmii10Mbit = Interface == ethernet::MediaInterface::Rmii && linkState_.up &&
							linkState_.speed == ethernet::LinkSpeed::Mbit10;
	if (rmii10Mbit)
		ETH->MTLRQOMR |= ETH_MTLRQOMR_FEP;
	else
		ETH->MTLRQOMR &= ~ETH_MTLRQOMR_FEP;
	ring_->setRmii10MbitErratumWorkaround(rmii10Mbit);
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
EthernetConfigResult
Eth<Storage, Interface, CachePolicy>::setInternalLoopback(bool enable)
{
	if (!initialized_) return EthernetConfigResult::NotInitialized;
	if (enable == internalLoopback_) return EthernetConfigResult::Ok;
	if (started_) return EthernetConfigResult::MacRunning;
	internalLoopback_ = enable;
	applyInternalLoopback();
	return EthernetConfigResult::Ok;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
void
Eth<Storage, Interface, CachePolicy>::applyInternalLoopback()
{
	if (internalLoopback_)
		ETH->MACCR |= ETH_MACCR_LM;
	else
		ETH->MACCR &= ~ETH_MACCR_LM;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
ethernet::MdioStatus
Eth<Storage, Interface, CachePolicy>::read(uint8_t phyAddress, uint8_t reg, uint16_t& value)
{
	if (phyAddress >= 32 || reg >= 32) return ethernet::MdioStatus::InvalidAddress;
	if (!initialized_) return ethernet::MdioStatus::Busy;
	if (!waitForMdio()) return ethernet::MdioStatus::Timeout;
	ETH->MACMDIOAR = mdioClockRange_ | (static_cast<uint32_t>(phyAddress) << 21) |
					 (static_cast<uint32_t>(reg) << 16) | ETH_MACMDIOAR_MOC_RD | ETH_MACMDIOAR_MB;
	if (!waitForMdio()) return ethernet::MdioStatus::Timeout;
	value = static_cast<uint16_t>(ETH->MACMDIODR & ETH_MACMDIODR_MD);
	return ethernet::MdioStatus::Ok;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
ethernet::MdioStatus
Eth<Storage, Interface, CachePolicy>::write(uint8_t phyAddress, uint8_t reg, uint16_t value)
{
	if (phyAddress >= 32 || reg >= 32) return ethernet::MdioStatus::InvalidAddress;
	if (!initialized_) return ethernet::MdioStatus::Busy;
	if (!waitForMdio()) return ethernet::MdioStatus::Timeout;
	ETH->MACMDIODR = value;
	ETH->MACMDIOAR = mdioClockRange_ | (static_cast<uint32_t>(phyAddress) << 21) |
					 (static_cast<uint32_t>(reg) << 16) | ETH_MACMDIOAR_MOC_WR | ETH_MACMDIOAR_MB;
	return waitForMdio() ? ethernet::MdioStatus::Ok : ethernet::MdioStatus::Timeout;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
EthernetDiagnostics
Eth<Storage, Interface, CachePolicy>::diagnostics()
{
	auto result = diagnostics_;
	if (ring_) result.ring = ring_->diagnostics();
	return result;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
bool
Eth<Storage, Interface, CachePolicy>::validStorage(const Storage& storage)
{
	using Hardware = stm32::dwmac::EthernetHardware;
	const bool aligned =
		(reinterpret_cast<uintptr_t>(storage.rxDescriptors.data()) % stm32::dwmac::CacheLineSize) ==
			0 &&
		(reinterpret_cast<uintptr_t>(storage.txDescriptors.data()) % stm32::dwmac::CacheLineSize) ==
			0 &&
		(reinterpret_cast<uintptr_t>(storage.rxBuffers.data()) % stm32::dwmac::CacheLineSize) ==
			0 &&
		(reinterpret_cast<uintptr_t>(storage.txBuffers.data()) % stm32::dwmac::CacheLineSize) == 0;
	return aligned &&
		   Hardware::isDmaAccessible(storage.rxDescriptors.data(), sizeof(storage.rxDescriptors)) &&
		   Hardware::isDmaAccessible(storage.txDescriptors.data(), sizeof(storage.txDescriptors)) &&
		   Hardware::isDmaAccessible(storage.rxBuffers.data(), sizeof(storage.rxBuffers)) &&
		   Hardware::isDmaAccessible(storage.txBuffers.data(), sizeof(storage.txBuffers));
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
bool
Eth<Storage, Interface, CachePolicy>::softwareReset(uint32_t timeoutMicroseconds)
{
	ETH->DMAMR |= ETH_DMAMR_SWR;
	while ((ETH->DMAMR & ETH_DMAMR_SWR) != 0)
	{
		if (timeoutMicroseconds-- == 0) return false;
		modm::delay_us(1);
	}
	return true;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
void
Eth<Storage, Interface, CachePolicy>::configurePeripheral()
{
	ETH->MACMDIOAR = mdioClockRange_;
	ETH->MAC1USTCR = ((hclk_ / 1'000'000u) - 1u) & ETH_MAC1USTCR_TIC1USCNTR;
	ETH->MACCR = ETH_MACCR_ACS | ETH_MACCR_CST;
	ETH->MACPFR = 0;
	ETH->MACHT0R = 0;
	ETH->MACHT1R = 0;
	programMacAddress();

	ETH->MTLTQOMR = ETH_MTLTQOMR_TSF;
	ETH->MTLRQOMR = ETH_MTLRQOMR_RSF;
	ETH->MTLQICSR = ETH_MTLQICSR_RXOVFIS | ETH_MTLQICSR_TXUNFIS;

	ETH->DMASBMR = ETH_DMASBMR_AAL | ETH_DMASBMR_FB;
	ETH->DMACCR = ETH_DMACCR_DSL_128BIT;
	ETH->DMACTCR = ETH_DMACTCR_TPBL_32PBL;
	ETH->DMACRCR = ETH_DMACRCR_RPBL_32PBL |
				   ((static_cast<uint32_t>(Storage::PacketBufferSize) << ETH_DMACRCR_RBSZ_Pos) &
					ETH_DMACRCR_RBSZ);
	ETH->DMACTDLAR = stm32::dwmac::EthernetHardware::dmaAddress(storage_->txDescriptors.data());
	ETH->DMACRDLAR = stm32::dwmac::EthernetHardware::dmaAddress(storage_->rxDescriptors.data());
	ETH->DMACTDRLR = Storage::TxDescriptorCount - 1;
	ETH->DMACRDRLR = Storage::RxDescriptorCount - 1;
	ETH->DMACTDTPR = stm32::dwmac::EthernetHardware::dmaAddress(storage_->txDescriptors.data());
	ETH->DMACRDTPR = stm32::dwmac::EthernetHardware::dmaAddress(ring_->initialRxTailDescriptor());

	ETH->MMCRIMR = ETH_MMCRIMR_RXLPITRCIM | ETH_MMCRIMR_RXLPIUSCIM | ETH_MMCRIMR_RXUCGPIM |
				   ETH_MMCRIMR_RXALGNERPIM | ETH_MMCRIMR_RXCRCERPIM;
	ETH->MMCTIMR = ETH_MMCTIMR_TXLPITRCIM | ETH_MMCTIMR_TXLPIUSCIM | ETH_MMCTIMR_TXGPKTIM |
				   ETH_MMCTIMR_TXMCOLGPIM | ETH_MMCTIMR_TXSCOLGPIM;
	ETH->DMACIER = 0;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
void
Eth<Storage, Interface, CachePolicy>::programMacAddress()
{
	ETH->MACA0HR = (static_cast<uint32_t>(macAddress_[5]) << 8) | macAddress_[4];
	ETH->MACA0LR = (static_cast<uint32_t>(macAddress_[3]) << 24) |
				   (static_cast<uint32_t>(macAddress_[2]) << 16) |
				   (static_cast<uint32_t>(macAddress_[1]) << 8) | macAddress_[0];
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
bool
Eth<Storage, Interface, CachePolicy>::waitForMdio()
{
	uint32_t remaining = mdioTimeoutMicroseconds_;
	while ((ETH->MACMDIOAR & ETH_MACMDIOAR_MB) != 0)
	{
		if (remaining-- == 0) return false;
		modm::delay_us(1);
	}
	return true;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
uint32_t
Eth<Storage, Interface, CachePolicy>::mdioClockRange(uint32_t hclk)
{
	if (hclk < 35'000'000u) return ETH_MACMDIOAR_CR_DIV16;
	if (hclk < 60'000'000u) return ETH_MACMDIOAR_CR_DIV26;
	if (hclk < 100'000'000u) return ETH_MACMDIOAR_CR_DIV42;
	if (hclk < 150'000'000u) return ETH_MACMDIOAR_CR_DIV62;
	if (hclk < 250'000'000u) return ETH_MACMDIOAR_CR_DIV102;
	return ETH_MACMDIOAR_CR_DIV124;
}

template<typename Storage, ethernet::MediaInterface Interface, typename CachePolicy>
void
Eth<Storage, Interface, CachePolicy>::updateRxTail()
{
	if (auto* descriptor = ring_->takeRxTailDescriptor())
		ETH->DMACRDTPR = stm32::dwmac::EthernetHardware::dmaAddress(descriptor);
}

}  // namespace modm::platform
