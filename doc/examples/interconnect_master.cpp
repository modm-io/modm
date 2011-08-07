
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
