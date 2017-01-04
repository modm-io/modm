/*
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Fabian Greif
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LPC11__SPI_0_HPP
#define XPCC_LPC11__SPI_0_HPP

#include <stdint.h>
#include <modm/communication/spi/spi_master.hpp>

#include "spi_registers.h"
namespace xpcc
{
	namespace lpc
	{
		/**
		 * \brief	Serial peripheral interface (SPI0)
		 *
		 * As the NXP peripherals provide a hardware buffer, this
		 * implementation is already bufferd.
		 *
		 * Features:
		 * - Maximum SPI speed of 25 Mbit/s (master) or 4.17 Mbit/s (slave) (in SSP mode)
		 * - Compatible with Motorola SPI, 4-wire Texas Instruments SSI, and National Semiconductor Microwire buses
		 * - Synchronous serial communication
		 * - Master or slave operation
		 * - 8-frame FIFOs for both transmit and receive
		 * - 4-bit to 16-bit frame
		 *
		 * \ingroup	lpc
		 *
		 */
		class SpiMaster0 : public xpcc::SpiMaster
		{
		public:
			enum class TransferSize
			{
				BIT_04 = 0x03,
				BIT_05 = 0x04,
				BIT_06 = 0x05,
				BIT_07 = 0x06,
				BIT_08 = 0x07,
				BIT_09 = 0x08,
				BIT_10 = 0x09,
				BIT_11 = 0x0a,
				BIT_12 = 0x0b,
				BIT_13 = 0x0c,
				BIT_14 = 0x0d,
				BIT_15 = 0x0d,
				BIT_16 = 0x0f,
			};

			enum class FrameFormat
			{
				SPI = 0x00,
				TI  = 0x01,
				MICROWIRE = 0x02,
			};

			enum class Mode
			{
				MODE_0 = 0,								///< SCK normal,   sample on rising  edge
				MODE_1 = SPI_CR0_CPHA,					///< SCK normal,   sample on falling edge
				MODE_2 = SPI_CR0_CPOL,					///< SCK inverted, sample on falling edge
				MODE_3 = SPI_CR0_CPOL | SPI_CR0_CPHA,	///< SCK inverted, sample on rising  edge
			};

			/**
			 * \brief	Clock prescaler.
			 *
			 * Must be an even number between 2 and 252
			 */
			enum class Presacler
			{
				DIV002 =   2,
				DIV004 =   4,
				DIV006 =   6,
				DIV008 =   8,
				DIV010 =  10,
				DIV012 =  12,
				DIV014 =  14,
				DIV016 =  16,
				DIV018 =  18,
				DIV020 =  20,
				DIV022 =  22,
				DIV024 =  24,
				DIV026 =  26,
				DIV028 =  28,
				DIV030 =  30,
				DIV032 =  32,
				DIV034 =  34,
				DIV036 =  36,
				DIV038 =  38,
				DIV040 =  40,
				DIV042 =  42,
				DIV044 =  44,
				DIV046 =  46,
				DIV048 =  48,
				DIV050 =  50,
				DIV052 =  52,
				DIV054 =  54,
				DIV056 =  56,
				DIV058 =  58,
				DIV060 =  60,
				DIV062 =  62,
				DIV064 =  64,
				DIV066 =  66,
				DIV068 =  68,
				DIV070 =  70,
				DIV072 =  72,
				DIV074 =  74,
				DIV076 =  76,
				DIV078 =  78,
				DIV080 =  80,
				DIV082 =  82,
				DIV084 =  84,
				DIV086 =  86,
				DIV088 =  88,
				DIV090 =  90,
				DIV092 =  92,
				DIV094 =  94,
				DIV096 =  96,
				DIV098 =  98,
				DIV100 = 100,
				DIV102 = 102,
				DIV104 = 104,
				DIV106 = 106,
				DIV108 = 108,
				DIV110 = 110,
				DIV112 = 112,
				DIV114 = 114,
				DIV116 = 116,
				DIV118 = 118,
				DIV120 = 120,
				DIV122 = 122,
				DIV124 = 124,
				DIV126 = 126,
				DIV128 = 128,
				DIV130 = 130,
				DIV132 = 132,
				DIV134 = 134,
				DIV136 = 136,
				DIV138 = 138,
				DIV140 = 140,
				DIV142 = 142,
				DIV144 = 144,
				DIV146 = 146,
				DIV148 = 148,
				DIV150 = 150,
				DIV152 = 152,
				DIV154 = 154,
				DIV156 = 156,
				DIV158 = 158,
				DIV160 = 160,
				DIV162 = 162,
				DIV164 = 164,
				DIV166 = 166,
				DIV168 = 168,
				DIV170 = 170,
				DIV172 = 172,
				DIV174 = 174,
				DIV176 = 176,
				DIV178 = 178,
				DIV180 = 180,
				DIV182 = 182,
				DIV184 = 184,
				DIV186 = 186,
				DIV188 = 188,
				DIV190 = 190,
				DIV192 = 192,
				DIV194 = 194,
				DIV196 = 196,
				DIV198 = 198,
				DIV200 = 200,
				DIV202 = 202,
				DIV204 = 204,
				DIV206 = 206,
				DIV208 = 208,
				DIV210 = 210,
				DIV212 = 212,
				DIV214 = 214,
				DIV216 = 216,
				DIV218 = 218,
				DIV220 = 220,
				DIV222 = 222,
				DIV224 = 224,
				DIV226 = 226,
				DIV228 = 228,
				DIV230 = 230,
				DIV232 = 232,
				DIV234 = 234,
				DIV236 = 236,
				DIV238 = 238,
				DIV240 = 240,
				DIV242 = 242,
				DIV244 = 244,
				DIV246 = 246,
				DIV248 = 248,
				DIV250 = 250,
				DIV252 = 252,
			};

			enum class MappingSck
			{
			PIO0_6  = 0,		///< Selects SCK0 function in pin location SWCLK/PIO0_10/SCK0/CT16B0_MAT2
				PIO0_10 = 1,		///< Selects SCK0 function in pin location PIO2_11/SCK0
				PIO2_11 = 2,		///< Selects SCK0 function in pin location PIO0_6/SCK0
			};

			static void
			configurePins(
					MappingSck mapping = xpcc::lpc::SpiMaster0::MappingSck::PIO0_6,
					bool useSsel = false);

		public:
			/**
			 * @brief	Initialize SPI module
			 */
			static void
			initialize(
					Mode mode = Mode::MODE_0,
					Presacler prescaler = Presacler::DIV002,
					uint8_t serialClockRate = 7,
					TransferSize transferSize = TransferSize::BIT_08,
					FrameFormat frameFormat = FrameFormat::SPI);

			/**
			 * \brief	Write a single byte with the SPI.
			 *
			 * The data is put into the FIFO but it blocks until the data
			 * was sent and data from the device is available.  
			 */
			static uint8_t
			write(uint8_t data);

			// static void
			// write(uint8_t * data, uint8_t size);
			
			static bool
			setBuffer(uint16_t length,
					  uint8_t* transmit=0, uint8_t* receive=0,
					  BufferIncrease bufferIncrease=BUFFER_INCR_BOTH);

			static bool
			transfer(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static xpcc_always_inline bool
			transferSync(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static bool
			isFinished();

		protected:
			static constexpr uint8_t
			fifoSize = 8;
		};
	} // lpc namespace
} // xpcc namespace
#endif // XPCC_LPC11__SPI_0_HPP
