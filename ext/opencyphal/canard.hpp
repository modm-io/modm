/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <canard.h>
#include <o1heap/o1heap.h>
#include <modm/architecture/interface/can.hpp>
#include <modm/architecture/interface/can_filter.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <cstdlib>
#include <cstring>
#include <span>

namespace modm::canard
{

/// Monotonic 64-bit microseconds for libcanard, extended from the 32-bit
/// micro_clock. Must be called at least once every 71 minutes, which
/// Node::update() and Node::poll() do.
inline canard_us_t
now()
{
	static uint32_t last{};
	static uint64_t high{};
	const uint32_t time = modm::chrono::micro_clock::now().time_since_epoch().count();
	if (time < last) high += uint64_t(1) << 32;
	last = time;
	return canard_us_t(high | time);
}

/// Frees the memory of a received payload. Every subscription callback must
/// call this once it no longer needs the payload.
inline void
release(canard_subscription_t* subscription, const canard_payload_t& payload)
{
	if (payload.origin.data == nullptr) return;
	const canard_mem_t mem = subscription->owner->mem.rx_payload;
	mem.vtable->free(mem, payload.origin.size, payload.origin.data);
}

/// Adapts a CAN driver object to the static interface of Node, for hosted
/// drivers like `modm::platform::SocketCan`: `Node<CanInstance<socket>>`.
template< auto& can >
struct CanInstance
{
	static bool getMessage(modm::can::Message& message) { return can.getMessage(message); }
	static bool sendMessage(const modm::can::Message& message) { return can.sendMessage(message); }
	static bool isReadyToSend() { return can.isReadyToSend(); }
};

/// Configuration of modm::canard::Node
struct NodeConfig
{
	/// Send Cyphal transfers as CAN FD frames with bit rate switching.
	/// DroneCAN transfers are always Classic CAN.
	bool fd = false;
	/// Maximum number of frames waiting for transmission
	size_t queue_capacity = 32;
	/// Let libcanard configure the extended filters of the driver.
	/// Disable it if several nodes share one CAN controller.
	bool filters = true;
	/// Drop the received echoes of the frames sent by this node, so that the
	/// node can be used with a CAN controller in loopback mode.
	bool loopback = false;
	/// Keep the node-ID if another node uses it too, as DroneCAN requires.
	/// The frames of the other node are dropped and counted in collisions().
	/// Otherwise libcanard moves to a free node-ID, as Cyphal does.
	bool keep_node_id = false;
};

/**
 * libcanard instance on one modm CAN driver, for Cyphal/CAN and DroneCAN.
 *
 * libcanard allocates TX frames and RX sessions from a private o1heap arena
 * inside this object, so that bus traffic cannot exhaust the application heap,
 * or with `ArenaSize = 0` from the application heap with malloc(). Use the
 * libcanard API on `canard` to publish and subscribe. The libcanard API and
 * this class must only be used from one context, not from interrupts.
 *
 * If the driver has extended filters (FDCAN), libcanard replaces all of them
 * with the filters for its subscriptions. Otherwise, or with `filters = false`,
 * configure the filters to accept all extended frames yourself.
 *
 * @tparam Can			modm CAN driver, e.g. `Fdcan1`, or a `CanInstance`
 * @tparam ArenaSize	bytes of the o1heap arena for libcanard, or 0 for malloc()
 */
template< class Can, size_t ArenaSize = 4096 >
class Node
{
public:
	canard_t canard{};

	using Config = NodeConfig;


	Node() = default;
	Node(const Node&) = delete;
	Node& operator=(const Node&) = delete;

	/// @param node_id 0-127, or CANARD_NODE_ID_ANONYMOUS for automatic allocation.
	///        With `keep_node_id`, anonymous is node-ID 0, which DroneCAN
	///        reserves for nodes without node-ID.
	/// @param seed must differ between nodes, e.g. a hash of the device UID.
	bool
	initialize(uint8_t node_id, uint64_t seed, Config config = {})
	{
		void* context{};
		if constexpr (ArenaSize)
		{
			if ((context = heap = o1heapInit(arena, ArenaSize)) == nullptr) return false;
		}
		const canard_mem_t mem{&memory_vtable, context};
		if (not canard_new(&canard, &vtable, {mem, mem, mem, mem, mem}, 1,
						   config.queue_capacity, seed, config.filters ? filterCount() : 0))
			return false;
		canard.user_context = this;
		canard.tx.fd = config.fd;
		loopback = config.loopback;
		keep_node_id = config.keep_node_id;
		if (keep_node_id and node_id > CANARD_NODE_ID_MAX) node_id = 0;
		if (node_id <= CANARD_NODE_ID_MAX) return canard_set_node_id(&canard, node_id);
		return true;
	}

	/// Passes all received frames to libcanard and transmits pending frames.
	/// Call this every few milliseconds.
	void
	update()
	{
		modm::can::Message message;
		while (Can::getMessage(message)) ingest(message);
		poll();
	}

	/// Passes one received frame to libcanard, for applications that read the
	/// CAN driver themselves, e.g. to pass the frames to several nodes.
	/// libcanard treats its own node-ID in a received frame as a collision and
	/// picks a new node-ID, so frames sent by this node must not be passed back,
	/// unless the node is configured with `loopback = true`.
	void
	ingest(const modm::can::Message& message)
	{
		if (not message.isExtended() or message.isRemoteTransmitRequest()) return;
		if (loopback and dropEcho(message.getIdentifier())) return;
		// the source node-ID is in the lowest 7 bits in Cyphal and DroneCAN
		if (keep_node_id and canard.node_id != 0 and
			(message.getIdentifier() & CANARD_NODE_ID_MAX) == canard.node_id)
		{
			collision_count++;
			return;
		}
		canard_ingest_frame(&canard, now(), 0, message.getIdentifier(),
							{message.getLength(), message.data});
	}

	/// Transmits pending frames, drops expired ones and updates the filters.
	void
	poll()
	{
		canard_poll(&canard, Can::isReadyToSend() ? 1 : 0);
	}

	/// Sends an anonymous DroneCAN message of up to 7 bytes as a single frame,
	/// e.g. the uavcan.protocol.dynamic_node_id.Allocation request of a node
	/// without node-ID. libcanard can't send anonymous frames, so the frame
	/// bypasses its TX queue. Only the 2 low bits of the data type ID are sent.
	/// @return false if the payload is too long or the driver is busy.
	bool
	publishAnonymous(uint16_t data_type_id, uint8_t transfer_id, std::span<const uint8_t> payload,
					 canard_prio_t priority = canard_prio_nominal)
	{
		if (payload.size() > 7) return false;
		// CRC-16-CCITT of the payload as discriminator, so that anonymous nodes
		// sending different payloads at the same time don't collide
		uint16_t crc = 0xffff;
		for (const uint8_t byte : payload)
		{
			crc ^= uint16_t(byte) << 8;
			for (int bit = 0; bit < 8; bit++)
				crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
		}
		const uint32_t id = (uint32_t(priority) << 26) | (uint32_t(crc & 0x7ffe) << 9) |
							(uint32_t(data_type_id & 0b11) << 8);
		modm::can::Message message(id, payload.size() + 1);
		message.setExtended(true);
		std::memcpy(message.data, payload.data(), payload.size());
		// single frame: start and end of transfer, toggle 0
		message.data[payload.size()] = 0xC0 | (transfer_id & CANARD_TRANSFER_ID_MAX);
		return send(message);
	}

	/// Received frames of other nodes with the node-ID of this node, with `keep_node_id`
	size_t
	collisions() const
	{
		return collision_count;
	}

	/// Peak usage and out-of-memory events of the arena
	O1HeapDiagnostics
	heapDiagnostics() const requires (ArenaSize > 0)
	{
		return o1heapGetDiagnostics(heap);
	}

private:
	static constexpr size_t
	filterCount()
	{
		if constexpr (requires { Can::ExtendedFilterCount; }) return Can::ExtendedFilterCount;
		else return 0;
	}

	/// Frames that can be on their way back in loopback mode: in the transmit
	/// and receive queues of the driver and the hardware.
	static constexpr size_t
	echoCapacity()
	{
		if constexpr (requires { Can::TxBufferSize + Can::TxHwQueueSize + Can::RxBufferSize; })
			return Can::TxBufferSize + Can::TxHwQueueSize + Can::RxBufferSize + 16;
		else return 64;
	}

	bool
	send(const modm::can::Message& message)
	{
		if (not Can::sendMessage(message)) return false;
		if (loopback)
		{
			// overwrite the oldest echo if the receiver falls behind
			echoes[(echo_start + echo_count) % echoCapacity()] = message.getIdentifier();
			if (echo_count < echoCapacity()) echo_count++;
			else echo_start = (echo_start + 1) % echoCapacity();
		}
		return true;
	}

	/// Removes a received identifier from the sent ones.
	/// @return true if it is the echo of a sent frame
	bool
	dropEcho(uint32_t id)
	{
		for (size_t ii = 0; ii < echo_count; ii++)
		{
			const size_t index = (echo_start + ii) % echoCapacity();
			if (echoes[index] != id) continue;
			// keep the order of the remaining echoes
			for (size_t jj = ii; jj > 0; jj--)
				echoes[(echo_start + jj) % echoCapacity()] = echoes[(echo_start + jj - 1) % echoCapacity()];
			echo_start = (echo_start + 1) % echoCapacity();
			echo_count--;
			return true;
		}
		return false;
	}

	static bool
	transmit(canard_t* canard, void*, canard_us_t, uint_least8_t, bool fd,
			 uint32_t id, canard_bytes_t data)
	{
		if (data.size > modm::can::Message::capacity) return false;
		modm::can::Message message(id, data.size);
		message.setExtended(true);
		message.setFlexibleData(fd);
		message.setBitRateSwitching(fd);
		std::memcpy(message.data, data.data, data.size);
		return static_cast<Node*>(canard->user_context)->send(message);
	}

	static bool
	filter(canard_t*, size_t count, const canard_filter_t* filters)
	{
		if constexpr (requires { Can::ExtendedFilterCount; })
		{
			Can::clearExtendedFilters();
			for (size_t ii = 0; ii < count; ii++)
			{
				if (not Can::setExtendedFilter(ii, Can::FilterConfig::Fifo0,
						modm::can::ExtendedIdentifier(filters[ii].extended_can_id),
						modm::can::ExtendedMask(filters[ii].extended_mask)))
					return false;
			}
			return true;
		}
		else return false;
	}

	static void*
	allocate(canard_mem_t mem, size_t size)
	{
		if constexpr (ArenaSize) return o1heapAllocate(static_cast<O1HeapInstance*>(mem.context), size);
		else return std::malloc(size);
	}

	static void
	deallocate(canard_mem_t mem, size_t, void* pointer)
	{
		if constexpr (ArenaSize) o1heapFree(static_cast<O1HeapInstance*>(mem.context), pointer);
		else std::free(pointer);
	}

	static canard_us_t
	clock(const canard_t*)
	{
		return now();
	}

	static constexpr canard_mem_vtable_t memory_vtable{deallocate, allocate};
	static constexpr canard_vtable_t vtable{clock, transmit, filterCount() ? filter : nullptr};

	O1HeapInstance* heap{};
	bool loopback{};
	bool keep_node_id{};
	size_t collision_count{};
	// identifiers of the sent frames whose echo has not been received yet
	uint32_t echoes[echoCapacity()];
	size_t echo_start{};
	size_t echo_count{};
	alignas(O1HEAP_ALIGNMENT) uint8_t arena[ArenaSize ? ArenaSize : 1];
};

} // namespace modm::canard
