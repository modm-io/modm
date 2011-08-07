
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
