/*
 * Copyright (c) 2023, Alexander Solovets
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/driver/atomic/flag.hpp>
#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/driver/inertial/qmc5883l.hpp>

using namespace Board;
using namespace std::chrono_literals;

using Compass = Qmc5883l<I2cMaster>;
Compass::Data data;
Compass compass(data);
modm::atomic::Flag dataReady(true);

MODM_ISR(INT2)
{
	dataReady.set();
}

int
main()
{
	Board::initialize();
	// DRDY pin must be connected to Board pin 19 (Port D2).
	Board::D19::setInput(Gpio::InputType::PullUp);
	Board::D19::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Board::D19::enableExternalInterrupt();

	RF_CALL_BLOCKING(compass.initialize());
	auto mode = Compass::Mode_t(Compass::Mode::Continious);
	auto rate = Compass::OutputDataRate_t(Compass::OutputDataRate::_10Hz);
	auto scale = Compass::FullScale_t(Compass::FullScale::_8G);
	RF_CALL_BLOCKING(compass.configure(mode, rate | scale));

	for (;;)
	{
		if (dataReady.testAndSet(false))
		{
			if (RF_CALL_BLOCKING(compass.readData()))
			{
				MODM_LOG_INFO << "X:" << compass.x() << " Y: " << compass.y()
							  << " Z: " << compass.z()
							  << " OVL: " << (compass.status() & Compass::Status::OVL)
							  << modm::endl;
			} else
			{
				serialStream << "readDataBlocking(): Error: " << uint8_t(I2cMaster::getErrorState())
							 << modm::endl;
			}
		}
	}
}
