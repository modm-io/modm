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
#include "adc.hpp"

// ----------------------------------------------------------------------------
xpcc::xAdc::xAdc(ADC_t &module)
: module(module)
{
}

void
xpcc::xAdc::initialize(uint8_t reference, ADC_PRESCALER_t prescaler)
{
    module.CTRLA = ADC_ENABLE_bm;
    module.REFCTRL = reference;
    module.PRESCALER = prescaler;
}

void
xpcc::xAdc::setEnable(bool enable)
{
    module.CTRLA = (module.CTRLA & ~ADC_ENABLE_bm) | (enable?ADC_ENABLE_bm:0);
}

void
xpcc::xAdc::flush()
{
    module.CTRLA |= ADC_FLUSH_bm;
}

void
xpcc::xAdc::setDMARequest(ADC_DMASEL_t selection)
{
    module.CTRLA = (module.CTRLA & ~ADC_DMASEL_gm) | selection;
}

void
xpcc::xAdc::setConversionMode(bool unsgnd)
{
    module.CTRLB = (module.CTRLB & ~ADC_CONMODE_bm) | (unsgnd?ADC_CONMODE_bm:0);
}

void
xpcc::xAdc::setFreeRunningMode(bool enable)
{
    module.CTRLB = (module.CTRLB & ~ADC_FREERUN_bm) | (enable?ADC_FREERUN_bm:0);
}

void
xpcc::xAdc::setResolution(ADC_RESOLUTION_t resolution)
{
    module.CTRLB = (module.CTRLB & ~ADC_RESOLUTION_gm) | resolution;
}

void
xpcc::xAdc::setReference(uint8_t reference)
{
    module.REFCTRL = reference;
}

void
xpcc::xAdc::setChannelEvent(uint8_t mode)
{
    module.EVCTRL = (module.EVCTRL & ~(ADC_EVACT_gm|ADC_EVSEL_gm)) | mode;
}

void
xpcc::xAdc::setChannelSweep(ADC_SWEEP_t sweep)
{
    module.EVCTRL = (module.EVCTRL & ~ADC_SWEEP_gm) | sweep;
}

void
xpcc::xAdc::setPrescaler(ADC_PRESCALER_t prescaler)
{
    module.PRESCALER = (module.PRESCALER & ~ADC_PRESCALER_gm) | prescaler;
}


// ----------------------------------------------------------------------------
xpcc::xAdcChannel::xAdcChannel(ADC_CH_t &channel)
: channel(channel)
{
}

void
xpcc::xAdcChannel::setInputMode(ADC_CH_INPUTMODE_t mode)
{
    channel.CTRL = (channel.CTRL & ~ADC_CH_INPUTMODE_gm) | mode;
}

void
xpcc::xAdcChannel::setGainFactor(ADC_CH_GAIN_t mode)
{
    channel.CTRL = (channel.CTRL & ~ADC_CH_GAINFAC_gm) | mode;
}

void
xpcc::xAdcChannel::startConversion()
{
    channel.INTFLAGS = 0;
    channel.CTRL |= ADC_CH_START_bm;
}

void
xpcc::xAdcChannel::setMux(uint8_t selection)
{
    channel.MUXCTRL = selection;
}

void
xpcc::xAdcChannel::enableInterrupt(uint8_t level)
{
    channel.INTCTRL = level;
}

uint16_t
xpcc::xAdcChannel::readChannel()
{
    startConversion();
    while (!isFinished())
        ;   // wait until the conversion is finished
    return getValue();
}

bool
xpcc::xAdcChannel::isFinished()
{
    return (channel.INTFLAGS & ADC_CH_CHIF_bm);
}

uint16_t
xpcc::xAdcChannel::getValue()
{
    {
        xpcc::atomic::Lock lock;
        return channel.RES;
    }
}

