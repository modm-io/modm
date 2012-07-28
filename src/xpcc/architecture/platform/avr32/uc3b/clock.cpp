
#include <xpcc/architecture/platform/avr32/uc3b.hpp>
#include "clock.hpp"

typedef union
{
  unsigned long                 cksel;
  avr32_pm_cksel_t              CKSEL;
} u_avr32_pm_cksel_t;

typedef union
{
  unsigned long                 oscctrl0;
  avr32_pm_oscctrl0_t           OSCCTRL0;
} u_avr32_pm_oscctrl0_t;

typedef union
{
  unsigned long                 mcctrl;
  avr32_pm_mcctrl_t             MCCTRL;
} u_avr32_pm_mcctrl_t;

typedef union
{
  unsigned long                 pll;
  avr32_pm_pll_t                PLL;
} u_avr32_pm_pll_t;

typedef union
{
  unsigned long                 gcctrl;
  avr32_pm_gcctrl_t             GCCTRL;
} u_avr32_pm_gcctrl_t;

void
xpcc::avr32::Clock::enableOsc(OscConfig mode)
{
	// Read
	u_avr32_pm_oscctrl0_t u_avr32_pm_oscctrl0 = {AVR32_PM.oscctrl0};
	// Modify
	u_avr32_pm_oscctrl0.OSCCTRL0.mode = static_cast<uint8_t>(mode);
	// Write
	AVR32_PM.oscctrl0 = u_avr32_pm_oscctrl0.oscctrl0;
}

void
xpcc::avr32::Clock::startOsc(StartupConfig startup)
{
	  // Read register
	  u_avr32_pm_oscctrl0_t u_avr32_pm_oscctrl0 = {AVR32_PM.oscctrl0};
	  // Modify
	  u_avr32_pm_oscctrl0.OSCCTRL0.startup = static_cast<uint8_t>(startup);
	  // Write back
	  AVR32_PM.oscctrl0 = u_avr32_pm_oscctrl0.oscctrl0;

	  // enable oscillator 0
	  AVR32_PM.mcctrl |= AVR32_PM_MCCTRL_OSC0EN_MASK;

	  // wait until clock is ready
	  while (!(AVR32_PM.poscsr & AVR32_PM_POSCSR_OSC0RDY_MASK));
}

void
xpcc::avr32::Clock::switchToClock(ClockSource source)
{
	  // Read
	  u_avr32_pm_mcctrl_t u_avr32_pm_mcctrl = {AVR32_PM.mcctrl};
	  // Modify
	  u_avr32_pm_mcctrl.MCCTRL.mcsel = static_cast<uint8_t>(source);
	  // Write back
	  AVR32_PM.mcctrl = u_avr32_pm_mcctrl.mcctrl;
}

void
xpcc::avr32::Clock::pllSetup(Pll pll, Mul mul, Div div, Osc osc, uint8_t lockcount, Range range, OutputDivider div2)
{
	  u_avr32_pm_pll_t u_avr32_pm_pll = {0};

	  u_avr32_pm_pll.PLL.pllosc   = static_cast<uint8_t>(osc);
	  u_avr32_pm_pll.PLL.plldiv   = static_cast<uint8_t>(div);
	  u_avr32_pm_pll.PLL.pllmul   = static_cast<uint8_t>(mul);
	  u_avr32_pm_pll.PLL.pllcount = lockcount;

	  uint8_t pll_wbwdisable = 0;

	  u_avr32_pm_pll.PLL.pllopt = (static_cast<uint8_t>(range) << 0) | (static_cast<uint8_t>(div2) << 1) | (pll_wbwdisable << 2);

	  AVR32_PM.pll[static_cast<uint8_t>(pll)] = u_avr32_pm_pll.pll;
}

void
xpcc::avr32::Clock::pllEnable(Pll pll)
{
	AVR32_PM.pll[static_cast<uint8_t>(pll)] |= AVR32_PM_PLLEN_MASK;
	switch (pll)
	{
	case Pll::PLL0:
		while (!(AVR32_PM.poscsr & AVR32_PM_POSCSR_LOCK0_MASK));
	break;
	case Pll::PLL1:
		while (!(AVR32_PM.poscsr & AVR32_PM_POSCSR_LOCK1_MASK));
	break;
	}
}

void
xpcc::avr32::Clock::selectClock(bool pbadiv, uint8_t pbasel, bool pbbdiv, uint8_t pbbsel, bool cpudiv, uint8_t cpusel)
{
	  u_avr32_pm_cksel_t u_avr32_pm_cksel = {0};

	  u_avr32_pm_cksel.CKSEL.cpusel = cpusel;
	  u_avr32_pm_cksel.CKSEL.cpudiv = cpudiv;
	  u_avr32_pm_cksel.CKSEL.hsbsel = cpusel;
	  u_avr32_pm_cksel.CKSEL.hsbdiv = cpudiv;
	  u_avr32_pm_cksel.CKSEL.pbasel = pbasel;
	  u_avr32_pm_cksel.CKSEL.pbadiv = pbadiv;
	  u_avr32_pm_cksel.CKSEL.pbbsel = pbbsel;
	  u_avr32_pm_cksel.CKSEL.pbbdiv = pbbdiv;

	  AVR32_PM.cksel = u_avr32_pm_cksel.cksel;

	  // Wait for ckrdy bit and then clear it
	  while (!(AVR32_PM.poscsr & AVR32_PM_POSCSR_CKRDY_MASK));
}

void
xpcc::avr32::Clock::gcSetup(GenericClock gc, GenericClockSource source, bool diven, uint8_t div)
{
	  u_avr32_pm_gcctrl_t u_avr32_pm_gcctrl = {0};

	  u_avr32_pm_gcctrl.GCCTRL.oscsel = static_cast<uint8_t>(source) & 0x01;
	  u_avr32_pm_gcctrl.GCCTRL.pllsel = static_cast<uint8_t>(source) & 0x02;
	  u_avr32_pm_gcctrl.GCCTRL.diven  = diven ? 1 : 0;
	  u_avr32_pm_gcctrl.GCCTRL.div    = div;

	  AVR32_PM.gcctrl[static_cast<uint8_t>(gc)] = u_avr32_pm_gcctrl.gcctrl;
}

void
xpcc::avr32::Clock::gcEnable(GenericClock gc)
{
	AVR32_PM.gcctrl[static_cast<uint8_t>(gc)] |= AVR32_PM_GCCTRL_CEN_MASK;
}

void
xpcc::avr32::Clock::enableUsbClock()
{
	// Use 12MHz from OSC0 and generate 96 MHz
	pllSetup(
			Pll::PLL1,
			Mul::MUL4,
			Div::DIV1,
			Osc::OSC0,
			16,
			Range::MHZ_80_TO_180,
			OutputDivider::FPLL_EQUAL_FVCO
			);

	// start PLL1 and wait for lock
	pllEnable(Pll::PLL1);

	gcSetup(GenericClock::USBB, GenericClockSource::PLL1, false, 0);

	// enable the USB clock.
	gcEnable(GenericClock::USBB);
}
