
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

// ----------------------------------------------------------------------------
/*xpcc::stm32::Debug debugUart(115200);
xpcc::IODeviceWrapper<xpcc::at91::Debug> loggerDevice(debugUart); 

xpcc::log::Logger xpcc::log::debug(loggerDevice); 
xpcc::log::Logger xpcc::log::info(loggerDevice); 
xpcc::log::Logger xpcc::log::warning(loggerDevice); 
xpcc::log::Logger xpcc::log::error(loggerDevice);*/

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedInverted1, C, 6);	// inverted, 0=on, 1=off
GPIO__OUTPUT(LedInverted2, C, 7);	// inverted, 0=on, 1=off
typedef xpcc::gpio::Invert<LedInverted1> Led1;
typedef xpcc::gpio::Invert<LedInverted2> Led2;

// ----------------------------------------------------------------------------
GPIO__INPUT(ButtonWakeUpInverted, A, 0);		// inverted, 0=pressed, 1=not pressed
GPIO__INPUT(ButtonTamperInverted, C, 13);	// inverted, 0=pressed, 1=not pressed
typedef xpcc::gpio::Invert<ButtonWakeUpInverted> ButtonWakeUp;
typedef xpcc::gpio::Invert<ButtonTamperInverted> ButtonTamper;

// ----------------------------------------------------------------------------
int
main(void)
{
	Led1::setOutput(xpcc::gpio::HIGH);
	Led2::setOutput(xpcc::gpio::HIGH);
	ButtonWakeUp::setInput();
	ButtonTamper::setInput();
	
	Led2::reset();
	
	while (1)
	{
		Led1::toggle();
		xpcc::delay_ms(200);
	}
}
