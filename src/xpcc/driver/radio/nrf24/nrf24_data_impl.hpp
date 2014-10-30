// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_DATA_HPP
#   error "Don't include this file directly, use 'nrf24_data.hpp' instead!"
#endif

#include "nrf24_data.hpp"
#include <string.h>

using namespace xpcc::nrf24;

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
BaseAddress xpcc::Nrf24Data<Nrf24Phy>::baseAddress;

template<typename Nrf24Phy>
Address xpcc::Nrf24Data<Nrf24Phy>::broadcastAddress;

template<typename Nrf24Phy>
Address xpcc::Nrf24Data<Nrf24Phy>::ownAddress;

template<typename Nrf24Phy>
Address xpcc::Nrf24Data<Nrf24Phy>::connections[3];

template<typename Nrf24Phy>
frame_t xpcc::Nrf24Data<Nrf24Phy>::assemblyFrame;

template<typename Nrf24Phy>
SendingState xpcc::Nrf24Data<Nrf24Phy>::state = SendingState::Undefined;


// --------------------------------------------------------------------------------------------------------------------


template<typename Nrf24Phy>
void
xpcc::Nrf24Data<Nrf24Phy>::initialize(BaseAddress base_address, Address own_address, Address broadcast_address)
{
    baseAddress = base_address;
    broadcastAddress = broadcast_address;

    // Initialized with broadcast address means unset
    connections[0]  = broadcastAddress;
    connections[1]  = broadcastAddress;
    connections[2]  = broadcastAddress;

    setAddress(own_address);

    // reset state
    state = SendingState::Undefined;

    // Clear assembly frame
    memset(&assemblyFrame, 0, sizeof(frame_t));

    // Set to fixed address length of 5 byte for now
    ConfigLayer::setAddressWidth(AddressWidth::Byte5);

    // Setup broadcast pipe
    Nrf24Phy::setRxAddress(1, assembleAddress(broadcast_address));

    // Disable auto ack
    ConfigLayer::enablePipe(Pipe::Pipe1, false);

    // Setup pipe 0 that will be used to receive acks and therefore configured
    // with an address to listen for. We want to already enable it, but not
    // setting an address could lead to erroneous packet coming from noise.
    // So we set it to the bitwise negated base address.
    Nrf24Phy::setRxAddress(0, ~assembleAddress(0x55));

    // don't enable auto ack here because we're not expecting data on this pipe
    ConfigLayer::enablePipe(Pipe::Pipe0, false);


    // Enable feature 'EN_DYN_ACK' to be able to send packets without expecting
    // an ACK as response (used for transmitting to broadcast address)
    ConfigLayer::enableFeatureNoAck();

    // Flush Fifos just to be sure
    Nrf24Phy::flushRxFifo();
    Nrf24Phy::flushTxFifo();

    /*
     * Configure some sensible defaults, may be changed later by user, but
     * sould be consistent among all other nodes
     */
    ConfigLayer::setCrc(Crc::Crc2Byte);
    ConfigLayer::setSpeed(Speed::MBps2);
    ConfigLayer::setAutoRetransmitDelay(AutoRetransmitDelay::us750);
    ConfigLayer::setAutoRetransmitCount(AutoRetransmitCount::Retransmit8);
    ConfigLayer::setRfPower(RfPower::dBm0);

    // Power up module in Rx mode
    ConfigLayer::setMode(Mode::Rx);
    ConfigLayer::powerUp();

    // don't save power, always in Rx-mode or standby-2 (see table 15, p. 24)
    Nrf24Phy::setCe();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Data<Nrf24Phy>::setAddress(Address address)
{
    // overwrite connection
    ownAddress = address;

    // address for pipe 2
    Nrf24Phy::setRxAddress(2, address);

    // enable pipe with auto ack
    ConfigLayer::enablePipe(Pipe::Pipe2, true);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::sendPacket(packet_t& packet)
{
    if(!isReadyToSend())
        return false;

    if(packet.length > getPayloadLength())
        return false;

    // assemble frame to transmit
    assemblyFrame.header.src = ownAddress;
    assemblyFrame.header.dest = packet.dest;
    memcpy(assemblyFrame.data, packet.data, packet.length);

    // set receivers address as tx address
    Nrf24Phy::setTxAddres(assembleAddress(packet.dest));

    if(packet.dest == getBroadcastAddress())
    {
        Nrf24Phy::writeTxPayloadNoAck(&assemblyFrame, packet.length + sizeof(header_t));

        // as frame was sent without requesting an acknowledgement we can't determine it's state
        state = SendingState::DontKnow;
    } else
    {
        // set pipe 0's address to tx address to receive ack packet
        Nrf24Phy::setRxAddress(0, assembleAddress(packet.dest));
        Nrf24Phy::writeTxPayload(&assemblyFrame, packet.length + sizeof(header_t));

        // mark state as busy, so when
        state = SendingState::Busy;
    }

    // switch to Tx mode, CE should be always set
    ConfigLayer::setMode(Mode::Tx);

    // wait until packet is sent
    while( !(Nrf24Phy::readRegister(Register::FIFO_STATUS) & FifoStatus::TX_EMPTY) );

    // switch back to Rx mode
    ConfigLayer::setMode(Mode::Rx);

    return true;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::getPacket(packet_t& packet)
{
    if(!isPacketAvailable())
        return false;

    // Don't care about pipe numbers for now as we use our own header within each packet
    //uint8_t pipe = ConfigLayer::getPayloadPipe();

    /*
     * TODO: Replace packet_t by frame_t because there's no reason to trade some bytes of RAM against the runtime
     *       penalty of copying to and from assembly frame every cycle.
     */

    // First read into buffer frame
    uint8_t payload_length = Nrf24Phy::readRxPayload(&assemblyFrame);

    // Then copy to user packet
    packet.dest = assemblyFrame.header.dest;
    packet.src = assemblyFrame.header.src;
    packet.length = payload_length;
    memcpy(assemblyFrame.data, packet.data, packet.length);

    return true;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::isReadyToSend()
{
    uint8_t fifo_status = Nrf24Phy::readRegister(Register::FIFO_STATUS);

    // Wait for TX Fifo to become empty, because otherwise we would need to make sure
    // that every packet has the same destination
    if(fifo_status & FifoStatus::TX_EMPTY)
        return true;
    else
        return false;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
SendingState
xpcc::Nrf24Data<Nrf24Phy>::updateSendingState()
{
    // directly return state if not busy, because nothing needs to be updated then
    if(state != SendingState::Busy)
        return state;


    // read relevant status registers
    uint8_t status = Nrf24Phy::readStatus();
    uint8_t fifo_status = Nrf24Phy::readRegister(Register::FIFO_STATUS);

    if(status & Status::MAX_RT)
    {
        state = SendingState::FinishedNack;

        // clear MAX_RT bit to enable further communication
        Nrf24Phy::setBits(Register::STATUS, Status::MAX_RT);
    }

    if(status & Status::TX_DS)
    {
        state = SendingState::FinishedAck;
    }

    return state;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
bool
xpcc::Nrf24Data<Nrf24Phy>::isPacketAvailable()
{
    uint8_t fifo_status = Nrf24Phy::readRegister(Register::FIFO_STATUS);

    // only check Rx Fifo for now, Status:RX_DR will be needed when using interrupts
    if( !(fifo_status & FifoStatus::RX_EMPTY) )
        return true;
    else
        return false;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
SendingState
xpcc::Nrf24Data<Nrf24Phy>::getSendingFeedback()
{
    return updateSendingState();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
Address
xpcc::Nrf24Data<Nrf24Phy>::getAddress()
{
    return ownAddress;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
uint64_t
xpcc::Nrf24Data<Nrf24Phy>::assembleAddress(Address address)
{
    return static_cast<uint64_t>((uint64_t)baseAddress | (uint64_t)address);
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
