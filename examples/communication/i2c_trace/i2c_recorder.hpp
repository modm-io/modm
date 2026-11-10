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
#include <modm/architecture/interface/i2c.hpp>
#include <modm/architecture/interface/i2c_master.hpp>
#include <modm/architecture/interface/atomic_lock.hpp>
#include <modm/io/iostream.hpp>
#include <iterator>

/**
 * The last steps of the I2C controllers, in one ring buffer per controller, fed by
 * MODM_I2C_TRACE. A failure (the TraceFailure step) freezes the buffer of its controller
 * until resume(), so that the steps up to it stay, while the others go on.
 *
 * The rings are kept apart because the calls for different controllers may interrupt each
 * other (the I2C interrupts of different priorities); the calls for one controller never do.
 * A controller gets its ring when it first traces, up to `Instances` controllers.
 */
template< size_t Steps = 32, size_t Instances = 4 >
class I2cRecorder
{
public:
	struct Step { uint8_t before, flag, action, after; };

	class Ring
	{
	public:
		/// Steps recorded, at most Steps.
		size_t size() const { return (count < Steps) ? count : Steps; }
		/// The steps, oldest first.
		Step operator[](size_t ii) const { return steps[(count < Steps) ? ii : (count + ii) % Steps]; }
		/// A failure froze the recording.
		bool isFrozen() const { return frozen; }

		/// Clears the steps and records again.
		void
		resume()
		{
			modm::atomic::Lock lock;
			count = 0;
			frozen = false;
		}

		/// Called from the I2C interrupt of this instance.
		void
		add(Step step)
		{
			if (frozen) return;
			steps[count++ % Steps] = step;
			if (step.flag == modm::I2c::TraceFailure) frozen = true;
		}

	private:
		Step steps[Steps]{};
		volatile size_t count{0};
		volatile bool frozen{false};
	};

	/// Call this from the function named by MODM_I2C_TRACE.
	void
	add(uintptr_t instance, Step step)
	{
		if (Ring *r = ring(instance)) r->add(step);
	}

	/// The ring of a controller, by the instance argument of MODM_I2C_TRACE (`I2C1_BASE`,
	/// BitBangI2cMaster<Scl, Sda>::traceInstance(), ...), claimed on first use; nullptr if
	/// all are taken.
	Ring *
	ring(uintptr_t instance)
	{
		modm::atomic::Lock lock;
		for (size_t ii = 0; ii < Instances; ii++)
		{
			if (instances[ii] == instance) return &rings[ii];
			if (instances[ii] == 0) { instances[ii] = instance; return &rings[ii]; }
		}
		return nullptr;
	}

private:
	uintptr_t instances[Instances]{};
	Ring rings[Instances];
};

/// Prints the steps of a ring, one per line: `state.flag.action.state`, with the names of
/// the driver class `Names` (modm::platform::I2cMasterBase for the STM32 drivers,
/// BitBangI2cMaster<Scl, Sda>), or the error of a failure.
template< class Names, class Ring >
void
printI2cTrace(modm::IOStream &stream, const Ring &ring)
{
	static constexpr const char *errors[] = {"NoError", "SoftwareReset", "AddressNack", "DataNack",
											 "ArbitrationLost", "BusCondition", "BusBusy", "Unknown"};
	for (size_t ii = 0; ii < ring.size(); ii++)
	{
		const auto step = ring[ii];
		if (step.flag == modm::I2c::TraceFailure)
		{
			stream << "  FAILURE " << ((step.action < std::size(errors)) ? errors[step.action] : "?") << modm::endl;
			continue;
		}
		stream << "  " << Names::stateName(step.before) << "." << Names::flagName(step.flag) << "."
			   << Names::actionName(step.action) << "." << Names::stateName(step.after) << modm::endl;
	}
	if (ring.isFrozen()) stream << "  (frozen at the failure)" << modm::endl;
}
