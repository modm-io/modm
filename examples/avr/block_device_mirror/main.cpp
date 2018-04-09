#include <modm/platform/platform.hpp>

#include <modm/driver/storage/block_device_heap.hpp>
#include <modm/driver/storage/block_device_mirror.hpp>

#include <modm/debug/logger.hpp>
#include <stdlib.h>

using namespace modm::platform;
typedef modm::platform::SystemClock clock;

// Create a new UART object and configure it to a baudrate of 115200
Uart0 uart;
modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull > loggerDevice(uart);

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

void printMemoryContent(const uint8_t* address, std::size_t size) {
	for (std::size_t i = 0; i < size; i++) {
		MODM_LOG_INFO.printf("%x", address[i]);
	}
}

int
main()
{
	/**
	 * This example/test writes alternating patterns into
	 * two `modm::BdFile` block device with a size of 1M
	 * to test the virtual `modm::BdMirror` block device.
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

	modm::BdMirror<modm::BdHeap<MemorySize>, modm::BdHeap<MemorySize>> storageDevice;

	if(!RF_CALL_BLOCKING(storageDevice.initialize())) {
		MODM_LOG_INFO << "Error: Unable to initialize device.";
		exit(1);
	}

	MODM_LOG_INFO << "Starting memory test!" << modm::endl;

	for(uint16_t iteration = 0; iteration < 10; iteration++) {
		uint8_t* pattern = (iteration % 2 == 0) ? bufferA : bufferB;

		if(!RF_CALL_BLOCKING(storageDevice.erase(0, MemorySize))) {
			MODM_LOG_INFO << "Error: Unable to erase device.";
			exit(1);
		}

		for(uint32_t i = 0; i < MemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.program(pattern, i, BlockSize))) {
				MODM_LOG_INFO << "Error: Unable to write data.";
				exit(1);
			}
		}

		for(uint32_t i = 0; i < MemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.read(bufferC, i, BlockSize))) {
				MODM_LOG_INFO << "Error: Unable to read data.";
				exit(1);
			}
			else if(std::memcmp(pattern, bufferC, BlockSize)) {
				MODM_LOG_INFO << "Error: Read '";
				printMemoryContent(bufferC, BlockSize);
				MODM_LOG_INFO << "', expected: '";
				printMemoryContent(pattern, BlockSize);
				MODM_LOG_INFO << "'." << modm::endl;
				break;
			}
		}
		MODM_LOG_INFO << ".";
	}

	MODM_LOG_INFO << modm::endl << "Finished!" << modm::endl;

	return 0;
}
