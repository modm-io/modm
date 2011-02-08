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
#include <stdint.h>

#if __DOXYGEN__
/**
 * Analog/Digital Converter
 *
 * ADC clock frequency should be at around 62kHz for maximum
 * resolution. If less than 12 bits are needed the frequency can be higher.
 * A differential input in signed mode increases accuracy by up to 6 bit (!).
 * \sa <a href="http://blog.frankvh.com/2010/01/03/atmel-xmega-adc-problems-solutions/">
 *                                  Xmega ADC Problems and Solutions</a>
 *
 * The Xmega ADC is more complex than the Mega ADC, and each ADC Module has
 * four ADC Channels which have a seperate class.
 * You first have to initialize the ADC Module to use any of the Channels.
 *
 * \code
 * #include <xpcc/architecture/driver/atxmega/adc.hpp>
 *
 * int main {
 *     ...
 *     ADC_MODULE(adcA, ADCA);
 *
 *     adcA::initialize(ADC_REFSEL_AREFA_gc);    // PortA AVcc
 *     adcA::setConversionMode(true);            // Signed conversion mode
 *     ...
 * }
 * \endcode
 *
 * Create a new class with the following methods:
 * \code
 * class name
 * {
 * public:
 *     static inline ADC_t
 *     &getModule();
 *
 *     static inline void
 *     initialize(uint8_t reference=ADC_REFSEL_VCC_gc)
 *
 *     static inline void
 *     initialize(uint8_t reference, ADC_PRESCALER_t prescaler);
 *
 *     // Enable the ADC module.
 *     static inline void
 *     setEnable(bool);
 *
 *     // Flush the ADC pipeline.
 *     static inline void
 *     flush();
 *
 *     static inline void
 *     setDMARequest(ADC_DMASEL_t);
 *
 *     // Set signed or unsigned conversion.
 *     static inline void
 *     setConversionMode(bool);
 *
 *     // Enable free running mode as defined in setChannelSweep.
 *     static inline void
 *     setFreeRunningMode(bool);
 *
 *     static inline void
 *     setResolution(ADC_RESOLUTION_t);
 *
 *     static inline void
 *     setReference(uint8_t);
 *
 *     static inline void
 *     setChannelEvent(uint8_t);
 *
 *     static inline void
 *     setChannelSweep(ADC_SWEEP_t);
 *
 *     static inline void
 *     setPrescaler(ADC_PRESCALER_t);
 * };
 * \endcode
 *
 * \sa AVR1300
 * \sa ADC_CHANNEL
 * \author Niklas Hauser
 * \ingroup	atxmega
 */
#define	ADC_MODULE(name, module)

/**
 * Analog/Digital Converter Channel
 *
 * To use a Channel, intialize the according ADC Module first.
 * Select either a single ended or differential input mode. Check the
 * conversion mode bit in the ADC Module for a signed conversion.
 * Then select the negative and/or positive inputs for the channel.
 *
 * \code
 * #include <xpcc/architecture/driver/atxmega/adc.hpp>
 * 
 * int main {
 *     ...
 *     ADC_MODULE(adcA,ADCA);
 *
 *     adcA::initialize(ADC_REFSEL_AREFA_gc);    // PortA AVcc
 *     adcA::setConversionMode(true);            // Signed conversion mode
 *
 *     ADC_CHANNEL(myChannel, ADCA.CH0);   // Channel Zero of ADCA
 *     // Differential input mode
 *     myChannel::setInputMode(ADC_CH_INPUTMODE_DIFF_gc);
 *     myChannel::setMux(ADC_CH_MUXPOS_PIN4_gc|ADC_CH_MUXNEG_PIN3_gc);
 *     // The result is always unsigned, so you want to cast it in signed mode
 *     int16_t result = (int16_t) myChannel::read();
 *     ...
 * }
 * \endcode
 *
 * Create a new class with the following methods:
 * \code
 * class name
 * {
 * public:
 *     static inline ADC_CH_t
 *     &getChannel();
 *
 *     // Select between single ended, differential (with gain)
 *     static inline void
 *     setInputMode(ADC_CH_INPUTMODE_t);
 *
 *     // Select a gain factor for the channel
 *     static inline void
 *     setGainFactor(ADC_CH_GAIN_t);
 *
 *     // Select the pos. and neg. input pins for the channel.
 *     static inline void
 *     setMux(uint8_t);
 *
 *     static inline void
 *     enableInterrupt(uint8_t);
 *
 *     // Read the value an analog channel
 *     static inline uint16_t
 *     read();
 *
 *     // Start a new conversion
 *     static inline void
 *     startConversion();
 *
 *     // Check if the conversion is finished
 *     static inline bool
 *     isFinished();
 *
 *     // Read the converted analog value
 *     static inline uint16_t
 *     getValue();
 * };
 * \endcode
 *
 * \sa AVR1300
 * \sa ADC_MODULE
 * \author Niklas Hauser
 * \ingroup	atxmega
 */
#define	ADC_CHANNEL(name, channel)

#else   // !__DOXYGEN__

#define	ADC_MODULE(name, module) \
    struct name { \
        ALWAYS_INLINE static ADC_t &getModule() { return module; } \
        ALWAYS_INLINE static void setEnable(bool enable=true) { \
            module.CTRLA = (module.CTRLA & ~ADC_ENABLE_bm) | (enable?ADC_ENABLE_bm:0); } \
        ALWAYS_INLINE static void flush() { module.CTRLA |= ADC_FLUSH_bm; } \
        ALWAYS_INLINE static void setDMARequest(ADC_DMASEL_t selection=ADC_DMASEL_OFF_gc) { \
            module.CTRLA = (module.CTRLA & ~ADC_DMASEL_gm) | selection; } \
        ALWAYS_INLINE static void setConversionMode(bool unsgnd=true) { \
            module.CTRLB = (module.CTRLB & ~ADC_CONMODE_bm) | (unsgnd?ADC_CONMODE_bm:0); } \
        ALWAYS_INLINE static void setFreeRunningMode(bool enable=true) { \
            module.CTRLB = (module.CTRLB & ~ADC_FREERUN_bm) | (enable?ADC_FREERUN_bm:0); } \
        ALWAYS_INLINE static void setResolution(ADC_RESOLUTION_t resolution=ADC_RESOLUTION_12BIT_gc) { \
            module.CTRLB = (module.CTRLB & ~ADC_RESOLUTION_gm) | resolution; } \
        ALWAYS_INLINE static void setReference(uint8_t reference=ADC_REFSEL_VCC_gc) { \
            module.REFCTRL = reference; } \
        ALWAYS_INLINE static void setChannelEvent(uint8_t mode) { \
            module.EVCTRL = (module.EVCTRL & ~(ADC_EVACT_gm|ADC_EVSEL_gm)) | mode; } \
        ALWAYS_INLINE static void setChannelSweep(ADC_SWEEP_t sweep) { \
            module.EVCTRL = (module.EVCTRL & ~ADC_SWEEP_gm) | sweep; } \
        ALWAYS_INLINE static void setPrescaler(ADC_PRESCALER_t prescaler=ADC_PRESCALER_DIV512_gc) { \
            module.PRESCALER = (module.PRESCALER & ~ADC_PRESCALER_gm) | prescaler; } \
        \
        ALWAYS_INLINE static void initialize(uint8_t reference=ADC_REFSEL_VCC_gc) { \
            module.CTRLA = ADC_ENABLE_bm; \
            module.REFCTRL = reference; \
            module.PRESCALER = ADC_PRESCALER_DIV512_gc; } \
        ALWAYS_INLINE static void initialize(uint8_t reference, ADC_PRESCALER_t prescaler) { \
            module.CTRLA = ADC_ENABLE_bm; \
            module.REFCTRL = reference; \
            module.PRESCALER = prescaler; } \
    };

#define	ADC_CHANNEL(name, channel) \
    struct name { \
        ALWAYS_INLINE static ADC_CH_t &getChannel() { return channel; } \
        ALWAYS_INLINE static void setInputMode(ADC_CH_INPUTMODE_t mode=ADC_CH_INPUTMODE_SINGLEENDED_gc) { \
            channel.CTRL = (channel.CTRL & ~ADC_CH_INPUTMODE_gm) | mode; } \
        ALWAYS_INLINE static void setGainFactor(ADC_CH_GAIN_t factor=ADC_CH_GAIN_1X_gc) { \
            channel.CTRL = (channel.CTRL & ~ADC_CH_GAINFAC_gm) | factor; } \
        ALWAYS_INLINE static void setMux(uint8_t selection) { \
            channel.MUXCTRL = selection; } \
        ALWAYS_INLINE static void enableInterrupt(uint8_t level=ADC_CH_INTLVL_OFF_gc) { \
            channel.INTCTRL = level; } \
        ALWAYS_INLINE static uint16_t read() { \
            channel.INTFLAGS = 0; \
            channel.CTRL |= ADC_CH_START_bm; \
            while(!(channel.INTFLAGS & ADC_CH_CHIF_bm)); \
            return channel.RES; } \
        ALWAYS_INLINE static void startConversion() { \
            channel.INTFLAGS = 0; \
            channel.CTRL |= ADC_CH_START_bm; } \
        ALWAYS_INLINE static bool isFinished() { \
            return (channel.INTFLAGS & ADC_CH_CHIF_bm); } \
        ALWAYS_INLINE static uint16_t getValue() { \
            return channel.RES; } \
    };

#endif  // __DOXYGEN__
#endif	// XPCC__ATXMEGA_ADC_HPP