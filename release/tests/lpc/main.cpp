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

#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Out, 0, 0);
GPIO__INPUT(In, 0, 1);
GPIO__IO(Io, 0, 2);

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

	while (1)
	{
	}
}
