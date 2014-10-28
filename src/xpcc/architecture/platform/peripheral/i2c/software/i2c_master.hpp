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
#include <xpcc/architecture/peripheral/i2c_master.hpp>
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
		  uint32_t BaudRate = xpcc::I2cMaster::Baudrate::Standard >
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
	template< class clockSource, uint32_t baudrate=Baudrate::Standard,
			uint16_t tolerance = xpcc::Tolerance::FivePercent >
	static void
	initialize()
	{
		scl.set();
		sda.set();
	}

public:
	// start documentation inherited
	static bool
	start(I2cTransaction *transaction, Configuration_t configuration = nullptr);

	static Error
	getErrorState()
	{ return errorState; }

	static void
	reset();
	// end documentation inherited

private:
	static void
	error(Error error);

private:
	static inline bool
	startCondition();

	static inline bool
	stopCondition();

	static inline bool
	write(uint8_t data);

	static inline bool
	read(uint8_t &data, bool ack);

private:
	static inline bool
	readBit(uint8_t &data);

	static inline bool
	writeBit(bool bit);

	static inline bool
	sclSetAndWait();

	static ALWAYS_INLINE void
	delay();

	enum
	{
		ACK = true,
		NACK = false,
	};

	// calculate the delay in microseconds needed to achieve the
	// requested SPI frequency
	static constexpr float delayTime = (1000000.0 / BaudRate) / 2.0;

	static SCL scl;
	static SDA sda;

	static xpcc::I2c::Operation nextOperation;
	static xpcc::I2cTransaction *transactionObject;
	static Error errorState;

	static xpcc::I2cTransaction::Starting starting;
	static xpcc::I2cTransaction::Writing writing;
	static xpcc::I2cTransaction::Reading reading;
};

} // namespace xpcc

#include "i2c_master_impl.hpp"

#endif // XPCC_SOFTWARE_I2C_HPP
