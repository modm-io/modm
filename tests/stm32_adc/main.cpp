#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>


#include "adc/adc_1.hpp"
#include "adc/adc_2.hpp"
#include "adc/adc_3.hpp"

/**! \file main.cpp
 * @author David Hebbeker
 */

using namespace xpcc::stm32;

#define __DISCOVERY (1)
//#define __OLIMEX (1)

static bool initClock()
{
#ifdef __DISCOVERY
	// use external 8MHz crystal
	if (!Clock::enableHse(Clock::HSE_CRYSTAL)) {
		return false;
	}

	Clock::enablePll(Clock::PLL_HSE, 4, 168);
	return Clock::switchToPll();
#elif defined __OLIMEX
	typedef xpcc::stm32::Clock C;

	// use external 8MHz crystal, stm32f1
	if (!C::enableHse(C::HSE_CRYSTAL)) {
		return false;
	}

	C::enablePll(C::PLL_HSE, C::PLL_MUL_9);
	return C::switchToPll();
#else
	// For using an external oscillator with 8 MHz use:
	using namespace Clock;
	if (enableHSE(HSE_BYPASS))
	 {
	     enablePll(PLL_HSE, PLL_MUL_9);
	     switchToPll();
	 }
#endif
}
#ifdef __OLIMEX
	xpcc::stm32::Usart2 uart(115200);
	xpcc::IODeviceWrapper<xpcc::stm32::Usart2> loggerDevice(uart); // Create a new UART object and configure it to a baudrate of 115200
#else
	xpcc::stm32::Uart5 uart(115200);
	xpcc::IODeviceWrapper<xpcc::stm32::Uart5> loggerDevice(uart); // Create a new UART object and configure it to a baudrate of 115200
#endif

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

#define ADC_CHANNEL_NUM (2)

uint16_t ADC_buffer[ADC_CHANNEL_NUM];

extern "C" void ADC_IRQHandler(void)
{
	static uint8_t index=0;
	ADC_buffer[index++] = ADC1->DR & 0xFFF;
	if(index == ADC_CHANNEL_NUM) index=0;
	ADC1->SR &= ~ADC_SR_EOC;
}

extern "C" void ADC1_2_IRQHandler(void)
{
	ADC1->SR &= ~ADC_SR_EOC;

	XPCC_LOG_INFO << "ADC_INT1" << xpcc::endl;
}

using namespace xpcc;

#define Adc Adc1

MAIN_FUNCTION
{
	initClock();
#ifdef __OLIMEX
	uart.configurePins(uart.REMAP_PA2_PA3);
#else
	uart.configurePins(uart.REMAP_PC12_PD2);
#endif

	XPCC_LOG_INFO << xpcc::endl << "This is an ADC Test program by dhebbeker for multiple devices. 14.04.2012" << xpcc::endl;

	Adc::initialize();
	Adc::enableFreeRunningMode();
#ifdef __OLIMEX
	Adc::setChannel(Adc::PIN_B0);
	/*
	Adc::enableTemperatureRefVMeasurement();
	Adc::setChannel(Adc::TEMPERATURE_SENSOR);
	*/
#elif defined(__DISCOVERY)
	Adc::setChannel(Adc::PIN_A1);
#endif
	Adc::startConversion();

	while(1)
	{
		for(uint32_t t1=0; t1<16200000; t1+=2) asm volatile ("NOP");
		XPCC_LOG_INFO << xpcc::ascii << "adc=" << Adc::getValue() << xpcc::endl; // send the value
	}

	Adc::shutdownADC();

	while (1)
	{
		asm volatile("NOP");
	}
}
