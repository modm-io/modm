/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>

using Out = modm::lpc::GpioOutput0_1;
using In  = modm::lpc::GpioInput0_2;
using Io  = modm::lpc::Gpio0_3;

int
main()
{
	Out::setOutput();
	Out::setOutput(true);
	Out::set();
	Out::reset();
	Out::toggle();

	In::setInput();
	In::read();

	Io::setOutput();
	Io::set();
	Io::setInput();
	Io::read();

	while (true)
	{
	}
}
