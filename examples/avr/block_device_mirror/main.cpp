#include <xpcc/architecture.hpp>

#include <xpcc/driver/storage/block_device_heap.hpp>
#include <xpcc/driver/storage/block_device_mirror.hpp>

#include <xpcc/debug/logger.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

// Create a new UART object and configure it to a baudrate of 115200
Uart0 uart;
xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > loggerDevice(uart);

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

void printMemoryContent(const uint8_t* address, std::size_t size) {
	for (std::size_t i = 0; i < size; i++) {
		XPCC_LOG_INFO.printf("%x", address[i]);
	}
}

int
main()
{
	/**
	 * This example/test writes alternating patterns into
	 * two `xpcc::BdFile` block device with a size of 1M
	 * to test the virtual `xpcc::BdMirror` block device.
	 * The memory content is afterwards read and compared
	 * to the pattern.
	 * Write and read operations are done on 64 byte blocks.
	 */

	constexpr uint32_t BlockSize = 8;
	constexpr uint32_t MemorySize = 64;

	uint8_t bufferA[BlockSize];
	uint8_t bufferB[BlockSize];
	uint8_t bufferC[BlockSize];
	std::memset(bufferA, 0xAA, BlockSize);
	std::memset(bufferB, 0x55, BlockSize);

	xpcc::BdMirror<xpcc::BdHeap<MemorySize>, xpcc::BdHeap<MemorySize>> storageDevice;

	if(!RF_CALL_BLOCKING(storageDevice.initialize())) {
		XPCC_LOG_INFO << "Error: Unable to initialize device.";
		exit(1);
	}

	XPCC_LOG_INFO << "Starting memory test!" << xpcc::endl;

	for(uint16_t iteration = 0; iteration < 10; iteration++) {
		uint8_t* pattern = (iteration % 2 == 0) ? bufferA : bufferB;

		if(!RF_CALL_BLOCKING(storageDevice.erase(0, MemorySize))) {
			XPCC_LOG_INFO << "Error: Unable to erase device.";
			exit(1);
		}

		for(uint32_t i = 0; i < MemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.program(pattern, i, BlockSize))) {
				XPCC_LOG_INFO << "Error: Unable to write data.";
				exit(1);
			}
		}

		for(uint32_t i = 0; i < MemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.read(bufferC, i, BlockSize))) {
				XPCC_LOG_INFO << "Error: Unable to read data.";
				exit(1);
			}
			else if(std::memcmp(pattern, bufferC, BlockSize)) {
				XPCC_LOG_INFO << "Error: Read '";
				printMemoryContent(bufferC, BlockSize);
				XPCC_LOG_INFO << "', expected: '";
				printMemoryContent(pattern, BlockSize);
				XPCC_LOG_INFO << "'." << xpcc::endl;
				break;
			}
		}
		XPCC_LOG_INFO << ".";
	}

	XPCC_LOG_INFO << xpcc::endl << "Finished!" << xpcc::endl;

	return 0;
}
