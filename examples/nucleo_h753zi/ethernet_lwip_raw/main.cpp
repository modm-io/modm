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

/*
 * Provides static IP ping and UDP/TCP echo on port 5001.
 * The locally administered MAC address is derived from the STM32 unique ID.
 * IPv4, UDP, and TCP checksums use hardware offload.
 * Outgoing IPv4 fragmentation is disabled because fragmented transmit traffic
 * requires software checksum handling. Incoming reassembly remains enabled.
 * The network fiber polls for received frames, link state, and lwIP timeouts.
 * Test the echo directly after flashing:
 *   ping 10.66.0.42
 *   echo hello | ncat --udp --idle-timeout 1s 10.66.0.42 5001
 *   echo hello | ncat 10.66.0.42 5001
 *
 * Optionally run the complete host checks from this directory:
 *   python ../../host/ethernet_echo.py 10.66.0.42
 * Add --stress for large, concurrent, reset, and reconnect tests.
 * Add --throughput for a validated 32 MiB TCP echo measurement.
 */

#include <modm/board.hpp>
#include <modm/driver/ethernet/lan8742a.hpp>
#include <modm/processing.hpp>

#include <modm/communication/lwip_ethernet/lwip_ethernet.hpp>

#include <lwip/tcp.h>
#include <lwip/udp.h>

#include <array>

using namespace Board;
using namespace std::chrono_literals;

// ----------------------------------------------------------------------------
// Common Ethernet configuration

namespace Ethernet
{
static constexpr uint16_t EchoPort = 5001;
static constexpr uint32_t InputBatchSize = 16;

static void
logMacAddress(const modm::ethernet::MacAddress& address)
{
	MODM_LOG_INFO << "MAC ";
	for (std::size_t index = 0; index < address.size(); ++index) {
		MODM_LOG_INFO << modm::hex << address[index] << modm::ascii;
		if (index + 1 < address.size()) {
			MODM_LOG_INFO << ':';
		}
	}
	MODM_LOG_INFO << modm::endl;
}

using LwipEthernet = modm::lwip::LwipEthernet<
		modm::platform::EthernetMac, modm::Lan8742a<Board::eth::PhyAddress>>;

struct ServiceDiagnostics
{
	uint32_t inputAllocationPressure = 0;
	uint32_t udpSendAllocationPressure = 0;
	uint32_t mdioBusy = 0;
	uint32_t mdioTimeout = 0;
	modm::Clock::time_point lastReport{};
	bool haveReported = false;
};

static ServiceDiagnostics diagnostics;
static modm::PeriodicTimer linkPollTimer{500ms};
static modm::PeriodicTimer statisticsReportTimer{10s};

static void
recordUdpSendError(err_t error)
{
	if (error == ERR_MEM) {
		diagnostics.udpSendAllocationPressure++;
		return;
	}
	modm_assert(error == ERR_OK, "lwip.udp.tx", "UDP echo transmit failed",
			static_cast<uintptr_t>(-error));
}

static void
reportStatistics()
{
	const auto receive = LwipEthernet::getCumulativeReceiveStatistics();
	const auto errors = LwipEthernet::getErrorCounters();
	const auto hardware = LwipEthernet::getHardwareErrorStatus();

	MODM_LOG_INFO << "Ethernet adapter RX: acquired=" << receive.acquiredFrames
		<< ", dropped=" << receive.droppedFrames
		<< ", checksum_drops=" << receive.checksumDrops
		<< ", allocation_drops=" << receive.allocationDrops << modm::endl;
	MODM_LOG_INFO << "Ethernet MAC RX errors: missed=" << errors.rxMissedPackets
		<< ", overflow=" << errors.rxOverflowPackets
		<< ", crc=" << errors.rxCrcErrors
		<< ", alignment=" << errors.rxAlignmentErrors << modm::endl;
	MODM_LOG_INFO << "Ethernet DMA errors: tx_descriptor=" << errors.txDescriptorErrors
		<< ", fatal_bus=" << errors.fatalBusErrors
		<< ", context_descriptor=" << errors.contextDescriptorErrors
		<< ", rx_watchdog=" << errors.rxWatchdogTimeouts << modm::endl;
	MODM_LOG_INFO << "Ethernet DMA events: rx_stopped=" << errors.rxProcessStopped
		<< ", rx_unavailable=" << errors.rxBufferUnavailable
		<< ", tx_stopped=" << errors.txProcessStopped
		<< ", tx_unavailable=" << errors.txBufferUnavailable << modm::endl;
	MODM_LOG_INFO << "Ethernet sticky status: dma=0x" << modm::hex
		<< hardware.dmaStatus << ", tx_descriptor=0x" << hardware.txDescriptorStatus
		<< modm::ascii << modm::endl;
}

static void
poll()
{
	const err_t inputError = LwipEthernet::pollInput(InputBatchSize);
	if (inputError == ERR_MEM)
		diagnostics.inputAllocationPressure++;
	modm_assert(inputError == ERR_OK or inputError == ERR_MEM,
			"lwip.input", "Ethernet input failed", static_cast<uintptr_t>(-inputError));

	if (linkPollTimer.execute()) {
		const auto link = LwipEthernet::pollLink();
		switch (link.phyError) {
			case modm::ethernet::MdioError::None:
				break;
			case modm::ethernet::MdioError::Busy:
				diagnostics.mdioBusy++;
				break;
			case modm::ethernet::MdioError::Timeout:
				diagnostics.mdioTimeout++;
				break;
			default:
				modm_assert(false, "eth.mdio", "Ethernet PHY link read failed",
						static_cast<uintptr_t>(link.phyError));
		}
		modm_assert(link.macError == LwipEthernet::LinkUpdateError::None,
				"eth.link", "Ethernet MAC link update failed",
				static_cast<uintptr_t>(link.macError));
	}
	modm::lwip::processTimeouts();
	if (statisticsReportTimer.execute())
		reportStatistics();

	if (diagnostics.inputAllocationPressure == 0 and
			diagnostics.udpSendAllocationPressure == 0 and
			diagnostics.mdioBusy == 0 and diagnostics.mdioTimeout == 0)
		return;
	const auto now = modm::Clock::now();
	if (diagnostics.haveReported and now - diagnostics.lastReport < 5s)
		return;

	if (diagnostics.inputAllocationPressure != 0) {
		MODM_LOG_WARNING << "Ethernet input allocation pressure: "
			<< diagnostics.inputAllocationPressure << " ERR_MEM events" << modm::endl;
	}
	if (diagnostics.udpSendAllocationPressure != 0) {
		MODM_LOG_WARNING << "UDP echo transmit allocation pressure: "
			<< diagnostics.udpSendAllocationPressure << " ERR_MEM events" << modm::endl;
	}
	if (diagnostics.mdioBusy != 0 or diagnostics.mdioTimeout != 0) {
		MODM_LOG_WARNING << "Ethernet transient MDIO errors: busy="
			<< diagnostics.mdioBusy << ", timeout=" << diagnostics.mdioTimeout
			<< modm::endl;
	}
	diagnostics.inputAllocationPressure = 0;
	diagnostics.udpSendAllocationPressure = 0;
	diagnostics.mdioBusy = 0;
	diagnostics.mdioTimeout = 0;
	diagnostics.lastReport = now;
	diagnostics.haveReported = true;
}
}

// ----------------------------------------------------------------------------
// UDP echo

static void
udpReceive(void*, struct udp_pcb* pcb, struct pbuf* p, const ip_addr_t* address, uint16_t port)
{
	if (p != nullptr) {
		Ethernet::recordUdpSendError(udp_sendto(pcb, p, address, port));
		pbuf_free(p);
	}
}

static bool
setupUdpEcho()
{
	modm::lwip::LwIPSingleThreadGuard guard;
	struct udp_pcb* pcb = udp_new();
	if (pcb == nullptr)
		return false;

	if (udp_bind(pcb, IP_ADDR_ANY, Ethernet::EchoPort) == ERR_OK) {
		udp_recv(pcb, udpReceive, nullptr);
		return true;
	}
	udp_remove(pcb);
	return false;
}

// ----------------------------------------------------------------------------
// TCP echo

static err_t
tcpReceive(void* argument, struct tcp_pcb* pcb, struct pbuf* p, err_t err);

static err_t
tcpSent(void* argument, struct tcp_pcb* pcb, uint16_t length);

static err_t
tcpPoll(void* argument, struct tcp_pcb* pcb);

static void
tcpError(void* argument, err_t err);

struct TcpEchoState
{
	struct pbuf* pending;
	bool remoteClosed;
};

static constexpr uint8_t TcpPollInterval = 2;

static void
freeTcpState(TcpEchoState* state)
{
	if (state == nullptr)
		return;

	if (state->pending != nullptr)
		pbuf_free(state->pending);
	mem_free(state);
}

static void
registerTcpCallbacks(struct tcp_pcb* pcb, TcpEchoState* state)
{
	tcp_arg(pcb, state);
	tcp_recv(pcb, tcpReceive);
	tcp_sent(pcb, tcpSent);
	tcp_poll(pcb, tcpPoll, TcpPollInterval);
	tcp_err(pcb, tcpError);
}

static err_t
abortTcpConnection(struct tcp_pcb* pcb)
{
	tcp_abort(pcb);
	return ERR_ABRT;
}

static err_t
closeTcpConnection(struct tcp_pcb* pcb, TcpEchoState* state)
{
	tcp_arg(pcb, nullptr);
	tcp_recv(pcb, nullptr);
	tcp_sent(pcb, nullptr);
	tcp_poll(pcb, nullptr, 0);
	tcp_err(pcb, nullptr);

	const err_t error = tcp_close(pcb);
	if (error == ERR_OK) {
		freeTcpState(state);
		return ERR_OK;
	}

	registerTcpCallbacks(pcb, state);
	if (error == ERR_MEM)
		return ERR_OK;
	return abortTcpConnection(pcb);
}

static err_t
sendTcpEcho(struct tcp_pcb* pcb, TcpEchoState* state)
{
	if (state == nullptr)
		return abortTcpConnection(pcb);

	bool queued = false;
	while (state->pending != nullptr and state->pending->len <= tcp_sndbuf(pcb)) {
		struct pbuf* current = state->pending;
		const err_t error = tcp_write(
			pcb, current->payload, current->len, TCP_WRITE_FLAG_COPY);
		if (error == ERR_MEM)
			break;
		if (error != ERR_OK)
			return abortTcpConnection(pcb);

		const uint16_t length = current->len;
		state->pending = current->next;
		if (state->pending != nullptr) {
			// Preserve the tail while pbuf_free() releases the consumed head.
			pbuf_ref(state->pending);
		}
		pbuf_free(current);
		tcp_recved(pcb, length);
		queued = queued or length != 0;
	}

	if (queued)
		(void) tcp_output(pcb);
	if (state->pending == nullptr and state->remoteClosed)
		return closeTcpConnection(pcb, state);
	return ERR_OK;
}

static err_t
tcpReceive(void* argument, struct tcp_pcb* pcb, struct pbuf* p, err_t err)
{
	if (err != ERR_OK)
		return err;

	auto* state = static_cast<TcpEchoState*>(argument);
	if (state == nullptr)
		return abortTcpConnection(pcb);

	if (p == nullptr) {
		state->remoteClosed = true;
		return sendTcpEcho(pcb, state);
	}

	if (state->pending != nullptr) {
		// Let lwIP retain one refused packet until the pending chain advances.
		return ERR_MEM;
	}
	state->pending = p;
	return sendTcpEcho(pcb, state);
}

static void
tcpError(void* argument, err_t)
{
	freeTcpState(static_cast<TcpEchoState*>(argument));
}

static err_t
tcpSent(void* argument, struct tcp_pcb* pcb, uint16_t)
{
	return sendTcpEcho(pcb, static_cast<TcpEchoState*>(argument));
}

static err_t
tcpPoll(void* argument, struct tcp_pcb* pcb)
{
	return sendTcpEcho(pcb, static_cast<TcpEchoState*>(argument));
}

static err_t
tcpAccept(void*, struct tcp_pcb* newPcb, err_t err)
{
	if (err != ERR_OK)
		return err;
	if (newPcb == nullptr)
		return ERR_VAL;

	auto* state = static_cast<TcpEchoState*>(mem_malloc(sizeof(TcpEchoState)));
	if (state == nullptr)
		return ERR_MEM;
	state->pending = nullptr;
	state->remoteClosed = false;
	registerTcpCallbacks(newPcb, state);
	return ERR_OK;
}

static bool
setupTcpEcho()
{
	modm::lwip::LwIPSingleThreadGuard guard;
	struct tcp_pcb* pcb = tcp_new();
	if (pcb == nullptr)
		return false;

	if (tcp_bind(pcb, IP_ADDR_ANY, Ethernet::EchoPort) != ERR_OK) {
		if (tcp_close(pcb) != ERR_OK)
			tcp_abort(pcb);
		return false;
	}

	struct tcp_pcb* listenPcb = tcp_listen_with_backlog(pcb, 4);
	if (listenPcb != nullptr) {
		tcp_accept(listenPcb, tcpAccept);
		return true;
	}
	if (tcp_close(pcb) != ERR_OK)
		tcp_abort(pcb);
	return false;
}

// ----------------------------------------------------------------------------
// Application

modm::Fiber networkFiber([]{
	while (true) {
		Ethernet::poll();
		(void) modm::this_fiber::poll_for(1ms, []{
			return Ethernet::LwipEthernet::hasPendingWakeupEvents();
		});
		(void) Ethernet::LwipEthernet::consumeWakeupEvents();
		modm::this_fiber::yield();
	}
});

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "\n\nReboot: lwIP raw Ethernet Example" << modm::endl;
	MODM_LOG_INFO << "IPv4 10.66.0.42, UDP/TCP echo port "
		<< Ethernet::EchoPort << modm::endl;

	Ethernet::LwipEthernet::connect<Board::eth::RefClk::Refclk,
		Board::eth::Mdc::Mdc,
		Board::eth::Mdio::Mdio,
		Board::eth::CrsDv::Rcccrsdv,
		Board::eth::TxEn::Txen,
		Board::eth::Txd0::Txd0,
		Board::eth::Txd1::Txd1,
		Board::eth::Rxd0::Rxd0,
		Board::eth::Rxd1::Rxd1>();

	const modm::lwip::StaticIPv4Configuration config {
		.macAddress = modm::platform::EthernetMac::getDefaultMacAddress(),
		.ipAddress = {{ 10, 66, 0, 42 }},
		.netmask = {{ 255, 255, 255, 0 }},
		.gateway = {{ 10, 66, 0, 1 }},
	};
	Ethernet::logMacAddress(config.macAddress);

	const auto initialization = Ethernet::LwipEthernet::initialize<Board::SystemClock>(config);
	if (not initialization) {
		MODM_LOG_ERROR << "Ethernet/lwIP initialization failed (MAC error "
			<< static_cast<int>(initialization.macError) << ", PHY error "
			<< static_cast<int>(initialization.phyError) << ", PHY MDIO error "
			<< static_cast<int>(initialization.phyMdioError) << ", link MDIO error "
			<< static_cast<int>(initialization.linkMdioError) << ", link update error "
			<< static_cast<int>(initialization.linkUpdateError) << ", adapter error "
			<< static_cast<int>(initialization.adapterError) << ")" << modm::endl;
	}
	const uintptr_t initializationErrorContext =
		(static_cast<uintptr_t>(initialization.macError) << 8) |
		static_cast<uintptr_t>(initialization.adapterError);
	modm_assert(initialization, "lwip.initialize", "Ethernet/lwIP initialization failed",
			initializationErrorContext);
	modm_assert(Ethernet::LwipEthernet::setDefault(), "lwip.default",
			"Failed to select the default network interface");
	modm_assert(setupUdpEcho(), "lwip.udp", "Failed to set up UDP echo");
	modm_assert(setupTcpEcho(), "lwip.tcp", "Failed to set up TCP echo");

	modm::fiber::Scheduler::run();
	return 0;
}
