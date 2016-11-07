/*
 * Copyright (c) 2014-2015, Daniel Krebs
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_DATA_HPP
#   error "Don't include this file directly, use 'nrf24_data.hpp' instead!"
#endif

#include "nrf24_data.hpp"
#include <string.h>

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
typename xpcc::Nrf24Data<Nrf24Phy>::BaseAddress
xpcc::Nrf24Data<Nrf24Phy>::baseAddress;

template<typename Nrf24Phy>
typename xpcc::Nrf24Data<Nrf24Phy>::Address
xpcc::Nrf24Data<Nrf24Phy>::broadcastAddress;

template<typename Nrf24Phy>
typename xpcc::Nrf24Data<Nrf24Phy>::Address
xpcc::Nrf24Data<Nrf24Phy>::ownAddress;

template<typename Nrf24Phy>
typename xpcc::Nrf24Data<Nrf24Phy>::Address
xpcc::Nrf24Data<Nrf24Phy>::connections[3];

template<typename Nrf24Phy>
typename xpcc::Nrf24Data<Nrf24Phy>::Frame
xpcc::Nrf24Data<Nrf24Phy>::assemblyFrame;

template<typename Nrf24Phy>
typename xpcc::Nrf24Data<Nrf24Phy>::SendingState
xpcc::Nrf24Data<Nrf24Phy>::state = SendingState::Undefined;

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::packetProcessed = false;

template<typename Nrf24Phy>
xpcc::Timeout
xpcc::Nrf24Data<Nrf24Phy>::sendingInterruptTimeout;

// --------------------------------------------------------------------------------------------------------------------


template<typename Nrf24Phy>
void
xpcc::Nrf24Data<Nrf24Phy>::initialize(BaseAddress base_address, Address own_address, Address broadcast_address)
{
	// Set base address and clear lower byte. When assembling full addresses,
	// each Address (1 byte) will be ORed to the lower byte of this base address
	baseAddress = base_address & ~(0xff);

	broadcastAddress = broadcast_address;

	// Initialized with broadcast address means unset
	connections[0]  = broadcastAddress;
	connections[1]  = broadcastAddress;
	connections[2]  = broadcastAddress;

	setAddress(own_address);

	// reset state
	state = SendingState::Undefined;

	// Clear assembly frame
	memset(&assemblyFrame, 0, sizeof(Frame));

	// Set to fixed address length of 5 byte for now
	Config::setAddressWidth(Config::AddressWidth::Byte5);

	// Setup broadcast pipe
	Phy::setRxAddress(Pipe::PIPE_1, assembleAddress(broadcast_address));

	// Disable auto ack
	Config::enablePipe(Pipe::PIPE_1, false);

	// Setup pipe 0 that will be used to receive acks and therefore configured
	// with an address to listen for. We want to already enable it, but not
	// setting an address could lead to erroneous packet coming from noise.
	// So we set it to the bitwise negated base address.
	Phy::setRxAddress(Pipe::PIPE_0, ~assembleAddress(0x55));

	// don't enable auto ack here because we're not expecting data on this pipe
	// It was observed that when set to 'false' no packet is received until the module sends a packet once.
	Config::enablePipe(Pipe::PIPE_0, false);


	// Enable feature 'EN_DYN_ACK' to be able to send packets without expecting
	// an ACK as response (used for transmitting to broadcast address)
	Config::enableFeatureNoAck();

	// Flush Fifos just to be sure
	Phy::flushRxFifo();
	Phy::flushTxFifo();

	/*
	 * Configure some sensible defaults, may be changed later by user, but
	 * should be consistent among all other nodes
	 */
	Config::setCrc(Config::Crc::Crc1Byte);
	Config::setSpeed(Config::Speed::kBps250);
	Config::setAutoRetransmitDelay(Config::AutoRetransmitDelay::us1000);
	Config::setAutoRetransmitCount(Config::AutoRetransmitCount::Retransmit15);
	Config::setRfPower(Config::RfPower::dBm0);

	// Power up module in Rx mode
	Config::setMode(Config::Mode::Rx);
	Config::powerUp();

	// don't save power, always in Rx-mode or standby-2 (see table 15, p. 24)
	Phy::setCe();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Data<Nrf24Phy>::setAddress(Address address)
{
	// overwrite connection
	ownAddress = address;

	// address for pipe 2
	Phy::setRxAddress(Pipe::PIPE_2, assembleAddress(address));

	// enable pipe with auto ack
	Config::enablePipe(Pipe::PIPE_2, true);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::sendPacket(Packet& packet)
{
	if(not isReadyToSend())
	{
		XPCC_LOG_WARNING << "Warning: Not ready to send" << xpcc::endl;
		state = SendingState::Failed;
		packetProcessed = true;
		return false;
	}

	if(packet.payload.length > getPayloadLength())
	{
		XPCC_LOG_ERROR << "Error: Payload length was " << packet.payload.length
		               << ", max is " << getPayloadLength() << xpcc::endl;
		state = SendingState::Failed;
		return false;
	}

	// Clear processed status. This is needed when status was not polled for the
	// last packet.
	packetProcessed = false;

	// switch to Tx mode
	Config::setMode(Config::Mode::Tx);

	// assemble frame to transmit
	assemblyFrame.header.src = ownAddress;
	assemblyFrame.header.dest = packet.dest;
	memcpy(assemblyFrame.data, packet.payload.data, packet.payload.length);

	// set receivers address as tx address
	Phy::setTxAddress(assembleAddress(packet.dest));

	if(packet.dest == getBroadcastAddress())
	{
		Phy::writeTxPayloadNoAck((uint8_t*)&assemblyFrame, packet.payload.length + sizeof(Header));

		// as frame was sent without requesting an acknowledgment we can't determine it's state
		state = SendingState::DontKnow;

		// no further action needed, this is fire-and-forget
		packetProcessed = true;
	} else
	{
		// set pipe 0's address to tx address to receive ack packet
		Phy::setRxAddress(Pipe::PIPE_0, assembleAddress(packet.dest));
		Config::enablePipe(Pipe::PIPE_0, true);

		Phy::writeTxPayload((uint8_t*)&assemblyFrame, packet.payload.length + sizeof(Header));

		// mark state as busy, so update() will switch back to Rx mode when
		// state has changed
		state = SendingState::Busy;
	}

	sendingInterruptTimeout.restart(interruptTimeoutAfterSending);

	return true;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::getPacket(Packet& packet)
{
	if(!isPacketAvailable())
		return false;

	// Don't care about pipe numbers for now as we use our own header within each packet
	// Pipe_t pipe = Config::getPayloadPipe();
	// XPCC_LOG_DEBUG.printf("Received on pipe %d\n", pipe.value);

	/*
	 * TODO: Replace Packet by Frame because there's no reason to trade some bytes of RAM against the runtime
	 *       penalty of copying to and from assembly frame every cycle.
	 */

	// First read into buffer frame
	uint8_t frame_size = Phy::readRxPayload((uint8_t*)&assemblyFrame);

	// Then copy to user packet
	packet.dest = assemblyFrame.header.dest;
	packet.src = assemblyFrame.header.src;
	packet.payload.length = frame_size - sizeof(Header);
	memcpy(packet.payload.data, assemblyFrame.data, packet.payload.length);

	// Acknowledge RX_DR interrupt
	Phy::clearInterrupt(InterruptFlag::RX_DR);


	return true;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::isReadyToSend()
{
	if(state == SendingState::Failed)
		return true;

	uint8_t fifo_status = Phy::readFifoStatus();

	// Wait for TX Fifo to become empty, because otherwise we would need to make sure
	// that every packet has the same destination
	if(fifo_status & (uint8_t)FifoStatus::TX_EMPTY)
		return true;
	else
		return false;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::updateSendingState()
{
	// directly return state if not busy, because nothing needs to be updated then
	if(state != SendingState::Busy)
		return false;

	// read relevant status registers
	uint8_t status = Phy::readStatus();


	if(sendingInterruptTimeout.execute())
	{
		XPCC_LOG_ERROR << "[nrf24-data] IRQ timed out" << xpcc::endl;

		state = SendingState::DontKnow;

		// We should flush the Tx Fifo because we have no clue if the
		// packet could be sent
		Phy::flushTxFifo();

	} else if(status & (uint8_t)Status::MAX_RT)
	{
		XPCC_LOG_DEBUG << "Interrupt: MAX_RT" << xpcc::endl;

		state = SendingState::FinishedNack;

		// clear MAX_RT bit to enable further communication and flush Tx fifo,
		// because the failed packet still resides there
		Phy::clearInterrupt(InterruptFlag::MAX_RT);
		Phy::flushTxFifo();

	} else if(status & (uint8_t)Status::TX_DS)
	{
		XPCC_LOG_DEBUG << "Interrupt: TX_DS" << xpcc::endl;

		state = SendingState::FinishedAck;

		// acknowledge TX_DS interrupt
		Phy::clearInterrupt(InterruptFlag::TX_DS);

	} else {
		// still busy
		return false;
	}

	// We are now finished with this packet, so user may take further action
	packetProcessed = true;

	return true;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Data<Nrf24Phy>::update()
{
	// When sending state changed the communication has finished and we switch
	// back to Rx mode
	if(updateSendingState())
	{
		// switch back to Rx mode
		Config::setMode(Config::Mode::Rx);
	}
}
// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::isPacketAvailable()
{
	uint8_t fifo_status = Phy::readFifoStatus();
	uint8_t status = Phy::readStatus();

	if( (status & (uint8_t)Status::RX_DR) ||
	    !(fifo_status & (uint8_t)FifoStatus::RX_EMPTY)) {
		return true;
	} else {
		return false;
	}
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::establishConnection()
{
	// not yet implemented
	return false;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::destroyConnection()
{
	// not yet implemented
	return false;
}
