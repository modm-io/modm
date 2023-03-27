/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <atomic>

namespace modm::platform
{

inline void
DmaChannel::startTransfer(const BlockTransfer& transfer)
{
	stopTransfer();
	auto& channel = XDMAC->XdmacChid[index_];

	// source address
	channel.XDMAC_CSA = transfer.csa;
	// destination address
	channel.XDMAC_CDA = transfer.cda;
	// disable linked list mode
	channel.XDMAC_CNDC = 0;
	// data length
	channel.XDMAC_CUBC = transfer.cubc;
	// configuration
	channel.XDMAC_CC = transfer.cc;
	// data stride
	channel.XDMAC_CDS_MSP = transfer.cds_msp;
	// one microblock, no microblock striding
	channel.XDMAC_CBC = 0;
	channel.XDMAC_CDUS = 0;
	channel.XDMAC_CSUS = 0;

	start();
}

template<typename... Descriptors>
void DmaChannel::startTransfer(LinkedListTransfer<Descriptors...>& transfer)
{
	stopTransfer();
	auto& channel = XDMAC->XdmacChid[index_];

	// Addresses, size and config are set by first descriptor fetch (first descriptor must be of
	// type 2 or 3). Thus, CSA, CDA, CUBC and CC are not configured here.

	// Reset microblock and striding settings in case the first descriptor is not of type 3
	channel.XDMAC_CBC = 0;
	channel.XDMAC_CDUS = 0;
	channel.XDMAC_CSUS = 0;
	channel.XDMAC_CDS_MSP = 0;

	uint32_t* const first = std::get<0>(transfer.descriptors_).data();
	// "first" points to uint32_t objects which are guaranteed to be 4-byte aligned
	// by language rules. Thus, the LSB of CNDA (NDAIF) is forced to 0. This selects
	// AHB interface 0 which can access all RAMs. "transfer" can't be in flash because
	// it is non-const.
	channel.XDMAC_CNDA = reinterpret_cast<uintptr_t>(first);
	using First = std::tuple_element_t<0, std::tuple<Descriptors...>>;
	channel.XDMAC_CNDC = detail::controlFlags<First, false>();

	start();
}

inline void
DmaChannel::start()
{
	// Prevent reordering of writing descriptors after transfer start when function is inlined
	std::atomic_signal_fence(std::memory_order_seq_cst);

	// guarantee pending writes to descriptors have been completed
	__DSB();

	// TODO: if data cache is used: SCB_CleanInvalidateDCache();
	// crashes CPU if cache is off

	XDMAC->XDMAC_GE = (XDMAC_GE_EN0 << index_);
}

inline void
DmaChannel::stopTransfer()
{
	if (XDMAC->XDMAC_GS & (XDMAC_GS_ST0 << index_)) {
		XDMAC->XDMAC_GD = (XDMAC_GD_DI0 << index_);
		while (XDMAC->XDMAC_GS & (XDMAC_GS_ST0 << index_));
	}
}

inline void
DmaChannel::enableInterrupts()
{
	XDMAC->XDMAC_GIE = (XDMAC_GIE_IE0 << index_);
}

inline void
DmaChannel::disableInterrupts()
{
	XDMAC->XDMAC_GID = (XDMAC_GID_ID0 << index_);
}

inline void
DmaChannel::enableInterruptFlag(Interrupt_t interrupt)
{
	XDMAC->XdmacChid[index_].XDMAC_CIE = interrupt.value;
}

inline void
DmaChannel::disableInterruptFlag(Interrupt_t interrupt)
{
	XDMAC->XdmacChid[index_].XDMAC_CID = interrupt.value;
}

inline DmaChannel::Interrupt_t
DmaChannel::interruptMask() const
{
	return Interrupt_t(XDMAC->XdmacChid[index_].XDMAC_CIM);
}

inline DmaChannel::Interrupt_t
DmaChannel::readAndClearFlags()
{
	return Interrupt_t(XDMAC->XdmacChid[index_].XDMAC_CIS);
}

} // namespace modm::platform
