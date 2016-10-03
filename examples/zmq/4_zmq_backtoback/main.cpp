
#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>

#include <xpcc/processing/timer.hpp>

#include <xpcc/communication.hpp>
#include <xpcc/communication/xpcc/backend/zeromq/connector.hpp>

xpcc::PeriodicTimer pt(2000);

int
main()
{
	const std::string endpointOut = "tcp://127.0.0.1:8211";
	const std::string endpointIn  = "tcp://127.0.0.1:8212";

	xpcc::ZeroMQConnector zmqConnectorServer(endpointIn, endpointOut, xpcc::ZeroMQConnector::Mode::PubPull);
	xpcc::ZeroMQConnector zmqConnectorClient(endpointOut, endpointIn, xpcc::ZeroMQConnector::Mode::SubPush);

	while(true)
	{
		zmqConnectorServer.update();
		zmqConnectorClient.update();

		if (zmqConnectorServer.isPacketAvailable())
		{
			xpcc::Header header = zmqConnectorServer.getPacketHeader();
			xpcc::SmartPointer payload = zmqConnectorServer.getPacketPayload();

			XPCC_LOG_DEBUG << "Server Received Header is:       " << header << xpcc::endl;
			XPCC_LOG_DEBUG << "Server Received Payload size is: " << payload.getSize() << xpcc::endl;
			XPCC_LOG_DEBUG << "Server Received Payload is:      " << payload << xpcc::endl;

			// zmqConnector.sendPacket(header, payload);

			zmqConnectorServer.dropPacket();
		}

		if (zmqConnectorClient.isPacketAvailable())
		{
			xpcc::Header header = zmqConnectorClient.getPacketHeader();
			xpcc::SmartPointer payload = zmqConnectorClient.getPacketPayload();

			XPCC_LOG_DEBUG << "Client Received Header is:       " << header << xpcc::endl;
			XPCC_LOG_DEBUG << "Client Received Payload size is: " << payload.getSize() << xpcc::endl;
			XPCC_LOG_DEBUG << "Client Received Payload is:      " << payload << xpcc::endl;

			// zmqConnector.sendPacket(header, payload);

			zmqConnectorClient.dropPacket();
		}

		if (pt.execute())
		{
			xpcc::Header header;

			uint8_t buf[] = { 0xde, 0xad, 0xbe, 0xef};
			xpcc::SmartPointer payload(&buf);

			zmqConnectorServer.sendPacket(header, payload);
		}

		xpcc::delayMilliseconds(100);
	}
}
