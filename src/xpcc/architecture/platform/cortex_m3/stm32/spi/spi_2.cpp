// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#include <libmaple/spi.h>

#define	LIBMAPLE
#include "../gpio.hpp"

#include "spi_2.hpp"



namespace
{
	GPIO__OUTPUT(Cs, B, 12);
	GPIO__OUTPUT(Sck, B, 13);
	GPIO__INPUT(Miso, B, 14);
	GPIO__OUTPUT(Mosi, B, 15);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Spi2::initialize(Mode mode, Prescaler prescaler)
{
	Cs::setOutput(xpcc::stm32::ALTERNATE, xpcc::stm32::PUSH_PULL);
	Cs::set();
	Sck::setOutput(xpcc::stm32::ALTERNATE, xpcc::stm32::PUSH_PULL);
	Miso::setInput(xpcc::stm32::INPUT, xpcc::stm32::FLOATING);
	Mosi::setOutput(xpcc::stm32::ALTERNATE, xpcc::stm32::PUSH_PULL);
	
	// Software slave management
	uint32_t flags = SPI_DFF_8_BIT | SPI_SW_SLAVE | SPI_SOFT_SS | SPI_FRAME_MSB;
	
	spi_init(SPI2);
	
	spi_irq_disable(SPI2, SPI_INTERRUPTS_ALL);
    spi_peripheral_disable(SPI2);
    SPI2->regs->CR1 = prescaler | mode | SPI_CR1_MSTR | flags;
    spi_peripheral_enable(SPI2);
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::Spi2::write(uint8_t data)
{
	while (!spi_is_tx_empty(SPI2)) {
		// wait until the previous transmission is finished
	}
	
	spi_tx_reg(SPI2, data);
	
	while (!spi_is_rx_nonempty(SPI2)) {
		// wait until the data is received
	}
	
	return spi_rx_reg(SPI2);
}

