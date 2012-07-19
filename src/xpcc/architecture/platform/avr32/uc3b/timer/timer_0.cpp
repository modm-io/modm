
#include "timer_0.hpp"

void
xpcc::avr32::Timer0::initWaveform(const tc_waveform_opt_t *opt)
{
  // Check for valid input.

  // GENERATE SIGNALS: Waveform operating mode.
	AVR32_TC.channel[0].cmr =
		static_cast<uint8_t>(opt->bswtrg) << AVR32_TC_BSWTRG_OFFSET |
		static_cast<uint8_t>(opt->beevt) << AVR32_TC_BEEVT_OFFSET |
		static_cast<uint8_t>(opt->bcpc) << AVR32_TC_BCPC_OFFSET |
		static_cast<uint8_t>(opt->bcpb) << AVR32_TC_BCPB_OFFSET |
		static_cast<uint8_t>(opt->aswtrg) << AVR32_TC_ASWTRG_OFFSET |
		static_cast<uint8_t>(opt->aeevt) << AVR32_TC_AEEVT_OFFSET |
		static_cast<uint8_t>(opt->acpc) << AVR32_TC_ACPC_OFFSET |
		static_cast<uint8_t>(opt->acpa) << AVR32_TC_ACPA_OFFSET |
					  1 << AVR32_TC_WAVE_OFFSET |
		static_cast<uint8_t>(opt->wavsel) << AVR32_TC_WAVSEL_OFFSET |
		static_cast<uint8_t>(opt->enetrg) << AVR32_TC_ENETRG_OFFSET |
		static_cast<uint8_t>(opt->eevt) << AVR32_TC_EEVT_OFFSET |
		static_cast<uint8_t>(opt->eevtedg) << AVR32_TC_EEVTEDG_OFFSET |
		static_cast<uint8_t>(opt->cpcdis) << AVR32_TC_CPCDIS_OFFSET |
		static_cast<uint8_t>(opt->cpcstop) << AVR32_TC_CPCSTOP_OFFSET |
		static_cast<uint8_t>(opt->burst) << AVR32_TC_BURST_OFFSET |
		static_cast<uint8_t>(opt->clki) << AVR32_TC_CLKI_OFFSET |
		static_cast<uint8_t>(opt->tcclks) << AVR32_TC_TCCLKS_OFFSET;
};
