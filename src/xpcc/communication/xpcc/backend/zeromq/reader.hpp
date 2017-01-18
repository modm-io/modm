// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__ZEROMQ_READER_HPP
#define	XPCC__ZEROMQ_READER_HPP

#include <thread>
#include <mutex>
#include <atomic>
#include <deque>

#include <zmqpp/zmqpp.hpp>

#include <xpcc/debug/logger.hpp>
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::ERROR

namespace xpcc
{

class ZeroMQReader
{
public:
	static constexpr int PollTimeoutMs = 100;

	struct Packet
	{
		Packet(uint8_t size, const Header& inHeader) :
			header(inHeader), payload(size) {}

		xpcc::Header header;
		xpcc::SmartPointer payload;
	};

	ZeroMQReader(zmqpp::socket& socketIn_, std::size_t maxQueueSize_ = 1000) :
		socketIn(socketIn_), maxQueueSize(maxQueueSize_), stopThread(false)
	{
	}

	~ZeroMQReader()
	{
		stop();
	}

	ZeroMQReader(const ZeroMQReader&) = delete;
	ZeroMQReader& operator=(const ZeroMQReader&) = delete;

	void
	start()
	{
		stop();

		this->stopThread = false;
		this->thread = std::thread([this]() {
			receiveThread();
		});
	}

	void
	stop()
	{
		if(this->thread.joinable()) {
			this->stopThread = true;
			this->thread.join();
		}
	}

	bool
	isPacketAvailable() const
	{
		std::lock_guard<std::mutex> lock(this->queueMutex);
		return not this->queue.empty();
	}

	const Packet&
	getPacket() const
	{
		std::lock_guard<std::mutex> lock(this->queueMutex);
		return this->queue.front();
	}

	void
	dropPacket()
	{
		std::lock_guard<std::mutex> lock(this->queueMutex);

		if(not this->queue.empty()) {
			this->queue.pop_front();
		}
	}

private:
	void
	receiveThread()
	{
		zmqpp::poller poller;
		zmqpp::message message;

		poller.add(this->socketIn, zmqpp::poller::poll_in);

		while(not this->stopThread) {
			while(this->socketIn.receive(message, /* no_block = */ true)) {
				readPacket(message);

#				if ZMQPP_VERSION_MAJOR < 4
				// swap and discard old message as in zmqpp 3
				// "receiving can only be done to empty messages"
				zmqpp::message emptyMessage;
				std::swap(emptyMessage, message);
#				endif
			}
			poller.poll(PollTimeoutMs);
		}
	}

	void
	readPacket(const zmqpp::message& message)
	{
		constexpr uint16_t headerSize = 5;

		// Maximum payload size of xpcc::SmartPointer
		constexpr uint16_t maxPayloadSize = 65529;

		const auto size = message.size(0);

		if(size >= headerSize && size <= (headerSize + maxPayloadSize)) {
			const uint8_t* const data = static_cast<const uint8_t*>(message.raw_data());
			const auto payloadSize = size - headerSize;

			xpcc::Header header = xpcc::Header(
				/* type = */ xpcc::Header::Type(data[0]),
				/* ack  = */ data[1],
				/* dest = */ data[2],
				/* src  = */ data[3],
				/* id   = */ data[4]);

			{
				std::lock_guard<std::mutex> lock(this->queueMutex);

				if (this->queue.size() >= this->maxQueueSize) {
					this->queue.pop_front();

					XPCC_LOG_ERROR << XPCC_FILE_INFO;
					XPCC_LOG_ERROR << "Receive queue is full, dropping packets" << xpcc::endl;
				}

				this->queue.emplace_back(payloadSize, header);

				// Copy received payload to packet
				uint8_t* const payloadBuffer = this->queue.back().payload.getPointer();
				std::copy_n(data + headerSize, payloadSize, payloadBuffer);
			}
		} else {
			XPCC_LOG_ERROR << XPCC_FILE_INFO;
			XPCC_LOG_ERROR << "Invalid message length: " << size << xpcc::endl;
		}
	}
private:
	zmqpp::socket& socketIn;

	std::deque<Packet> queue;
	mutable std::mutex queueMutex;
	const std::size_t maxQueueSize;

	std::thread thread;
	std::atomic<bool> stopThread;
};

} // xpcc namespace

#endif // XPCC__ZEROMQ_CONNECTOR_HPP
