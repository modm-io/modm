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

#include <modm/architecture/interface/interrupt.hpp>

#include "eth.hpp"

namespace modm::platform
{

bool
EthBase::claimOwnership(const void* owner)
{
	modm::atomic::Lock lock;
	if (owner_ != nullptr && owner_ != owner) return false;
	owner_ = owner;
	return true;
}

void
EthBase::releaseOwnership(const void* owner)
{
	modm::atomic::Lock lock;
	if (owner_ == owner) owner_ = nullptr;
}

void
EthBase::handleInterrupt()
{
	constexpr uint32_t DmaStatusMask = ETH_DMACSR_NIS | ETH_DMACSR_AIS | ETH_DMACSR_CDE |
									   ETH_DMACSR_FBE | ETH_DMACSR_ETI | ETH_DMACSR_RWT |
									   ETH_DMACSR_RPS | ETH_DMACSR_RBU | ETH_DMACSR_RI |
									   ETH_DMACSR_TBU | ETH_DMACSR_TPS | ETH_DMACSR_TI;
	constexpr uint32_t MtlStatusMask = ETH_MTLQICSR_RXOVFIS | ETH_MTLQICSR_TXUNFIS;
	constexpr uint32_t MtlEnableMask = ETH_MTLQICSR_RXOIE | ETH_MTLQICSR_TXUIE;

	const uint32_t dmaStatus = ETH->DMACSR & DmaStatusMask;
	const uint32_t mtlRegister = ETH->MTLQICSR;
	const uint32_t mtlStatus = mtlRegister & MtlStatusMask;
	if (dmaStatus != 0) ETH->DMACSR = dmaStatus;
	if (mtlStatus != 0) ETH->MTLQICSR = (mtlRegister & MtlEnableMask) | mtlStatus;

	dmaEvents_ = dmaEvents_ | dmaStatus;
	mtlEvents_ = mtlEvents_ | mtlStatus;
	interruptCount_ = interruptCount_ + 1;
}

uint32_t
EthBase::takeDmaEvents()
{
	modm::atomic::Lock lock;
	const uint32_t events = dmaEvents_;
	dmaEvents_ = 0;
	return events;
}

uint32_t
EthBase::takeMtlEvents()
{
	modm::atomic::Lock lock;
	const uint32_t events = mtlEvents_;
	mtlEvents_ = 0;
	return events;
}

uint32_t
EthBase::takeInterruptCount()
{
	modm::atomic::Lock lock;
	const uint32_t count = interruptCount_;
	interruptCount_ = 0;
	return count;
}

void
EthBase::clearEvents()
{
	modm::atomic::Lock lock;
	dmaEvents_ = 0;
	mtlEvents_ = 0;
	interruptCount_ = 0;
}

}  // namespace modm::platform

MODM_ISR(ETH)
{ modm::platform::EthBase::handleInterrupt(); }
