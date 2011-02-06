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

#ifndef XPCC__ATXMEGA_ADC_HPP
#define XPCC__ATXMEGA_ADC_HPP

#include <avr/io.h>
#include <xpcc/architecture/driver/atomic/lock.hpp>

namespace xpcc
{
    /**
	 * \brief	Analog/Digital Converter
	 *
	 * ADC clock frequency should be at around 62kHz for maximum
	 * resolution. If less than 12 bits are needed the frequency can be higher.
     * A differential input in signed mode increases accuracy by up to 6 bit (!).
     * \sa http://blog.frankvh.com/2010/01/03/atmel-xmega-adc-problems-solutions/
	 *
     * The Xmega ADC is more complex than the Mega ADC, and each ADC Module has
     * four ADC Channels which have to a seperate class.
     * You first have to initialize the ADC Module to use any of the Channels.
     *
	 * \code
     * #include <xpcc/architecture/driver/atxmega/xadc.hpp>
     *
     * int main {
     *     xpcc::xAdc adcA(ADCA);
     *
     *     adcA.initialize(ADC_REFSEL_AREFA_gc);    // PortA AVcc
     *     adcA.setConversionMode(true);            // Signed conversion
     * }
     * \endcode
	 *
     * \sa AVR1300
	 * \author Niklas Hauser
	 * \ingroup	atxmega
	 */
	class xAdc
	{
	public:
        xAdc(ADC_t &module);
        
        void
        initialize(uint8_t reference=ADC_REFSEL_VCC_gc)
        {
            this->initialize(reference, ADC_PRESCALER_DIV512_gc);
        }
        
        void
        initialize(uint8_t reference, ADC_PRESCALER_t prescaler);
        
        /// Enable the ADC module.
        void
        setEnable(bool);
        
        /// Flush the ADC pipeline.
        void
        flush();
        
        void
        setDMARequest(ADC_DMASEL_t);
        
        /// Set signed or unsigned conversion.
        void
        setConversionMode(bool);
        
        /// Enable free running mode as defined in \c setChannelSweep.
        void
        setFreeRunningMode(bool);
        
        void
        setResolution(ADC_RESOLUTION_t);
        
        void
        setReference(uint8_t);
        
        void
        setChannelEvent(uint8_t);
        
        void
        setChannelSweep(ADC_SWEEP_t);
        
        void
        setPrescaler(ADC_PRESCALER_t);
        
    private:
        ADC_t &module;
	};
    
    /**
	 * \brief	Analog/Digital Converter Channel
     *
     * To use a Channel, intialize the according ADC Module first.
     * Select either a single ended or differential input mode. Check the
     * conversion mode bit in the ADC Module for a signed conversion.
     * Then select the negative and/or positive inputs for the channel.
     *
     * \code
     * #include <xpcc/architecture/driver/atxmega/xadc.hpp>
     * 
     * int main {
     *     xpcc::xAdc adcA(ADCA);
     *
     *     adcA.initialize(ADC_REFSEL_AREFA_gc);    // PortA AVcc
     *     adcA.setConversionMode(true);            // Signed conversion
     *
     *     xpcc::xAdcChannel myChannel(ADCA.CH0);
     *
     *     myChannel.setInputMode(ADC_CH_INPUTMODE_DIFF_gc);
     *     myChannel.setMux(ADC_CH_MUXPOS_PIN4_gc|ADC_CH_MUXNEG_PIN3_gc);
     * }
     * \endcode
	 *
     * \sa AVR1300
	 * \author Niklas Hauser
	 * \ingroup	atxmega
	 */
    class xAdcChannel
    {
    public:
        xAdcChannel(ADC_CH_t&);
        
        /// Select between single ended, differential (with gain)
        void
        setInputMode(ADC_CH_INPUTMODE_t);
        
        /// Select a gain factor for the channel
        void
        setGainFactor(ADC_CH_GAIN_t);
        
        /// Select the pos. and neg. input pins for the channel.
        void
        setMux(uint8_t);
        
        void
        enableInterrupt(uint8_t);
        
        /// Read the value an analog channel
        uint16_t
		readChannel();
        
        /// Start a new conversion
        void
        startConversion();
        
        /// Check if the conversion is finished
        bool
		isFinished();
        
        /// Read the converted analog value
        uint16_t
		getValue();
        
    private:
        ADC_CH_t &channel;
    };
}

#endif	// XPCC__ATXMEGA_ADC_HPP
