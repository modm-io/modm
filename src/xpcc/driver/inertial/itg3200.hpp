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
 * $Id: itg3200.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ITG3200_HPP
#define XPCC__ITG3200_HPP

#include <xpcc/driver/connectivity/i2c/master.hpp>

namespace xpcc
{
	namespace itg3200
	{
		/// The addresses of the Configuration and Data Registers
		enum Register
		{
			REGISTER_I2C_ID = 0x00,			///< I2C ID 0x68
			REGISTER_SMPLRT_DIV = 0x15,		///< Sample Rate Divider
			REGISTER_DLPF_FS = 0x16,		///< Digital Low Pass Filter and Full Scale
			REGISTER_INTERRUPT = 0x17,		///< Interrupt Configuration
			REGISTER_STATUS = 0x1A,			///< Interrupt Status
			REGISTER_DATA_T0 = 0x1B,		///< Temperature Data MSB
			REGISTER_DATA_T1 = 0x1C,		///< Temperature Data LSB
			REGISTER_DATA_X0 = 0x1D,		///< X-Axis Data MSB
			REGISTER_DATA_X1 = 0x1E,		///< X-Axis Data LSB
			REGISTER_DATA_Y0 = 0x1F,		///< Y-Axis Data MSB
			REGISTER_DATA_Y1 = 0x20,		///< Y-Axis Data LSB
			REGISTER_DATA_Z0 = 0x21,		///< Z-Axis Data MSB
			REGISTER_DATA_Z1 = 0x22,		///< Z-Axis Data LSB
			REGISTER_POWER = 0x3E,			///< Power Management
			
		};
		
		/// The options of REGISTER_DLPF_FS
		enum Filter {
			SCALE_FS_gm = (0x03 << 3),
			SCALE_FS_2000_gc = (0x03 << 3),
			FILTER_DLPF_gm = 0x07,
			FILTER_DLPF_256_gc = 0x00,
			FILTER_DLPF_188_gc = 0x01,
			FILTER_DLPF_98_gc = 0x02,
			FILTER_DLPF_42_gc = 0x03,
			FILTER_DLPF_20_gc = 0x04,
			FILTER_DLPF_10_gc = 0x05,
			FILTER_DLPF_5_gc = 0x06
		};
		
		/// The interrupt bit masks of REGISTER_INTERRUPT
		enum Interrupt {
			INTERRUPT_ACTL_bm = 0x80,				///< Logic Level for INT output Pin, 1=active low, 0=active high
			INTERRUPT_OPEN_bm = 0x40,				///< Drive type for INT output pin, 1=open drain, 0=push-pull
			INTERRUPT_LATCH_INT_EN_bm = 0x20,		///< Latch mode, 1=latch until interrupt is cleared, 0=50us pulse
			INTERRUPT_INT_ANYRD_2CLEAR_bm = 0x10,	///< Latch clear method, 1=any register read, 0=status register read only
			INTERRUPT_ITG_RDY_EN_bm = 0x04,			///< Enable interrupt when device is ready (PLL ready after changing clock source)
			INTERRUPT_RAW_RDY_EN_bm = 0x01,			///< Enable interrupt when data is available
		};
		
		/// The interrupt bit masks of REGISTER_STATUS
		enum Status {
			STATUS_ITG_RDY_bm = 0x04,			//!< PLL ready
			STATUS_RAW_RDY_bm = 0x01,			//!< Raw data is ready
		};
		
		/// The power options of REGISTER_POWER
		enum Power {
			POWER_RESET_bm = 0x80,
			POWER_SLEEP_bm = 0x40,
			POWER_STBY_XG_bm = 0x20,
			POWER_STBY_YG_bm = 0x10,
			POWER_STBY_ZG_gm = 0x08,
			POWER_CLK_SEL_gm = 0x07,
			POWER_CLK_INTERNAL_gc = 0x00,
			POWER_CLK_PLL_XG_gc = 0x01,
			POWER_CLK_PLL_YG_gc = 0x02,
			POWER_CLK_PLL_ZG_gc = 0x03,
			POWER_CLK_EXTERNAL_32k_gc = 0x04,
			POWER_CLK_EXTERNAL_19M_gc = 0x05,
		};
	}
	
	/**
	 * \brief Basic ITG3200 digital gyroscope sensor driver
	 *
	 * For further information on the special sensing functions, consult the
	 * <a href="http://invensense.com/mems/gyro/documents/PS-ITG-3200-00-01.4.pdf">
	 * datasheet</a>.
	 *  
	 * \author	Niklas Hauser
	 * \ingroup inertial
	 *
	 * \tparam TwiMaster Asynchronous Two Wire interface
	 */
	template < typename TwiMaster >
	class ITG3200 : public xpcc::i2c::Delegate
	{
	public:
		/**
		 * \brief	Constructor
		 * \param	address		address is 0x68 with SDO pin low else 0x69
		 */
		ITG3200(uint8_t address=0x68);
		
		/**
		 * Configures the sensor to full scale resolution with 
		 */
		bool
		initialize(uint8_t divider=0, itg3200::Filter filter=itg3200::FILTER_DLPF_20_gc, bool interrupt=true);
		
		/**
		 * only read the X-ZDATA0-1 registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		bool
		readGyroscope();
		
		/**
		 * read the T0-1 and X-ZDATA0-1 registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		bool
		readTemperatureGyroscope();
		
		/* \return pointer to 8bit array containing xyz and accelerations
		 * Be aware that the array is in BIG ENDIAN format, so you cannot
		 * simply reinterpret the result as int16_t!!
		 */
		uint8_t*
		getData();
		
		/* \return pointer to 8bit array containing txyz temperature and accelerations
		 * Be aware that the array is in BIG ENDIAN format, so you cannot
		 * simply reinterpret the result as int16_t!!
		 */
		uint8_t*
		getTemperatureData();
		
		/**
		 * \c true, when new data has been from the sensor and buffered,
		 * \c false, when the data has been accessed, or data is being 
		 * copied into the buffer.
		 */
		bool
		isNewDataAvailable();
		
		/**
		 * \c true, when new data has been from the sensor and buffered,
		 * \c false, when the data has been accessed, or data is being 
		 * copied into the buffer.
		 */
		bool
		isNewGyroDataAvailable();
		
		/**
		 * Reads the sensor register if new results have been computed.
		 * Use this for low Bandwidth checks.
		 */
		bool
		isDataReady();
		
		/// Sets the specified sample rate divider
		bool
		setSampleRateDivider(uint8_t divider=0);
		
		/// Sets the specified interrupts
		bool
		setInterrupts(itg3200::Interrupt interrupts=itg3200::INTERRUPT_RAW_RDY_EN_bm);
		
	private:
		/**
		 * writes 8bit data to a register, blocking!
		 * \param reg register address
		 * \param data 8bit data to write
		 */
		bool
		writeRegister(itg3200::Register reg, uint8_t data);
		
		/**
		 * reads a 8bit register, blocking!
		 * \param reg the 8bit register to read
		 * \return 8bit content
		 */
		uint8_t
		readRegister(itg3200::Register reg);
		
		/**
		 * this delegate function gets called after calling readAcceleration()
		 * \return always \c false, since we do not want to continue using the bus
		 */
		void
		twiCompletion(const uint8_t *data, std::size_t index, bool reading);
		
		bool newData;
		bool newGyro;
		uint8_t data[8];
		uint8_t deviceAddress;
	};
	
}

#include "itg3200_impl.hpp"

#endif // XPCC__ITG3200_HPP
