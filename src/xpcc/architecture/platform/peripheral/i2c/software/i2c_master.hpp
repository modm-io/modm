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
#include <xpcc/architecture/peripheral/gpio.hpp>
#include <xpcc/architecture/peripheral/i2c.hpp>
#include "type_ids.hpp"

namespace xpcc
{

/**
 * @brief	Software emulation of a I2C master implementation
 *
 * @tparam	SCL			an Open-Drain Output pin
 * @tparam	SDA			an Open-Drain Output pin
 * @tparam	Baudrate	in Hz (default frequency is 100kHz)
 *
 * @ingroup	i2c
 * @author	Niklas Hauser
 * @see		gpio
 */
template< typename SCL,
		  typename SDA,
		  uint32_t Baudrate = xpcc::I2cMaster::DataRate::Standard >
class SoftwareI2cMaster : public xpcc::I2cMaster
{
public:
	static const TypeId::SoftwareI2cMasterSda Sda;
	static const TypeId::SoftwareI2cMasterScl Scl;

public:
	/**
	 * @brief	Initialize the hardware
	 *
	 * @warning	this call cannot modify the baudrate anymore, since it is defined
	 * 			by the template parameter Baudrate.
	 */
	template< class clockSource, uint32_t baudrate=DataRate::Standard,
			uint16_t tolerance = xpcc::Tolerance::FivePercent >
	static void
	initialize();

public:
	// start documentation inherited
	static bool
	start(xpcc::I2cTransaction *transaction);

	static ALWAYS_INLINE bool
	startBlocking(xpcc::I2cTransaction *transaction)
	{
		return start(transaction);
	};

	static Error
	getErrorState();

	static void
	reset(DetachCause cause=DetachCause::SoftwareReset);
	// end documentation inherited

private:
	static void
	error();

private:
	static inline void
	startCondition();

	static inline void
	stopCondition();

	static inline bool
	write(uint8_t data);

	static inline uint8_t
	read(bool ack);

private:
	static inline bool
	readBit();

	static inline void
	writeBit(bool bit);

	static inline void
	sclSetAndWait();

	static ALWAYS_INLINE void
	delay();

	// calculate the delay in microseconds needed to achieve the
	// requested SPI frequency
	static constexpr float delayTime = (1000000.0 / Baudrate) / 2.0;

	static SCL scl;
	static SDA sda;

	static xpcc::I2c::Operation nextOperation;
	static xpcc::I2cTransaction *delegate;
	static Error errorState;

	static xpcc::I2cTransaction::Starting starting;
	static xpcc::I2cTransaction::Writing writing;
	static xpcc::I2cTransaction::Reading reading;
};

} // namespace xpcc

#include "i2c_master_impl.hpp"

#endif // XPCC_SOFTWARE_I2C_HPP
