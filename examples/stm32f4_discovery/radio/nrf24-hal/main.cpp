#include <xpcc/architecture/platform.hpp>
#include "../../stm32f4_discovery.hpp"

#include <xpcc/debug/logger.hpp>

#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

#include <xpcc/driver/radio/nrf24/nrf24_hal.hpp>


// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2 > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

typedef GpioOutputE12 Csn;

typedef xpcc::Nrf24Hal<SpiSimpleMaster2, Csn> nrf24hal;

MAIN_FUNCTION
{
	defaultSystemClock::enable();

	Csn::setOutput(xpcc::Gpio::High);

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::High);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	// Enable SPI 2
	GpioOutputB15::connect(SpiSimpleMaster2::Mosi);
	GpioInputB14::connect(SpiSimpleMaster2::Miso);
	GpioOutputB13::connect(SpiSimpleMaster2::Sck);
	SpiSimpleMaster2::initialize<defaultSystemClock, MHz10>();

	// Enable UART 2
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<defaultSystemClock, 115200>(12);


	// Initialize nRF24-HAL
	nrf24hal::initialize();

	XPCC_LOG_INFO << "Hello from nRF24-HAL example" << xpcc::endl;

	nrf24hal::setRxAddress(0, (uint64_t)0xdeadb33f05);

	uint64_t addr = nrf24hal::getRxAddress(0);

	XPCC_LOG_INFO.printf("RX address is: 0x%x%x\n", static_cast<uint32_t>((addr >> 32) & 0xffffffff), static_cast<uint32_t>(addr & 0xffffffff));

	nrf24hal::setTxAddress(0xabcdef55ff);

	addr = nrf24hal::getTxAddress();

	XPCC_LOG_INFO.printf("TX address is: 0x%x%x\n", static_cast<uint32_t>((addr >> 32) & 0xffffffff), static_cast<uint32_t>(addr & 0xffffffff));


	uint8_t rf_ch = nrf24hal::readRegister(xpcc::nrf24::Register::RF_CH);

	XPCC_LOG_INFO.printf("RF_CH is: 0x%x\n", rf_ch);

	while (1)
	{
		SpiSimpleMaster2::writeBlocking(0xF0);
	}

	return 0;
}
