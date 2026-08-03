/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_COMMUNICATION_LWIP_ETHERNET_HPP
#define MODM_COMMUNICATION_LWIP_ETHERNET_HPP

#include <modm/architecture/interface/assert.hpp>
#include <modm/architecture/interface/ethernet.hpp>
#include <modm_lwip.hpp>

#include <lwip/opt.h>
#include <lwip/etharp.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/pbuf.h>
#include <lwip/stats.h>
#include <lwip/timeouts.h>
#include <netif/ethernet.h>

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>

#ifndef MODM_LWIP_CHECKSUM_HARDWARE
#define MODM_LWIP_CHECKSUM_HARDWARE 0
#endif
#if MODM_LWIP_CHECKSUM_HARDWARE && IP_FRAG
#error "Hardware checksum mode does not support outgoing IPv4 fragmentation"
#endif
#if MODM_LWIP_CHECKSUM_HARDWARE && !LWIP_CHECKSUM_CTRL_PER_NETIF
#error "Hardware checksum mode requires LWIP_CHECKSUM_CTRL_PER_NETIF"
#endif
#if !LWIP_IPV4 || !LWIP_ETHERNET || !LWIP_ARP
#error "modm::lwip::LwipEthernet requires LWIP_IPV4, LWIP_ETHERNET, and LWIP_ARP"
#endif
#if LWIP_IPV6 || LWIP_IGMP || LWIP_IPV6_MLD
#error "modm::lwip::LwipEthernet supports IPv4 unicast and broadcast only"
#endif

namespace modm::lwip
{

struct IPv4Address
{
	std::array<uint8_t, 4> bytes;
};

struct StaticIPv4Configuration
{
	ethernet::MacAddress macAddress;
	IPv4Address ipAddress;
	IPv4Address netmask;
	IPv4Address gateway;
};

namespace detail
{

inline constexpr uint16_t EthernetMtu = 1500;

template <class Mac>
concept EthernetMac = ethernet::Clause22Mdio<Mac> and
		requires(std::size_t size, ethernet::LinkStatus linkStatus) {
	typename Mac::MediaInterface;
	typename Mac::Configuration;
	typename Mac::ChecksumMode;
	typename Mac::InitializationError;
	typename Mac::TransmitError;
	typename Mac::TransmitBufferLease;
	typename Mac::ReceiveError;
	typename Mac::ReceiveChecksumStatus;
	typename Mac::ReceiveBufferLease;
	typename Mac::LinkUpdateError;
	typename Mac::LinkUpdateResult;
	{ Mac::MaxFrameSize } -> std::convertible_to<std::size_t>;
	{ Mac::acquireTransmitBuffer(size) } -> std::same_as<typename Mac::TransmitBufferLease>;
	{ Mac::tryAcquireReceiveBuffer() } -> std::same_as<typename Mac::ReceiveBufferLease>;
	{ Mac::getLinkStatus() } -> std::same_as<ethernet::LinkStatus>;
	{ Mac::notifyUpdatedLinkStatus(linkStatus) } -> std::same_as<typename Mac::LinkUpdateResult>;
};

template <class Phy, class Mac>
concept EthernetPhy = EthernetMac<Mac> and requires {
	typename Phy::InitializationError;
	typename Phy::InitializationResult;
	typename Phy::LinkStatusResult;
	{ Phy::template initialize<Mac>() } -> std::same_as<typename Phy::InitializationResult>;
	{ Phy::template readLinkStatus<Mac>() } -> std::same_as<typename Phy::LinkStatusResult>;
};

template <class Mac>
struct EthernetInterfaceState
{
	struct ReceiveStatistics
	{
		uint64_t acquiredFrames{};
		uint64_t droppedFrames{};
		uint64_t checksumDrops{};
		uint64_t allocationDrops{};
	};

	void const* adapterOwner{};
	bool netifRegistered{};
	bool adapterReady{};
	struct netif netif{};
	ethernet::MacAddress macAddress{};
	ReceiveStatistics receiveStatistics{};
};

template <class Mac>
inline EthernetInterfaceState<Mac> ethernetInterfaceState{};

inline ip4_addr_t
makeIp4(IPv4Address address)
{
	ip4_addr_t ip;
	IP4_ADDR(&ip, address.bytes[0], address.bytes[1], address.bytes[2], address.bytes[3]);
	return ip;
}

} // namespace detail

template <detail::EthernetMac Mac, class Phy>
	requires detail::EthernetPhy<Phy, Mac>
class LwipEthernet
{
	using State = detail::EthernetInterfaceState<Mac>;

public:
	/// Cumulative receive counters for the physical MAC.
	using ReceiveStatistics = typename State::ReceiveStatistics;
	using MediaInterface = ethernet::MediaInterface;
	using LinkState = ethernet::LinkState;
	using LinkUpdateError = typename Mac::LinkUpdateError;

	template <class... Signals>
	static void
	connect()
	{
		Mac::template connect<Signals...>();
	}

	enum class AdapterInitializationError
	{
		None,
		MacAlreadyBound,
		NetifRegistrationFailed,
	};

	struct [[nodiscard]] InitializationResult
	{
		typename Mac::InitializationError macError = Mac::InitializationError::None;
		typename Phy::InitializationError phyError = Phy::InitializationError::None;
		ethernet::MdioError phyMdioError = ethernet::MdioError::None;
		ethernet::MdioError linkMdioError = ethernet::MdioError::None;
		typename Mac::LinkUpdateError linkUpdateError = Mac::LinkUpdateError::None;
		AdapterInitializationError adapterError = AdapterInitializationError::None;

		explicit operator bool() const
		{
			return macError == Mac::InitializationError::None and
					phyError == Phy::InitializationError::None and
					phyMdioError == ethernet::MdioError::None and
					linkMdioError == ethernet::MdioError::None and
					linkUpdateError == Mac::LinkUpdateError::None and
					adapterError == AdapterInitializationError::None;
		}
	};

	struct [[nodiscard]] LinkPollResult
	{
		ethernet::LinkStatus status;
		ethernet::MdioError phyError = ethernet::MdioError::None;
		typename Mac::LinkUpdateError macError = Mac::LinkUpdateError::None;

		constexpr explicit operator bool() const noexcept
		{
			return phyError == ethernet::MdioError::None and
					macError == Mac::LinkUpdateError::None;
		}
	};

	/**
	 * Initialize the MAC and register or refresh its lwIP network interface.
	 *
	 * The call initializes lwIP, then the MAC and PHY, registers the netif, and
	 * applies the first observed link state. A failed retry leaves an existing
	 * netif down and may be retried.
	 */
	template <class SystemClock,
			typename Mac::MediaInterface Interface = Mac::MediaInterface::RMII>
	[[nodiscard]] static InitializationResult
	initialize(StaticIPv4Configuration const& config, uint8_t priority = 5)
	{
		modm::lwip::initialize();

		auto& state = getState();
		if (not claimAdapterOwnership())
			return {.adapterError = AdapterInitializationError::MacAlreadyBound};

		// Initialize the MAC with the checksum mode selected by the lwIP module
		const typename Mac::Configuration macConfiguration{
			.macAddress = config.macAddress,
			.checksumMode = MODM_LWIP_CHECKSUM_HARDWARE ?
					Mac::ChecksumMode::Hardware : Mac::ChecksumMode::Software,
		};
		const auto macResult =
				Mac::template initialize<SystemClock, Interface>(macConfiguration, priority);
		if (not macResult) {
			state.adapterReady = false;
			markNetifUnavailable();
			if (not state.netifRegistered)
				state.adapterOwner = nullptr;
			return {.macError = macResult.error};
		}

		// Reset and configure the PHY
		const auto phyResult = Phy::template initialize<Mac>();
		if (not phyResult) {
			state.adapterReady = false;
			markNetifUnavailable();
			return {.phyError = phyResult.error, .phyMdioError = phyResult.mdioError};
		}

		// Register the netif or refresh its addressing
		if (const auto error = configureNetif(config);
				error != AdapterInitializationError::None)
			return {.adapterError = error};

		// Apply the first observed physical link state to the MAC and lwIP
		state.adapterReady = true;
		const auto link = updateLinkStatus();
		if (link.phyError != ethernet::MdioError::None)
			return {.linkMdioError = link.phyError};
		if (link.macError != Mac::LinkUpdateError::None)
			return {.linkUpdateError = link.macError};
		return {};
	}

	/// Access the underlying netif for direct lwIP API use.
	static struct netif*
	netif()
	{
		return &getState().netif;
	}

	/// Select this interface as lwIP's default route. Interface must already be initialized.
	[[nodiscard]] static bool
	setDefault()
	{
		LwIPSingleThreadGuard guard;
		auto& state = getState();
		if (not ownsState() or not state.adapterReady or not state.netifRegistered)
			return false;
		netif_set_default(&state.netif);
		return true;
	}

	/**
	 * Deliver at most @p frameLimit acquired frames to lwIP.
	 *
	 * The call stops when the receive ring is empty, the limit is reached, or an
	 * error occurs. Call after a receive wakeup or often enough to avoid filling
	 * the MAC receive ring.
	 */
	[[nodiscard]] static err_t
	pollInput(std::size_t frameLimit = Mac::RxDescriptorCount)
	{
		LwIPSingleThreadGuard guard;
		auto& state = getState();
		if (not ownsState() or not state.adapterReady or not state.netifRegistered)
			return ERR_IF;
		return drainInput(frameLimit);
	}

	/**
	 * Service the physical link and notify lwIP of its current state.
	 *
	 * Scheduling and rate limiting are owned by the application.
	 */
	[[nodiscard]] static LinkPollResult
	pollLink()
	{
		auto& state = getState();
		if (not ownsState() or not state.adapterReady or not state.netifRegistered)
			return {Mac::getLinkStatus(), {}, Mac::LinkUpdateError::NotInitialized};

		return updateLinkStatus();
	}

	/// Return current cumulative receive statistics.
	[[nodiscard]] static ReceiveStatistics
	getCumulativeReceiveStatistics()
	{
		return getState().receiveStatistics;
	}

	// Pass-throughs to the configured MAC.
	static ethernet::LinkStatus
	getLinkStatus()
	{
		return Mac::getLinkStatus();
	}

	static auto
	consumeWakeupEvents()
		requires requires { Mac::consumeWakeupEvents(); }
	{
		return Mac::consumeWakeupEvents();
	}

	static bool
	hasPendingWakeupEvents()
		requires requires { Mac::hasPendingWakeupEvents(); }
	{
		return Mac::hasPendingWakeupEvents();
	}

	static auto
	getErrorCounters()
		requires requires { Mac::getErrorCounters(); }
	{
		return Mac::getErrorCounters();
	}

	static auto
	getHardwareErrorStatus()
		requires requires { Mac::getHardwareErrorStatus(); }
	{
		return Mac::getHardwareErrorStatus();
	}

	static void
	resetErrorCounters()
		requires requires { Mac::resetErrorCounters(); }
	{
		Mac::resetErrorCounters();
	}

	static void
	resetHardwareErrorStatus()
		requires requires { Mac::resetHardwareErrorStatus(); }
	{
		Mac::resetHardwareErrorStatus();
	}

private:
	// Dummy static memory location to give a per-(MAC x PHY) address for ownership
	// tracking. Per-MAC EthernetInterfaceState remembers which LwipEthernet
	// configured it to ensure we don't confuse different PHYs with the same MAC.
	// Today we don't support multiple PHYs at once.
	inline static constexpr char OwnerMarker{};

	static bool
	claimAdapterOwnership()
	{
		auto& state = getState();
		auto const* const owner = &OwnerMarker;
		if (state.adapterOwner != nullptr and state.adapterOwner != owner)
			return false;
		state.adapterOwner = owner;
		return true;
	}

	static State&
	getState()
	{
		return detail::ethernetInterfaceState<Mac>;
	}

	static bool
	ownsState()
	{
		return getState().adapterOwner == &OwnerMarker;
	}

	static AdapterInitializationError
	configureNetif(StaticIPv4Configuration const& config)
	{
		auto& state = getState();
		const ip4_addr_t ipAddress = detail::makeIp4(config.ipAddress);
		const ip4_addr_t netmask = detail::makeIp4(config.netmask);
		const ip4_addr_t gateway = detail::makeIp4(config.gateway);

		LwIPSingleThreadGuard guard;
		state.macAddress = config.macAddress;
		if (state.netifRegistered) {
			std::memcpy(state.netif.hwaddr, state.macAddress.data(), state.macAddress.size());
			netif_set_addr(&state.netif, &ipAddress, &netmask, &gateway);
#if LWIP_CHECKSUM_CTRL_PER_NETIF
			NETIF_SET_CHECKSUM_CTRL(&state.netif, buildConfiguredChecksumFlags());
#endif
		}
		else {
			std::memset(&state.netif, 0, sizeof(state.netif));
			if (netif_add(&state.netif, &ipAddress, &netmask, &gateway, nullptr,
					&LwipEthernet::netifInitialize, ethernet_input) == nullptr) {
				state.adapterOwner = nullptr;
				return AdapterInitializationError::NetifRegistrationFailed;
			}
			state.netifRegistered = true;
		}
		netif_set_up(&state.netif);
		return AdapterInitializationError::None;
	}

	static LinkPollResult
	updateLinkStatus()
	{
		const auto observed = Phy::template readLinkStatus<Mac>();
		if (not observed)
			return {Mac::getLinkStatus(), observed.error, {}};
		const auto updated = Mac::notifyUpdatedLinkStatus(observed.status);
		{
			LwIPSingleThreadGuard guard;
			setNetifLinkStatus(updated.status);
		}
		return {updated.status, {}, updated.error};
	}

	static void
	markNetifUnavailable()
	{
		auto& state = getState();
		if (not state.netifRegistered)
			return;
		LwIPSingleThreadGuard guard;
		netif_set_down(&state.netif);
		netif_set_link_down(&state.netif);
	}

	static err_t
	netifInitialize(struct netif* netif)
	{
		auto& state = getState();
		netif->name[0] = 'e';
		netif->name[1] = 'n';
		netif->output = etharp_output;
		netif->linkoutput = &LwipEthernet::lowLevelOutput;
		netif->mtu = detail::EthernetMtu;
		netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
		// NETIF_FLAG_UP and NETIF_FLAG_LINK_UP are managed by the netif APIs.
		netif->hwaddr_len = ETH_HWADDR_LEN;
		std::memcpy(netif->hwaddr, state.macAddress.data(), state.macAddress.size());
#if LWIP_CHECKSUM_CTRL_PER_NETIF
		NETIF_SET_CHECKSUM_CTRL(netif, buildConfiguredChecksumFlags());
#endif
		return ERR_OK;
	}

	static err_t
	lowLevelOutput(struct netif*, struct pbuf* p)
	{
		auto& state = getState();
		if (not ownsState() or not state.adapterReady or not state.netifRegistered)
			return ERR_IF;

		const std::size_t length = p->tot_len;
		if (length > Mac::MaxFrameSize)
			return ERR_BUF;

		auto lease = Mac::acquireTransmitBuffer(length);
		if (not lease) {
			incrementTransmitErrorStatistics(lease.error());
			return mapTransmitErrorToLwipError(lease.error());
		}
		auto buffer = lease.buffer();
		modm_assert(buffer.size() >= length, "lwip.tx.buffer",
				"Ethernet MAC returned an undersized transmit buffer", buffer.size());

		std::size_t offset = 0;
		for (struct pbuf* q = p; q != nullptr; q = q->next) {
			if (offset + q->len > length)
				return ERR_BUF;
			std::memcpy(buffer.data() + offset, q->payload, q->len);
			offset += q->len;
		}

		if (offset != length)
			return ERR_BUF;
		const auto result = lease.commit();
		if (not result) {
			incrementTransmitErrorStatistics(result.error);
			return mapTransmitErrorToLwipError(result.error);
		}
		LINK_STATS_INC(link.xmit);
		return ERR_OK;
	}

	static err_t
	drainInput(std::size_t frameLimit)
	{
		auto& state = getState();
		for (std::size_t processed = 0; processed < frameLimit; ++processed) {
			auto lease = Mac::tryAcquireReceiveBuffer();
			if (not lease) {
				if (lease.error() == Mac::ReceiveError::NoFrameAvailable)
					return ERR_OK;
				LINK_STATS_INC(link.err);
				return ERR_IF;
			}

			incrementSaturating(state.receiveStatistics.acquiredFrames);
			const auto checksumStatus = lease.checksumStatus();
			if (checksumStatus == Mac::ReceiveChecksumStatus::Invalid) {
				incrementSaturating(state.receiveStatistics.droppedFrames);
				incrementSaturating(state.receiveStatistics.checksumDrops);
				LINK_STATS_INC(link.chkerr);
				LINK_STATS_INC(link.drop);
				continue;
			}

			const auto frame = lease.buffer();
			struct pbuf* p = pbuf_alloc(PBUF_RAW, frame.size(), PBUF_POOL);
			if (p == nullptr) {
				incrementSaturating(state.receiveStatistics.droppedFrames);
				incrementSaturating(state.receiveStatistics.allocationDrops);
				LINK_STATS_INC(link.memerr);
				LINK_STATS_INC(link.drop);
				return ERR_MEM;
			}

			const err_t copyError = pbuf_take(p, frame.data(), frame.size());
			lease.release();
			modm_assert(copyError == ERR_OK, "lwip.pbuf",
					"receive frame copy failed", copyError);

			LINK_STATS_INC(link.recv);
#if MODM_LWIP_CHECKSUM_HARDWARE
			ChecksumCheckScope checksumScope{
					state.netif, checksumStatus == Mac::ReceiveChecksumStatus::NotChecked};
#endif
			const err_t inputError = state.netif.input(p, &state.netif);
			if (inputError != ERR_OK) {
				pbuf_free(p);
				incrementSaturating(state.receiveStatistics.droppedFrames);
				LINK_STATS_INC(link.err);
				LINK_STATS_INC(link.drop);
				return inputError;
			}
		}
		return ERR_OK;
	}

	static void
	setNetifLinkStatus(ethernet::LinkStatus status)
	{
		if (status.state == ethernet::LinkState::Up)
			netif_set_link_up(&getState().netif);
		else
			netif_set_link_down(&getState().netif);
	}

#if LWIP_CHECKSUM_CTRL_PER_NETIF
	static constexpr uint16_t
	buildConfiguredChecksumFlags()
	{
		// Disable lwIP software checksums when hardware checksums are enabled.
		// ChecksumCheckScope temporarily re-enables RX software checksums for
		// incoming fragments that the MAC leaves unchecked.
		return MODM_LWIP_CHECKSUM_HARDWARE ?
				uint16_t(0) :
				uint16_t(NETIF_CHECKSUM_ENABLE_ALL);
	}
#endif

#if MODM_LWIP_CHECKSUM_HARDWARE
	class ChecksumCheckScope
	{
	public:
		ChecksumCheckScope(struct netif& netif, bool enable) :
			netif(netif), previous(netif.chksum_flags), changed(enable and MODM_LWIP_CHECKSUM_HARDWARE)
		{
			if (changed) {
				constexpr uint16_t ReceiveChecksums = NETIF_CHECKSUM_CHECK_IP |
						NETIF_CHECKSUM_CHECK_UDP | NETIF_CHECKSUM_CHECK_TCP |
						NETIF_CHECKSUM_CHECK_ICMP;
				NETIF_SET_CHECKSUM_CTRL(&netif, uint16_t(previous | ReceiveChecksums));
			}
		}

		~ChecksumCheckScope()
		{
			if (changed)
				NETIF_SET_CHECKSUM_CTRL(&netif, previous);
		}

	private:
		struct netif& netif;
		uint16_t previous;
		bool changed;
	};
#endif

	static err_t
	mapTransmitErrorToLwipError(typename Mac::TransmitError error)
	{
		switch (error) {
		case Mac::TransmitError::None:
			return ERR_OK;
		case Mac::TransmitError::Busy:
			return ERR_MEM;
		case Mac::TransmitError::InvalidLength:
			return ERR_BUF;
		case Mac::TransmitError::UnsupportedFragmentation:
			return ERR_VAL;
		case Mac::TransmitError::LinkDown:
		case Mac::TransmitError::NotInitialized:
		case Mac::TransmitError::Faulted:
			return ERR_IF;
		}
		return ERR_IF;
	}

	static void
	incrementTransmitErrorStatistics(typename Mac::TransmitError error)
	{
		switch (error) {
		case Mac::TransmitError::None:
			return;
		case Mac::TransmitError::Busy:
			LINK_STATS_INC(link.memerr);
			return;
		case Mac::TransmitError::InvalidLength:
			LINK_STATS_INC(link.lenerr);
			return;
		case Mac::TransmitError::UnsupportedFragmentation:
		case Mac::TransmitError::LinkDown:
		case Mac::TransmitError::NotInitialized:
		case Mac::TransmitError::Faulted:
			LINK_STATS_INC(link.err);
			return;
		}
	}

	static void
	incrementSaturating(uint64_t& counter)
	{
		if (counter != std::numeric_limits<uint64_t>::max())
			++counter;
	}
};

} // namespace modm::lwip

#endif // MODM_COMMUNICATION_LWIP_ETHERNET_HPP
