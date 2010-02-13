typedef xpcc::apb::Interface <...> PeripheralInterface;

while (1)
{
	uint8_t length = PeripheralInterface::checkForMessage();
	if (length > 0)
	{
		// ... do something usefull with the data ...
		
		PeripheralInterface::dropMessage();
	}
	
	PeripheralInterface::update();
}
