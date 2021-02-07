/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/ethernet/lan8720a.hpp>
#include <modm/processing/rtos.hpp>
#include <FreeRTOS_IP.h>
#include <FreeRTOS_Sockets.h>

using namespace Board;

namespace Ethernet
{
	using RMII_Ref_Clk = GpioInputA1;
	using RMII_Mdio = GpioA2;
	using RMII_Crs_Dv = GpioInputA7;
	using RMII_Tx_En = GpioOutputG11;
	using RMII_Tx_D0 = GpioOutputG13;
	using RMII_Tx_D1 = GpioOutputB13;
	using RMII_Mdc = GpioOutputC1;
	using RMII_Rx_D0 = GpioInputC4;
	using RMII_Rx_D1 = GpioInputC5;
	using Port = Eth<modm::Lan8720a>;
}

UBaseType_t ulNextRand;

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent);

class NetworkInitTask : modm::rtos::Thread
{
public:
	NetworkInitTask()
	: Thread(5, 2048, "network_init")
	{}

	void
	run()
	{
		uint8_t ipAddress[4] { 192, 168, 1, 1 };
		uint8_t netmask[4] { 255, 255, 255, 0 };
		uint8_t gatewayAddress[4] { 0, 0, 0, 0 };
		uint8_t dnsAddress[4] { 0, 0, 0, 0 };

		// local MAC address
		uint8_t macAddress[] { 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };

		// A real MAC address can be retrieved from the Microchip 24AA02E48
		// I2C EEPROM, which is locaed at address 0xFA.

		// initialize random numbers
	    time_t now;
	    time(&now);
	    ulNextRand = uint32_t(now);

	    FreeRTOS_IPInit(ipAddress,
	    		netmask,
				gatewayAddress,
				dnsAddress,
				&macAddress[0]);

	    vTaskDelete(0);
	}
};

class HttpConnection
{
	static constexpr TickType_t shutdownTimeout { pdMS_TO_TICKS(5000) };
	static constexpr TickType_t receiveTimeout { pdMS_TO_TICKS(5000) };
	static constexpr TickType_t sendTimeout { pdMS_TO_TICKS(5000) };

public:
	static constexpr char name[] { "HTTPConnection" };
	static constexpr uint8_t httpText[] = {
			"HTTP/1.1 200 OK \r\n"
			"Content-Type: text/html\r\n"
			"Connection: keep-alive\r\n"
			"\r\n"
			"<html><body><h1>Hello from your STM32!</h1></body></html>"
	};

	enum class
	ResponseStatus : uint16_t {
		Ok = 200,
		BadRequest = 400,
		NotFound = 404,
	};

	static void
	run(void *parameter)
	{
		Socket_t connectedSocket = reinterpret_cast<Socket_t>(parameter);
		uint8_t *buffer = reinterpret_cast<uint8_t *>(pvPortMalloc(ipconfigTCP_MSS));

		if (buffer) {
			FreeRTOS_setsockopt(connectedSocket, 0, FREERTOS_SO_RCVTIMEO, &receiveTimeout,
					sizeof(receiveTimeout));
			FreeRTOS_setsockopt(connectedSocket, 0, FREERTOS_SO_SNDTIMEO, &sendTimeout,
					sizeof(sendTimeout));

			while (true) {
				std::memset(buffer, 0, ipconfigTCP_MSS);
				int32_t bytes = FreeRTOS_recv(connectedSocket, buffer, ipconfigTCP_MSS, 0);
				if (bytes <= 0)
					break;
				if (FreeRTOS_send(connectedSocket, httpText, sizeof(httpText) - 1, 0) < 0)
					break;
			}
		}

		FreeRTOS_shutdown(connectedSocket, FREERTOS_SHUT_RDWR);
		TickType_t shutdownTime { xTaskGetTickCount() };
		do {
			if (FreeRTOS_recv(connectedSocket, buffer, ipconfigTCP_MSS, 0) < 0)
				break;
		} while ((xTaskGetTickCount() - shutdownTime) < shutdownTimeout);

		vPortFree(buffer);
		FreeRTOS_closesocket(connectedSocket);
		vTaskDelete(0);

	}
};

class HttpServerListener
{
	static constexpr TickType_t receiveTimeout { portMAX_DELAY };
	static constexpr BaseType_t backlog { 20 };

public:
	static constexpr char name[] { "HTTPListener" };

	static void
	run(void *)
	{
		Socket_t listeningSocket;
		Socket_t connectedSocket;

		listeningSocket = FreeRTOS_socket(FREERTOS_AF_INET,
				FREERTOS_SOCK_STREAM,
				FREERTOS_IPPROTO_TCP);
		FreeRTOS_setsockopt(listeningSocket, 0, FREERTOS_SO_RCVTIMEO, &receiveTimeout,
				sizeof(receiveTimeout));

#if ipconfigUSE_TCP_WIN == 1
		WinProperties_t winProps {
			.lTxBufSize = ipconfigTCP_TX_BUFFER_LENGTH,
			.lTxWinSize = 2,
			.lRxBufSize = ipconfigTCP_RX_BUFFER_LENGTH,
			.lRxWinSize = 2,
		};
		FreeRTOS_setsockopt(listeningSocket, 0, FREERTOS_SO_WIN_PROPERTIES,
				reinterpret_cast<void *>(&winProps), sizeof(winProps));
#endif

		struct freertos_sockaddr bindAddress {
			.sin_port = FreeRTOS_htons(80),
		};
		FreeRTOS_bind(listeningSocket, &bindAddress, sizeof(bindAddress));
		FreeRTOS_listen(listeningSocket, backlog);

		struct freertos_sockaddr clientAddress;

		while (true) {
			connectedSocket = FreeRTOS_accept(listeningSocket, &clientAddress, 0);
			char buffer[16];
			FreeRTOS_inet_ntoa(clientAddress.sin_addr, buffer);
			xTaskCreate(HttpConnection::run, HttpConnection::name, configMINIMAL_STACK_SIZE * 5,
					reinterpret_cast<void *>(connectedSocket), configMAX_PRIORITIES, 0);
		}
	}
};

NetworkInitTask networkInit;

int
main()
{
	Board::initialize();
	Leds::setOutput();
	MODM_LOG_INFO << "\n\nReboot: Ethernet Example" << modm::endl;

	Ethernet::Port::connect<Ethernet::RMII_Ref_Clk::Refclk,
		Ethernet::RMII_Mdc::Mdc,
		Ethernet::RMII_Mdio::Mdio,
		Ethernet::RMII_Crs_Dv::Rcccrsdv,
		Ethernet::RMII_Tx_En::Txen,
		Ethernet::RMII_Tx_D0::Txd0,
		Ethernet::RMII_Tx_D1::Txd1,
		Ethernet::RMII_Rx_D0::Rxd0,
		Ethernet::RMII_Rx_D1::Rxd1>();

    modm::rtos::Scheduler::schedule();

	// we should never get here
	return 0;
}

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent)
{
	static bool taskCreated = false;

	if (eNetworkEvent != eNetworkUp)
		return;

	if (not taskCreated) {
		xTaskCreate(HttpServerListener::run, HttpServerListener::name, configMINIMAL_STACK_SIZE * 2, 0, configMAX_PRIORITIES + 1, 0);
		taskCreated = true;
	}

	uint32_t ipAddress;
	uint32_t netmask;
	uint32_t gateway;
	uint32_t dns;
	char buffer[16];

	FreeRTOS_GetAddressConfiguration(&ipAddress, &netmask, &gateway, &dns);
	FreeRTOS_inet_ntoa(ipAddress, buffer);
	MODM_LOG_DEBUG << "IP address: " << buffer << modm::endl;
	FreeRTOS_inet_ntoa(netmask, buffer);
	MODM_LOG_DEBUG << "Netmask   : " << buffer << modm::endl;
	FreeRTOS_inet_ntoa(gateway, buffer);
	MODM_LOG_DEBUG << "Gateway   : " << buffer << modm::endl;
	FreeRTOS_inet_ntoa(dns, buffer);
	MODM_LOG_DEBUG << "DNS       : " << buffer << modm::endl;
}

UBaseType_t uxRand( void )
{
	static constexpr uint32_t ulMultiplier = 0x015a4e35UL;
	static constexpr uint32_t ulIncrement = 1UL;

	/* Utility function to generate a pseudo random number. */

	ulNextRand = ( ulMultiplier * ulNextRand ) + ulIncrement;
	return( ( int ) ( ulNextRand >> 16UL ) & 0x7fffUL );
}

BaseType_t xApplicationGetRandomNumber(uint32_t* pulNumber)
{
	*(pulNumber) = uxRand();
	return pdTRUE;
}

uint32_t ulApplicationGetNextSequenceNumber(uint32_t, uint16_t, uint32_t, uint16_t)
{
	return uxRand();
}
