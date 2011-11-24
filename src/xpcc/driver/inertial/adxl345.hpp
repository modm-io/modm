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

#ifndef XPCC__ADXL345_HPP
#define XPCC__ADXL345_HPP

#include <xpcc/driver/connectivity/i2c/sync_master.hpp>

namespace xpcc
{
	/**
	 * \brief Basic ADXL345 digital accelerometer sensor driver
	 *
	 * The ADXL345 is a 3-axis accelerometer with high resolution (13-bit)
	 * measurement at up to +-16 g. Digital output data is formatted as 16-bit
	 * twos complement and is accessible through I2C digital interface.
	 *
	 * Its high resolution (4 mg/LSB) enables measurement of inclination 
	 * changes less than 1.0 degree.
	 * Several special sensing functions are provided. Activity and inactivity
	 * sensing detect the presence or lack of motion and if the acceleration on
	 * any axis exceeds a user-set level. Tap sensing detects single and double
	 * taps. Free-fall sensing detects if the device is falling. These functions
	 * can be mapped to one of two interrupt output pins. An integrated 32-level
	 * first in, first out (FIFO) buffer can be used to store data to minimize
	 * host processor intervention.
	 * Low power modes enable intelligent motion-based power management with
	 * threshold sensing and active acceleration measurement at extremely low
	 * power dissipation.
	 *
	 * For further information on the special sensing functions, consult the
	 * <a href="http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf">
	 * datasheet</a>.
	 *  
	 * \author	Niklas Hauser
	 * \ingroup inertial
	 *
	 * \tparam I2C Asynchronous Two Wire interface
	 */
	template < typename I2C >
	class Adxl345
	{
	public:
		/// The addresses of the Configuration and Data Registers
		enum Register
		{
			REGISTER_DEVID = 0x00,			///< Device ID 0xE5
			REGISTER_THRESH_TAP = 0x1D,		///< Tap threshold
			REGISTER_OFSX = 0x1E,			///< X-axis offset
			REGISTER_OFSY = 0x1F,			///< Y-axis offset
			REGISTER_OFSZ = 0x20,			///< Z-axis offset
			REGISTER_DUR = 0x21,			///< Tap duration
			REGISTER_LATENT = 0x22,			///< Tap latency
			REGISTER_WINDOW = 0x23,			///< Tap window
			REGISTER_THRESH_ACT = 0x24,		///< Activity threshold
			REGISTER_THRESH_INACT = 0x25,	///< Inactivity threshold
			REGISTER_TIME_INACT = 0x26,		///< Inactivity time
			REGISTER_ACT_INACT_CTL = 0x27,	///< Axix enable control for activity and inactivity detection
			REGISTER_THRESH_FF = 0x28,		///< Free-fall threshold
			REGISTER_TIME_FF = 0x29,		///< Free-fall time
			REGISTER_TAP_AXES = 0x2A,		///< Axis control for tap/double tap
			REGISTER_ACT_TAP_STATUS = 0x2B,	///< Source of tap/double tap
			REGISTER_BW_RATE = 0x2C,		///< Date rate and power control
			REGISTER_POWER_CTL = 0x2D,		///< Power-saving features control
			REGISTER_INT_ENABLE = 0x2E,		///< Interrupt enable control
			REGISTER_INT_MAP = 0x2F,		///< Interrupt mapping control
			REGISTER_INT_SOURCE = 0x30,		///< Source of interrupts
			REGISTER_DATA_FORMAT = 0x31,	///< Data format control
			REGISTER_DATA_X0 = 0x32,		///< X-Axis Data LSB
			REGISTER_DATA_X1 = 0x33,		///< X-Axis Data MSB
			REGISTER_DATA_Y0 = 0x34,		///< Y-Axis Data LSB
			REGISTER_DATA_Y1 = 0x35,		///< Y-Axis Data MSB
			REGISTER_DATA_Z0 = 0x36,		///< Z-Axis Data LSB
			REGISTER_DATA_Z1 = 0x37,		///< Z-Axis Data MSB
			REGISTER_FIFO_CTL = 0x38,		///< FIFO control
			REGISTER_FIFO_STATUS = 0x39		///< FIFO Status
		};
		
		/// The bandwidth options of REGISTER_BW_RATE.
		/// Output frequency = 2 * bandwidth
		enum Bandwidth {
			BANDWIDTH_1600 = 0x0F,
			BANDWIDTH_800 = 0x0E,
			BANDWIDTH_400 = 0x0D,
			BANDWIDTH_200 = 0x0C,
			BANDWIDTH_100 = 0x0B,
			BANDWIDTH_50 = 0x0A,
			BANDWIDTH_25 = 0x09,
			BANDWIDTH_12 = 0x08,
			BANDWIDTH_6 = 0x07,
			BANDWIDTH_3 = 0x06
		};
		
		/// The power options of REGISTER_POWER_CTL.
		enum Power {
			POWER_LINK_bm = 0x20,
			POWER_AUTO_SLEEP_bm = 0x10,
			POWER_MEASURE_bm = 0x08,
			POWER_SLEEP_bm = 0x04,
			POWER_WAKEUP_gm = 0x03,
			POWER_WAKEUP_1HZ_gc = 0x03,
			POWER_WAKEUP_2HZ_gc = 0x02,
			POWER_WAKEUP_4HZ_gc = 0x01,
			POWER_WAKEUP_8HZ_gc = 0x00,
			// BW_RATE register
			POWER_LOW_POWER = 0x10
		};
		
		/// The interrupt bit masks of REGISTER_INT_ENABLE, REGISTER_INT_MAP and
		/// REGISTER_INT_SOURCE.
		enum Interrupt {
			INTERRUPT_DATA_READY_bm = 0x80,
			INTERRUPT_SINGLE_TAP_bm = 0x40,
			INTERRUPT_DOUBLE_TAP_bm = 0x20,
			INTERRUPT_ACTIVITY_bm = 0x10,
			INTERRUPT_INACTIVITY_bm = 0x08,
			INTERRUPT_FREE_FALL_bm = 0x04,
			INTERRUPT_WATERMARK_bm = 0x02,
			INTERRUPT_OVERRUN_bm = 0x01,
		};
		
		/// The options of REGISTER_DATA_FORMAT
		enum DataFormat {
			DATAFORMAT_SELF_TEST_bm = 0x80,
			DATAFORMAT_SPI_bm = 0x40,
			DATAFORMAT_INT_INVERT_bm = 0x20,
			DATAFORMAT_FULL_RES_bm = 0x08,
			DATAFORMAT_JUSTIFY_bm = 0x04,
			DATAFORMAT_RANGE_gm = 0x03,
			DATAFORMAT_RANGE_16G_gc = 0x03,
			DATAFORMAT_RANGE_8G_gc = 0x02,
			DATAFORMAT_RANGE_4G_gc = 0x01,
			DATAFORMAT_RANGE_2G_gc = 0x00
		};
		
		/// The options of REGISTER_FIFO_CTL and REGISTER_FIFO_STATUS
		enum FIFO {
			FIFO_CTL_MODE_gm = 0xc0,
			FIFO_CTL_MODE_TRIGGER_gc = 0xc0,
			FIFO_CTL_MODE_STREAM_gc = 0x80,
			FIFO_CTL_MODE_FIFO_gc = 0x40,
			FIFO_CTL_MODE_BYPASS_gc = 0x00,
			FIFO_CTL_TRIGGER_bm = 0x20,
			FIFO_CTL_SAMPLES_gm = 0x0f,
			// FIFO_STATUS register
			FIFO_STATUS_TRIG_bm = 0x80,
			FIFO_STATUS_ENTRIES_gm = 0x3f
		};
		
		/**
		 * \brief	Constructor
		 * \param	address		address is 0xa6 with SDO pin low else 0x3a
		 */
		Adxl345(uint8_t address=0x1d);
		
		/**
		 * Configures the sensor to measurement mode with full resolution with
		 * the 32-level buffer in Stream Mode and the specified bandwidth.
		 */
		void
		initialize(Bandwidth bandwidth=BANDWIDTH_50, bool streamMode=false);
		
		/**
		 * read the X-ZDATA0-1 registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		void
		readAcceleration();
		
		/**
		 * read the X-ZDATA0-1 registers 32 times (the entire 32-level buffer 
		 * in stream mode) and calculates the average.
		 * During execution isNewDataAvailable() is set to \c false, because
		 * the adding and division is done in-place on the data buffer.
		 * This method takes a very long time, since all i2c reads are blocking!
		 */
		void
		readAccelerationAverage();
		
		/// \return pointer to 8bit array containing xyz accelerations
		/// Use reinterpret_cast<int16*>(&getData()) to get the results.
		/// Devide by approx. 256 LSB/g to get the data in gravity.
		uint8_t*
		getData();
		
		/**
		 * \c true, when new data has been from the sensor and buffered,
		 * \c false, when the data has already been read, or data is being 
		 * copied into the buffer (by readAccelerationAverage()).
		 */
		bool
		isNewDataAvailable();
		
		/**
		 * Reads the sensor register if new results have been computed.
		 * Use this for low Bandwidth checks.
		 */
		bool
		isDataReady();
		
	private:
		
		/**
		 * writes 8bit data to a register, non blocking!
		 * \param reg register address
		 * \param data 8bit data to write
		 */
		void
		writeRegister(Register reg, uint8_t data);
		
		/**
		 * reads a 8bit register, blocking!
		 * \param reg the 8bit register to read
		 * \return 8bit content
		 */
		uint8_t
		readRegister(Register reg);
		
		bool
		readData(Register reg, uint8_t *data, uint8_t size);

		bool newData;
		uint8_t data[6];
		uint8_t deviceAddress;
		typedef xpcc::i2c::SyncMaster<I2C> MySyncI2C;
	};
	
}

#include "adxl345_impl.hpp"

#endif // XPCC__ADXL345_HPP
