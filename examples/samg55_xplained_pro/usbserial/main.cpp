#include <modm/board.hpp>
#include <modm/platform.hpp>

using namespace modm::platform;

int main() {
	Board::initialize();

	// Pull DP low briefly on reset to make sure USB host disconnects/reconnects
	MATRIX->CCFG_SYSIO |= (CCFG_SYSIO_SYSIO11 | CCFG_SYSIO_SYSIO10);
	GpioA22::setOutput(false);
	modm::delay_ms(5);
	GpioA22::setInput();

	Board::initializeUsbFs();

	tusb_init();

	while (true)
	{
		// Read any received data and echo it back
		uint8_t buf[64];
		uint32_t read_count;
		tud_task();
		read_count = UsbUart0::read(buf, sizeof(buf));
		UsbUart0::write(buf, read_count);
		Board::Led::toggle();
	}
}