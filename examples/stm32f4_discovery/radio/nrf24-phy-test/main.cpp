#include "../../stm32f4_discovery.hpp"
#include <xpcc/driver/radio/nrf24/nrf24_phy.hpp>
#include <xpcc/debug/logger.hpp>

/*
 * Test SPI communication by writing and reading out registers on the
 * nRF24L01+ module. Check output on UART
 *
 * Connect as follows
 *
 * STM32  NRF24
 * ------------
 * PB13 - SCK
 * PB14 - MISO
 * PB15 - MOSI
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

typedef GpioOutputE12 Csn;

// just needed to make Nrf24Phy happy
typedef GpioOutputA1  Ce;


typedef xpcc::Nrf24Phy<SpiMaster2, Csn, Ce> nrf24phy;

int
main()
{
	Board::initialize();

	Csn::setOutput(xpcc::Gpio::High);

	// Enable SPI 2
	GpioOutputB15::connect(SpiMaster2::Mosi);
	GpioInputB14::connect(SpiMaster2::Miso);
	GpioOutputB13::connect(SpiMaster2::Sck);
	SpiMaster2::initialize<Board::systemClock, MHz10>();

	// Enable UART 2
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);


	// Initialize nRF24-Phy
	nrf24phy::initialize();

	XPCC_LOG_INFO << "Hello from nRF24-phy-test example" << xpcc::endl;


	uint8_t rf_ch;
	uint64_t addr;


	while (1)
	{

		nrf24phy::setRxAddress(nrf24phy::Pipe::PIPE_0, 0xdeadb33f05);
		addr = nrf24phy::getRxAddress(nrf24phy::Pipe::PIPE_0);
		XPCC_LOG_INFO.printf("Setting RX_P0 address to:  0xDEADB33F05\n");
		XPCC_LOG_INFO.printf("Reading RX_P0 address:     0x%x%x\n", static_cast<uint32_t>((addr >> 32) & 0xffffffff), static_cast<uint32_t>(addr & 0xffffffff));

		nrf24phy::setTxAddress(0xabcdef55ff);
		addr = nrf24phy::getTxAddress();
		XPCC_LOG_INFO.printf("Setting TX address to:     0xABCDEF55FF\n");
		XPCC_LOG_INFO.printf("Reading TX address:        0x%x%x\n", static_cast<uint32_t>((addr >> 32) & 0xffffffff), static_cast<uint32_t>(addr & 0xffffffff));

		rf_ch = nrf24phy::readRegister(nrf24phy::NrfRegister::RF_CH);
		XPCC_LOG_INFO.printf("Expected output for RF_CH: 0x2\n");
		XPCC_LOG_INFO.printf("Reading RF_CH:             0x%x\n\n", rf_ch);

		xpcc::delayMilliseconds(1000);
	}

	return 0;
}
