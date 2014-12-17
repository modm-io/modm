// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_PHY_HPP
#define XPCC__NRF24_PHY_HPP

#include <stdint.h>

#include "nrf24_definitions.hpp"

namespace xpcc
{

//namespace nrf24
//{

/**
* @brief Hardware abstraction layer for nRF24L01+
*
*
* @ingroup  radio
* @author   Daniel Krebs
*/

template <typename Spi, typename Csn, typename Ce>
class Nrf24Phy : public Nrf24Register
{

public:

    /** @brief Initialize nRF24-HAL
     *
     * Call this function before using this class!
     *
     * @param payload_length    Set fixed payload length in bytes , set 0 for dynamic payload length
     *
     * Note: Dynamic payload length is *not yet implemented*
     */
    static void
    initialize(uint8_t payload_length = max_payload_length)
    {
        // When payload length is configured to 0, length will be polled every
        // time a packet is fetched from Rx Fifo

        /* For now always use fixed payload size */
        payload_len = (payload_length == 0) ? max_payload_length : payload_length;
    }

    /** @brief Get the maximum payload size the hardware can transmit in one packet
     */
    static uint16_t
    getMaxPayload()
    {
        return max_payload_length;
    }


    /** @brief Read simple 8 bit register
     */
    static uint8_t
    readRegister(NrfRegister_t reg);


    /** @brief Write simple 8 bit register
     */
    static void
    writeRegister(NrfRegister_t, uint8_t data);


    /** @brief Set bits inside a register
     *
     * Only set individual bits and leave the rest untouched
     *
     * @param reg   Register where to set bits
     * @param flags Which bits to set
     */
    static void
    setBits(NrfRegister_t reg, Flags_t flags);


    /** @brief Clear bits inside a register
     *
     * Only clear individual bits and leave the rest untouched.
     *
     * Note: To clear RX_DR, TX_DS, MAX_RT you must use setBits() !
     *
     * @param reg   Register where to clear bits
     * @param flags Which bits to set
     */
    static void
    clearBits(NrfRegister_t reg, Flags_t flags);

    /** @brief Read received payload
     *
     * Used in RX mode.
     * Payload is deleted from FIFO after it is read.
     *
     *  @param buffer   buffer where to put payload, should be 32 byte wide
     *  @return         length of received payload
     */
    static uint8_t
    readRxPayload(uint8_t* buffer);


    /** @brief Write payload to be send
     *
     *  @param buffer   Buffer from where to read the payload
     *  @param len      How many bytes the payload is wide
     */
    static void
    writeTxPayload(uint8_t* buffer, uint8_t len);


    /** @brief Same as writeTxPayload() but disable auto ACK for this packet
     */
    static void
    writeTxPayloadNoAck(uint8_t* buffer, uint8_t len);


    /** @brief Write payload to be transmitted together with ACK packet
     *
     * Used in RX mode.
     * Maximum three ACK packet payloads can be pending. Payloads with
     * same Pipe are handled using first in - first out principle. Write
     * payload: 1– 32 bytes. A write operation always starts at byte 0.
     *
     * @param pipe Pipe for which the ACK payload is destined, must be in range 0 to 5
     * @param buffer Buffer from where to read the payload
     * @param len How many bytes the payload is wide
     */
    static void
    writeAckPayload(Pipe_t pipe, uint8_t* buffer, uint8_t len);


    /** @brief Send a high pulse of 10us length on Ce pin (blocking)
     *
     */
    static void
    pulseCe();

    /** @brief Set Ce pin high
     *
     *  If Ce was high before the pin won't be set low before waiting for 10us.
     */
    static void
    setCe();

    /** @brief Set Ce pin low
     *
     */
    static void
    resetCe();

    /**@brief Flush Tx Fifo
     *
     * Used in Tx mode
     */
    static void
    flushTxFifo();


    /** @brief Flush Rx Fifo
     *
     * Used in Rx mode.
     * Should not be executed during transmission of
     * acknowledge, that is, acknowledge package will
     * not be completed.
     */
    static void
    flushRxFifo();


    /** @brief Reuse last transmitted payload
     *
     * Used in PTX mode.
     * TX payload reuse is active until W_TX_PAYLOAD or FLUSH TX is
     * executed. TX payload reuse must not be activated or deactivated
     * during package transmission.
     */
    static void
    reuseTxPayload();


    /** @brief Read Rx payload width for top of Rx Fifo
     *
     * Note: Flush RX FIFO if the read value is larger than 32 bytes.
     */
    static uint8_t
    readRxPayloadWidth();


    /** @brief Read new status
     *
     * Note: status will be automatically updated every time a command is
     *       issued, so it might not be necessary to call this explicitly.
     *
     *  @return Status register
     */
    static uint8_t
    readStatus();


    /** @brief Set Rx address for a pipe
     *
     *  Note: pipe 0 and pipe 1 have a 5 byte wide address while pipes 2 to 5
     *        share the upper 4 bytes with pipe 1, so when setting the address
     *        of a pipes 2 to 5, only 1 byte (LSB) will be written to SPI.
     *
     *  @param pipe     Pipe number, must be in range 0 to 5
     *  @param address  Address for which packets will be put into pipe, see
     *                  description concerning differences by pipe
     */
    static void
    setRxAddress(Pipe_t pipe, uint64_t address);


    /** @brief Set Tx address
     *
     * Used in PTX mode only.
     * Set RX_ADDR_P0 equal to this address to handle
     * automatic acknowledge if this is a PTX device with
     * Enhanced ShockBurst enabled.
     *
     * @param address   Address where to send packets
     */
    static void
    setTxAddress(uint64_t address);


    /** @brief Get Rx address of pipe
     *
     *  @param pipe     Pipe number
     *  @return         Address set for pipe
     */
    static uint64_t
    getRxAddress(Pipe_t pipe);


    /** @brief Get Tx address
     */
    static uint64_t
    getTxAddress();


    /** @brief
     *
     */
    static uint16_t
    getPayloadLength()
    {
        return payload_len;
    }


private:
    static uint8_t
    writeCommandSingleData(Command_t cmd, uint8_t data);

    static void
    writeCommandNoData(Command_t cmd);

    /** @brief Read and write multiple bytes via SPI
     *
     * Supplying NULL as argv or retv is allowed.
     *  argv = nullptr  -> 0x00 is send
     *  retv = nullptr  -> return values are discarded
     */
    static void
    writeCommandMultiData(Command_t cmd, uint8_t* argv, uint8_t* retv, uint8_t argc);
/*
    static Command_t
    toCommand(nrf24::Command cmd, nrf24::Register reg, uint8_t offset)
    {
        return static_cast<nrf24::Command>((cmd | reg) + offset);
    }

    static nrf24::Command
    toCommand(nrf24::Command cmd, uint8_t offset)
    {
        return static_cast<nrf24::Command>(cmd + offset);
    }

    static nrf24::Register
    toRegister(nrf24::Register reg, uint8_t offset)
    {
        return static_cast<nrf24::Register>(reg + offset);
    }
*/

private:
    static constexpr uint8_t max_payload_length = 32;
    static constexpr uint8_t address_size = 5;          /** Size of Rx/Tx addresses in bytes */
    static constexpr uint8_t rx_pipe_count = 6;         /** Number of Rx pipes (0 to 5) */
    static uint8_t status;
    static uint8_t payload_len;                         /** Fixed length of payload in bytes. 0 means dynamic payload*/

};

//} // namespace nrf24

} // namespace xpcc

#include "nrf24_phy_impl.hpp"

#endif
