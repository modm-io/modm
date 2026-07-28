/*
 * Copyright (c) 2026, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <cstring>
#include <modm/driver/ethernet/lan8742a.hpp>
#include <modm/platform.hpp>

#include "FreeRTOS_ARP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkBufferManagement.h"

using EMAC = modm::platform::Eth<modm::Lan8742a>;

namespace modm
{

struct ethernet
{
	static constexpr BaseType_t MAX_PACKET_SIZE{1536};
	static constexpr BaseType_t RX_BUFFER_SIZE{1536};
	static constexpr BaseType_t TX_BUFFER_SIZE{1536};
	static constexpr BaseType_t RX_BUFFER_NUMBER{5};
	static constexpr BaseType_t TX_BUFFER_NUMBER{5};

	static constexpr configSTACK_DEPTH_TYPE emacTaskStackDepth{configMINIMAL_STACK_SIZE * 2};
	static constexpr UBaseType_t emacTaskPriority{configMAX_PRIORITIES - 1};

	enum class InitStatus : uint8_t
	{
		Init,
		Pass,
		Failed
	};

	enum class TxDescriptor2 : uint32_t
	{
		InterruptOnCompletion = modm::Bit31,
	};
	MODM_FLAGS32(TxDescriptor2);

	enum class TxDescriptor3 : uint32_t
	{
		DmaOwned = modm::Bit31,
		FirstSegment = modm::Bit29,
		LastSegment = modm::Bit28,
		ChecksumFull = modm::Bit17 | modm::Bit16,
	};
	MODM_FLAGS32(TxDescriptor3);

	enum class RxDescriptorReady : uint32_t
	{
		DmaOwned = modm::Bit31,
		InterruptOnCompletion = modm::Bit30,
		Buffer1Valid = modm::Bit24,
	};
	MODM_FLAGS32(RxDescriptorReady);

	enum class RxDescriptorStatus : uint32_t
	{
		DmaOwned = modm::Bit31,
		FirstSegment = modm::Bit29,
		LastSegment = modm::Bit28,
		ErrorSummary = modm::Bit15,
	};
	MODM_FLAGS32(RxDescriptorStatus);

	static constexpr uint32_t Buffer1LengthMask{0x00003fff};
	static constexpr uint32_t Buffer2LengthMask{0x3fff0000};
	static constexpr uint32_t FrameLengthMask{0x00007fff};

	struct DmaDescriptor
	{
		__IO uint32_t DESC0;
		__IO uint32_t DESC1;
		__IO uint32_t DESC2;
		__IO uint32_t DESC3;
		uint32_t BackupAddr0;
		uint32_t BackupAddr1;
	};
	using DmaDescriptor_t = DmaDescriptor;

	static InitStatus initStatus;
	static SemaphoreHandle_t txDescriptorSemaphore;
	static TaskHandle_t emacTaskHandle;

	static modm::platform::eth::Event_t isrEvent;

	static TimeOut_t phyLinkStatusTimer;
	static constexpr TickType_t PhyLinkStatusHighMs{pdMS_TO_TICKS(2'000)};
	static constexpr TickType_t PhyLinkStatusLowMs{pdMS_TO_TICKS(1'000)};
	static TickType_t phyLinkStatusRemaining;
	static modm::platform::eth::LinkStatus lastPhyLinkStatus;

	modm_aligned(32)
		modm_section(".bss_d2_sram1") static DmaDescriptor_t DmaRxDescriptorTable[RX_BUFFER_NUMBER];

	modm_aligned(32)
		modm_section(".bss_d2_sram1") static DmaDescriptor_t DmaTxDescriptorTable[TX_BUFFER_NUMBER];

	modm_aligned(32)
		modm_section(".bss_d2_sram1") static uint8_t RxBuffers[RX_BUFFER_NUMBER][RX_BUFFER_SIZE];

	modm_aligned(32)
		modm_section(".bss_d2_sram2") static uint8_t TxBuffers[TX_BUFFER_NUMBER][TX_BUFFER_SIZE];

	static DmaDescriptor_t *RxDescriptor;
	static DmaDescriptor_t *TxDescriptor;
	static DmaDescriptor_t *DmaTxDescriptorToClear;

	static constexpr uintptr_t CacheLineSize{32};

	static bool
	isDCacheEnabled()
	{
#if (__DCACHE_PRESENT == 1U)
		return (SCB->CCR & SCB_CCR_DC_Msk) != 0;
#else
		return false;
#endif
	}

	static void
	cleanDCache(const void *address, std::size_t size)
	{
#if (__DCACHE_PRESENT == 1U)
		if (not isDCacheEnabled() or address == nullptr or size == 0) return;

		const uintptr_t start = uintptr_t(address) & ~(CacheLineSize - 1);
		const uintptr_t end =
			(uintptr_t(address) + size + CacheLineSize - 1) & ~(CacheLineSize - 1);
		SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(start), end - start);
#else
		(void)address;
		(void)size;
#endif
	}

	static void
	invalidateDCache(const void *address, std::size_t size)
	{
#if (__DCACHE_PRESENT == 1U)
		if (not isDCacheEnabled() or address == nullptr or size == 0) return;

		const uintptr_t start = uintptr_t(address) & ~(CacheLineSize - 1);
		const uintptr_t end =
			(uintptr_t(address) + size + CacheLineSize - 1) & ~(CacheLineSize - 1);
		SCB_InvalidateDCache_by_Addr(reinterpret_cast<uint32_t *>(start), end - start);
#else
		(void)address;
		(void)size;
#endif
	}

	static void
	DMATxDescListInit()
	{
		std::memset(DmaTxDescriptorTable, 0, sizeof(DmaTxDescriptorTable));
		cleanDCache(DmaTxDescriptorTable, sizeof(DmaTxDescriptorTable));
		EMAC::setDmaTxDescriptorTable(uint32_t(TxDescriptor), TX_BUFFER_NUMBER);
	}

	static void
	DMARxDescListInit()
	{
		ETH->DMACRCR = (ETH->DMACRCR & ~ETH_DMACRCR_RBSZ) |
					   ((uint32_t(RX_BUFFER_SIZE) << ETH_DMACRCR_RBSZ_Pos) & ETH_DMACRCR_RBSZ);

		for (BaseType_t index = 0; index < RX_BUFFER_NUMBER; ++index)
		{
			auto &descriptor = RxDescriptor[index];
			std::memset(&descriptor, 0, sizeof(descriptor));
			descriptor.BackupAddr0 = uint32_t(RxBuffers[index]);
			descriptor.DESC0 = uint32_t(RxBuffers[index]);
			descriptor.DESC3 = RxDescriptorReady_t(RxDescriptorReady::DmaOwned |
												   RxDescriptorReady::InterruptOnCompletion |
												   RxDescriptorReady::Buffer1Valid)
								   .value;

			cleanDCache(RxBuffers[index], sizeof(RxBuffers[index]));
			cleanDCache(&descriptor, sizeof(descriptor));
		}

		EMAC::setDmaRxDescriptorTable(uint32_t(RxDescriptor), RX_BUFFER_NUMBER);
		__DMB();
		ETH->DMACRDTPR = uint32_t(&RxDescriptor[RX_BUFFER_NUMBER - 1]);
	}

	static void
	clearTxBuffers()
	{
		const std::size_t count = TX_BUFFER_NUMBER - uxSemaphoreGetCount(txDescriptorSemaphore);

		for (std::size_t index = 0; index < count; ++index)
		{
			invalidateDCache(DmaTxDescriptorToClear, sizeof(*DmaTxDescriptorToClear));
			if ((DmaTxDescriptorToClear->DESC3 & uint32_t(TxDescriptor3::DmaOwned)) != 0) break;

			DmaTxDescriptorToClear->DESC0 = 0;
			DmaTxDescriptorToClear->DESC1 = 0;
			DmaTxDescriptorToClear->DESC2 = 0;
			DmaTxDescriptorToClear->DESC3 = 0;
			DmaTxDescriptorToClear->BackupAddr0 = 0;
			DmaTxDescriptorToClear->BackupAddr1 = 0;

			if (++DmaTxDescriptorToClear == &DmaTxDescriptorTable[TX_BUFFER_NUMBER])
				DmaTxDescriptorToClear = DmaTxDescriptorTable;

			xSemaphoreGive(txDescriptorSemaphore);
		}
	}

	static bool
	mayAcceptPacket(uint8_t *buffer)
	{
		const auto *protocolPacket = reinterpret_cast<const ProtocolPacket_t *>(buffer);

		switch (protocolPacket->xTCPPacket.xEthernetHeader.usFrameType)
		{
			case ipARP_FRAME_TYPE:
				return true;
			case ipIPv4_FRAME_TYPE:
				break;
			default:
				return false;
		}

#if ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1
		static constexpr uint16_t ipFragmentOffsetBitMask{0x0fff};
		const auto *ipHeader = &(protocolPacket->xTCPPacket.xIPHeader);
		const uint16_t fragmentOffset =
			FreeRTOS_ntohs(ipHeader->usFragmentOffset) & ipFragmentOffsetBitMask;
		if (fragmentOffset != 0) return false;

		if (ipHeader->ucVersionHeaderLength < 0x45 or ipHeader->ucVersionHeaderLength > 0x4f)
			return false;

		const uint32_t destIpAddress = ipHeader->ulDestinationIPAddress;
		if (destIpAddress != *ipLOCAL_IP_ADDRESS_POINTER and
			(FreeRTOS_ntohl(destIpAddress) & 0xff) != 0xff and *ipLOCAL_IP_ADDRESS_POINTER)
		{
			return false;
		}

		if (ipHeader->ucProtocol == ipPROTOCOL_UDP)
		{
			const uint16_t sourcePort =
				FreeRTOS_ntohs(protocolPacket->xUDPPacket.xUDPHeader.usSourcePort);
			const uint16_t destPort =
				FreeRTOS_ntohs(protocolPacket->xUDPPacket.xUDPHeader.usDestinationPort);

			if (not xPortHasUDPSocket(destPort) and sourcePort != ipDNS_PORT) return false;
		}
#endif

		return true;
	}

	static void
	passMessage(NetworkBufferDescriptor_t *descriptor)
	{
		IPStackEvent_t rxEvent{.eEventType = eNetworkRxEvent,
							   .pvData = reinterpret_cast<void *>(descriptor)};

		if (xSendEventStructToIPTask(&rxEvent, TickType_t(1000)) != pdPASS)
		{
			do
			{
				NetworkBufferDescriptor_t *next = descriptor->pxNextBuffer;
				vReleaseNetworkBufferAndDescriptor(descriptor);
				descriptor = next;
			} while (descriptor);

			iptraceETHERNET_RX_EVENT_LOST();
		} else
		{
			iptraceNETWORK_INTERFACE_RECEIVE();
		}
	}

	static bool
	emacInterfaceInput()
	{
		static constexpr TickType_t descriptorWaitTime{pdMS_TO_TICKS(250)};

		NetworkBufferDescriptor_t *currentDescriptor{nullptr};
		NetworkBufferDescriptor_t *newDescriptor{nullptr};
		NetworkBufferDescriptor_t *firstDescriptor{nullptr};
		NetworkBufferDescriptor_t *lastDescriptor{nullptr};
		BaseType_t receivedLength{0};
		auto *dmaRxDescriptor = RxDescriptor;

		invalidateDCache(dmaRxDescriptor, sizeof(*dmaRxDescriptor));
		while ((dmaRxDescriptor->DESC3 & uint32_t(RxDescriptorStatus::DmaOwned)) == 0)
		{
			bool accepted = true;
			newDescriptor = nullptr;
			receivedLength = (dmaRxDescriptor->DESC3 & FrameLengthMask) - 4;
			auto *buffer = reinterpret_cast<uint8_t *>(dmaRxDescriptor->BackupAddr0);
			invalidateDCache(buffer, receivedLength);

			if ((dmaRxDescriptor->DESC3 & uint32_t(RxDescriptorStatus::ErrorSummary)) != 0)
			{
				accepted = false;
			} else if ((dmaRxDescriptor->DESC3 & uint32_t(RxDescriptorStatus::LastSegment)) == 0)
			{
				accepted = false;
			} else
			{
				accepted = mayAcceptPacket(buffer);
			}

			if (accepted)
			{
				newDescriptor =
					pxGetNetworkBufferWithDescriptor(receivedLength, descriptorWaitTime);
				if (newDescriptor == nullptr) accepted = false;
			}

			if (accepted)
			{
				currentDescriptor = newDescriptor;
				std::memcpy(currentDescriptor->pucEthernetBuffer, buffer, receivedLength);
				currentDescriptor->xDataLength = receivedLength;
				currentDescriptor->pxNextBuffer = nullptr;
				if (firstDescriptor == nullptr)
					firstDescriptor = currentDescriptor;
				else if (lastDescriptor)
					lastDescriptor->pxNextBuffer = currentDescriptor;
				lastDescriptor = currentDescriptor;
			}

			dmaRxDescriptor->DESC0 = dmaRxDescriptor->BackupAddr0;
			dmaRxDescriptor->DESC1 = 0;
			dmaRxDescriptor->DESC2 = 0;
			__DMB();
			dmaRxDescriptor->DESC3 = RxDescriptorReady_t(RxDescriptorReady::DmaOwned |
														 RxDescriptorReady::InterruptOnCompletion |
														 RxDescriptorReady::Buffer1Valid)
										 .value;
			cleanDCache(dmaRxDescriptor, sizeof(*dmaRxDescriptor));

			__DMB();
			ETH->DMACRDTPR = uint32_t(dmaRxDescriptor);

			if (++dmaRxDescriptor == &DmaRxDescriptorTable[RX_BUFFER_NUMBER])
				dmaRxDescriptor = DmaRxDescriptorTable;
			RxDescriptor = dmaRxDescriptor;
			invalidateDCache(dmaRxDescriptor, sizeof(*dmaRxDescriptor));
		}

		if (firstDescriptor) passMessage(firstDescriptor);

		return receivedLength > 0;
	}

	static void
	updateConfig(bool force)
	{
		using modm::platform::eth;

		if (force or lastPhyLinkStatus == eth::LinkStatus::Up)
		{
			const bool autoNegotiationFailed = not EMAC::phyStartAutoNegotiation();
			EMAC::configureMac(autoNegotiationFailed);
			EMAC::start();
		} else
		{
			EMAC::stop();
		}
	}

	static bool
	phyCheckLinkStatus(bool hasReceived)
	{
		using modm::platform::eth;

		if (hasReceived)
		{
			vTaskSetTimeOutState(&phyLinkStatusTimer);
			phyLinkStatusRemaining = pdMS_TO_TICKS(PhyLinkStatusHighMs);
			return false;
		}

		bool checkNeeded{false};
		if (xTaskCheckForTimeOut(&phyLinkStatusTimer, &phyLinkStatusRemaining))
		{
			const eth::LinkStatus phyLinkStatus = EMAC::phyReadLinkStatus();
			if (lastPhyLinkStatus != phyLinkStatus)
			{
				lastPhyLinkStatus = phyLinkStatus;
				if (phyLinkStatus == eth::LinkStatus::Down)
				{
					IPStackEvent_t rxEvent = {eNetworkDownEvent, nullptr};
					xSendEventStructToIPTask(&rxEvent, 0);
				}
				checkNeeded = true;
			}

			vTaskSetTimeOutState(&phyLinkStatusTimer);
			phyLinkStatusRemaining = pdMS_TO_TICKS(
				phyLinkStatus == eth::LinkStatus::Up ? PhyLinkStatusHighMs : PhyLinkStatusLowMs);
		}

		return checkNeeded;
	}

	static void
	emacHandlerTask(void *)
	{
		using modm::platform::eth;

		static constexpr TickType_t maxBlockTime{pdMS_TO_TICKS(100)};

		bool result{false};

		for (;;)
		{
			result = false;

			if (isrEvent == eth::Event(0))
				ulTaskNotifyTake(pdFALSE, maxBlockTime);
			else
			{
				if ((isrEvent & eth::Event::Receive) == eth::Event::Receive)
				{
					isrEvent = isrEvent & ~eth::Event::Receive;
					result = emacInterfaceInput();
				}
				if ((isrEvent & eth::Event::Transmit) == eth::Event::Transmit)
				{
					isrEvent = isrEvent & ~eth::Event::Transmit;
					clearTxBuffers();
				}
				if ((isrEvent & eth::Event::Error) == eth::Event::Error)
				{
					isrEvent = isrEvent & ~eth::Event::Error;
				}
			}

			if (phyCheckLinkStatus(result)) updateConfig(false);
		}
	}
};

ethernet::InitStatus ethernet::initStatus = ethernet::InitStatus::Init;
SemaphoreHandle_t ethernet::txDescriptorSemaphore{nullptr};
TaskHandle_t ethernet::emacTaskHandle{nullptr};

modm::platform::eth::Event_t ethernet::isrEvent{modm::platform::eth::Event::None};

TimeOut_t ethernet::phyLinkStatusTimer;
modm::platform::eth::LinkStatus ethernet::lastPhyLinkStatus{modm::platform::eth::LinkStatus::Down};
TickType_t ethernet::phyLinkStatusRemaining{0};

modm_aligned(32) modm_section(
	".bss_d2_sram1") ethernet::DmaDescriptor_t ethernet::DmaRxDescriptorTable[RX_BUFFER_NUMBER];
modm_aligned(32) modm_section(
	".bss_d2_sram1") ethernet::DmaDescriptor_t ethernet::DmaTxDescriptorTable[TX_BUFFER_NUMBER];
modm_aligned(32)
	modm_section(".bss_d2_sram1") uint8_t ethernet::RxBuffers[RX_BUFFER_NUMBER][RX_BUFFER_SIZE];
modm_aligned(32)
	modm_section(".bss_d2_sram2") uint8_t ethernet::TxBuffers[TX_BUFFER_NUMBER][TX_BUFFER_SIZE];
ethernet::DmaDescriptor_t *ethernet::RxDescriptor{nullptr};
ethernet::DmaDescriptor_t *ethernet::TxDescriptor{nullptr};
ethernet::DmaDescriptor_t *ethernet::DmaTxDescriptorToClear{nullptr};

}  // namespace modm

extern "C" BaseType_t
xNetworkInterfaceInitialise()
{
	using modm::ethernet;

	if (ethernet::initStatus == ethernet::InitStatus::Init)
	{
		ethernet::txDescriptorSemaphore = xSemaphoreCreateCounting(
			UBaseType_t(ethernet::TX_BUFFER_NUMBER), UBaseType_t(ethernet::TX_BUFFER_NUMBER));
		if (ethernet::txDescriptorSemaphore == nullptr)
		{
			ethernet::initStatus = ethernet::InitStatus::Failed;
			return pdFAIL;
		}

		EMAC::setMacAddress(EMAC::MacAddressIndex::Index0, FreeRTOS_GetMACAddress());
#if (ipconfigUSE_LLMNR != 0)
		EMAC::setMacAddress(EMAC::MacAddressIndex::Index1,
							reinterpret_cast<uint8_t const *>(xLLMNR_MACAddress));
#endif

		(void)EMAC::initialize<modm::platform::eth::MediaInterface::RMII>();

		ethernet::TxDescriptor = ethernet::DmaTxDescriptorTable;
		ethernet::RxDescriptor = ethernet::DmaRxDescriptorTable;
		ethernet::DmaTxDescriptorToClear = ethernet::DmaTxDescriptorTable;

		ethernet::DMATxDescListInit();
		ethernet::DMARxDescListInit();
		ethernet::updateConfig(true);

		if (not xTaskCreate(ethernet::emacHandlerTask, "EMAC", ethernet::emacTaskStackDepth,
							nullptr, ethernet::emacTaskPriority, &ethernet::emacTaskHandle))
		{
			ethernet::initStatus = ethernet::InitStatus::Failed;
			return pdFAIL;
		}

		ethernet::initStatus = ethernet::InitStatus::Pass;
	}

	if (ethernet::initStatus != ethernet::InitStatus::Pass) return pdFAIL;

	if (EMAC::getLinkStatus() == modm::platform::eth::LinkStatus::Up)
	{
		EMAC::enableInterrupt(
			EMAC::Interrupt_t(EMAC::Interrupt::NormalIrqSummary |
							  EMAC::Interrupt::AbnormalIrqSummary | EMAC::Interrupt::FatalBusError |
							  EMAC::Interrupt::ReceiveBufferUnavailable | EMAC::Interrupt::Receive |
							  EMAC::Interrupt::TransmitStopped | EMAC::Interrupt::Transmit));
		return pdPASS;
	}

	return pdFAIL;
}

extern "C" BaseType_t
xNetworkInterfaceOutput(NetworkBufferDescriptor_t *const descriptor, BaseType_t releaseAfterSend)
{
	using modm::ethernet;

	static constexpr TickType_t blockTimeTicks{pdMS_TO_TICKS(50)};

	BaseType_t result{pdFAIL};

	do
	{
		auto *packet = reinterpret_cast<ProtocolPacket_t *>(descriptor->pucEthernetBuffer);
#if (ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 1)
		if (packet->xICMPPacket.xIPHeader.ucProtocol == ipPROTOCOL_ICMP)
			packet->xICMPPacket.xICMPHeader.usChecksum = 0;
#else
		(void)packet;
#endif

		if (EMAC::getLinkStatus() == modm::platform::eth::LinkStatus::Down) break;

		ethernet::clearTxBuffers();
		if (xSemaphoreTake(ethernet::txDescriptorSemaphore, blockTimeTicks) != pdPASS) break;

		auto *dmaTxDescriptor = ethernet::TxDescriptor;
		bool descriptorReady{false};
		TimeOut_t timeout;
		TickType_t remainingTime{blockTimeTicks};
		vTaskSetTimeOutState(&timeout);

		while (not descriptorReady)
		{
			ethernet::invalidateDCache(dmaTxDescriptor, sizeof(*dmaTxDescriptor));
			descriptorReady =
				(dmaTxDescriptor->DESC3 & uint32_t(ethernet::TxDescriptor3::DmaOwned)) == 0;
			if (descriptorReady) break;

			// Keep the software ring bookkeeping in sync with DMA write-back before
			// giving up on a descriptor under sustained transmit load.
			ethernet::clearTxBuffers();
			if (xTaskCheckForTimeOut(&timeout, &remainingTime) == pdTRUE)
			{
				xSemaphoreGive(ethernet::txDescriptorSemaphore);
				break;
			}

			taskYIELD();
		}
		if (not descriptorReady) break;

		uint32_t transmitSize = descriptor->xDataLength;
		if (transmitSize > ethernet::TX_BUFFER_SIZE) transmitSize = ethernet::TX_BUFFER_SIZE;

		std::memcpy(ethernet::TxBuffers[dmaTxDescriptor - ethernet::DmaTxDescriptorTable],
					descriptor->pucEthernetBuffer, transmitSize);
		ethernet::cleanDCache(ethernet::TxBuffers[dmaTxDescriptor - ethernet::DmaTxDescriptorTable],
							  transmitSize);

		dmaTxDescriptor->DESC0 =
			uint32_t(ethernet::TxBuffers[dmaTxDescriptor - ethernet::DmaTxDescriptorTable]);
		dmaTxDescriptor->DESC1 = 0;
		dmaTxDescriptor->DESC2 =
			(transmitSize & ethernet::Buffer1LengthMask) |
			ethernet::TxDescriptor2_t(ethernet::TxDescriptor2::InterruptOnCompletion).value;
		dmaTxDescriptor->DESC3 = ethernet::TxDescriptor3_t(ethernet::TxDescriptor3::FirstSegment |
														   ethernet::TxDescriptor3::LastSegment)
									 .value |
								 (transmitSize & ethernet::FrameLengthMask);
#if (ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 1)
		dmaTxDescriptor->DESC3 |=
			ethernet::TxDescriptor3_t(ethernet::TxDescriptor3::ChecksumFull).value;
#endif

		__DMB();
		dmaTxDescriptor->DESC3 |= uint32_t(ethernet::TxDescriptor3::DmaOwned);
		ethernet::cleanDCache(dmaTxDescriptor, sizeof(*dmaTxDescriptor));

		if (++ethernet::TxDescriptor == &ethernet::DmaTxDescriptorTable[ethernet::TX_BUFFER_NUMBER])
			ethernet::TxDescriptor = ethernet::DmaTxDescriptorTable;

		__DSB();
		ETH->DMACTDTPR = uint32_t(ethernet::TxDescriptor);
		iptraceNETWORK_INTERFACE_TRANSMIT();
		result = pdPASS;
	} while (0);

	if (releaseAfterSend) vReleaseNetworkBufferAndDescriptor(descriptor);

	return result;
}

extern "C" BaseType_t
xGetPhyLinkStatus()
{ return EMAC::getLinkStatus() == modm::platform::eth::LinkStatus::Up ? pdTRUE : pdFALSE; }

MODM_ISR(ETH)
{
	using modm::ethernet;
	using modm::platform::eth;

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	const EMAC::InterruptFlags_t irq = EMAC::getInterruptFlags();
	EMAC::acknowledgeInterrupt(irq);

	if (irq & (eth::InterruptFlags::Receive | eth::InterruptFlags::ReceiveBufferUnavailable))
	{
		ethernet::isrEvent |= eth::Event::Receive;
	}
	if (irq & eth::InterruptFlags::Transmit) { ethernet::isrEvent |= eth::Event::Transmit; }
	if (irq & (eth::InterruptFlags::AbnormalIrqSummary | eth::InterruptFlags::FatalBusError))
		ethernet::isrEvent |= eth::Event::Error;

	if (ethernet::emacTaskHandle)
	{
		vTaskNotifyGiveFromISR(ethernet::emacTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
