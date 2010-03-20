// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#include "config.h"
#include <xpcc/architecture/general/gpio.hpp>

// LOGGER ######################################################################
#include <xpcc/debug/logger/logger.hpp>
#include <xpcc/debug/logger/style_wrapper.hpp>
#include <xpcc/debug/logger/style/prefix.hpp>
#include <xpcc/architecture/atmega/uart/uart0.hpp>
#include <xpcc/communication/io/iodevice_wrapper.hpp>

// set the Loglevel
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DEBUG

xpcc::BufferedUart0 loggerUart(9600);
xpcc::IODeviceWrapper<xpcc::BufferedUart0> loggerDevice(loggerUart);

xpcc::log::StyleWrapper< xpcc::log::Prefix< char[10] > > loggerDeviceDebug (
		xpcc::log::Prefix< char[10] > ("Debug:   ", loggerDevice ) );
xpcc::log::Logger xpcc::log::debug( loggerDeviceDebug );

xpcc::log::StyleWrapper< xpcc::log::Prefix< char[10] > > loggerDeviceInfo (
		xpcc::log::Prefix< char[10] > ("Info:    ", loggerDevice ) );
xpcc::log::Logger xpcc::log::info( loggerDeviceInfo );

xpcc::log::StyleWrapper< xpcc::log::Prefix< char[10] > > loggerDeviceWarning (
		xpcc::log::Prefix< char[10] > ("Warning: ", loggerDevice ) );
xpcc::log::Logger xpcc::log::warning( loggerDeviceWarning );

xpcc::log::StyleWrapper< xpcc::log::Prefix< char[10] > > loggerDeviceError (
		xpcc::log::Prefix< char[10] > ("Error    ", loggerDevice ) );
xpcc::log::Logger xpcc::log::error( loggerDeviceError );

// COMMUNICATION ###############################################################
#include <xpcc/communication/xpcc/backend/can/can.hpp>
#include <xpcc/driver/can/mcp2515.hpp>
#include <xpcc/driver/software_spi.hpp>

namespace mcp2515
{
	GPIO__OUTPUT(CS, B, 4);
	GPIO__INPUT(INT, B, 2);

	GPIO__OUTPUT(SCLK, B, 7);
	GPIO__OUTPUT(MOSI, B, 5);
	GPIO__INPUT(MISO, B, 6);

	typedef xpcc::SoftwareSpi<SCLK, MOSI, MISO> Spi;
	typedef xpcc::Mcp2515< Spi, mcp2515::CS, mcp2515::INT > Mcp;
}

namespace
{
	GPIO__OUTPUT(LED1, B, 0);
	GPIO__OUTPUT(LED2, B, 1);
}

xpcc::CanConnector< mcp2515::Mcp > canConnector;

// GENERAL HEADER FILES ########################################################
#include <xpcc/architecture/general/time/delay.hpp>
#include <xpcc/architecture/general/time/clock.hpp>
#include <avr/interrupt.h>

struct LongData {
		uint8_t a;
		uint8_t b;
		uint8_t c;
		uint8_t d;
		uint8_t f;
		uint8_t g;
		uint8_t h;
		uint8_t i;
		uint8_t j;
		uint8_t k;
		uint8_t l;
		uint8_t m;
		uint8_t n;
		uint8_t o;
		uint8_t p;
}  __attribute__((packed));

int
main()
{
	LED1::setOutput();
	LED2::setOutput();

	mcp2515::CS::setOutput();
	mcp2515::MISO::configure(xpcc::gpio::PULLUP);
	mcp2515::INT::configure(xpcc::gpio::PULLUP);

	xpcc::BufferedUart0::setBaudrate( 9600 );

	mcp2515::Spi::initialize();
	mcp2515::Mcp::initialize( xpcc::Can::BITRATE_125_KBPS );

	sei();

	xpcc::log::info << "########## XPCC CAN communication TEST TRANSMIT ##########" << xpcc::flush;
	delay_ms(1000);

//	tipc.addReceiverId(0x20);

	xpcc::Header actionHeader( xpcc::Header::REQUEST, false, 0x10, 0x20, 0x01 );	// ACTION
	xpcc::Header eventHeader( xpcc::Header::REQUEST, false, 0x0, 0x20, 0x02 );	// EVENT

	int data(0);

	LongData longData;
	longData.a = 1;
	longData.b = 2;
	longData.c = 3;
	longData.d = 4;
	longData.p = 5;

	xpcc::Timestamp lastTime( xpcc::Clock::now() );
	while(1) {

		if ( xpcc::Clock::now() - lastTime > 1000 ) {
			lastTime = xpcc::Clock::now() ;

			data++;
			xpcc::SmartPointer payload(&data);
			xpcc::log::info << "data=" << data << xpcc::flush;

			canConnector.sendPacket(actionHeader, payload);

			longData.a--;
			xpcc::SmartPointer payloadLong(&longData);
			xpcc::log::info << "long=" << payloadLong << xpcc::flush;
			delay_ms(10);
			canConnector.sendPacket(eventHeader, payloadLong);
			/*
			tipc.sendPacket(eventHeader, payload);

			if ( tipc.isPacketAvailable() ) {
				const xpcc::Header& header =  tipc.getPacketHeader();

				XPCC_LOG_INFO << XPCC_FILE_INFO << "has";
				XPCC_LOG_INFO << ((header.destination != 0) ? " ACTION" : " EVENT");
				XPCC_LOG_INFO << ((header.isAcknowledge) ? " ACK" : "");
				XPCC_LOG_INFO << " from:" << (int)header.source;
				XPCC_LOG_INFO << xpcc::flush;

				tipc.dropPacket();
			}*/
			LED1::toggle();
		}
		canConnector.update();
		xpcc::Clock::increment();
		delay_ms(1);
	}


	xpcc::log::info << "########## XPCC TIPC RAW Test END ##########" << xpcc::flush;
}
