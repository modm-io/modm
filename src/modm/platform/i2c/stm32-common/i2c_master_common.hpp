/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <modm/architecture/interface/i2c_master.hpp>
#include <modm/architecture/interface/i2c_transaction.hpp>
#include <modm/platform/device.hpp>
#include <cstddef>
#include <cstdint>


namespace modm::platform
{

/**
 * The part of the STM32 I2C controller driver that does not depend on the peripheral
 * version: attaching and detaching, fetching the buffers, and tracing the steps. I2cMasterBase adds the state machine of the peripheral.
 *
 * @ingroup	modm_platform_i2c
 */
class I2cMasterCommon : public ::modm::I2c
{
public:
	using Error = ::modm::I2cMaster::Error;

	/// @param	peripheral	the address of the I2C registers
	constexpr I2cMasterCommon(uintptr_t peripheral) :
		peripheral(peripheral) {}

	Error
	getErrorState() const
	{ return error; }

	/// A transaction is attached or its transfer still runs.
	bool
	busy() const
	{ return active or transaction; }

	/// Clears a bus that a device holds, see I2c::clearBus(), `true` after a read was cut
	/// off. Called by reset() and when the bus is still busy at a START.
	void (*recoverBus)(bool afterRead) = nullptr;

	/// pseudo flags of the software steps, see MODM_I2C_TRACE in modm/architecture/interface/i2c.hpp
	enum : uint8_t { START = 16, POLL = 17, DEADLINE = 18, BUSY_AT_START = 19, FAILURE = modm::I2c::TraceFailure };

	/// The name of a pseudo flag, nullptr for others.
	static constexpr const char *
	pseudoFlagName(uint8_t flag)
	{
		switch (flag)
		{
			case START: return "START";
			case POLL: return "POLL";
			case DEADLINE: return "DEADLINE";
			case BUSY_AT_START: return "BUSY_AT_START";
			case FAILURE: return "FAILURE";
			default: return nullptr;
		}
	}

protected:
	/// Attaches the transaction if the bus is free, call with interrupts locked.
	bool
	attach(I2cTransaction *transaction, ConfigurationHandler handler);

	/// Detaches the current transaction.
	void
	finish(DetachCause cause);

	/// Detaches the current transaction after a reset.
	void
	detachAll();

	/// The next buffer: op is what the transaction said follows (Read, Write or Stop for
	/// a ping). `false` for a read of length 0, which is refused.
	bool
	fetch(Operation op);

	void
	trace([[maybe_unused]] uint8_t before, [[maybe_unused]] uint8_t flag,
		  [[maybe_unused]] uint8_t action, [[maybe_unused]] uint8_t after)
	{
#ifdef MODM_I2C_TRACE
		::MODM_I2C_TRACE(peripheral, before, flag, action, after);
#endif
	}

	I2C_TypeDef *
	regs() const
	{ return reinterpret_cast<I2C_TypeDef *>(peripheral); }

	const uintptr_t peripheral;

	I2cTransaction *volatile transaction{nullptr};
	ConfigurationHandler configuration{nullptr};

	// the current buffer, fetched before its START
	uint8_t address{0};
	bool reading{false};
	size_t length{0}, index{0};
	uint8_t *rbuf{nullptr};
	const uint8_t *wbuf{nullptr};
	Operation next{Operation::Stop};	///< after this buffer: Stop, Restart or Write (no Sr)

	volatile bool active{false};
	volatile Error error{Error::NoError};
};

} // namespace modm::platform
