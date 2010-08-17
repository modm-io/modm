
#include <xpcc/architecture/driver.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp> 
#include <iostream>

int main()
{
	xpcc::pc::SerialPort port( "/dev/ttyUSB0", 9600);
	for(int i=0; i<100; i++)
	{
		port.write(i);
		std::cout<<"Send: "<<i<<std::endl;
    }
    while(true)
    {   char a;
		if(port.read(a))
		{
			std::cout<<"Read: "<<a<<std::endl;
			if(a =='x')
			{
				break;
			} 
		}
	}
	port.close();
}
