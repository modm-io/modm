xpcc::sab::Interface < ... > interface;

while (1)
{
	interface.update();
	if (interface.isMessageAvailable())
	{
		// do something ... for example print the received message
		stream << xpcc::hex;
		
		const uint8_t *data = interface.getPayload();
		for (std::size_t i = 0; i < interface.getPayloadLength(); ++i)
		{
			stream << *data++;
		}
		
		stream << xpcc::endl;
		
		interface.dropMessage();
	}
}
