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

#include "i2c_master_common.hpp"

namespace modm::platform
{

bool
I2cMasterCommon::attach(I2cTransaction *t, ConfigurationHandler handler)
{
	// There is no queue: while the bus is in use, start() fails without calling the
	// transaction, and the caller tries again; with fibers the scheduler decides the order.
	if (transaction or active) return false;
	// called right before the transfer starts, so that the transaction can still refuse
	if (not t->attaching())
	{
		t->detaching(DetachCause::FailedToAttach);
		return false;
	}
	if (handler and handler != configuration) { configuration = handler; handler(); }
	transaction = t;
	return true;
}

void
I2cMasterCommon::finish(DetachCause cause)
{
	active = false;
	if (cause == DetachCause::ErrorCondition and
		error != Error::AddressNack and error != Error::DataNack)
		trace(0xff, FAILURE, uint8_t(error), 0xff);
	I2cTransaction *t = transaction;
	transaction = nullptr;
	if (t) t->detaching(cause);	// may call start() again
}

void
I2cMasterCommon::detachAll()
{
	error = Error::SoftwareReset;
	trace(0xff, FAILURE, uint8_t(error), 0xff);
	active = false;
	if (I2cTransaction *t = transaction)
	{
		transaction = nullptr;
		t->detaching(DetachCause::ErrorCondition);
	}
}

bool
I2cMasterCommon::fetch(Operation op)
{
	index = 0;
	if (op == Operation::Read)
	{
		const auto r = transaction->reading();
		reading = true; rbuf = r.buffer; length = r.length; next = Operation(r.next);
		return length > 0;
	}
	reading = false;
	if (op == Operation::Write)
	{
		const auto w = transaction->writing();
		wbuf = w.buffer; length = w.length; next = Operation(w.next);
	}
	else { wbuf = nullptr; length = 0; next = Operation::Stop; }
	return true;
}

} // namespace modm::platform
