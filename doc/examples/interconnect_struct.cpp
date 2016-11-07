/*
 * Copyright (c) 2011, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

struct MasterToSlave
{
	uint8_t commandCounter;
	Parameter commandParameter;
};

struct SlaveToMaster
{
	uint8_t commandResponse;
};
