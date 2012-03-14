// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SPI__MASTER_HPP
#define XPCC_SPI__MASTER_HPP

#include <stdint.h>
#include <xpcc/driver/interface.hpp>

namespace xpcc
{
	/**
	 * \brief		Interface of SPI Master
	 * 
	 * The design of SPI is very simple, therefore very fast clock speed and
	 * little overhead are features of SPI data transfers.
	 * 
	 * However, the fast clock speeds make it unreasonable to use an interrupt
	 * based approach to shifting out each byte of the data, since the interrupt
	 * handling might decrease performance over busy waiting especially for
	 * targets operating on low frequencies. 
	 * 
	 * Therefore, no interrupt-based asynchronous operation is implemented,
	 * however, depending on the target architecture, DMA support for all
	 * operations can be activated by providing certain definitions.
	 * 
	 * In case of DMA support the interface operation remains the same, however
	 * the transfer() function has an option to return directly after the call
	 * or wait for the DMA to finish the operation.
	 * Synchronous operation with DMA support can be more efficient, since the
	 * CPU can continue to work with values stored in CPU registers.
	 * However DMA support is only implemented for transfers between memory and
	 * SPI peripheral, so there is no peripheral-peripheral transfer support.
	 * 
	 * \author		Niklas Hauser
	 * \ingroup		spi
	 */
	class SpiMaster : public Interface
	{
	public:
		/// initiates a SPI transfer and returns the received byte
		static uint8_t
		write(uint8_t data);
		
		/**
		 * Set the data buffers and data length.
		 * 
		 * \param	length		length of buffer or number of dummy bytes to be send
		 * \param	transmit	pointer to transmit buffer, set to zero to send dummy bytes
		 * \param	receive		pointer to receive buffer, if set to zero transmit buffer is used
		 * \param	transmitIncr	increment or decrement transmit buffer
		 * \param	receiveIncr		increment or decrement receive buffer
		 * \return	\c true		if buffer has been successfully copied
		 */
		static bool
		setBuffer(uint16_t length,
				  uint8_t* transmit=0, uint8_t* receive=0,
				  bool transmitIncr=true, bool receiveIncr=true);
		
		/**
		 * Request a transfer, not buffered, blocking if not using DMA.
		 * 
		 * \param	send	set to false to send dummy data (0xff)
		 * \param	receive	set to true to write received data back in buffer.
		 * \param	wait	set to true to wait until (DMA) operation finished
		 * \return	\c true if request was successfully serviced
		 */
		static bool
		transfer(bool send=true, bool receive=false, bool wait=true);
		
		/// \return \c true if any DMA operation finished using the SPI peripheral
		static bool
		isFinished();
	};
}

#endif // XPCC_SPI__MASTER_HPP
