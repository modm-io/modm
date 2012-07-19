
// this header will include the right header for our architecture
#include <xpcc/architecture.hpp>

// create a wrapper for a output pin
GPIO__OUTPUT(Led, A, 12);
//GPIO__OUTPUT(Led2, B, 1);

//typedef xpcc::gpio::Invert< Led > LedInverted;

int
main(void)
{
	Led::setOutput();
//	Led::setOutput();

	while (1)
	{
		Led::reset();
		for (uint32_t ii = 0; ii < 8192; ++ii)
		{
			asm volatile ("nop");
		}
		Led::set();
		for (uint32_t ii = 0; ii < 16384; ++ii)
		{
			asm volatile ("nop");
		}
	}

//	LedInverted::setOutput();
//	LedInverted::reset();

	while (1)
	{
	}
}




/* bin2hex:
 *
 * avr-objcopy -I binary -O ihex meinbinfile.bin mainhexfile.hex
 *
 * program:
 * dfu-programmer at32uc3b0256 erase
 * dfu-programmer at32uc3b0256 flash b.hex --suppress-bootloader-mem
 * dfu-programmer at32uc3b0256 start
 *
 * linker script
 * 0x80002000 when bootloader is used
 *
 * header to hex-file
 *
 * :0200000480007A
 */
