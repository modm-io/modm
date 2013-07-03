// coding: utf-8
#include <xpcc/architecture/platform.hpp>

// LED FADER ##################################################################
#include <xpcc/driver/pwm/tlc594x.hpp>
#include <xpcc/driver/pwm/max6966.hpp>
GPIO__OUTPUT(VPROG, D, 4);
GPIO__INPUT(XERR, D, 1);
GPIO__OUTPUT(BLANK, D, 3);
GPIO__OUTPUT(XLAT, D, 2);
GPIO__OUTPUT(CLKOUT, C, 7);

#include <xpcc/ui/led.hpp>

typedef xpcc::xmega::UartSpiMasterD1 LedSpi;
typedef xpcc::xmega::TimerD0 LedTimer;
typedef xpcc::TLC594X< 16, LedSpi, XLAT, VPROG, XERR > ledController;
typedef xpcc::MAX6966< LedSpi, XLAT, 4 > ledController2;

xpcc::led::TLC594XLed< ledController > red(2);
xpcc::led::TLC594XLed< ledController > green(3);
xpcc::led::TLC594XLed< ledController > blue(4);

xpcc::led::Pulse pulse(&green);
xpcc::led::Indicator indicator(&blue);
xpcc::led::DoubleIndicator strobe(&red);

// INTERRUPTS #################################################################
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/architecture/driver/delay.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>
ISR(TCC1_OVF_vect)
{
	xpcc::Clock::increment();
	
	ledController::writeChannels();
}

MAIN_FUNCTION // FINALLY ######################################################
{	// INIT BASICS
	// Switch main clock to 32 MHz
	xpcc::xmega::enableExternalOscillator(OSC_FRQRANGE_2TO9_gc, OSC_XOSCSEL_XTAL_256CLK_gc);
	xpcc::xmega::enablePll(OSC_PLLSRC_XOSC_gc, 4);
	xpcc::xmega::setSystemClockPrescaler(CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc);
	xpcc::xmega::selectSystemClockSource(CLK_SCLKSEL_PLL_gc);
	
	xpcc::xmega::TimerC1::setMsTimer();
	
	// LED FADER **************************************************************
	xpcc::xmega::DmaController::initialize();
	LedSpi::initialize(F_CPU/2);
	XERR::setInput(xpcc::xmega::PULLUP);
	BLANK::setOutput(xpcc::Gpio::LOW);
	
	// initialize the tlc5940
	ledController::initialize(0, 63, true, false);
	ledController::setDotCorrection(3, 10);
	ledController::setDotCorrection(4, 20);
	ledController::writeDotCorrection();
	
	// GSCLK, 32MHz Grayscale clock
	CLKOUT::setOutput();
	PORTCFG.CLKEVOUT = PORTCFG_CLKOUT_PC7_gc;
	
	// the Grayscale counter needs to be manually reset to 0
	// This happens at 32000kHz/4096 = 7.8125kHz
	// So we need to set and reset the BLANK pin at a slightly lower frequency
	// 32000kHz/8/513 = ~7.79kHz
	// Please look at the datasheet figure 11 !!!
	LedTimer::setClockSource(TC_CLKSEL_DIV8_gc);
	LedTimer::setWaveformGenerationMode(TC_WGMODE_SS_gc);
	LedTimer::enableCompareCapture(TC0_CCDEN_bm);
	LedTimer::getModuleBase().CCD = 1;
	LedTimer::getModuleBase().PER = 513;
	
	// The MAX6966 requires a bit less external hardware 
	ledController2::initialize(xpcc::max6966::CURRENT_20mA);
	
	// INTERRUPTS *************************************************************
	xpcc::xmega::enableInterruptLevel(xpcc::xmega::INTERRUPT_CONTROL_LEVEL_ALL);
	xpcc::xmega::enableInterrupts();
	
	pulse.pulse(20);
	indicator.indicate(200);
	strobe.indicate(20);
	
	ledController2::setChannel(17, 200);
	
	while (1)
	{
		pulse.run();
		indicator.run();
		strobe.run();
	}
}

