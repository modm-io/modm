/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2015, 2017, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_I2C_HPP
#define MODM_INTERFACE_I2C_HPP

#include <modm/architecture/detect.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/fiber.hpp>
#include <cstdint>

#ifdef MODM_I2C_TRACE
/**
 * Called by the I2C controller drivers that support it (STM32 I2cMasterN,
 * BitBangI2cMaster) for every step of their transfers, off by default. Name a function
 * with `modm:build:cppdefines` as `MODM_I2C_TRACE=my_trace` and define
 * `void my_trace(uintptr_t instance, uint8_t before, uint8_t flag, uint8_t action,
 * uint8_t after)`, for example to record the steps; see the example `communication/i2c_trace`.
 *
 * - `instance` tells the controllers apart: the address of the peripheral's registers
 *   (`I2C1_BASE`, ...), or BitBangI2cMaster<Scl, Sda>::traceInstance().
 * - `before` and `after` are the driver's states, `flag` the event that ended the step
 *   (a flag of the peripheral, or a pseudo flag for software steps), `action` what the
 *   driver did. The driver class names them with stateName(), flagName() and actionName()
 *   (for the STM32 drivers: modm::platform::I2cMasterBase).
 * - The flag `modm::I2c::TraceFailure` marks the end of a transfer with an error other
 *   than an address or data NACK, or a reset(): `action` is the I2cMaster::Error then,
 *   `before` and `after` are 0xff.
 *
 * The STM32 drivers call it from their interrupt or with the interrupts disabled,
 * BitBangI2cMaster from the fiber in start(). The calls of one controller never interrupt
 * each other, but those of another controller may: keep the state of each instance apart.
 */
void MODM_I2C_TRACE(uintptr_t instance, uint8_t before, uint8_t flag, uint8_t action, uint8_t after);
#endif

namespace modm
{

/// @ingroup modm_architecture_i2c
struct I2c
{
	/// The flag of the step that marks a failure in MODM_I2C_TRACE, the same in every driver.
	static constexpr uint8_t TraceFailure = 20;

	static constexpr uint8_t Write = 0x00;	///< Add the Write bit to the slave address
	static constexpr uint8_t Read  = 0x01;	///< Add the Read bit to the slave address

	/// The signature of the configuration function.
	using ConfigurationHandler = void(*)();

	/// This tells the `I2cTransaction` why it was detached
	/// @see I2cTransaction
	enum class
	DetachCause : uint8_t
	{
		NormalStop,		///< All operations finished normally
		ErrorCondition,	///< A bus error occurred and the bus was reset
		FailedToAttach	///< The I2cTransaction failed to attach to the I2cMaster
	};

	///@{
	/// I2C Operations that the `I2cTransaction` can give the master on callback
	/// @see I2cTransaction

	/// All possible I2C Operations.
	enum class
	Operation : uint8_t
	{
		Stop    = 0,	///< Generate a Stop Condition
		Restart = 1,	///< Generate a Restart
		Write   = 2,	///< Write data to the slave
		Read    = 3,	///< Read data from the slave
	};

	/// Further operations after start operation.
	enum class
	OperationAfterStart : uint8_t
	{
		Stop  = uint8_t(Operation::Stop),		///< Generate a Stop Condition
		Write = uint8_t(Operation::Write),		///< Write data to the slave
		Read  = uint8_t(Operation::Read),		///< Read data from the slave
	};

	/// Further operations after write operation.
	enum class
	OperationAfterWrite : uint8_t
	{
		Stop    = uint8_t(Operation::Stop),		///< Generate a Stop Condition
		Restart = uint8_t(Operation::Restart),	///< Generate a Restart
		Write   = uint8_t(Operation::Write),	///< Write data to the slave
	};

	/// Further operations after read operation.
	enum class
	OperationAfterRead : uint8_t
	{
		Stop    = uint8_t(Operation::Stop),		///< Generate a Stop Condition
		Restart = uint8_t(Operation::Restart),	///< Generate a Restart
	};
	///@}

	/// State of a I2c Transaction
	enum class
	TransactionState : uint8_t
	{
		Idle,	///< No error occurred, detached normally
		Busy,	///< The transaction object is busy with data transfer
		Error	///< An error occurred, check the masters `getErrorCode()`
	};

	/**
	 * Reset all slave devices connected to an I2C bus.
	 *
	 * During normal operation, I2C slave device may pull the SDA line low.
	 * However, if the master is reset during a transaction, the I2C clock
	 * may stop while the slave is outputting a low data bit and the slave will
	 * continue to hold this bit (forever, and ever and ever).
	 * The I2C master is then unable to generate a I2C start condition, since SDA
	 * is still held low by the slave device, resulting in a deadlock.
	 *
	 * "You can always get it back to standby mode by allowing the SDA line to
	 * float high and give it 9 clocks.
	 * This assures that the device will not receive the acknowledge bit at
	 * the end the current byte and will abort the command and go to standby."
	 *
	 * @see	Application Note AN572 by Microchip
	 *
	 * This clears the bus as described in the I2C specification (UM10204 3.1.16):
	 * Only while SDA is held low, it clocks SCL until SDA is released, at most nine
	 * times, since a device may take more clock pulses as a data byte and write it to
	 * a register. Then it generates a start and a stop condition.
	 *
	 * @warning	Must be called **before** connecting SDA and SCL to I2cMaster,
	 *			with both pins configured as open-drain outputs!
	 *
	 * @tparam	Scl		The clock pin of the bus to be reset.
	 * @tparam	Sda		The data pin of the bus to be reset.
 	 */
	template< class Scl, class Sda >
	static void
	resetDevices(uint32_t baudrate = 100'000)
	{
		const std::chrono::microseconds delay{500'000ul / baudrate};
		clearBus<Scl, Sda>([&] { modm::this_fiber::sleep_for(delay); });
	}

	/// The sequence of resetDevices() with `wait()` for each half clock period, so that
	/// drivers can run it with a busy wait, for example from an interrupt.
	/// @param	finishRead	clock all nine pulses, even while SDA is high: a device cut off
	///			while it sends a byte releases SDA on its 1 bits only and needs the NACK
	///			of the ninth pulse to stop (measured with a CS43L22, which does not reset on
	///			START in the middle of a byte). Only after a read, since a device that
	///			receives would take the pulses as a data byte.
	template< class Scl, class Sda, class Wait >
	static void
	clearBus(Wait&& wait, bool finishRead = false)
	{
		Scl::set();
		Sda::set();
		wait();
		for (uint_fast8_t ii = 0; ii < 9 and (finishRead or not Sda::read()); ++ii) {
			Scl::reset();
			wait();
			Scl::set();
			wait();
		}
		// START and STOP while SCL is high and SDA released. A device still sending a
		// byte drives its next bit after SCL falls, so a STOP prepared with SCL low fails
		// whenever that bit is 0 (SDA stays low, the next START loses arbitration).
		// A START is detected at any time and resets the device's byte state.
		Sda::reset();
		wait();
		Sda::set();
		wait();
	}

};

}	// namespace modm

#if MODM_HAS_IOSTREAM
#include <modm/io/iostream.hpp>

namespace modm
{

/// @ingroup modm_architecture_i2c
/// @{

inline modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::DetachCause detach_cause)
{
	switch (detach_cause)
	{
		case modm::I2c::DetachCause::NormalStop:     s << "NormalStop";     break;
		case modm::I2c::DetachCause::ErrorCondition: s << "ErrorCondition"; break;
		case modm::I2c::DetachCause::FailedToAttach: s << "FailedToAttach"; break;
	}
	return s;
}

inline modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::Operation op)
{
	switch (op)
	{
		case modm::I2c::Operation::Stop:    s << "Stop";    break;
		case modm::I2c::Operation::Restart: s << "Restart"; break;
		case modm::I2c::Operation::Write:   s << "Write";   break;
		case modm::I2c::Operation::Read:    s << "Read";    break;
	}
	return s;
}

inline modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::OperationAfterStart op)
{
	s << static_cast<modm::I2c::Operation>(op);
	return s;
}

inline modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::OperationAfterRead op)
{
	s << static_cast<modm::I2c::Operation>(op);
	return s;
}

inline modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::OperationAfterWrite op)
{
	s << static_cast<modm::I2c::Operation>(op);
	return s;
}

/// @}

}	// namespace modm
#endif

#endif // MODM_INTERFACE_I2C_HPP
