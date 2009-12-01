
#include <xpcc/hal/flash/flash_pointer.hpp>

//extern const int a PROGMEM;
//const int a = 12;

FLASH(int foo) = 12;

FLASH_STRING(string) = "Hallo Welt!\n";

FLASH(int32_t bla[4]) = {1,2,3,4};

volatile uint8_t out;

void
function(xpcc::FlashPointer<char> s)
{
	char c;
	while ((c = *s++)) {
		out = c;
	}
}

int
main(void)
{
	xpcc::FlashPointer<int> bar(&foo);
	
	out = *bar;
	
	function(xpcc::toFlashPointer(string));
	
	xpcc::FlashPointer<int32_t> blub(bla);
	
	out = blub[2];
	
	while (1) {
		
	}
}
