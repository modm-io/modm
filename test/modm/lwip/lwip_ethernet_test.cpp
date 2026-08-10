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

#include "lwip_ethernet_test.hpp"

#include <modm-test/mock/clock.hpp>
#include <modm_lwip.hpp>
#include <modm_lwip/ethernet.hpp>

#include <lwip/stats.h>
#include <lwip/timeouts.h>
#include <lwip/inet_chksum.h>
#include <lwip/udp.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>

#if defined(__unix__)
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace
{

struct TestClock {};

template <int Tag>
struct FakeMac
{
	using MediaInterface = modm::ethernet::MediaInterface;
	using ChecksumMode = modm::ethernet::ChecksumMode;
	using Speed = modm::ethernet::Speed;
	using DuplexMode = modm::ethernet::DuplexMode;
	using LinkMode = modm::ethernet::LinkMode;
	using LinkState = modm::ethernet::LinkState;
	using LinkStatus = modm::ethernet::LinkStatus;
	enum class InitializationError { None, Failed };
	enum class TransmitError {
		None, Busy, InvalidLength, LinkDown, NotInitialized, Faulted,
		UnsupportedFragmentation,
	};
	enum class ReceiveError { None, NoFrameAvailable, NotInitialized, Faulted };
	enum class ReceiveChecksumStatus { NotChecked, Valid, Invalid };
	enum class LinkUpdateError {
		None, NotInitialized, MacStopTimeout, OutstandingLease, FatalBusError,
	};

	struct Configuration
	{
		std::array<uint8_t, 6> macAddress{};
		ChecksumMode checksumMode{ChecksumMode::Software};
	};

	struct InitializationResult
	{
		InitializationError error{InitializationError::None};
		explicit operator bool() const { return error == InitializationError::None; }
	};

	struct TransmitResult
	{
		TransmitError error{TransmitError::None};
		explicit operator bool() const { return error == TransmitError::None; }
	};

	struct LinkUpdateResult
	{
		LinkStatus status{};
		LinkUpdateError error{LinkUpdateError::None};
		explicit operator bool() const { return error == LinkUpdateError::None; }
	};

	static constexpr std::size_t MaxFrameSize = 1514;
	static constexpr std::size_t RxDescriptorCount = 8;

	class TransmitBufferLease
	{
	public:
		TransmitBufferLease() = default;
		TransmitBufferLease(const TransmitBufferLease&) = delete;
		TransmitBufferLease& operator=(const TransmitBufferLease&) = delete;

		TransmitBufferLease(TransmitBufferLease&& other) noexcept
		: error_(other.error_), length_(other.length_), active_(other.active_)
		{
			other.active_ = false;
		}

		TransmitBufferLease&
		operator=(TransmitBufferLease&& other) noexcept
		{
			if (this != &other) {
				cancel();
				error_ = other.error_;
				length_ = other.length_;
				active_ = other.active_;
				other.active_ = false;
			}
			return *this;
		}

		~TransmitBufferLease() { cancel(); }

		explicit operator bool() const
		{
			return active_ and error_ == TransmitError::None;
		}

		TransmitError error() const { return error_; }

		std::span<uint8_t>
		buffer()
		{
			return *this ? std::span<uint8_t>{FakeMac::tx.data(), length_} : std::span<uint8_t>{};
		}

		TransmitResult
		commit()
		{
			if (not *this)
				return {error_};

			active_ = false;
			FakeMac::txLeaseActive = false;
			FakeMac::committedLength = length_;
			if (FakeMac::committedFrameCount < FakeMac::committedFrames.size()) {
				const auto index = FakeMac::committedFrameCount++;
				FakeMac::committedLengths[index] = length_;
				std::copy_n(FakeMac::tx.begin(), length_, FakeMac::committedFrames[index].begin());
			}
			FakeMac::commitCalls++;
			return {FakeMac::commitError};
		}

		void
		cancel()
		{
			if (active_) {
				active_ = false;
				FakeMac::txLeaseActive = false;
				FakeMac::cancelCalls++;
			}
		}

	private:
		friend FakeMac;

		explicit TransmitBufferLease(TransmitError error)
		: error_(error)
		{}

		TransmitBufferLease(std::size_t length, bool active)
		: length_(length), active_(active)
		{}

		TransmitError error_{TransmitError::None};
		std::size_t length_{0};
		bool active_{false};
	};

	class ReceiveBufferLease
	{
	public:
		ReceiveBufferLease() = default;
		ReceiveBufferLease(const ReceiveBufferLease&) = delete;
		ReceiveBufferLease& operator=(const ReceiveBufferLease&) = delete;

		ReceiveBufferLease(ReceiveBufferLease&& other) noexcept
		: error_(other.error_), length_(other.length_), checksum_(other.checksum_), active_(other.active_)
		{
			other.active_ = false;
		}

		ReceiveBufferLease&
		operator=(ReceiveBufferLease&& other) noexcept
		{
			if (this != &other) {
				release();
				error_ = other.error_;
				length_ = other.length_;
				checksum_ = other.checksum_;
				active_ = other.active_;
				other.active_ = false;
			}
			return *this;
		}

		~ReceiveBufferLease() { release(); }

		explicit operator bool() const
		{
			return active_ and error_ == ReceiveError::None;
		}

		ReceiveError error() const { return error_; }

		std::span<const uint8_t>
		buffer() const
		{
			return *this ? std::span<const uint8_t>{FakeMac::rx.data(), length_} :
					std::span<const uint8_t>{};
		}

		ReceiveChecksumStatus checksumStatus() const { return checksum_; }

		void
		release()
		{
			if (active_) {
				active_ = false;
				FakeMac::rxLeaseActive = false;
				if (FakeMac::rxFrames != 0)
					FakeMac::rxFrames--;
				FakeMac::rxReady = FakeMac::rxFrames != 0;
				FakeMac::releaseCalls++;
			}
		}

	private:
		friend FakeMac;

		explicit ReceiveBufferLease(ReceiveError error)
		: error_(error)
		{}

		ReceiveBufferLease(std::size_t length, ReceiveChecksumStatus checksum)
		: length_(length), checksum_(checksum), active_(true)
		{}

		ReceiveError error_{ReceiveError::None};
		std::size_t length_{0};
		ReceiveChecksumStatus checksum_{ReceiveChecksumStatus::NotChecked};
		bool active_{false};
	};

	static_assert(not std::is_copy_constructible_v<TransmitBufferLease>);
	static_assert(std::is_move_constructible_v<TransmitBufferLease>);
	static_assert(not std::is_copy_constructible_v<ReceiveBufferLease>);
	static_assert(std::is_move_constructible_v<ReceiveBufferLease>);

	static void
	reset()
	{
		initializeError = InitializationError::None;
		acquireError = TransmitError::None;
		commitError = TransmitError::None;
		receiveError = ReceiveError::None;
		rxChecksum = ReceiveChecksumStatus::NotChecked;
		initializeCalls = 0;
		linkUpdateCalls = 0;
		releaseCalls = 0;
		commitCalls = 0;
		cancelCalls = 0;
		committedLength = 0;
		committedFrameCount = 0;
		committedLengths.fill(0);
		rxLength = 0;
		rxReady = false;
		rxFrames = 0;
		txLeaseActive = false;
		rxLeaseActive = false;
		configuration = {};
		linkStatus = {};
		linkUpdateError = LinkUpdateError::None;
		mdioError = modm::ethernet::MdioError::None;
		tx.fill(0);
		rx.fill(0);
	}

	template <class, MediaInterface>
	static InitializationResult
	initialize(Configuration const& config, uint8_t)
	{
		initializeCalls++;
		configuration = config;
		return {initializeError};
	}

	static TransmitBufferLease
	acquireTransmitBuffer(std::size_t size)
	{
		if (acquireError != TransmitError::None)
			return TransmitBufferLease{acquireError};
		if (size == 0 or size > tx.size())
			return TransmitBufferLease{TransmitError::InvalidLength};
		if (txLeaseActive)
			return TransmitBufferLease{TransmitError::Busy};
		txLeaseActive = true;
		return TransmitBufferLease{size, true};
	}

	static TransmitResult
	transmit(std::span<const uint8_t> frame)
	{
		auto lease = acquireTransmitBuffer(frame.size());
		if (not lease)
			return {lease.error()};
		std::copy(frame.begin(), frame.end(), lease.buffer().begin());
		return lease.commit();
	}

	static ReceiveBufferLease
	tryAcquireReceiveBuffer()
	{
		if (receiveError != ReceiveError::None)
			return ReceiveBufferLease{receiveError};
		if (not rxReady)
			return ReceiveBufferLease{ReceiveError::NoFrameAvailable};
		if (rxLeaseActive)
			return ReceiveBufferLease{ReceiveError::NoFrameAvailable};
		rxLeaseActive = true;
		return ReceiveBufferLease{rxLength, rxChecksum};
	}

	static LinkUpdateResult
	notifyUpdatedLinkStatus(LinkStatus status)
	{
		linkUpdateCalls++;
		if (linkUpdateError == LinkUpdateError::None)
			linkStatus = status;
		return {linkStatus, linkUpdateError};
	}

	static LinkStatus
	getLinkStatus()
	{
		return linkStatus;
	}

	static modm::ethernet::MdioError
	readPhyRegister(uint8_t, uint8_t, uint16_t& value)
	{
		value = 0;
		return mdioError;
	}

	static modm::ethernet::MdioError
	writePhyRegister(uint8_t, uint8_t, uint16_t)
	{
		return mdioError;
	}

	static inline InitializationError initializeError{InitializationError::None};
	static inline TransmitError acquireError{TransmitError::None};
	static inline TransmitError commitError{TransmitError::None};
	static inline ReceiveError receiveError{ReceiveError::None};
	static inline ReceiveChecksumStatus rxChecksum{ReceiveChecksumStatus::NotChecked};
	static inline unsigned initializeCalls{0};
	static inline unsigned linkUpdateCalls{0};
	static inline unsigned releaseCalls{0};
	static inline unsigned commitCalls{0};
	static inline unsigned cancelCalls{0};
	static inline std::size_t committedLength{0};
	static inline std::size_t committedFrameCount{0};
	static inline std::array<std::size_t, 3> committedLengths{};
	static inline std::array<std::array<uint8_t, MaxFrameSize>, 3> committedFrames{};
	static inline std::size_t rxLength{0};
	static inline bool rxReady{false};
	static inline std::size_t rxFrames{0};
	static inline bool txLeaseActive{false};
	static inline bool rxLeaseActive{false};
	static inline Configuration configuration{};
	static inline LinkStatus linkStatus{};
	static inline LinkUpdateError linkUpdateError{LinkUpdateError::None};
	static inline modm::ethernet::MdioError mdioError{modm::ethernet::MdioError::None};
	static inline std::array<uint8_t, MaxFrameSize> tx{};
	static inline std::array<uint8_t, MaxFrameSize> rx{};
};

template <int Tag, int Variant = 0>
struct FakePhy
{
	enum class InitializationError { None, Failed };

	struct InitializationResult
	{
		InitializationError error{InitializationError::None};
		modm::ethernet::MdioError mdioError{modm::ethernet::MdioError::None};
		explicit operator bool() const
		{
			return error == InitializationError::None and
					mdioError == modm::ethernet::MdioError::None;
		}
	};

	struct LinkStatusResult
	{
		modm::ethernet::LinkStatus status{};
		modm::ethernet::MdioError error{modm::ethernet::MdioError::None};
		explicit operator bool() const { return error == modm::ethernet::MdioError::None; }
	};

	static void reset()
	{
		initializationError = InitializationError::None;
		initializationMdioError = modm::ethernet::MdioError::None;
		linkMdioError = modm::ethernet::MdioError::None;
		initializeCalls = 0;
		linkReadCalls = 0;
	}

	template <class Mac>
	static InitializationResult initialize()
	{
		initializeCalls++;
		return {initializationError, initializationMdioError};
	}

	template <class Mac>
	static LinkStatusResult readLinkStatus()
	{
		linkReadCalls++;
		return {FakeMac<Tag>::linkStatus, linkMdioError};
	}

	static inline InitializationError initializationError{InitializationError::None};
	static inline modm::ethernet::MdioError initializationMdioError{
			modm::ethernet::MdioError::None};
	static inline modm::ethernet::MdioError linkMdioError{
			modm::ethernet::MdioError::None};
	static inline unsigned initializeCalls{0};
	static inline unsigned linkReadCalls{0};
};

template <int Tag>
void
resetFakeEthernet()
{
	FakeMac<Tag>::reset();
	FakePhy<Tag>::reset();
}

constexpr modm::lwip::StaticIPv4Configuration ConfigA{
	.macAddress = {0x02, 0, 0, 0, 0, 1},
	.ipAddress = {{10, 0, 0, 2}},
	.netmask = {{255, 255, 255, 0}},
	.gateway = {{10, 0, 0, 1}},
};

constexpr modm::lwip::StaticIPv4Configuration ConfigB{
	.macAddress = {0x02, 0, 0, 0, 0, 2},
	.ipAddress = {{10, 0, 1, 2}},
	.netmask = {{255, 255, 255, 0}},
	.gateway = {{10, 0, 1, 1}},
};

std::array<uint8_t, 64> capturedInput{};
std::size_t capturedInputLength{0};
err_t inputResult{ERR_OK};
bool timerExpired{false};
bool checksumCheckingObserved{false};
bool icmpChecksumCheckingObserved{false};
unsigned udpDeliveryCount{0};
std::array<uint8_t, 32> udpPayload{};
std::size_t udpPayloadLength{0};

void
write16(uint8_t* data, uint16_t value)
{
	data[0] = uint8_t(value >> 8);
	data[1] = uint8_t(value);
}

ip4_addr_t
makeIp4(modm::lwip::IPv4Address address)
{
	ip4_addr_t ip;
	IP4_ADDR(&ip, address.bytes[0], address.bytes[1], address.bytes[2], address.bytes[3]);
	return ip;
}

uint16_t
udpChecksum(std::span<const uint8_t> datagram, ip4_addr_t const& source,
		ip4_addr_t const& destination)
{
	modm::lwip::LwIPSingleThreadGuard guard;
	auto* p = pbuf_alloc(PBUF_RAW, datagram.size(), PBUF_RAM);
	if (p == nullptr)
		return 0;
	pbuf_take(p, datagram.data(), datagram.size());
	const uint16_t result = inet_chksum_pseudo(p, IP_PROTO_UDP, datagram.size(),
			&source, &destination);
	pbuf_free(p);
	return result;
}

std::size_t
makeUdpFrame(std::array<uint8_t, FakeMac<9>::MaxFrameSize>& frame,
		std::span<const uint8_t> payload, uint16_t identification, bool zeroChecksum=false)
{
	constexpr std::array<uint8_t, 6> SourceMac{{0x02, 0, 0, 0, 0, 0x55}};
	std::copy(ConfigA.macAddress.begin(), ConfigA.macAddress.end(), frame.begin());
	std::copy(SourceMac.begin(), SourceMac.end(), frame.begin() + 6);
	write16(frame.data() + 12, 0x0800);
	auto* ip = frame.data() + 14;
	const uint16_t udpLength = uint16_t(8 + payload.size());
	const uint16_t ipLength = uint16_t(20 + udpLength);
	std::fill(ip, ip + ipLength, 0);
	ip[0] = 0x45;
	write16(ip + 2, ipLength);
	write16(ip + 4, identification);
	ip[8] = 64;
	ip[9] = IP_PROTO_UDP;
	std::copy(ConfigA.gateway.bytes.begin(), ConfigA.gateway.bytes.end(), ip + 12);
	std::copy(ConfigA.ipAddress.bytes.begin(), ConfigA.ipAddress.bytes.end(), ip + 16);
	auto* udp = ip + 20;
	write16(udp, 1234);
	write16(udp + 2, 4321);
	write16(udp + 4, udpLength);
	std::copy(payload.begin(), payload.end(), udp + 8);
	const auto source = makeIp4(ConfigA.gateway);
	const auto destination = makeIp4(ConfigA.ipAddress);
	if (not zeroChecksum) {
		const uint16_t checksum = udpChecksum({udp, udpLength}, source, destination);
		std::memcpy(udp + 6, &checksum, sizeof(checksum));
	}
	const uint16_t ipChecksum = inet_chksum(ip, 20);
	std::memcpy(ip + 10, &ipChecksum, sizeof(ipChecksum));
	return 14 + ipLength;
}

void
receiveUdp(void*, struct udp_pcb*, struct pbuf* p, const ip_addr_t*, uint16_t)
{
	LWIP_ASSERT_CORE_LOCKED();
	udpDeliveryCount++;
	udpPayloadLength = std::min<std::size_t>(p->tot_len, udpPayload.size());
	pbuf_copy_partial(p, udpPayload.data(), udpPayloadLength, 0);
	pbuf_free(p);
}

bool
checksumEnabled(struct netif const* netif, uint16_t flag)
{
#if LWIP_CHECKSUM_CTRL_PER_NETIF
	return (netif->chksum_flags & flag) != 0;
#else
	(void) netif;
	(void) flag;
	return CHECKSUM_CHECK_IP and CHECKSUM_CHECK_UDP and
			CHECKSUM_CHECK_TCP and CHECKSUM_CHECK_ICMP;
#endif
}

err_t
captureInput(struct pbuf* p, struct netif*)
{
	LWIP_ASSERT_CORE_LOCKED();
	capturedInputLength = p->tot_len;
	pbuf_copy_partial(p, capturedInput.data(), p->tot_len, 0);
	if (inputResult == ERR_OK)
		pbuf_free(p);
	return inputResult;
}

err_t
captureChecksumState(struct pbuf* p, struct netif* netif)
{
	LWIP_ASSERT_CORE_LOCKED();
#if LWIP_CHECKSUM_CTRL_PER_NETIF
	checksumCheckingObserved = checksumEnabled(netif, NETIF_CHECKSUM_CHECK_IP) and
			checksumEnabled(netif, NETIF_CHECKSUM_CHECK_UDP) and
			checksumEnabled(netif, NETIF_CHECKSUM_CHECK_TCP);
	icmpChecksumCheckingObserved = checksumEnabled(netif, NETIF_CHECKSUM_CHECK_ICMP);
#else
	(void) netif;
	checksumCheckingObserved = CHECKSUM_CHECK_IP and CHECKSUM_CHECK_UDP and CHECKSUM_CHECK_TCP;
	icmpChecksumCheckingObserved = CHECKSUM_CHECK_ICMP;
#endif
	pbuf_free(p);
	return ERR_OK;
}

void
expireTimer(void*)
{
	LWIP_ASSERT_CORE_LOCKED();
	timerExpired = true;
}

std::size_t
netifCount()
{
	std::size_t count = 0;
	for (auto* item = netif_list; item != nullptr; item = item->next)
		count++;
	return count;
}

bool
containsNetif(struct netif* expected)
{
	for (auto* item = netif_list; item != nullptr; item = item->next) {
		if (item == expected)
			return true;
	}
	return false;
}

template <int Tag>
using Interface = modm::lwip::LwipEthernet<FakeMac<Tag>, FakePhy<Tag>>;

} // namespace

void
LwipEthernetTest::testInitializationRequired()
{
	using TestPort = FakeMac<13>;
	using TestInterface = Interface<13>;
	resetFakeEthernet<13>();

	const auto result = TestInterface::initialize<TestClock>(ConfigA);
	TEST_ASSERT_TRUE(bool(result));
	TEST_ASSERT_EQUALS(TestPort::initializeCalls, 1u);
	TEST_ASSERT_EQUALS(FakePhy<13>::initializeCalls, 1u);
	TEST_ASSERT_EQUALS(FakePhy<13>::linkReadCalls, 1u);
}

void
LwipEthernetTest::testInitializationAndRouting()
{
	modm::lwip::initialize();
	resetFakeEthernet<0>();
	resetFakeEthernet<1>();
	const auto initialCount = netifCount();

	TEST_ASSERT_TRUE(bool(Interface<0>::initialize<TestClock>(ConfigA)));
	TEST_ASSERT_TRUE(bool(Interface<1>::initialize<TestClock>(ConfigB)));
	TEST_ASSERT_EQUALS(netifCount(), initialCount + 2);
	TEST_ASSERT_TRUE(containsNetif(Interface<0>::netif()));
	TEST_ASSERT_TRUE(containsNetif(Interface<1>::netif()));
	TEST_ASSERT_EQUALS(FakeMac<0>::initializeCalls, 1u);
	TEST_ASSERT_EQUALS(FakeMac<1>::initializeCalls, 1u);
	TEST_ASSERT_TRUE(FakeMac<0>::configuration.macAddress == ConfigA.macAddress);
	const auto expectedChecksumMode = MODM_LWIP_CHECKSUM_HARDWARE ?
			FakeMac<0>::ChecksumMode::Hardware : FakeMac<0>::ChecksumMode::Software;
	TEST_ASSERT_TRUE(FakeMac<0>::configuration.checksumMode == expectedChecksumMode);
	TEST_ASSERT_EQUALS(Interface<0>::netif()->mtu, 1500u);
	TEST_ASSERT_EQUALS(Interface<0>::netif()->name[0], 'e');
	TEST_ASSERT_EQUALS(Interface<0>::netif()->name[1], 'n');
	TEST_ASSERT_EQUALS(Interface<1>::netif()->name[0], 'e');
	TEST_ASSERT_EQUALS(Interface<1>::netif()->name[1], 'n');
	TEST_ASSERT_DIFFERS(Interface<0>::netif()->num, Interface<1>::netif()->num);
	TEST_ASSERT_TRUE(Interface<0>::netif()->output != nullptr);
	TEST_ASSERT_TRUE(Interface<0>::netif()->linkoutput != nullptr);
	TEST_ASSERT_TRUE((Interface<0>::netif()->flags & NETIF_FLAG_ETHARP) != 0);
	TEST_ASSERT_TRUE((Interface<0>::netif()->flags & NETIF_FLAG_ETHERNET) != 0);
#if MODM_LWIP_CHECKSUM_HARDWARE
	TEST_ASSERT_FALSE(checksumEnabled(Interface<0>::netif(), NETIF_CHECKSUM_GEN_IP));
	TEST_ASSERT_FALSE(checksumEnabled(Interface<0>::netif(), NETIF_CHECKSUM_GEN_ICMP6));
	TEST_ASSERT_FALSE(checksumEnabled(Interface<0>::netif(), NETIF_CHECKSUM_CHECK_ICMP6));
#endif

	TEST_ASSERT_TRUE(Interface<0>::setDefault());
	TEST_ASSERT_TRUE(netif_default == Interface<0>::netif());
	TEST_ASSERT_TRUE(Interface<1>::setDefault());
	TEST_ASSERT_TRUE(netif_default == Interface<1>::netif());

	TEST_ASSERT_TRUE(bool(Interface<0>::initialize<TestClock>(ConfigA)));
	TEST_ASSERT_EQUALS(FakeMac<0>::initializeCalls, 2u);
	TEST_ASSERT_EQUALS(netifCount(), initialCount + 2);
}

void
LwipEthernetTest::testPhysicalMacBinding()
{
	using Mac = FakeMac<11>;
	using FirstPhy = FakePhy<11, 0>;
	using SecondPhy = FakePhy<11, 1>;
	using FirstInterface = modm::lwip::LwipEthernet<Mac, FirstPhy>;
	using SecondInterface = modm::lwip::LwipEthernet<Mac, SecondPhy>;
	Mac::reset();
	FirstPhy::reset();
	SecondPhy::reset();

	TEST_ASSERT_TRUE(bool(FirstInterface::initialize<TestClock>(ConfigA)));
	TEST_ASSERT_TRUE(bool(FirstInterface::initialize<TestClock>(ConfigB)));
	const auto result = SecondInterface::initialize<TestClock>(ConfigA);
	TEST_ASSERT_FALSE(bool(result));
	TEST_ASSERT_TRUE(result.adapterError ==
			SecondInterface::AdapterInitializationError::MacAlreadyBound);
	TEST_ASSERT_EQUALS(Mac::initializeCalls, 2u);
	TEST_ASSERT_TRUE(FirstInterface::netif() == SecondInterface::netif());
}

void
LwipEthernetTest::testFailedBindingCanBeReplaced()
{
	using Mac = FakeMac<14>;
	using FailedPhy = FakePhy<14, 0>;
	using ReplacementPhy = FakePhy<14, 1>;
	using FailedInterface = modm::lwip::LwipEthernet<Mac, FailedPhy>;
	using ReplacementInterface = modm::lwip::LwipEthernet<Mac, ReplacementPhy>;
	Mac::reset();
	FailedPhy::reset();
	ReplacementPhy::reset();
	Mac::initializeError = Mac::InitializationError::Failed;
	const auto initialCount = netifCount();

	const auto failed = FailedInterface::initialize<TestClock>(ConfigA);
	TEST_ASSERT_FALSE(bool(failed));
	TEST_ASSERT_TRUE(failed.macError == Mac::InitializationError::Failed);
	TEST_ASSERT_EQUALS(Mac::initializeCalls, 1u);

	Mac::initializeError = Mac::InitializationError::None;
	const auto replacement = ReplacementInterface::initialize<TestClock>(ConfigB);
	TEST_ASSERT_TRUE(bool(replacement));
	TEST_ASSERT_EQUALS(Mac::initializeCalls, 2u);
	TEST_ASSERT_EQUALS(netifCount(), initialCount + 1);
	TEST_ASSERT_TRUE(containsNetif(ReplacementInterface::netif()));
}

void
LwipEthernetTest::testReinitializationAndRecovery()
{
	using Mac = FakeMac<8>;
	using TestInterface = Interface<8>;
	resetFakeEthernet<8>();
	Mac::linkStatus = {
		Mac::LinkState::Up,
		Mac::LinkMode{Mac::Speed::Speed100M, Mac::DuplexMode::Full},
	};
	const auto initialCount = netifCount();
	TEST_ASSERT_TRUE(bool(TestInterface::initialize<TestClock>(ConfigA)));
	auto* const registeredInterface = TestInterface::netif();
	TEST_ASSERT_EQUALS(netifCount(), initialCount + 1);
	TEST_ASSERT_TRUE(netif_is_link_up(registeredInterface));

	Mac::initializeError = Mac::InitializationError::Failed;
	const auto failure = TestInterface::initialize<TestClock>(ConfigB);
	TEST_ASSERT_FALSE(bool(failure));
	TEST_ASSERT_TRUE(failure.macError == Mac::InitializationError::Failed);
	TEST_ASSERT_TRUE(TestInterface::netif() == registeredInterface);
	TEST_ASSERT_EQUALS(netifCount(), initialCount + 1);
	TEST_ASSERT_FALSE(netif_is_up(registeredInterface));
	TEST_ASSERT_FALSE(netif_is_link_up(registeredInterface));

	Mac::initializeError = Mac::InitializationError::None;
	TEST_ASSERT_TRUE(bool(TestInterface::initialize<TestClock>(ConfigB)));
	TEST_ASSERT_EQUALS(Mac::initializeCalls, 3u);
	TEST_ASSERT_TRUE(TestInterface::netif() == registeredInterface);
	TEST_ASSERT_EQUALS(netifCount(), initialCount + 1);
	TEST_ASSERT_TRUE(netif_is_link_up(registeredInterface));
	TEST_ASSERT_TRUE(std::equal(ConfigB.macAddress.begin(), ConfigB.macAddress.end(),
			registeredInterface->hwaddr));
	const auto expectedAddress = makeIp4(ConfigB.ipAddress);
	const auto expectedNetmask = makeIp4(ConfigB.netmask);
	const auto expectedGateway = makeIp4(ConfigB.gateway);
	TEST_ASSERT_TRUE(ip4_addr_cmp(netif_ip4_addr(registeredInterface), &expectedAddress));
	TEST_ASSERT_TRUE(ip4_addr_cmp(netif_ip4_netmask(registeredInterface), &expectedNetmask));
	TEST_ASSERT_TRUE(ip4_addr_cmp(netif_ip4_gw(registeredInterface), &expectedGateway));
	const auto expectedChecksumMode = MODM_LWIP_CHECKSUM_HARDWARE ?
			Mac::ChecksumMode::Hardware : Mac::ChecksumMode::Software;
	TEST_ASSERT_TRUE(Mac::configuration.checksumMode == expectedChecksumMode);
}

void
LwipEthernetTest::testConfigurationAndLeaseContract()
{
	using Mac = FakeMac<6>;
	resetFakeEthernet<6>();

	{
		auto lease = Mac::acquireTransmitBuffer(32);
		TEST_ASSERT_TRUE(bool(lease));
		TEST_ASSERT_EQUALS(lease.buffer().size(), 32u);
		TEST_ASSERT_TRUE(Mac::acquireTransmitBuffer(32).error() == Mac::TransmitError::Busy);

		auto moved = std::move(lease);
		TEST_ASSERT_FALSE(bool(lease));
		TEST_ASSERT_TRUE(bool(moved));
		std::fill(moved.buffer().begin(), moved.buffer().end(), 0xa5);
		TEST_ASSERT_TRUE(bool(moved.commit()));
		TEST_ASSERT_EQUALS(Mac::commitCalls, 1u);
		TEST_ASSERT_EQUALS(Mac::committedLength, 32u);
	}
	TEST_ASSERT_EQUALS(Mac::cancelCalls, 0u);

	{
		auto lease = Mac::acquireTransmitBuffer(16);
		TEST_ASSERT_TRUE(bool(lease));
	}
	TEST_ASSERT_EQUALS(Mac::cancelCalls, 1u);
	{
		auto lease = Mac::acquireTransmitBuffer(16);
		TEST_ASSERT_TRUE(bool(lease));
	}
	TEST_ASSERT_EQUALS(Mac::cancelCalls, 2u);
	TEST_ASSERT_TRUE(Mac::acquireTransmitBuffer(0).error() == Mac::TransmitError::InvalidLength);
	TEST_ASSERT_TRUE(Mac::acquireTransmitBuffer(Mac::MaxFrameSize + 1).error() ==
			Mac::TransmitError::InvalidLength);

	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxLength = 24;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::Valid;
	{
		auto lease = Mac::tryAcquireReceiveBuffer();
		TEST_ASSERT_TRUE(bool(lease));
		TEST_ASSERT_EQUALS(lease.buffer().size(), 24u);
		TEST_ASSERT_TRUE(lease.checksumStatus() == Mac::ReceiveChecksumStatus::Valid);
		TEST_ASSERT_TRUE(Mac::tryAcquireReceiveBuffer().error() ==
				Mac::ReceiveError::NoFrameAvailable);
		auto moved = std::move(lease);
		TEST_ASSERT_FALSE(bool(lease));
		TEST_ASSERT_TRUE(bool(moved));
	}
	TEST_ASSERT_EQUALS(Mac::releaseCalls, 1u);
	TEST_ASSERT_TRUE(Mac::tryAcquireReceiveBuffer().error() ==
			Mac::ReceiveError::NoFrameAvailable);
}

void
LwipEthernetTest::testTransmitPath()
{
	resetFakeEthernet<2>();
	TEST_ASSERT_TRUE(bool(Interface<2>::initialize<TestClock>(ConfigA)));
	auto* netif = Interface<2>::netif();
	modm::lwip::LwIPSingleThreadGuard guard;

	const std::array<uint8_t, 3> first{{1, 2, 3}};
	const std::array<uint8_t, 4> second{{4, 5, 6, 7}};
	auto* head = pbuf_alloc(PBUF_RAW, first.size(), PBUF_RAM);
	auto* tail = pbuf_alloc(PBUF_RAW, second.size(), PBUF_RAM);
	TEST_ASSERT_TRUE(head != nullptr);
	TEST_ASSERT_TRUE(tail != nullptr);
	TEST_ASSERT_EQUALS(pbuf_take(head, first.data(), first.size()), ERR_OK);
	TEST_ASSERT_EQUALS(pbuf_take(tail, second.data(), second.size()), ERR_OK);
	pbuf_cat(head, tail);
	TEST_ASSERT_EQUALS(netif->linkoutput(netif, head), ERR_OK);
	TEST_ASSERT_EQUALS(FakeMac<2>::committedLength, 7u);
	const std::array<uint8_t, 7> expected{{1, 2, 3, 4, 5, 6, 7}};
	TEST_ASSERT_TRUE(std::equal(expected.begin(), expected.end(), FakeMac<2>::tx.begin()));
	pbuf_free(head);

	auto* packet = pbuf_alloc(PBUF_RAW, 64, PBUF_RAM);
	TEST_ASSERT_TRUE(packet != nullptr);
	constexpr std::array transmitErrorMappings{
		std::pair{FakeMac<2>::TransmitError::Busy, ERR_MEM},
		std::pair{FakeMac<2>::TransmitError::InvalidLength, ERR_BUF},
		std::pair{FakeMac<2>::TransmitError::LinkDown, ERR_IF},
		std::pair{FakeMac<2>::TransmitError::NotInitialized, ERR_IF},
		std::pair{FakeMac<2>::TransmitError::Faulted, ERR_IF},
		std::pair{FakeMac<2>::TransmitError::UnsupportedFragmentation, ERR_VAL},
	};
	for (const auto& [error, expected] : transmitErrorMappings) {
		FakeMac<2>::acquireError = error;
		TEST_ASSERT_EQUALS(netif->linkoutput(netif, packet), expected);
	}
	FakeMac<2>::acquireError = FakeMac<2>::TransmitError::None;
	FakeMac<2>::commitError = FakeMac<2>::TransmitError::Faulted;
	TEST_ASSERT_EQUALS(netif->linkoutput(netif, packet), ERR_IF);
	FakeMac<2>::commitError = FakeMac<2>::TransmitError::None;
	pbuf_free(packet);

	auto* oversized = pbuf_alloc(PBUF_RAW, FakeMac<2>::MaxFrameSize + 1, PBUF_RAM);
	TEST_ASSERT_TRUE(oversized != nullptr);
	TEST_ASSERT_EQUALS(netif->linkoutput(netif, oversized), ERR_BUF);
	pbuf_free(oversized);
}

void
LwipEthernetTest::testReceivePath()
{
	resetFakeEthernet<3>();
	TEST_ASSERT_TRUE(bool(Interface<3>::initialize<TestClock>(ConfigA)));
	Interface<3>::netif()->input = captureInput;
	for (std::size_t index = 0; index < 60; ++index)
		FakeMac<3>::rx[index] = uint8_t(index);
	FakeMac<3>::rxLength = 60;
	FakeMac<3>::rxReady = true;
	capturedInputLength = 0;
	inputResult = ERR_OK;

	TEST_ASSERT_EQUALS(Interface<3>::pollInput(FakeMac<3>::RxDescriptorCount), ERR_OK);
	TEST_ASSERT_EQUALS(FakeMac<3>::releaseCalls, 1u);
	TEST_ASSERT_EQUALS(capturedInputLength, 60u);
	TEST_ASSERT_TRUE(std::equal(capturedInput.begin(), capturedInput.begin() + 60,
			FakeMac<3>::rx.begin()));

	FakeMac<3>::rxReady = true;
	FakeMac<3>::rxFrames = 1;
	inputResult = ERR_IF;
	TEST_ASSERT_EQUALS(Interface<3>::pollInput(FakeMac<3>::RxDescriptorCount), ERR_IF);
	TEST_ASSERT_EQUALS(FakeMac<3>::releaseCalls, 2u);
	TEST_ASSERT_FALSE(FakeMac<3>::rxReady);

	inputResult = ERR_OK;
	FakeMac<3>::rxReady = true;
	FakeMac<3>::rxFrames = 20;
	TEST_ASSERT_EQUALS(Interface<3>::pollInput(FakeMac<3>::RxDescriptorCount), ERR_OK);
	TEST_ASSERT_EQUALS(FakeMac<3>::releaseCalls, 10u);
	TEST_ASSERT_EQUALS(FakeMac<3>::rxFrames, 12u);
	TEST_ASSERT_TRUE(FakeMac<3>::rxReady);
	TEST_ASSERT_EQUALS(Interface<3>::pollInput(16), ERR_OK);
	TEST_ASSERT_EQUALS(FakeMac<3>::releaseCalls, 22u);
	TEST_ASSERT_FALSE(FakeMac<3>::rxReady);

	const auto cumulative = Interface<3>::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(cumulative.acquiredFrames, 22u);
	TEST_ASSERT_EQUALS(cumulative.droppedFrames, 1u);
	TEST_ASSERT_EQUALS(cumulative.checksumDrops, 0u);
	TEST_ASSERT_EQUALS(cumulative.allocationDrops, 0u);
	TEST_ASSERT_TRUE(bool(Interface<3>::initialize<TestClock>(ConfigB)));
	const auto afterReinitialization = Interface<3>::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(afterReinitialization.acquiredFrames, cumulative.acquiredFrames);
	TEST_ASSERT_EQUALS(afterReinitialization.droppedFrames, cumulative.droppedFrames);
	TEST_ASSERT_EQUALS(afterReinitialization.checksumDrops, cumulative.checksumDrops);
	TEST_ASSERT_EQUALS(afterReinitialization.allocationDrops, cumulative.allocationDrops);
}

void
LwipEthernetTest::testReceiveStatisticsAndChecksums()
{
	using Mac = FakeMac<7>;
	using TestInterface = Interface<7>;
	resetFakeEthernet<7>();
	TEST_ASSERT_TRUE(bool(TestInterface::initialize<TestClock>(ConfigA)));
	TestInterface::netif()->input = captureChecksumState;
	Mac::rxLength = 60;

#if LINK_STATS
	const auto initialLinkReceive = lwip_stats.link.recv;
	const auto initialLinkDrop = lwip_stats.link.drop;
	const auto initialLinkMemoryError = lwip_stats.link.memerr;
	const auto initialLinkChecksumError = lwip_stats.link.chkerr;
	const auto initialLinkError = lwip_stats.link.err;
#endif

	const auto beforeZeroLimit = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(TestInterface::pollInput(0), ERR_OK);
	TEST_ASSERT_EQUALS(TestInterface::getCumulativeReceiveStatistics().acquiredFrames,
			beforeZeroLimit.acquiredFrames);

	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::NotChecked;
	checksumCheckingObserved = false;
	icmpChecksumCheckingObserved = false;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_OK);
	TEST_ASSERT_TRUE(checksumCheckingObserved);
	TEST_ASSERT_TRUE(icmpChecksumCheckingObserved);
	auto statistics = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(statistics.acquiredFrames, beforeZeroLimit.acquiredFrames + 1);
	TEST_ASSERT_EQUALS(statistics.droppedFrames, beforeZeroLimit.droppedFrames);
	TEST_ASSERT_EQUALS(statistics.checksumDrops, beforeZeroLimit.checksumDrops);
	TEST_ASSERT_EQUALS(statistics.allocationDrops, beforeZeroLimit.allocationDrops);
#if LINK_STATS
	TEST_ASSERT_EQUALS(lwip_stats.link.recv, initialLinkReceive + 1);
#endif

	const auto beforeSecondFrame = TestInterface::getCumulativeReceiveStatistics();
	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::NotChecked;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_OK);
	const auto afterSecondFrame = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(afterSecondFrame.acquiredFrames, beforeSecondFrame.acquiredFrames + 1);

	const auto beforeReceiveError = TestInterface::getCumulativeReceiveStatistics();
	Mac::receiveError = Mac::ReceiveError::Faulted;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_IF);
	const auto afterReceiveError = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(afterReceiveError.acquiredFrames, beforeReceiveError.acquiredFrames);
	TEST_ASSERT_EQUALS(afterReceiveError.droppedFrames, beforeReceiveError.droppedFrames);
#if LINK_STATS
	TEST_ASSERT_EQUALS(lwip_stats.link.err, initialLinkError + 1);
#endif
	Mac::receiveError = Mac::ReceiveError::None;

#if MODM_LWIP_CHECKSUM_HARDWARE
	TEST_ASSERT_FALSE(checksumEnabled(TestInterface::netif(), NETIF_CHECKSUM_CHECK_IP));
	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::Valid;
	checksumCheckingObserved = true;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_OK);
	TEST_ASSERT_FALSE(checksumCheckingObserved);

	const auto beforeInvalidChecksum = TestInterface::getCumulativeReceiveStatistics();
	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::Invalid;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_OK);
	statistics = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(statistics.acquiredFrames, beforeInvalidChecksum.acquiredFrames + 1);
	TEST_ASSERT_EQUALS(statistics.droppedFrames, beforeInvalidChecksum.droppedFrames + 1);
	TEST_ASSERT_EQUALS(statistics.checksumDrops, beforeInvalidChecksum.checksumDrops + 1);
#if LINK_STATS
	TEST_ASSERT_EQUALS(lwip_stats.link.drop, initialLinkDrop + 1);
	TEST_ASSERT_EQUALS(lwip_stats.link.chkerr, initialLinkChecksumError + 1);
#endif
#else
#if LWIP_CHECKSUM_CTRL_PER_NETIF
	TEST_ASSERT_TRUE(checksumEnabled(TestInterface::netif(), NETIF_CHECKSUM_CHECK_IP));
#else
	TEST_ASSERT_EQUALS(CHECKSUM_CHECK_IP, 1);
#endif
	const auto beforeInvalidChecksum = TestInterface::getCumulativeReceiveStatistics();
	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::Invalid;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_OK);
	statistics = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(statistics.acquiredFrames, beforeInvalidChecksum.acquiredFrames + 1);
	TEST_ASSERT_EQUALS(statistics.droppedFrames, beforeInvalidChecksum.droppedFrames + 1);
	TEST_ASSERT_EQUALS(statistics.checksumDrops, beforeInvalidChecksum.checksumDrops + 1);
#if LINK_STATS
	TEST_ASSERT_EQUALS(lwip_stats.link.chkerr, initialLinkChecksumError + 1);
#endif
#endif

	const auto beforeInputError = TestInterface::getCumulativeReceiveStatistics();
	TestInterface::netif()->input = captureInput;
	inputResult = ERR_IF;
	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::NotChecked;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_IF);
	statistics = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(statistics.acquiredFrames, beforeInputError.acquiredFrames + 1);
	TEST_ASSERT_EQUALS(statistics.droppedFrames, beforeInputError.droppedFrames + 1);
#if LINK_STATS
	TEST_ASSERT_EQUALS(lwip_stats.link.err, initialLinkError + 2);
#endif
	inputResult = ERR_OK;
	TestInterface::netif()->input = captureChecksumState;

	std::array<struct pbuf*, PBUF_POOL_SIZE + 1> heldBuffers{};
	std::size_t heldCount = 0;
	{
		modm::lwip::LwIPSingleThreadGuard guard;
		while (heldCount < heldBuffers.size()) {
			auto* held = pbuf_alloc(PBUF_RAW, 1, PBUF_POOL);
			if (held == nullptr)
				break;
			heldBuffers[heldCount++] = held;
		}
		TEST_ASSERT_TRUE(heldCount != 0);
		auto* exhaustionCheck = pbuf_alloc(PBUF_RAW, 1, PBUF_POOL);
		TEST_ASSERT_TRUE(exhaustionCheck == nullptr);
		if (exhaustionCheck != nullptr)
			pbuf_free(exhaustionCheck);
	}
	Mac::rxReady = true;
	Mac::rxFrames = 1;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::NotChecked;
	const auto beforeAllocationDrop = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_MEM);
	statistics = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(statistics.acquiredFrames, beforeAllocationDrop.acquiredFrames + 1);
	TEST_ASSERT_EQUALS(statistics.droppedFrames, beforeAllocationDrop.droppedFrames + 1);
	TEST_ASSERT_EQUALS(statistics.allocationDrops, beforeAllocationDrop.allocationDrops + 1);
#if LINK_STATS
	TEST_ASSERT_EQUALS(lwip_stats.link.drop, initialLinkDrop + 3);
	TEST_ASSERT_EQUALS(lwip_stats.link.memerr, initialLinkMemoryError + 1);
#endif
	{
		modm::lwip::LwIPSingleThreadGuard guard;
		for (std::size_t index = 0; index < heldCount; ++index)
			pbuf_free(heldBuffers[index]);
	}

	const auto beforeLimit = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_TRUE(beforeLimit.droppedFrames >=
			beforeLimit.checksumDrops + beforeLimit.allocationDrops);
	Mac::rxReady = true;
	Mac::rxFrames = Mac::RxDescriptorCount + 3;
	Mac::rxChecksum = Mac::ReceiveChecksumStatus::NotChecked;
	TEST_ASSERT_EQUALS(TestInterface::pollInput(Mac::RxDescriptorCount), ERR_OK);
	TEST_ASSERT_EQUALS(Mac::rxFrames, 3u);
	const auto afterLimit = TestInterface::getCumulativeReceiveStatistics();
	TEST_ASSERT_EQUALS(afterLimit.acquiredFrames,
			beforeLimit.acquiredFrames + Mac::RxDescriptorCount);
}

void
LwipEthernetTest::testRealUdpChecksumAndReassembly()
{
	using Mac = FakeMac<9>;
	using TestInterface = Interface<9>;
	resetFakeEthernet<9>();
	Mac::linkStatus = {
		Mac::LinkState::Up,
		Mac::LinkMode{Mac::Speed::Speed100M, Mac::DuplexMode::Full},
	};
	TEST_ASSERT_TRUE(bool(TestInterface::initialize<TestClock>(ConfigA)));
	struct udp_pcb* pcb = nullptr;
	{
		modm::lwip::LwIPSingleThreadGuard guard;
		pcb = udp_new();
		TEST_ASSERT_TRUE(pcb != nullptr);
		TEST_ASSERT_EQUALS(udp_bind(pcb, IP_ADDR_ANY, 4321), ERR_OK);
		udp_recv(pcb, receiveUdp, nullptr);
	}

	auto deliver = [](std::span<const uint8_t> frame) {
		std::copy(frame.begin(), frame.end(), Mac::rx.begin());
		Mac::rxLength = frame.size();
		Mac::rxFrames = 1;
		Mac::rxReady = true;
		Mac::rxChecksum = Mac::ReceiveChecksumStatus::NotChecked;
		return TestInterface::pollInput(Mac::RxDescriptorCount);
	};

	std::array<uint8_t, Mac::MaxFrameSize> frame{};
	constexpr std::array<uint8_t, 5> OddPayload{{1, 2, 3, 4, 5}};
	udpDeliveryCount = 0;
	auto length = makeUdpFrame(frame, OddPayload, 0x100);
	TEST_ASSERT_EQUALS(deliver({frame.data(), length}), ERR_OK);
	TEST_ASSERT_EQUALS(udpDeliveryCount, 1u);
	TEST_ASSERT_EQUALS(udpPayloadLength, OddPayload.size());
	TEST_ASSERT_TRUE(std::equal(OddPayload.begin(), OddPayload.end(), udpPayload.begin()));

	length = makeUdpFrame(frame, OddPayload, 0x101);
	frame[42] ^= 0x80;
	TEST_ASSERT_EQUALS(deliver({frame.data(), length}), ERR_OK);
	TEST_ASSERT_EQUALS(udpDeliveryCount, 1u);

	length = makeUdpFrame(frame, OddPayload, 0x102, true);
	TEST_ASSERT_EQUALS(deliver({frame.data(), length}), ERR_OK);
	TEST_ASSERT_EQUALS(udpDeliveryCount, 2u);

	std::array<uint8_t, 17> fragmentedPayload{};
	for (std::size_t index = 0; index < fragmentedPayload.size(); ++index)
		fragmentedPayload[index] = uint8_t(0x30 + index);
	std::array<uint8_t, Mac::MaxFrameSize> complete{};
	makeUdpFrame(complete, fragmentedPayload, 0x103);
	auto makeFragment = [&](std::size_t offset, std::size_t fragmentLength, uint16_t fragmentField) {
		std::array<uint8_t, Mac::MaxFrameSize> fragment{};
		std::copy_n(complete.begin(), 14, fragment.begin());
		auto* ip = fragment.data() + 14;
		std::copy_n(complete.data() + 14, 20, ip);
		write16(ip + 2, uint16_t(20 + fragmentLength));
		write16(ip + 6, fragmentField);
		write16(ip + 10, 0);
		std::copy_n(complete.data() + 34 + offset, fragmentLength, ip + 20);
		const uint16_t checksum = inet_chksum(ip, 20);
		std::memcpy(ip + 10, &checksum, sizeof(checksum));
		return std::pair{fragment, 14 + 20 + fragmentLength};
	};
	auto first = makeFragment(0, 16, 0x2000);
	auto second = makeFragment(16, 9, 2);
	TEST_ASSERT_EQUALS(deliver({first.first.data(), first.second}), ERR_OK);
	TEST_ASSERT_EQUALS(udpDeliveryCount, 2u);
	TEST_ASSERT_EQUALS(deliver({second.first.data(), second.second}), ERR_OK);
	TEST_ASSERT_EQUALS(udpDeliveryCount, 3u);
	TEST_ASSERT_EQUALS(udpPayloadLength, fragmentedPayload.size());
	TEST_ASSERT_TRUE(std::equal(fragmentedPayload.begin(), fragmentedPayload.end(), udpPayload.begin()));

	{
		modm::lwip::LwIPSingleThreadGuard guard;
		udp_remove(pcb);
	}
}

void
LwipEthernetTest::testOutgoingSoftwareFragmentation()
{
#if IP_FRAG
	using Mac = FakeMac<10>;
	using TestInterface = Interface<10>;
	resetFakeEthernet<10>();
	Mac::linkStatus = {
		Mac::LinkState::Up,
		Mac::LinkMode{Mac::Speed::Speed100M, Mac::DuplexMode::Full},
	};
	TEST_ASSERT_TRUE(bool(TestInterface::initialize<TestClock>(ConfigA)));
	Mac::committedFrameCount = 0;
	Mac::committedLengths.fill(0);
	std::array<uint8_t, 2001> payload{};
	for (std::size_t index = 0; index < payload.size(); ++index)
		payload[index] = uint8_t((index * 17 + 3) % 251);

	{
		modm::lwip::LwIPSingleThreadGuard guard;
		auto* pcb = udp_new();
		TEST_ASSERT_TRUE(pcb != nullptr);
		ip_set_option(pcb, SOF_BROADCAST);
		auto* p = pbuf_alloc(PBUF_TRANSPORT, payload.size(), PBUF_RAM);
		TEST_ASSERT_TRUE(p != nullptr);
		TEST_ASSERT_EQUALS(pbuf_take(p, payload.data(), payload.size()), ERR_OK);
		ip_addr_t destination;
		IP_ADDR4(&destination, 10, 0, 0, 255);
		TEST_ASSERT_EQUALS(udp_sendto_if(pcb, p, &destination, 5555,
				TestInterface::netif()), ERR_OK);
		pbuf_free(p);
		udp_remove(pcb);
	}

	TEST_ASSERT_EQUALS(Mac::committedFrameCount, 2u);
	std::array<uint8_t, 8 + payload.size()> datagram{};
	auto read16 = [](const uint8_t* data) {
		return uint16_t((uint16_t(data[0]) << 8) | data[1]);
	};
	std::size_t reconstructedLength = 0;
	uint16_t identification = 0;
	bool sawFirst = false;
	bool sawLast = false;
	for (std::size_t index = 0; index < Mac::committedFrameCount; ++index) {
		const auto& captured = Mac::committedFrames[index];
		const auto frameLength = Mac::committedLengths[index];
		TEST_ASSERT_TRUE(frameLength <= Mac::MaxFrameSize);
		if (frameLength < 34 or frameLength > Mac::MaxFrameSize) {
			TEST_ASSERT_TRUE(false);
			continue;
		}
		TEST_ASSERT_EQUALS(read16(captured.data() + 12), 0x0800u);
		const auto* ip = captured.data() + 14;
		const std::size_t headerLength = std::size_t(ip[0] & 0x0f) * 4;
		TEST_ASSERT_EQUALS(headerLength, 20u);
		TEST_ASSERT_EQUALS(inet_chksum(ip, headerLength), 0u);
		const uint16_t totalLength = read16(ip + 2);
		TEST_ASSERT_EQUALS(frameLength, std::size_t(14 + totalLength));
		if (headerLength > totalLength or totalLength > frameLength - 14) {
			TEST_ASSERT_TRUE(false);
			continue;
		}
		const uint16_t fragment = read16(ip + 6);
		const std::size_t offset = std::size_t(fragment & 0x1fff) * 8;
		const std::size_t fragmentLength = totalLength - headerLength;
		TEST_ASSERT_TRUE(offset + fragmentLength <= datagram.size());
		if (offset > datagram.size() or fragmentLength > datagram.size() - offset) {
			TEST_ASSERT_TRUE(false);
			continue;
		}
		std::copy_n(ip + headerLength, fragmentLength, datagram.begin() + offset);
		reconstructedLength += fragmentLength;
		if (offset == 0) {
			sawFirst = true;
			identification = read16(ip + 4);
			TEST_ASSERT_TRUE((fragment & 0x2000) != 0);
		}
		else {
			sawLast = true;
			TEST_ASSERT_EQUALS(read16(ip + 4), identification);
			TEST_ASSERT_TRUE((fragment & 0x2000) == 0);
		}
	}
	TEST_ASSERT_TRUE(sawFirst);
	TEST_ASSERT_TRUE(sawLast);
	TEST_ASSERT_EQUALS(reconstructedLength, datagram.size());
	TEST_ASSERT_EQUALS(read16(datagram.data() + 2), 5555u);
	TEST_ASSERT_EQUALS(read16(datagram.data() + 4), datagram.size());
	TEST_ASSERT_TRUE(read16(datagram.data() + 6) != 0);
	TEST_ASSERT_TRUE(std::equal(payload.begin(), payload.end(), datagram.begin() + 8));
	const auto source = makeIp4(ConfigA.ipAddress);
	const modm::lwip::IPv4Address broadcast{{10, 0, 0, 255}};
	const auto destination = makeIp4(broadcast);
	TEST_ASSERT_EQUALS(udpChecksum(datagram, source, destination), 0u);
#else
	TEST_ASSERT_EQUALS(IP_FRAG, 0);
#endif
}

void
LwipEthernetTest::testLinkPollingAndTimers()
{
	resetFakeEthernet<4>();
	TEST_ASSERT_TRUE(bool(Interface<4>::initialize<TestClock>(ConfigA)));
	TEST_ASSERT_EQUALS(FakePhy<4>::linkReadCalls, 1u);
	TEST_ASSERT_EQUALS(FakeMac<4>::linkUpdateCalls, 1u);
	TEST_ASSERT_FALSE(netif_is_link_up(Interface<4>::netif()));

	FakeMac<4>::linkStatus = {
		FakeMac<4>::LinkState::Up,
		FakeMac<4>::LinkMode{FakeMac<4>::Speed::Speed100M, FakeMac<4>::DuplexMode::Full},
	};
	TEST_ASSERT_TRUE(bool(Interface<4>::pollLink()));
	TEST_ASSERT_EQUALS(FakePhy<4>::linkReadCalls, 2u);
	TEST_ASSERT_EQUALS(FakeMac<4>::linkUpdateCalls, 2u);
	TEST_ASSERT_TRUE(netif_is_link_up(Interface<4>::netif()));
	FakeMac<4>::linkStatus = {};
	FakePhy<4>::linkMdioError = modm::ethernet::MdioError::Timeout;
	const auto errorResult = Interface<4>::pollLink();
	TEST_ASSERT_EQUALS(FakePhy<4>::linkReadCalls, 3u);
	TEST_ASSERT_EQUALS(FakeMac<4>::linkUpdateCalls, 2u);
	TEST_ASSERT_TRUE(errorResult.phyError == modm::ethernet::MdioError::Timeout);
	TEST_ASSERT_TRUE(netif_is_link_up(Interface<4>::netif()));
	FakePhy<4>::linkMdioError = modm::ethernet::MdioError::None;
	TEST_ASSERT_TRUE(bool(Interface<4>::pollLink()));
	TEST_ASSERT_EQUALS(FakePhy<4>::linkReadCalls, 4u);
	TEST_ASSERT_EQUALS(FakeMac<4>::linkUpdateCalls, 3u);
	TEST_ASSERT_FALSE(netif_is_link_up(Interface<4>::netif()));

	timerExpired = false;
	{
		modm::lwip::LwIPSingleThreadGuard guard;
		sys_timeout(0, expireTimer, nullptr);
	}
	modm::lwip::processTimeouts();
	TEST_ASSERT_TRUE(timerExpired);
}

void
LwipEthernetTest::testInitializationFailure()
{
	resetFakeEthernet<5>();
	FakeMac<5>::initializeError = FakeMac<5>::InitializationError::Failed;
	const auto initialCount = netifCount();
	const auto result = Interface<5>::initialize<TestClock>(ConfigA);
	TEST_ASSERT_FALSE(bool(result));
	TEST_ASSERT_TRUE(result.macError == FakeMac<5>::InitializationError::Failed);
	TEST_ASSERT_TRUE(result.adapterError == Interface<5>::AdapterInitializationError::None);
	TEST_ASSERT_EQUALS(FakeMac<5>::initializeCalls, 1u);
	TEST_ASSERT_EQUALS(netifCount(), initialCount);
	TEST_ASSERT_FALSE(containsNetif(Interface<5>::netif()));
}

void
LwipEthernetTest::testSingleThreadGuard()
{
	static_assert(not std::is_copy_constructible_v<modm::lwip::LwIPSingleThreadGuard>);
	static_assert(not std::is_move_constructible_v<modm::lwip::LwIPSingleThreadGuard>);
	modm::lwip::LwIPSingleThreadGuard guard;
	LWIP_ASSERT_CORE_LOCKED();
#if defined(__unix__)
	const auto child = fork();
	if (child == 0) {
		modm::lwip::LwIPSingleThreadGuard nested;
		_exit(0);
	}
	int status = 0;
	TEST_ASSERT_TRUE(child > 0);
	TEST_ASSERT_EQUALS(waitpid(child, &status, 0), child);
	TEST_ASSERT_TRUE(WIFSIGNALED(status));
	TEST_ASSERT_EQUALS(WTERMSIG(status), SIGABRT);
#endif
	TEST_ASSERT_TRUE(true);
}
