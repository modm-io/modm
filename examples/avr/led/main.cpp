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

typedef xpcc::atxmega::UartSpiMasterD1 LedSpi;
typedef xpcc::atxmega::TimerD0 LedTimer;
typedef xpcc::TLC594X< 16, LedSpi, XLAT, VPROG, XERR > ledController;
typedef xpcc::MAX6966< LedSpi, XLAT, 4 > ledController2;

#include <xpcc/driver/ui/led.hpp>
typedef xpcc::led::TLC594XLed< ledController, 2 > Red;
typedef xpcc::led::TLC594XLed< ledController, 3 > Green;
typedef xpcc::led::TLC594XLed< ledController, 4 > Blue;

xpcc::led::Pulse<Green> pulse;
xpcc::led::Indicator<Blue> indicator;
xpcc::led::DoubleIndicator<Red> strobe;

// INTERRUPTS #################################################################
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/architecture/driver/delay.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>
ISR(TCC1_OVF_vect)
{
	xpcc::Clock::increment();
	xpcc::atomic::Unlock unlock;
	
	pulse.run();
	indicator.run();
	strobe.run();
	
	ledController::writeChannels();
}

MAIN_FUNCTION // FINALLY ######################################################
{	// INIT BASICS
	// Switch main clock to 32 MHz
	xpcc::atxmega::enableExternalOscillator(OSC_FRQRANGE_2TO9_gc, OSC_XOSCSEL_XTAL_256CLK_gc);
	xpcc::atxmega::enablePll(OSC_PLLSRC_XOSC_gc, 4);
	xpcc::atxmega::setSystemClockPrescaler(CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc);
	xpcc::atxmega::selectSystemClockSource(CLK_SCLKSEL_PLL_gc);
	
	xpcc::atxmega::TimerC1::setMsTimer();
	
	// LED FADER **************************************************************
	xpcc::atxmega::DmaController::initialize();
	LedSpi::initialize(F_CPU/2);
	XERR::setInput(xpcc::atxmega::PULLUP);
	BLANK::setOutput(xpcc::gpio::LOW);
	ledController::initialize(0, 63, true, false);
	ledController::setDotCorrection(3, 10);
	ledController::setDotCorrection(4, 20);
	ledController::writeDotCorrection();
	
	CLKOUT::setOutput();
	PORTCFG.CLKEVOUT = PORTCFG_CLKOUT_PC7_gc;
	
	LedTimer::setClockSource(TC_CLKSEL_DIV8_gc);
	LedTimer::setWaveformGenerationMode(TC_WGMODE_SS_gc);
	LedTimer::enableCompareCapture(TC0_CCDEN_bm);
	LedTimer::getModuleBase().CCD = 1;
	LedTimer::getModuleBase().PER = 513;
	
	ledController2::initialize(xpcc::max6966::CURRENT_20mA);
	
	// INTERRUPTS *************************************************************
	xpcc::atxmega::enableInterruptLevel(xpcc::atxmega::INTERRUPT_CONTROL_LEVEL_ALL);
	xpcc::atxmega::enableInterrupts();
	
	pulse.pulse(20);
	indicator.indicate(200);
	strobe.indicate(20);
	
	ledController2::setChannel(17, 200);
	
	while (1)
	{
	}
}

