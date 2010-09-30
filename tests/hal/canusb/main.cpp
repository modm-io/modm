
#include <xpcc/driver/can/canusb.hpp>
#include <xpcc/communication/backend/can/can_connector.hpp>
#include <xpcc/debug/logger/imp/std.hpp>
#include <iostream>
#include <iomanip>

int main()
{
	/*xpcc::CanUsb usb("/dev/ttyUSB0", 9600);
	
	if(usb.open()) {
		std::cout<<"SUCCESS!";
	}
	else{
		std::cout<<"FAILED";
	}
	*/
	xpcc::CanUsb usb("/dev/ttyUSB0", 9600);
	std::cout<<"canusb created"<<std::endl;
	usb.open();
	std::cout<<"canusb opened"<<std::endl;
	xpcc::CanConnector<xpcc::CanUsb> connector(&usb);
	std::cout<<"canConnector created"<<std::endl;
	static int messageCount=0;
	while(messageCount<100)
	{
		if(connector.isPacketAvailable())
		{
			messageCount++;
			xpcc::Header header=connector.getPacketHeader();
			connector.dropPacket();
			XPCC_LOG_INFO << header << xpcc::endl;
			
			
		}
		connector.update();
	}
	usb.close();
	std::cout<<"usb cloesed"<<std::endl;
	
	if(usb.open())
	{
		std::cout<<"usb reopened"<<std::endl;
	}
	else
	{
		std::cout<<"usb not opened"<<std::endl;
	}
	messageCount=0;
	while(messageCount<100)
	{
		if(connector.isPacketAvailable())
		{
			messageCount++;
			xpcc::Header header=connector.getPacketHeader();
			connector.dropPacket();
			XPCC_LOG_INFO << header << xpcc::endl;
			
			
		}
		connector.update();
	}
	
	return 0;
}
