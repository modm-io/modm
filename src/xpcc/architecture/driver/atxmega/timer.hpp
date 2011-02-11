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
 * $Id: spi.hpp 474 2011-02-11 12:40:19Z salkinium $
 */
// ----------------------------------------------------------------------------

/**
 * \ingroup	atxmega
 * \defgroup atxmega_timer ATxmega TC Module
 *
 * Timer/Counter Module
 *
 * TC type 0 has got four Compare/Capture Units, type 1 only two.
 * Be aware that both type 0 and 1 share the same HIRES Extension and, when
 * available for the TC, also the same AWEX Extension!
 *
 * The Timer classes extend the HighRes and Waveform classes when available.
 * These classes are meant to configure the TC Module, there is no access to
 * any data registers!
 * Please write your own template for your special TC purposes.
 *
 * The Xmega Timer/Counters are very complex hardware.
 * It is highly recommended that you consult the Xmega Manual and write your
 * own template to configure the Timer/Counter and its Extensions.
 *
 * \sa AVR1306
 * \author Niklas Hauser
 */

#include "timer/awex_c.hpp"
#include "timer/awex_d.hpp"
#include "timer/awex_e.hpp"
#include "timer/awex_f.hpp"
#include "timer/hires_c.hpp"
#include "timer/hires_d.hpp"
#include "timer/hires_e.hpp"
#include "timer/hires_f.hpp"
#include "timer/timer_c0.hpp"
#include "timer/timer_c1.hpp"
#include "timer/timer_d0.hpp"
#include "timer/timer_d1.hpp"
#include "timer/timer_e0.hpp"
#include "timer/timer_e1.hpp"
#include "timer/timer_f0.hpp"
#include "timer/timer_f1.hpp"
