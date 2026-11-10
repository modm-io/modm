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
#include <iterator>
#include "i2c_master_common.hpp"

namespace modm::platform
{

/**
 * The I2C controller driver shared by all instances of the SB/ADDR/BTF peripheral (I2C v1).
 *
 * It implements modm::I2cMaster on the peripheral at `peripheral`, the static
 * `I2cMasterN` classes forward to one object of it each. The state machine was
 * derived from the reference manual and errata models and stress tested on
 * hardware, the states carry the names of these models.
 *
 * @ingroup	modm_platform_i2c
 */
class I2cMasterBase : public I2cMasterCommon
{
public:
	using I2cMasterCommon::I2cMasterCommon;

	/// Attaches the transaction and starts it, `false` while the bus is in use.
	bool
	start(I2cTransaction *transaction, ConfigurationHandler handler = nullptr);

	/// Ends the transfer with a STOP after the current byte, detaches the transaction,
	/// resets the peripheral and clears the bus with `recoverBus`
	/// if a device still holds it. Busy waits up to 4 ms; not from an interrupt.
	void
	reset();

	/// The interrupt handler of the event and error interrupts.
	void
	isr();

	/// Disables the peripheral and sets the clock registers.
	void
	configure(uint8_t peripheralFrequencyMHz, uint8_t riseTime, uint16_t clockControl);

	// the state machine, with the names of the peripheral model
	enum class
	State : uint8_t
	{ Idle, SbWait, Addr, Tx, TxBtf, Rx, RxBtf, RxEnd, AfWait, Stopping, Corrupt, BusyOther, Disabled, RxPos };
	enum
	Flag : uint8_t
	{ SB = 0, ADDR = 1, BTF = 2, RXNE = 6, TXE = 7, BERR = 8, ARLO = 9, AF = 10 };
	enum class
	Action : uint8_t
	{
		None, Start, Address, ClearTx, Write, Read, Wait, Late, Extra, Nack, Berr, Arlo, Reset, Unexpected,
		ClearPingP, ClearPingSr, ClearRx1P, ClearRx1Sr, EndTxP, EndTxSr, ReadEndP, ReadEndSr, BtfEndP, BtfEndSr,
		ReadLastP, ReadLastSr, ClearRx, BtfPosP, BtfPosSr, ClearRxPos
	};

	/// The names of the states, flags and actions, for example to print a trace.
	static constexpr const char *
	stateName(uint8_t state)
	{
		constexpr const char *names[] = {"Idle", "SbWait", "Addr", "Tx", "TxBtf", "Rx", "RxBtf", "RxEnd", "AfWait", "Stopping", "Corrupt", "BusyOther", "Disabled", "RxPos"};
		return (state < std::size(names)) ? names[state] : "?";
	}
	static constexpr const char *
	flagName(uint8_t flag)
	{
		switch (flag)
		{
			case SB: return "SB";
			case ADDR: return "ADDR";
			case BTF: return "BTF";
			case RXNE: return "RXNE";
			case TXE: return "TXE";
			case BERR: return "BERR";
			case ARLO: return "ARLO";
			case AF: return "AF";
			default: { const char *name = pseudoFlagName(flag); return name ? name : "?"; }
		}
	}
	static constexpr const char *
	actionName(uint8_t action)
	{
		constexpr const char *names[] = {"None", "Start", "Address", "ClearTx", "Write", "Read", "Wait", "Late", "Extra", "Nack", "Berr", "Arlo", "Reset", "Unexpected", "ClearPingP", "ClearPingSr", "ClearRx1P", "ClearRx1Sr", "EndTxP", "EndTxSr", "ReadEndP", "ReadEndSr", "BtfEndP", "BtfEndSr", "ReadLastP", "ReadLastSr", "ClearRx", "BtfPosP", "BtfPosSr", "ClearRxPos"};
		return (action < std::size(names)) ? names[action] : "?";
	}

private:
	void begin();
	bool segment(I2cTransaction::Starting starting);
	void unexpected(uint8_t flag);
	void refuse();
	void detach(DetachCause cause);
	void hwReset();
	bool busBusy() const;
	bool more();
	void finishSegment();
	void end(bool stop);
	void buffer(bool on);

	void
	go(State before, uint8_t flag, Action action, State after)
	{
		trace(uint8_t(before), flag, uint8_t(action), uint8_t(after));
		state = after;
	}

	volatile State state{State::Idle};
	uint8_t freq{0}, trise{0};
	uint16_t ccr{0};
};

} // namespace modm::platform
