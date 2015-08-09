#include "../../../stm32f4_discovery.hpp"
#include <xpcc/debug/logger.hpp>
#include <xpcc/processing.hpp>

#include <xpcc/driver/radio/nrf24/nrf24_phy.hpp>
#include <xpcc/driver/radio/nrf24/nrf24_config.hpp>
#include <xpcc/driver/radio/nrf24/nrf24_data.hpp>


/*
 * This example showcases a simple usage of the data layer implementation for
 * the nRF24L01+ radio modules.
 *
 * Check output on UART.
 *
 * Connect as follows
 *
 * STM32  NRF24
 * ------------
 * PB13 - SCK
 * PB14 - MISO
 * PB15 - MOSI
 * PE11 - CE
 * PE12 - CSN
 *
 * STM32  USB2UART
 * ---------------
 * PA2  - TXD
 */

#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO


// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

/* nrf24 interface */
typedef SpiMaster2      Spi;
typedef GpioOutputE12   Csn;
typedef GpioOutputE11   Ce;

/* Setup and connect layers */
typedef xpcc::Nrf24Phy<Spi, Csn, Ce> nrf24phy;
typedef xpcc::Nrf24Config<nrf24phy> nrf24config;
typedef xpcc::Nrf24Data<nrf24phy> nrf24data;

MAIN_FUNCTION
{
	Board::initialize();

	// Setup GPIOs
	Csn::setOutput(xpcc::Gpio::High);
	Ce::setOutput(xpcc::Gpio::Low);

	// Enable SPI 2
	GpioOutputB15::connect(Spi::Mosi);
	GpioInputB14::connect(Spi::Miso);
	GpioOutputB13::connect(Spi::Sck);
	Spi::initialize<Board::systemClock, MHz10>();

	// Enable UART 2
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	XPCC_LOG_INFO << "Hello from nrf24-data-tx example" << xpcc::endl;

	// Send a packet every 500ms
	xpcc::PeriodicTimer sendTimer(500);

	// Initialize physical layer with payload size of 6 bytes, this gives us
	// a payload size of 4 bytes in the data layer, as the header takes 2 bytes.
	nrf24phy::initialize(6);

	// This is the base address of our network, imagine this as an IP subnet
	// like 192.168.1.0/24. Coincidentally, our the network also has 256
	// addresses. The lower byte should be zero because addresses will be placed
	// there.
	nrf24data::BaseAddress base_address = 0xdeadbeef00;

	// We need to define a broadcast address, this can be any address.
	nrf24data::Address broadcast_address = 0xff;

	// Now we define the addresses of our 2 endpoints in this example.
	nrf24data::Address address_tx_side = 0x11;
	nrf24data::Address address_rx_side = 0x33;

	// Initialize data layer, this also rudimentary configures the physical
	// layer (e.g. speed, crc) so we just need to set a channel and are good to
	// go.
	nrf24data::initialize(base_address, address_tx_side, broadcast_address);
	nrf24config::setChannel(10);

	// Using the data layer, we send and receive packets, so we need to allocate
	// one. It's automatically allocated with the max. payload length configured.
	nrf24data::Packet packet;

	// Send this packet to the other endpoint of this example
	packet.dest = address_rx_side;

	// We have 4 bytes payload, so interpret them as an uint32_t and increment
	// over time
	uint32_t* data = reinterpret_cast<uint32_t*>(packet.payload.data);

	while(1)
	{
		if(sendTimer.execute())
		{
			XPCC_LOG_INFO << "Send packet" << xpcc::endl;
			nrf24data::sendPacket(packet);

			*data += 1;
		}

		nrf24data::update();
	}

	return 0;
}
