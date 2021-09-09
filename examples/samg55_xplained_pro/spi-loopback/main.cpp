#include <modm/board.hpp>

using namespace modm::platform;

int main() {
	// Test SPI send and receive in loopback mode. If we receive the expected
	// characters back, flash slowly. Otherwise, flash fast.
	Board::initialize();

	SpiMaster0::connect<GpioB0::Sck, GpioA9::Miso, GpioA10::Mosi>();
	SpiMaster0::initialize<Board::SystemClock, 1_MHz>();

	SpiMaster0::setLocalLoopback(true);

	while (true)
	{
		uint32_t flash_time_ms;
		uint8_t tx[] = {0xa5, 0x21};
		uint8_t rx[2];

		SpiMaster0::transferBlocking(tx, rx, 2);

		if(rx[0] == 0xa5 && rx[1] == 0x21) {
			flash_time_ms = 500;
		} else {
			flash_time_ms = 100;
		}

		for(uint32_t i=0; i<5; i++) {
			Board::Led::set();
			modm::delay_ms(flash_time_ms);
			Board::Led::reset();
			modm::delay_ms(flash_time_ms);
		}
	}
}