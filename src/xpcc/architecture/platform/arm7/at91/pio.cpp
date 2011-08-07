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

#include "pio.hpp"
#include "device.h"

// If PIOB is not available just redirect everything to PIA. This is not
// the cleanest way to do it but won't cause any error and simplifies the
// general handling of the different PIO controllers.
#ifndef AT91C_ID_PIOB
#	define	AT91C_ID_PIOB	AT91C_ID_PIOA
#endif

// ----------------------------------------------------------------------------
void
xpcc::at91::Pio::enable()
{
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PIOA) | (1 << AT91C_ID_PIOB);
}

void
xpcc::at91::Pio::disable()
{
	AT91C_BASE_PMC->PMC_PCDR = (1 << AT91C_ID_PIOA) | (1 << AT91C_ID_PIOB);
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Pio::registerInterruptHandler(PioController controller, Aic::Handler handler)
{
	if (controller == PIOA) {
		Aic::registerHandler(AT91C_ID_PIOA, handler);
		Aic::enableInterrupt(AT91C_ID_PIOA);
	}
	else {
		Aic::registerHandler(AT91C_ID_PIOB, handler);
		Aic::enableInterrupt(AT91C_ID_PIOB);
	}
}

void
xpcc::at91::Pio::removeInterruptHandler(PioController controller)
{
	if (controller == PIOA) {
		Aic::disableInterrupt(AT91C_ID_PIOA);
	}
	else {
		Aic::disableInterrupt(AT91C_ID_PIOB);
	}
}
