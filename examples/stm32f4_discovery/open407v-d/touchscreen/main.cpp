
#include <xpcc/architecture.hpp>

#include "parallel_tft.hpp"
#include <xpcc/driver/ui/display/image.hpp>

#include "lcd.h"
#include "touchscreen_calibrator.hpp"

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedOrange, D, 13);		// User LED 3
GPIO__OUTPUT(LedGreen, D, 12);		// User LED 4
GPIO__OUTPUT(LedRed, D, 14);		// User LED 5
GPIO__OUTPUT(LedBlue, D, 15);		// User LED 6

GPIO__OUTPUT(VBusPresent, A, 9);		// green LED (LD7)
GPIO__OUTPUT(VBusOvercurrent, D, 5);	// red LED   (LD8)

GPIO__INPUT(Button, A, 0);

// FSMC
GPIO__IO(D0, D, 14);
GPIO__IO(D1, D, 15);
GPIO__IO(D2, D, 0);
GPIO__IO(D3, D, 1);
GPIO__IO(D4, E, 7);
GPIO__IO(D5, E, 8);
GPIO__IO(D6, E, 9);
GPIO__IO(D7, E, 10);
GPIO__IO(D8, E, 11);
GPIO__IO(D9, E, 12);
GPIO__IO(D10, E, 13);
GPIO__IO(D11, E, 14);
GPIO__IO(D12, E, 15);
GPIO__IO(D13, D, 8);
GPIO__IO(D14, D, 9);
GPIO__IO(D15, D, 10);

GPIO__OUTPUT(A16, D, 11);
GPIO__OUTPUT(A23, E, 2);

GPIO__OUTPUT(NOE, D, 4);
GPIO__OUTPUT(NWE, D, 5);

GPIO__OUTPUT(CS, D, 7);

using namespace xpcc::stm32;

static bool
initClock()
{
	// use external 8MHz crystal
	if (!Clock::enableHse(Clock::HseConfig::HSE_CRYSTAL)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, 4, 168);
	return Clock::switchToPll();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	initClock();

	LedOrange::setOutput(xpcc::gpio::HIGH);
	LedGreen::setOutput(xpcc::gpio::LOW);
	LedRed::setOutput(xpcc::gpio::HIGH);
	LedBlue::setOutput(xpcc::gpio::HIGH);
	
	Fsmc::initialize();
		
	D0::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D1::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D2::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D3::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D4::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D5::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D6::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D7::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D8::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D9::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D10::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D11::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D12::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D13::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D14::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	D15::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	
	A16::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	A23::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	
	NOE::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	NWE::setAlternateFunction(AF_FSMC, PUSH_PULL, SPEED_100MHZ);
	
	CS::setOutput();
	CS::reset();
	
	fsmc::NorSram::AsynchronousTiming timing = {
		// read
		15,
		0,
		15,
		
		// write
		15,
		0,
		15,
		
		// bus turn around 
		0
	};
	
	fsmc::NorSram::configureAsynchronousRegion(
			fsmc::NorSram::CHIP_SELECT_1,
			fsmc::NorSram::NO_MULTIPLEX_16BIT,
			fsmc::NorSram::SRAM_ROM,
			fsmc::NorSram::MODE_A,
			timing);
	
	fsmc::NorSram::enableRegion(fsmc::NorSram::CHIP_SELECT_1);
	
	xpcc::TftMemoryBus parallelBus(
			(volatile uint16_t *) 0x60000000,
			(volatile uint16_t *) 0x60020000);
	
	xpcc::ParallelTft<xpcc::TftMemoryBus> tft(parallelBus);
	
	tft.initialize();
	
	tft.setColor(xpcc::glcd::Color::blue());
	tft.drawLine(0, 0, 100, 100);
	
	tft.setColor(xpcc::glcd::Color::red());
	tft.drawImage(xpcc::glcd::Point(100, 100), xpcc::accessor::asFlash(bitmap::logo_xpcc_90x64));
	
	tft.setCursor(100, 20);
	tft.setFont(xpcc::font::Assertion);
	tft.setColor(xpcc::glcd::Color::yellow());
	tft << "Hello World!";
	
	TP_Init();
	//LCD_Initializtion();
	TouchPanel_Calibrate();
	
	while (1)	
	{
		getDisplayPoint(&display, Read_Ads7846(), &matrix);
		TP_DrawPoint(display.x,display.y);
		
		LedBlue::toggle();
		LedGreen::toggle();
		//xpcc::delay_ms(500); 
	}

	return 0;
}

