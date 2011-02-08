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
 * 
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__XMEGA_SPI_HPP
#define XPCC__XMEGA_SPI_HPP

#include <avr/io.h>
#include <stdint.h>

#if __DOXYGEN__

/**
 * SPI Master Module
 *
 * This SPI hardware implementation is compatible with both Software SPI as
 * well as Uart SPI.
 * The write method is blocking until the SPI Interrupt Flag is set (=> SPI
 * transfer is complete).
 * It can be used for higher SPI clock speeds than feaseable with Software SPI.
 *
 * \bug Loss of GPIO functionality on Slave Select Pin (Pin 4). This pin must be
 *      pulled high even when configured as an output, contrary to the manual.
 *
 * \code
 * #include <xpcc/architecture/driver/atxmega/spi.hpp>
 *
 * int main {
 *     ...
 *     SPI_MASTER(Spi, E, SPI_PRESCALER_DIV64_gc);
 *
 *     Spi::initialize();
 *     uint8_t result = Spi::write(0xf3);   // write a byte
 *     ...
 * }
 * \endcode
 *
 * Create a new class with the following methods:
 * \code
 * class name
 * {
 * public:
 *     static inline SPI_t
 *     &getModuleBase();
 *
 *     // Enable the SPI module.
 *     static inline void
 *     setEnable(bool);
 *
 *     static inline void
 *     setMaster(bool);
 *
 *     static inline void
 *     setPrescaler(SPI_PRESCALER_t);
 *
 *     static inline void
 *     setDoubleSpeed(bool);
 *
 *     static inline void
 *     setDataOrder(bool);
 *
 *     static inline void
 *     setMode(SPI_MODE_t);
 *
 *     static inline void
 *     enableInterrupt(SPI_INTLVL_t);
 *
 *     // clears interrupt flag and returns true if it was set
 *     static inline void
 *     isInterrupt();
 *
 *     // initiates a SPI transfer and returns the received byte
 *     static inline uint8_t
 *     write(uint8_t);
 *
 *     static inline void
 *     initialize(SPI_MODE_t mode, SPI_PRESCALER_t _prescaler);
 *
 *     static inline void
 *     initialize(SPI_MODE_t mode = SPI_MODE_0_gc);
 * };
 * \endcode
 *
 * \sa AVR1309
 * \sa xpcc::SoftwareSpi
 * \author Niklas Hauser
 * \ingroup	atxmega
 */
#define	SPI_MASTER(name, port, prescaler)

#else   // !__DOXYGEN__

#define	SPI_MASTER(name, port, prescaler)\
    struct name { \
        ALWAYS_INLINE static SPI_t &getModuleBase() { return SPI ## port; } \
        ALWAYS_INLINE static void setEnable(bool enable) { SPI ## port ## _CTRL = (SPI ## port ## _CTRL & ~SPI_ENABLE_bm) | (enable?SPI_ENABLE_bm:0); } \
        ALWAYS_INLINE static void setMaster(bool master) { SPI ## port ## _CTRL = (SPI ## port ## _CTRL & ~SPI_MASTER_bm) | (master?SPI_MASTER_bm:0); } \
        ALWAYS_INLINE static void setPrescaler(SPI_PRESCALER_t prescaler) { SPI ## port ## _CTRL = (SPI ## port ## _CTRL & ~SPI_PRESCALER_gm) | prescaler; } \
        ALWAYS_INLINE static void setDoubleSpeed(bool doubleSpeed) { SPI ## port ## _CTRL = (SPI ## port ## _CTRL & ~SPI_CLK2X_bm) | (doubleSpeed?SPI_CLK2X_bm:0); } \
        ALWAYS_INLINE static void setDataOrder(bool LSBFirst) { SPI ## port ## _CTRL = (SPI ## port ## _CTRL & ~SPI_DORD_bm) | (LSBFirst?SPI_DORD_bm:0); } \
        ALWAYS_INLINE static void setMode(SPI_MODE_t mode) { SPI ## port ## _CTRL = (SPI ## port ## _CTRL & ~SPI_MODE_gm) | mode; } \
        ALWAYS_INLINE static void enableInterrupt(SPI_INTLVL_t level) { SPI ## port ## _INTCTRL = (SPI ## port ## _CTRL & ~SPI_INTLVL_gm) | level; } \
        ALWAYS_INLINE static bool isInterrupt() { bool flag = SPI ## port ## _STATUS & SPI_IF_bm; SPI ## port ## _DATA; return flag; } \
        \
        static uint8_t \
        write(uint8_t data) { \
            SPI ## port ## _STATUS; \
            SPI ## port ## _DATA = data; \
            while (!(SPI ## port ## _STATUS & SPI_IF_bm)); \
            return SPI ## port ## _DATA; \
        } \
        \
        static void \
        initialize(SPI_MODE_t mode, SPI_PRESCALER_t _prescaler) { \
            PORT ## port ## _DIRSET = (1 << 4); /* Slave Select Pin must remain high */ \
            PORT ## port ## _DIRSET = (1 << 5); /* MOSI */ \
            PORT ## port ## _DIRSET = (1 << 7); /* SCK */ \
            SPI ## port ## _CTRL = SPI_ENABLE_bm \
                                 | SPI_MASTER_bm \
                                 | mode \
                                 | _prescaler; \
        } \
        \
        static void \
        initialize(SPI_MODE_t mode = SPI_MODE_0_gc) { \
            PORT ## port ## _DIRSET = (1 << 4); \
            PORT ## port ## _DIRSET = (1 << 5); \
            PORT ## port ## _DIRSET = (1 << 7); \
            SPI ## port ## _CTRL = SPI_ENABLE_bm \
                                 | SPI_MASTER_bm \
                                 | mode \
                                 | prescaler; \
        } \
    };

#endif  // __DOXYGEN__
#endif // XPCC__XMEGA_SPI_HPP