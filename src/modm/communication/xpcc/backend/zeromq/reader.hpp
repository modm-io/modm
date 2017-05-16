// coding: utf-8
/* Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	MOD_EROMQ_READER_HPP
#define	MOD_EROMQ_READER_HPP

#include <thread>
#include <mutex>
#include <atomic>
#include <deque>

#include <zmqpp/zmqpp.hpp>

#include "../header.hpp"

#include <modm/debug/logger.hpp>
#undef MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::ERROR

namespace modm
{

/**
 * @brief	Reads packets from a zmqpp socket in a background thread
 *
 * @ingroup	backend
 *
 * @author	Christopher Durand <christopher.durand@rwth-aachen.de>
 * @author	strongly-typed
 */
class ZeroMQReader
{
public:
	static constexpr int PollTimeoutMs = 100;

	struct Packet
	{
		Packet(uint16_t size, const Header& inHeader) :
			header(inHeader), payload(size) {}

		modm::Header header;
		modm::SmartPointer payload;
	};

	ZeroMQReader(zmqpp::socket& socketIn_, std::size_t maxQueueSize_ = 1000);

	~ZeroMQReader();

	ZeroMQReader(const ZeroMQReader&) = delete;
	ZeroMQReader& operator=(const ZeroMQReader&) = delete;

	void
	start();

	void
	stop();

	bool
	isPacketAvailable() const;

	const Packet&
	getPacket() const;

	void
	dropPacket();

private:
	void
	receiveThread();

	void
	readPacket(const zmqpp::message& message);

private:
	zmqpp::socket& socketIn;

	std::deque<Packet> queue;
	mutable std::mutex queueMutex;
	const std::size_t maxQueueSize;

	std::thread thread;
	std::atomic<bool> stopThread;
};

} // modm namespace

#endif // MOD_EROMQ_CONNECTOR_HPP
