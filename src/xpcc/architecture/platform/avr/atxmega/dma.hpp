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
 * 
 * $Id: dma.hpp 751 2012-03-02 21:38:40Z salkinium $
 */		
// ----------------------------------------------------------------------------

/**
 * \ingroup		atxmega
 * \defgroup	atxmega_dma	Direct Memory Access Controller
 * 
 * The XMEGA has a Direct Memory Access (DMA) Controller to move data between
 * memories and peripherals in the data space. The DMA controller uses the same
 * data bus as the CPU to transfer data.
 *
 * It has 4 channels that can be configured independently. Each DMA channel can
 * perform data transfers in blocks of configurable size from 1 to 64K bytes. A
 * repeat counter can be used to repeat each block transfer for single
 * transactions up to 16M bytes. Each DMA channel can be configured to access
 * the source and destination memory address with incrementing, decrementing or
 * static addressing. The addressing is independent for source and destination
 * address. When the transaction is complete the original source and destination
 * address can automatically be reloaded to be ready for the next transaction.
 *
 * The DMAC can access all the peripherals through their I/O memory registers,
 * and the DMA may be used for automatic transfer of data to/from communication
 * modules, as well as automatic data retrieval from ADC conversions, data
 * transfer to DAC conversions, or data transfer to or from port pins. A wide
 * range of transfer triggers is available from the peripherals, Event System
 * and software. Each DMA channel has different transfer triggers.
 *
 * \author Niklas Hauser
 */

#include "dma/dmac.hpp"
#include "dma/dma_channel0.hpp"
#include "dma/dma_channel1.hpp"
#include "dma/dma_channel2.hpp"
#include "dma/dma_channel3.hpp"
