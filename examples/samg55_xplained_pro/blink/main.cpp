#include "modm/board.hpp"

using namespace modm::platform;

int main() {
	Board::initialize();

	while(true) {
		Board::Led::toggle();
		modm::delay_ms(500);
	}

}