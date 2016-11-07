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

InterconnectInterface<MasterToSlave, SlaveToMaster> interface;

void
executeCommand(const Parameter& parameter)
{
	MasterToSlave *transmit = interface.getTransmitBuffer();
	
	transmit->commandCounter++;
	transmit->commandParameter = parameter;
	
	interface.releaseTransmitBuffer();
}

bool
isFinished()
{
	const MasterToSlave *transmit = interface.readTransmitBuffer();
	const SlaveToMaster *receive = interface.getReceiveBuffer();
	
	bool isFinished =
			(transmit->commandCounter == receive->commandResponse);
	
	interface.releaseReceiveBuffer();
	return isFinished;
}
