
#include <iostream>
#include <xpcc/architecture/driver/linux/serial_interface.hpp>
#include <xpcc/debug/logger/imp/std.hpp>

int
main()
{
	xpcc::pc::SerialInterface port("/dev/ttyUSB0", 115200);
	
	if (!port.open()) {
		std::cerr << "Could not open port: " << port.getDeviceName() << std::endl;
	}
	
    while(true)
    {
    	char a;
		port.readBytes(&a, 1);
		std::cout << "Read: " << a << std::endl; 
		
		/*char a;
		if (port.read(a))
		{
			std::cout << "Read: " << a << std::endl; 
		}*/
	}
    
	port.close();
}
