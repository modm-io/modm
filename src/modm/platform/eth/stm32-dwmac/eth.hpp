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

#include <array>
#include <cstddef>
#include <cstdint>
#include <modm/architecture/interface/atomic_lock.hpp>
#include <modm/platform/gpio/connector.hpp>
#include <optional>
#include <span>

#include "../device.hpp"
#include "eth_hardware.hpp"
#include "eth_ring.hpp"

namespace modm::platform
{

template<std::size_t RxCount = 4, std::size_t TxCount = 4, std::size_t BufferSize = 1536>
using EthernetStorage = stm32::dwmac::Storage<RxCount, TxCount, BufferSize>;

using EthernetMacAddress = std::array<uint8_t, 6>;

enum class EthernetInitResult : uint8_t
{
	Ok,
	AlreadyInitialized,
	InvalidClock,
	InvalidStorage,
	ResetTimeout,
};

enum class EthernetPollResult : uint8_t
{
	Idle,
	Activity,
	RecoveryRequired,
};

enum class EthernetRecoveryResult : uint8_t
{
	Ok,
	NotInitialized,
	RxFrameOutstanding,
	ResetTimeout,
};

enum class EthernetConfigResult : uint8_t
{
	Ok,
	NotInitialized,
	MacRunning,
};

struct EthernetDiagnostics
{
	stm32::dwmac::RingDiagnostics ring{};
	uint32_t interruptCount{0};
	uint32_t receiveBufferUnavailable{0};
	uint32_t receiveWatchdogTimeouts{0};
	uint32_t receiveProcessStopped{0};
	uint32_t transmitProcessStopped{0};
	uint32_t transmitBufferUnavailable{0};
	uint32_t transmitUnderflows{0};
	uint32_t receiveOverflows{0};
	uint32_t abnormalInterrupts{0};
	uint32_t fatalBusErrors{0};
	uint32_t contextDescriptorErrors{0};
	uint32_t recoveries{0};
	uint32_t lastDmaStatus{0};
	uint32_t lastMtlStatus{0};
	bool recoveryRequired{false};
};

class EthBase
{
public:
	static void
	handleInterrupt();

protected:
	static bool
	claimOwnership(const void* owner);
	static void
	releaseOwnership(const void* owner);
	static uint32_t
	takeDmaEvents();
	static uint32_t
	takeMtlEvents();
	static uint32_t
	takeInterruptCount();
	static void
	clearEvents();

private:
	static inline const void* owner_{nullptr};
	static inline volatile uint32_t dmaEvents_{0};
	static inline volatile uint32_t mtlEvents_{0};
	static inline volatile uint32_t interruptCount_{0};
};

/**
 * STM32H5/H7 Ethernet MAC with caller-owned descriptor and packet storage.
 *
 * Transmission copies one complete frame into a static DMA buffer. Reception
 * returns a move-only zero-copy lease; at most one RX lease can be outstanding.
 * PHY control is intentionally separate and can use this class as its Clause-22
 * MDIO transport. Only one Eth specialization may own the peripheral at a time.
 */
template<typename Storage, ethernet::MediaInterface Interface = ethernet::MediaInterface::Rmii,
		 typename CachePolicy = stm32::dwmac::DefaultEthernetCachePolicy>
class Eth : public EthBase
{
	static_assert(Interface == ethernet::MediaInterface::Mii ||
					  Interface == ethernet::MediaInterface::Rmii,
				  "Eth media interface must be MII or RMII");

public:
	using Ring = stm32::dwmac::DescriptorRing<Storage, CachePolicy>;
	using RxFrame = typename Ring::RxFrame;

	template<class... Signals>
	static void
	connect()
	{
		using Connector = GpioConnector<Peripheral::Eth, Signals...>;
		using Mdc = typename Connector::template GetSignal<Gpio::Signal::Mdc>;
		using Mdio = typename Connector::template GetSignal<Gpio::Signal::Mdio>;
		static_assert(Connector::template IsValid<Mdc> && Connector::template IsValid<Mdio>,
					  "Eth::connect() requires Mdc and Mdio signals");

		if constexpr (Interface == ethernet::MediaInterface::Rmii)
		{
			using RefClk = typename Connector::template GetSignal<Gpio::Signal::Refclk>;
			using CrsDv = typename Connector::template GetSignal<Gpio::Signal::Rcccrsdv>;
			using Rxd0 = typename Connector::template GetSignal<Gpio::Signal::Rxd0>;
			using Rxd1 = typename Connector::template GetSignal<Gpio::Signal::Rxd1>;
			using TxEn = typename Connector::template GetSignal<Gpio::Signal::Txen>;
			using Txd0 = typename Connector::template GetSignal<Gpio::Signal::Txd0>;
			using Txd1 = typename Connector::template GetSignal<Gpio::Signal::Txd1>;
			static_assert(
				Connector::template IsValid<RefClk> && Connector::template IsValid<CrsDv> &&
					Connector::template IsValid<Rxd0> && Connector::template IsValid<Rxd1> &&
					Connector::template IsValid<TxEn> && Connector::template IsValid<Txd0> &&
					Connector::template IsValid<Txd1> && sizeof...(Signals) == 9,
				"RMII requires Mdc, Mdio, Refclk, Rcccrsdv, Rxd0, Rxd1, Txen, Txd0 and Txd1");
		} else
		{
			using Crs = typename Connector::template GetSignal<Gpio::Signal::Crs>;
			using Col = typename Connector::template GetSignal<Gpio::Signal::Col>;
			using RxClk = typename Connector::template GetSignal<Gpio::Signal::Rxclk>;
			using RxDv = typename Connector::template GetSignal<Gpio::Signal::Rxdv>;
			using RxEr = typename Connector::template GetSignal<Gpio::Signal::Rxer>;
			using Rxd0 = typename Connector::template GetSignal<Gpio::Signal::Rxd0>;
			using Rxd1 = typename Connector::template GetSignal<Gpio::Signal::Rxd1>;
			using Rxd2 = typename Connector::template GetSignal<Gpio::Signal::Rxd2>;
			using Rxd3 = typename Connector::template GetSignal<Gpio::Signal::Rxd3>;
			using TxClk = typename Connector::template GetSignal<Gpio::Signal::Txclk>;
			using TxEn = typename Connector::template GetSignal<Gpio::Signal::Txen>;
			using TxEr = typename Connector::template GetSignal<Gpio::Signal::Txer>;
			using Txd0 = typename Connector::template GetSignal<Gpio::Signal::Txd0>;
			using Txd1 = typename Connector::template GetSignal<Gpio::Signal::Txd1>;
			using Txd2 = typename Connector::template GetSignal<Gpio::Signal::Txd2>;
			using Txd3 = typename Connector::template GetSignal<Gpio::Signal::Txd3>;
			static_assert(
				Connector::template IsValid<Crs> && Connector::template IsValid<Col> &&
					Connector::template IsValid<RxClk> && Connector::template IsValid<RxDv> &&
					Connector::template IsValid<RxEr> && Connector::template IsValid<Rxd0> &&
					Connector::template IsValid<Rxd1> && Connector::template IsValid<Rxd2> &&
					Connector::template IsValid<Rxd3> && Connector::template IsValid<TxClk> &&
					Connector::template IsValid<TxEn> && Connector::template IsValid<TxEr> &&
					Connector::template IsValid<Txd0> && Connector::template IsValid<Txd1> &&
					Connector::template IsValid<Txd2> && Connector::template IsValid<Txd3> &&
					sizeof...(Signals) == 18,
				"MII requires all MDC, MDIO, CRS/COL, RX and TX clock/data/control signals");
		}

		(GpioStatic<typename Signals::Data>::configure(Gpio::OutputType::PushPull,
													   Gpio::OutputSpeed::VeryHigh),
		 ...);
		Connector::connect();
	}

	template<class SystemClock>
	static EthernetInitResult
	initialize(Storage& storage, const EthernetMacAddress& macAddress,
			   uint8_t interruptPriority = 5, uint32_t resetTimeoutMicroseconds = 1'000,
			   uint32_t mdioTimeoutMicroseconds = 1'000);

	static void
	start();
	static void
	stop();
	static ethernet::TransmitResult
	tryTransmit(std::span<const uint8_t> frame);
	static std::optional<RxFrame>
	receive();
	static EthernetPollResult
	poll();
	static EthernetRecoveryResult
	recover();

	static EthernetConfigResult
	setLinkState(const ethernet::LinkState& state);
	static ethernet::LinkState
	linkState()
	{ return linkState_; }
	static EthernetConfigResult
	setInternalLoopback(bool enable);

	static ethernet::MdioStatus
	read(uint8_t phyAddress, uint8_t reg, uint16_t& value);
	static ethernet::MdioStatus
	write(uint8_t phyAddress, uint8_t reg, uint16_t value);

	static EthernetDiagnostics
	diagnostics();
	static bool
	isInitialized()
	{ return initialized_; }
	static bool
	isStarted()
	{ return started_; }

private:
	static bool
	validStorage(const Storage& storage);
	static bool
	softwareReset(uint32_t timeoutMicroseconds);
	static void
	configurePeripheral();
	static void
	applyLinkState();
	static void
	applyInternalLoopback();
	static void
	programMacAddress();
	static bool
	waitForMdio();
	static uint32_t
	mdioClockRange(uint32_t hclk);
	static void
	updateRxTail();

private:
	static inline Storage* storage_{nullptr};
	static inline std::optional<Ring> ring_{};
	static inline uint8_t ownershipToken_{0};
	static inline EthernetMacAddress macAddress_{};
	static inline ethernet::LinkState linkState_{};
	static inline EthernetDiagnostics diagnostics_{};
	static inline uint32_t hclk_{0};
	static inline uint32_t mdioClockRange_{0};
	static inline uint32_t resetTimeoutMicroseconds_{1'000};
	static inline uint32_t mdioTimeoutMicroseconds_{1'000};
	static inline bool initialized_{false};
	static inline bool started_{false};
	static inline bool restartAfterRecovery_{false};
	static inline bool internalLoopback_{false};
	static inline bool dmaError_{false};
};

}  // namespace modm::platform

#include "eth_impl.hpp"
