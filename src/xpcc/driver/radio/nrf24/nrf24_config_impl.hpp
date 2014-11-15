// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_CONFIG_HPP
#   error "Don't include this file directly, use 'nrf24_config.hpp' instead!"
#endif

#include "nrf24_config.hpp"

using namespace xpcc::nrf24;

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::powerUp()
{
    Nrf24Phy::setBits(Register::CONFIG, Config::PWR_UP);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::powerDown()
{
    Nrf24Phy::clearBits(Register::CONFIG, Config::PWR_UP);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setChannel(uint8_t channel)
{
    Nrf24Phy::writeRegister(Register::RF_CH, channel);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setMode(Mode mode)
{
    if(mode == Mode::Rx)
    {
        Nrf24Phy::setBits(Register::CONFIG, Config::PRIM_RX);
    } else
    {
        Nrf24Phy::clearBits(Register::CONFIG, Config::PRIM_RX);
    }
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setSpeed(Speed speed)
{
    if(speed == Speed::kBps250)
    {
        Nrf24Phy::clearBits(Register::RF_SETUP, (uint8_t)RfSetup::RF_DR_HIGH);
        Nrf24Phy::setBits(Register::RF_SETUP, (uint8_t)RfSetup::RF_DR_LOW);
    }
    else if(speed == Speed::MBps1)
    {
        Nrf24Phy::clearBits(Register::RF_SETUP, (uint8_t)RfSetup::RF_DR_LOW);
        Nrf24Phy::clearBits(Register::RF_SETUP, (uint8_t)RfSetup::RF_DR_HIGH);
    }
    else if(speed == Speed::MBps1)
    {
        Nrf24Phy::setBits(Register::RF_SETUP, (uint8_t)RfSetup::RF_DR_HIGH);
        Nrf24Phy::clearBits(Register::RF_SETUP, (uint8_t)RfSetup::RF_DR_LOW);
    }
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void xpcc::Nrf24Config<Nrf24Phy>::setCrc(Crc crc)
{
    if(crc == Crc::NoCrc)
    {
        Nrf24Phy::clearBits(Register::CONFIG, Config::EN_CRC);

    } else
    {
        Nrf24Phy::setBits(Register::CONFIG, Config::EN_CRC);

        if (crc == Crc::Crc1Byte)
        {
            Nrf24Phy::clearBits(Register::CONFIG, Config::CRC0);
        }
        else if (crc == Crc::Crc2Byte)
        {
            Nrf24Phy::setBits(Register::CONFIG, Config::CRC0);
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setAddressWidth(AddressWidth width)
{
    Nrf24Phy::writeRegister(Register::SETUP_AW, static_cast<uint8_t>(width));
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setRfPower(RfPower power)
{
    Nrf24Phy::writeRegister(Register::RF_SETUP, static_cast<uint8_t>(power) << 1);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setAutoRetransmitDelay(AutoRetransmitDelay delay)
{
    Nrf24Phy::writeRegister(Register::SETUP_RETR, static_cast<uint8_t>(delay) << 4);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setAutoRetransmitCount(AutoRetransmitCount count)
{
    Nrf24Phy::writeRegister(Register::SETUP_RETR, static_cast<uint8_t>(count));
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::enableFeatureNoAck()
{
    Nrf24Phy::setBits(Register::FEATURE, Feature::EN_DYN_ACK);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::disableFeatureNoAck()
{
    Nrf24Phy::clearBits(Register::FEATURE, Feature::EN_DYN_ACK);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::enablePipe(nrf24::Pipe pipe, bool enableAutoAck)
{

    uint16_t payload_length = Nrf24Phy::getPayloadLength();


    /* Set payload width for pipe */
    if(pipe == Pipe::Pipe0)
        Nrf24Phy::writeRegister(nrf24::Register::RX_PW_P0, payload_length);
    else if(pipe == Pipe::Pipe1)
        Nrf24Phy::writeRegister(nrf24::Register::RX_PW_P1, payload_length);
    else if(pipe == Pipe::Pipe2)
        Nrf24Phy::writeRegister(nrf24::Register::RX_PW_P2, payload_length);
    else if(pipe == Pipe::Pipe3)
        Nrf24Phy::writeRegister(nrf24::Register::RX_PW_P3, payload_length);
    else if(pipe == Pipe::Pipe4)
        Nrf24Phy::writeRegister(nrf24::Register::RX_PW_P4, payload_length);
    else if(pipe == Pipe::Pipe5)
        Nrf24Phy::writeRegister(nrf24::Register::RX_PW_P5, payload_length);

    /* Enable or disable auto acknowledgement for this pipe */
    if(enableAutoAck)
    {
        Nrf24Phy::setBits(nrf24::Register::EN_AA, static_cast<uint8_t>(pipe));
    } else
    {
        Nrf24Phy::clearBits(nrf24::Register::EN_AA, static_cast<uint8_t>(pipe));
    }

    /* enable pipe */
    Nrf24Phy::setBits(nrf24::Register::EN_RX_ADDR, static_cast<uint8_t>(pipe));
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::disablePipe(nrf24::Pipe pipe)
{
    /* DISABLE pipe */
    Nrf24Phy::clearBits(nrf24::Register::EN_RX_ADDR, static_cast<uint8_t>(pipe));
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
uint8_t
xpcc::Nrf24Config<Nrf24Phy>::getPayloadPipe()
{
    uint8_t status = Nrf24Phy::readStatus();

    return ((status & Status::RX_P_NO) >> 1);
}
