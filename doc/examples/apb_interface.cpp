typedef xpcc::apb::Interface <...> interface;

while (1)
{
	if (interface.isMessageAvailable())
	{
		const uint8_t *data = interface.getData();
		
		// do something ... for example print the received message
		stream << xpcc::hex;
		for (uint_fast8_t i = 0; i < interface.getLength(); ++i) {
			stream << *data++;
		}
		stream << xpcc::endl;
		
		interface.dropMessage();
	}
	
	interface.update();
}
