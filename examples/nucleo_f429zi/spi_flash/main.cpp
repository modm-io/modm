#include <xpcc/architecture/platform.hpp>

#include <xpcc/driver/storage/block_device_spiflash.hpp>

using namespace Board;

void printMemoryContent(const uint8_t* address, std::size_t size) {
	for (std::size_t i = 0; i < size; i++) {
		XPCC_LOG_INFO.printf("%x", address[i]);
	}
}

using SpiMaster = SpiMaster1;

// Spi flash chip wiring:
using Cs = GpioA4;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
// Connect WP and HOLD pins to +3V3
// and of course Vdd to +3V3 and Vss to GND


constexpr uint32_t BlockSize = 256;
constexpr uint32_t MemorySize = 8*1024*1024;
constexpr uint32_t TestMemorySize = 8*1024;

uint8_t bufferA[BlockSize];
uint8_t bufferB[BlockSize];
uint8_t bufferC[BlockSize];

xpcc::BdSpiFlash<SpiMaster, Cs, MemorySize> storageDevice;

void doMemoryTest()
{
	LedBlue::set();
	XPCC_LOG_INFO << "Starting memory test!" << xpcc::endl;

	for(uint16_t iteration = 0; iteration < 4; iteration++) {
		uint8_t* pattern = (iteration % 2 == 0) ? bufferA : bufferB;

		if(!RF_CALL_BLOCKING(storageDevice.erase(0, TestMemorySize))) {
			XPCC_LOG_INFO << "Error: Unable to erase device.";
			return;
		}

		for(uint32_t i = 0; i < TestMemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.program(pattern, i, BlockSize))) {
				XPCC_LOG_INFO << "Error: Unable to write data.";
				return;
			}
			XPCC_LOG_INFO << ".";
		}

		for(uint32_t i = 0; i < TestMemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.read(bufferC, i, BlockSize))) {
				XPCC_LOG_INFO << "Error: Unable to read data.";
				return;
			}
			else if(std::memcmp(pattern, bufferC, BlockSize)) {
				XPCC_LOG_INFO << "i=" << i << xpcc::endl;
				XPCC_LOG_INFO << "Error: Read '";
				printMemoryContent(bufferC, BlockSize);
				XPCC_LOG_INFO << "', expected: '";
				printMemoryContent(pattern, BlockSize);
				XPCC_LOG_INFO << "'." << xpcc::endl;
				return;
			}
		}
		XPCC_LOG_INFO << "." << xpcc::endl;
	}

	XPCC_LOG_INFO << xpcc::endl << "Finished!" << xpcc::endl;
	LedBlue::reset();
}

int
main()
{
	/**
	 * This example/test writes alternating patterns a 64MBit
	 * flash chip (SST26VF064B) attached to SPI0 using the
	 * `xpcc::BdSpiFlash` block device interface.
	 * The memory content is afterwards read and compared
	 * to the pattern.
	 * Write and read operations are done on 64 byte blocks.
	 *
	 * See above for how to wire the flash chip.
	 */

	// initialize board and SPI
	Board::initialize();
	Mosi::connect(SpiMaster::Mosi);
	Miso::connect(SpiMaster::Miso);
	Sck::connect(SpiMaster::Sck);
	SpiMaster::initialize<Board::systemClock, 11000000>();

	std::memset(bufferA, 0xAA, BlockSize);
	std::memset(bufferB, 0x55, BlockSize);

	bool initializeSuccess = false;

	XPCC_LOG_INFO << "Erasing complete flash chip... (This may take a while)" << xpcc::endl;

	if(!RF_CALL_BLOCKING(storageDevice.initialize())) {
		XPCC_LOG_INFO << "Error: Unable to initialize device.";
	}
	else if(!RF_CALL_BLOCKING(storageDevice.erase(0, MemorySize))) {
		XPCC_LOG_INFO << "Error: Unable to erase device.";
	}
	else {
		auto id = RF_CALL_BLOCKING(storageDevice.readId());
		XPCC_LOG_INFO << "deviceId=" << id.deviceId << " manufacturerId=" << id.manufacturerId;
		XPCC_LOG_INFO << " deviceType=" << id.deviceType << xpcc::endl;

		XPCC_LOG_INFO << "status=" << static_cast<uint8_t>(RF_CALL_BLOCKING(storageDevice.readStatus())) << xpcc::endl;

		XPCC_LOG_INFO << "Press USER button to start the memory test." << xpcc::endl;
		initializeSuccess = true;
	}

	while (1)
	{
		if(initializeSuccess && Button::read())
		{
			doMemoryTest();
		}
	}

	return 0;
}
