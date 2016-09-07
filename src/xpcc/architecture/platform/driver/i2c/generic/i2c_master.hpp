// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_I2C_HPP
#define XPCC_SOFTWARE_I2C_HPP

#include <xpcc/architecture/driver/delay.hpp>
#include <xpcc/architecture/interface/gpio.hpp>
#include <xpcc/architecture/interface/i2c_master.hpp>
#include "type_ids.hpp"

namespace xpcc
{

/**
 * Software emulation of a I2C master implementation
 *
 * @tparam	SCL			an Open-Drain pin
 * @tparam	SDA			an Open-Drain pin
 *
 * @ingroup	i2c
 * @author	Niklas Hauser
 * @see		gpio
 */
template< class SCL,
		  class SDA >
class SoftwareI2cMaster : public xpcc::I2cMaster
{
public:
	static const TypeId::SoftwareI2cMasterSda Sda;
	static const TypeId::SoftwareI2cMasterScl Scl;

public:
	/// Initializes the hardware, with the baudrate limited to about 250kbps.
	template< class SystemClock, uint32_t baudrate=Baudrate::Standard,
			uint16_t tolerance = xpcc::Tolerance::FivePercent >
	static void
	initialize()
	{
		delayTime = uint32_t(250000000) / baudrate;
		if (delayTime == 0) delayTime = 1;

		SCL::set();
		SDA::set();
	}

public:
	// start documentation inherited
	static bool
	start(I2cTransaction *transaction, ConfigurationHandler handler = nullptr);

	static Error xpcc_always_inline
	getErrorState()
	{ return errorState; }

	static inline void
	reset();
	// end documentation inherited

private:
	// error handling
	/// releases bus lines, sets error state and detaches the TO
	static inline void
	error(Error error);

	// bus condition operations
	/// generate a start or restart condition
	/// @return	`true` if success, `false` if bus in unknown condition
	static inline bool
	startCondition();

	/// generate a stop condition
	/// @return	`true` if success, `false` if bus in unknown condition
	static inline bool
	stopCondition();

	/// release the clock and wait for any slaves to release it too
	/// @return	`true` if success, `false` if slave stretched the clock for too long
	static inline bool
	sclSetAndWait();

	// byte operations
	/// write one byte to the bus
	/// @return	`true` if success, `false` if arbitation, too much clock stretching or NACK received
	static inline bool
	write(uint8_t data);

	/// read one byte from the bus
	/// @param	ack	acknowledge bit of read operation, `ACK` or `NACK`
	/// @return	`true` if success, `false` if arbitation occured
	static inline bool
	read(uint8_t &data, bool ack);

	/// write one bit to the bus
	/// @return	`true` if success, `false` if slave stretched the clock for too long
	static inline bool
	writeBit(bool bit);

	// bit operations
	/// read one bit from the bus
	/// @return	`true` if success, `false` if slave stretched the clock for too long
	static inline bool
	readBit(uint8_t &data);

	// timings
	/// busy waits a **half** clock cycle
	static xpcc_always_inline void
	delay2()
	{ xpcc::delayNanoseconds(delayTime*2); }

	// timings
	/// busy waits **quarter** clock cycle
	static xpcc_always_inline void
	delay4()
	{ xpcc::delayNanoseconds(delayTime); }

	enum
	{
		ACK = true,
		NACK = false,
	};

	// calculate the delay in microseconds needed to achieve the
	// requested I2C frequency
	static uint16_t delayTime;

	static xpcc::I2c::Operation nextOperation;
	static xpcc::I2cTransaction *transactionObject;
	static Error errorState;
	static xpcc::I2c::ConfigurationHandler configuration;

	static xpcc::I2cTransaction::Starting starting;
	static xpcc::I2cTransaction::Writing writing;
	static xpcc::I2cTransaction::Reading reading;
};

} // namespace xpcc

#include "i2c_master_impl.hpp"

#endif // XPCC_SOFTWARE_I2C_HPP
