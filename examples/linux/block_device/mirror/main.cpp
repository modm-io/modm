#include <modm/platform/platform.hpp>
#include <modm/debug/logger.hpp>

#include <modm/driver/storage/block_device_file.hpp>
#include <modm/driver/storage/block_device_mirror.hpp>
#include <iostream>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

void printMemoryContent(const uint8_t* address, std::size_t size) {
	for (std::size_t i = 0; i < size; i++) {
		MODM_LOG_INFO.printf("%x", address[i]);
	}
}
struct FilenameA {
	static constexpr const char* name = "testA.bin~";
};

struct FilenameB {
	static constexpr const char* name = "testB.bin~";
};

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

	// create empty `testA.bin~` file
	std::ofstream testfileA(FilenameA::name);
	testfileA.close();
	// create empty `testB.bin~` file
	std::ofstream testfileB(FilenameB::name);
	testfileB.close();

	constexpr uint32_t BlockSize = 256;
	constexpr uint32_t MemorySize = 16*1024*1024;

	uint8_t bufferA[BlockSize];
	uint8_t bufferB[BlockSize];
	uint8_t bufferC[BlockSize];
	std::memset(bufferA, 0xAA, BlockSize);
	std::memset(bufferB, 0x55, BlockSize);

	modm::BdMirror<modm::BdFile<FilenameA, MemorySize>, modm::BdFile<FilenameB, MemorySize>> storageDevice;

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
