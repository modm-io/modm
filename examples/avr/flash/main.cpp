
#include <xpcc/architecture/general/accessor/flash.hpp>

//extern const int a PROGMEM;
//const int a = 12;

FLASH_STORAGE(int foo) = 12;

FLASH_STORAGE_STRING(string) = "Hallo Welt!\n";

FLASH_STORAGE(int32_t bla[4]) = {1,2,3,4};

volatile uint8_t out;

void
function(xpcc::accessor::Flash<char> s)
{
	char c;
	while ((c = *s++)) {
		out = c;
	}
}

int
main(void)
{
	xpcc::accessor::Flash<int> bar(&foo);
	
	out = *bar;
	
	function(xpcc::modifier::asFlash(string));
	
	xpcc::accessor::Flash<int32_t> blub(bla);
	
	out = blub[2];
	
	while (1) {
		
	}
}
