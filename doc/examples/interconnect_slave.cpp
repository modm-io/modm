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

InterconnectInterface<SlaveToMaster, MasterToSlave> interface;

uint8_t commandCounter;

void
processReceivedData()
{
	const MasterToSlave *receive = interface.getReceiveBuffer();
	
	if (receive->commandCounter != commandCounter)
	{
		commandCounter = receive->commandCounter;
		
		// execute the command
		doSomething(receive->commandParameter);
	}
	interface.releaseReceiveBuffer();
}

void
DataExchange::prepareDataForTransmission()
{
	SlaveToMaster *transmit = interface.getTransmitBuffer();
	
	if (isSomethingFinished()) {
		transmit->commandResponse = commandCounter;
	}
	interface.releaseTransmitBuffer();
}
