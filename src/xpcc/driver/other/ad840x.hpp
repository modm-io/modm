/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__AD840x_HPP
#define XPCC__AD840x_HPP

#include <stdint.h>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/driver/delay.hpp>

namespace xpcc
{
	namespace ad840x
	{
		/**
		 * \brief	Definition of the AD8400/2/3 channels
		 * 
		 * \see		AD840x
		 * \ingroup	driver_other
		 * 
		 * \hideinitializer
		 */
		enum Channel
		{
			CHANNEL1 = 0x0,		
			CHANNEL2 = 0x1,		///< only for AD8402 and AD8403
			CHANNEL3 = 0x2,		///< only for AD8403
			CHANNEL4 = 0x3		///< only for AD8403
		};
	}
	
	/**
	 * \brief	AD8400/2/3 Digital Potentiometers (1-,2-,4-Channel)
	 * 
	 * <h2>Features</h2>
	 * 
	 *  - 256-position variable resistance device
	 *  - Replaces 1, 2, or 4 potentiometers
	 *  - 1 kΩ, 10 kΩ, 50 kΩ, 100 kΩ nominal resistance
	 *  - 3-wire, SPI-compatible serial data input
	 *  - 10 MHz update data loading rate
	 *  - 2.7 V to 5.5 V single-supply operation
	 * 
	 * The nominal resistance of the VR (Variable Resistor) (RDAC) between
	 * Terminal A and Terminal B is available with values of 1 kΩ, 10 kΩ,
	 * 50 kΩ, and 100 kΩ. The final digits of the part number determine the
	 * nominal resistance value; that is, 10 kΩ = 10; 100 kΩ = 100 etc.
	 * 
	 * The nominal resistance (RAB) of the VR has 256 contact points accessible
	 * by the wiper terminal. The AD840x do not have power-on midscale preset,
	 * so the wiper can be at any random position at power-up.
	 * 
	 * <h2>Serial Connection</h2>
	 * 
	 * Data is shifted in on the positive edges of the clock (SPI Mode 0).
	 * Maximum SPI frequency is 10 MHz.
	 * 
	 * The output is updated after the time ts after the rising edge of CS.
	 * ts depends on the nominal resistance value.
	 * 
	 * \code
	 * Version | Time ts
	 * --------+---------
	 *   10 kΩ |   2 µs
	 *   50 kΩ |   9 µs
	 *  100 kΩ |  18 µs
	 * \endcode
	 * 
	 * The serial data output (SDO) pin, which exists only on the AD8403 and
	 * not on the AD8400 or AD8402, contains an open-drain, n-channel FET that
	 * requires a pull-up resistor to transfer data to the SDI pin of the next
	 * package.
	 * 
	 * The pull-up resistor termination voltage may be larger than the VDD
	 * supply (but less than the max VDD of 8 V) of the AD8403 SDO output
	 * device.
	 * 
	 * \tparam	Spi		SPI interface (MISO pin is not used)
	 * \tparam	Cs		Chip-Select pin
	 * \tparam	Rs		Reset (low active), use xpcc::Unused for the AD8400
	 * \tparam	Shdn	Shutdown (low active), use xpcc::Unused for the AD8400
	 * 
	 * \see		ad840x
	 * \author	Fabian Greif
	 * \ingroup	driver_other
	 */
	template< typename Spi,
			  typename Cs,
			  typename Rs,
			  typename Shdn>
	class AD840x
	{
	public:
		/**
		 * \brief	Initialize SPI interface and set pins
		 */
		static void
		initialize();
		
		/**
		 * \brief	Reset channels
		 * 
		 * Forces the whiper positions to 0x80 = 128 = midscale.
		 */
		static void
		reset();
		
		/**
		 * \brief	Enter shutdown mode
		 * 
		 * \warning	Only for AD8402 and AD8403
		 * 
		 * Both parts have a power shutdown SHDN pin that places the
		 * VR (variable resistor) in a zero-power-consumption state where
		 * Terminal Ax is open-circuited and the Wiper Wx is connected to
		 * Terminal Bx, resulting in the consumption of only the leakage
		 * current in the VR.
		 * 
		 * The digital interface is still active in shutdown, except that SDO
		 * is deactivated. Code changes in the registers can be made during
		 * shutdown that will produce new wiper positions when the device is
		 * taken out of shutdown.
		 * 
		 */
		static inline void
		shutdown();
		
		/**
		 * \brief	Resume normal mode after shutdown
		 */
		static inline void
		resume();
		
		/**
		 * \brief	Set a new register value
		 * 
		 * The wiper’s first connection starts at the B terminal for data 00H.
		 * This B terminal connection has a wiper contact resistance of 50 Ω.
		 * The second connection (for the 10 kΩ part) is the first tap point
		 * located at 89 Ω = [RAB (nominal resistance) + RW = 39 Ω + 50 Ω] for
		 * data 01H.
		 * The third connection is the next tap point representing
		 * 78 Ω + 50 Ω = 128 Ω for data 02H. Each LSB data value increase
		 * moves the wiper up the resistor ladder until the last tap point is
		 * reached at 10,011 Ω.
		 * 
		 * Note that the wiper does not directly connect to the B terminal
		 * even for data 00H. 
		 * 
		 * \code
		 * data | Rwb (Ohm) | Output State
		 * -----+-----------+-----------------
		 *  255 |   10 011  | Full scale
		 *  128 |    5 050  | Midscale (reset() condition)
		 *    1 |       89  | 1 LSB
		 *    0 |       50  | Zero-scale (wiper contact resistance)
		 * \endcode
		 * 
		 * Rwb is the resistance between the wiper and Terminal B. The resitance
		 * for Terminal A is symmetrical:
		 * 
		 * \code
		 * data | Rwa (Ohm) | Output State
		 * -----+-----------+-----------------
		 *  255 |       89  | Full scale
		 *  128 |    5 050  | Midscale (reset() condition)
		 *    1 |   10 011  | 1 LSB
		 *    0 |   10 050  | Zero-scale
		 * \endcode
		 * 
		 * It is possible to set up all four channels at once by using the
		 * shutdown mode and then setting all four channels individually. After
		 * resuming normal operation the wipers will take their new positions.
		 * 
		 * \param	channel		Channel which should be changed
		 * \param	data		New wiper value
		 */
		static void
		setValue(ad840x::Channel channel, uint8_t data);
	};
}

#include "ad840x_impl.hpp"

#endif // XPCC__AD840x_HPP
