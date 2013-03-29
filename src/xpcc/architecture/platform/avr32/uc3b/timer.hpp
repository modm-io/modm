#ifndef XPCC_AVR32__TIMER_HPP
#define XPCC_AVR32__TIMER_HPP

#include <avr32/io.h>

#ifndef __DOXYGEN__

namespace xpcc
{
namespace avr32
{
namespace timer
{

/*! \name External Clock Signal 0 Selection
 */
//! @{
#define TC_CH0_EXT_CLK0_SRC_TCLK0               AVR32_TC_TC0XC0S_TCLK0
#define TC_CH0_EXT_CLK0_SRC_NO_CLK              AVR32_TC_TC0XC0S_NO_CLK
#define TC_CH0_EXT_CLK0_SRC_TIOA1               AVR32_TC_TC0XC0S_TIOA1
#define TC_CH0_EXT_CLK0_SRC_TIOA2               AVR32_TC_TC0XC0S_TIOA2
//! @}

/*! \name External Clock Signal 1 Selection
 */
//! @{
#define TC_CH1_EXT_CLK1_SRC_TCLK1               AVR32_TC_TC1XC1S_TCLK1
#define TC_CH1_EXT_CLK1_SRC_NO_CLK              AVR32_TC_TC1XC1S_NO_CLK
#define TC_CH1_EXT_CLK1_SRC_TIOA0               AVR32_TC_TC1XC1S_TIOA0
#define TC_CH1_EXT_CLK1_SRC_TIOA2               AVR32_TC_TC1XC1S_TIOA2
//! @}

/*! \name External Clock Signal 2 Selection
 */
//! @{
#define TC_CH2_EXT_CLK2_SRC_TCLK2               AVR32_TC_TC2XC2S_TCLK2
#define TC_CH2_EXT_CLK2_SRC_NO_CLK              AVR32_TC_TC2XC2S_NO_CLK
#define TC_CH2_EXT_CLK2_SRC_TIOA0               AVR32_TC_TC2XC2S_TIOA0
#define TC_CH2_EXT_CLK2_SRC_TIOA1               AVR32_TC_TC2XC2S_TIOA1
//! @}

/*! \name Event/Trigger Actions on Output
 */
//! @{
#define TC_EVT_EFFECT_NOOP                      AVR32_TC_NONE
#define TC_EVT_EFFECT_SET                       AVR32_TC_SET
#define TC_EVT_EFFECT_CLEAR                     AVR32_TC_CLEAR
#define TC_EVT_EFFECT_TOGGLE                    AVR32_TC_TOGGLE
//! @}

/*! \name RC Compare Trigger Enable
 */
//! @{
#define TC_NO_TRIGGER_COMPARE_RC                0
#define TC_TRIGGER_COMPARE_RC                   1
//! @}

/*! \name Waveform Selection
 */
//! @{
#define TC_WAVEFORM_SEL_UP_MODE                 AVR32_TC_WAVSEL_UP_NO_AUTO
#define TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER      AVR32_TC_WAVSEL_UP_AUTO
#define TC_WAVEFORM_SEL_UPDOWN_MODE             AVR32_TC_WAVSEL_UPDOWN_NO_AUTO
#define TC_WAVEFORM_SEL_UPDOWN_MODE_RC_TRIGGER  AVR32_TC_WAVSEL_UPDOWN_AUTO
//! @}

/*! \name TIOA or TIOB External Trigger Selection
 */
//! @{
#define TC_EXT_TRIG_SEL_TIOA                    1
#define TC_EXT_TRIG_SEL_TIOB                    0
//! @}

/*! \name External Event Selection
 *
 * \attention See \ref tc_attention_eevt "this note" for important details about
 * this setting.
 */
//! @{
#define TC_EXT_EVENT_SEL_TIOB_INPUT             AVR32_TC_EEVT_TIOB_INPUT
#define TC_EXT_EVENT_SEL_XC0_OUTPUT             AVR32_TC_EEVT_XC0_OUTPUT
#define TC_EXT_EVENT_SEL_XC1_OUTPUT             AVR32_TC_EEVT_XC1_OUTPUT
#define TC_EXT_EVENT_SEL_XC2_OUTPUT             AVR32_TC_EEVT_XC2_OUTPUT
//! @}

/*! \name Edge Selection
 */
//! @{
#define TC_SEL_NO_EDGE                          AVR32_TC_EEVTEDG_NO_EDGE
#define TC_SEL_RISING_EDGE                      AVR32_TC_EEVTEDG_POS_EDGE
#define TC_SEL_FALLING_EDGE                     AVR32_TC_EEVTEDG_NEG_EDGE
#define TC_SEL_EACH_EDGE                        AVR32_TC_EEVTEDG_BOTH_EDGES
//! @}

/*! \name Burst Signal Selection
 */
//! @{
#define TC_BURST_NOT_GATED                      AVR32_TC_BURST_NOT_GATED
#define TC_BURST_CLK_AND_XC0                    AVR32_TC_BURST_CLK_AND_XC0
#define TC_BURST_CLK_AND_XC1                    AVR32_TC_BURST_CLK_AND_XC1
#define TC_BURST_CLK_AND_XC2                    AVR32_TC_BURST_CLK_AND_XC2
//! @}

/*! \name Clock Invert
 */
//! @{
#define TC_CLOCK_RISING_EDGE                    0
#define TC_CLOCK_FALLING_EDGE                   1
//! @}

/*! \name Clock Selection
 */
//! @{
#define TC_CLOCK_SOURCE_TC1                     AVR32_TC_TCCLKS_TIMER_CLOCK1
#define TC_CLOCK_SOURCE_TC2                     AVR32_TC_TCCLKS_TIMER_CLOCK2
#define TC_CLOCK_SOURCE_TC3                     AVR32_TC_TCCLKS_TIMER_CLOCK3
#define TC_CLOCK_SOURCE_TC4                     AVR32_TC_TCCLKS_TIMER_CLOCK4
#define TC_CLOCK_SOURCE_TC5                     AVR32_TC_TCCLKS_TIMER_CLOCK5
#define TC_CLOCK_SOURCE_XC0                     AVR32_TC_TCCLKS_XC0
#define TC_CLOCK_SOURCE_XC1                     AVR32_TC_TCCLKS_XC1
#define TC_CLOCK_SOURCE_XC2                     AVR32_TC_TCCLKS_XC2
//! @}

enum class Effect : uint8_t
{
	NOOP 	= TC_EVT_EFFECT_NOOP,
	SET 	= TC_EVT_EFFECT_SET,
	CLEAR	= TC_EVT_EFFECT_CLEAR,
	TOGGLE	= TC_EVT_EFFECT_TOGGLE,
};

enum class Waveform : uint8_t
{
	UP_MODE 				= TC_WAVEFORM_SEL_UP_MODE,
	UP_MODE_RC_TRIGGER		= TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
	UPDOWN_MODE				= TC_WAVEFORM_SEL_UPDOWN_MODE,
	UPDOWN_MODE_RC_TRIGGER 	= TC_WAVEFORM_SEL_UPDOWN_MODE_RC_TRIGGER,
};

enum class
ExternalEventTrigger : bool
{
	ENABLED		= true,
	DISABLED	= false,
};

enum class
ExternalEvent : uint8_t
{
	TIOB	= TC_EXT_EVENT_SEL_TIOB_INPUT,
	XC0 	= TC_EXT_EVENT_SEL_XC0_OUTPUT,
	XC1 	= TC_EXT_EVENT_SEL_XC1_OUTPUT,
	XC2 	= TC_EXT_EVENT_SEL_XC2_OUTPUT,
};

enum class
ExternalEventEdge : uint8_t
{
	NO_EDGE			= TC_SEL_NO_EDGE,
	RISING_EDGE 	= TC_SEL_RISING_EDGE,
	FALLING_EDGE	= TC_SEL_FALLING_EDGE,
	EACH_EDGE		= TC_SEL_EACH_EDGE,
};

enum class
CounterClockDisable : bool
{
	NOT_WITH_RC_COMPARE	= false,
	WITH_RC_COMPARE 	= true,
};

enum class
CounterClockStop
{
	NOT_WITH_RC_COMPARE	= false,
	WITH_RC_COMPARE 	= true,
};

enum class
BurstSignal : uint8_t
{
	NOT_GATED	= TC_BURST_NOT_GATED,
	CLK_AND_XC0	= TC_BURST_CLK_AND_XC0,
	CLK_AND_XC1	= TC_BURST_CLK_AND_XC1,
	CLK_AND_XC2	= TC_BURST_CLK_AND_XC2,
};

enum class
ClockInvert : uint8_t
{
	RISING_EDGE		= TC_CLOCK_RISING_EDGE,
	FALLING_EDGE	= TC_CLOCK_FALLING_EDGE,
};

enum class
ClockSelection : uint8_t
{
	TC1	= TC_CLOCK_SOURCE_TC1,
	TC2	= TC_CLOCK_SOURCE_TC2,
	TC3	= TC_CLOCK_SOURCE_TC3,
	TC4	= TC_CLOCK_SOURCE_TC4,
	TC5	= TC_CLOCK_SOURCE_TC5,
	XC0	= TC_CLOCK_SOURCE_XC0,
	XC1	= TC_CLOCK_SOURCE_XC1,
	XC2	= TC_CLOCK_SOURCE_XC2,
};

//! Parameters when initializing a timer/counter in waveform mode.
typedef struct
{
  //! Channel to initialize.
//  unsigned int channel            ;

  //! Software trigger effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int bswtrg          : 2;
	Effect bswtrg;

  //! External event effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int beevt           : 2;
	Effect beevt;

  //! RC compare effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int bcpc            : 2;
	Effect bcpc;

  //! RB compare effect on TIOB:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int bcpb            : 2;
	Effect bcpb;

  //! Software trigger effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int aswtrg          : 2;
	Effect aswtrg;

  //! External event effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int aeevt           : 2;
	Effect aeevt;

  //! RC compare effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int acpc            : 2;
	Effect acpc;

  //! RA compare effect on TIOA:\n
  //!   - \ref TC_EVT_EFFECT_NOOP;\n
  //!   - \ref TC_EVT_EFFECT_SET;\n
  //!   - \ref TC_EVT_EFFECT_CLEAR;\n
  //!   - \ref TC_EVT_EFFECT_TOGGLE.
//  unsigned int acpa            : 2;
	Effect acpa;

  unsigned int                 : 1;

  //! Waveform selection:\n
  //!   - \ref TC_WAVEFORM_SEL_UP_MODE;\n
  //!   - \ref TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER;\n
  //!   - \ref TC_WAVEFORM_SEL_UPDOWN_MODE;\n
  //!   - \ref TC_WAVEFORM_SEL_UPDOWN_MODE_RC_TRIGGER.
//  unsigned int wavsel          : 2;
  Waveform wavsel;

  //! External event trigger enable:\n
  //!   - \c false;\n
  //!   - \c true.
//  unsigned int enetrg          : 1;
  ExternalEventTrigger enetrg;

  //! External event selection:\n
  //!   - \ref TC_EXT_EVENT_SEL_TIOB_INPUT;\n
  //!   - \ref TC_EXT_EVENT_SEL_XC0_OUTPUT;\n
  //!   - \ref TC_EXT_EVENT_SEL_XC1_OUTPUT;\n
  //!   - \ref TC_EXT_EVENT_SEL_XC2_OUTPUT.
  //! \attention See \ref tc_attention_eevt "this note" for important details
  //! about this setting.
//  unsigned int eevt            : 2;
  ExternalEvent	eevt;

  //! External event edge selection:\n
  //!   - \ref TC_SEL_NO_EDGE;\n
  //!   - \ref TC_SEL_RISING_EDGE;\n
  //!   - \ref TC_SEL_FALLING_EDGE;\n
  //!   - \ref TC_SEL_EACH_EDGE.
//  unsigned int eevtedg         : 2;
  ExternalEventEdge	eevtedg;

  //! Counter clock disable with RC compare:\n
  //!   - \c false;\n
  //!   - \c true.
  CounterClockDisable cpcdis;
//  unsigned int cpcdis          : 1;

  //! Counter clock stopped with RC compare:\n
  //!   - \c false;\n
  //!   - \c true.
//  unsigned int cpcstop         : 1;
  CounterClockStop cpcstop;

  //! Burst signal selection:\n
  //!   - \ref TC_BURST_NOT_GATED;\n
  //!   - \ref TC_BURST_CLK_AND_XC0;\n
  //!   - \ref TC_BURST_CLK_AND_XC1;\n
  //!   - \ref TC_BURST_CLK_AND_XC2.
//  unsigned int burst           : 2;
  BurstSignal burst;

  //! Clock invert:\n
  //!   - \ref TC_CLOCK_RISING_EDGE;\n
  //!   - \ref TC_CLOCK_FALLING_EDGE.
//  unsigned int clki            : 1;
  ClockInvert clki;

  //! Clock selection:\n
  //!   - \ref TC_CLOCK_SOURCE_TC1;\n
  //!   - \ref TC_CLOCK_SOURCE_TC2;\n
  //!   - \ref TC_CLOCK_SOURCE_TC3;\n
  //!   - \ref TC_CLOCK_SOURCE_TC4;\n
  //!   - \ref TC_CLOCK_SOURCE_TC5;\n
  //!   - \ref TC_CLOCK_SOURCE_XC0;\n
  //!   - \ref TC_CLOCK_SOURCE_XC1;\n
  //!   - \ref TC_CLOCK_SOURCE_XC2.
//  unsigned int tcclks          : 3;
  ClockSelection tcclks;
} waveform;

//! Timer/counter interrupts.
typedef struct
{
  //! External trigger interrupt.
  uint8_t etrgs           : 1;

  //! RB load interrupt.
  uint8_t ldrbs           : 1;

  //! RA load interrupt.
  uint8_t ldras           : 1;

  //! RC compare interrupt.
  uint8_t cpcs            : 1;

  //! RB compare interrupt.
  uint8_t cpbs            : 1;

  //! RA compare interrupt.
  uint8_t cpas            : 1;

  //! Load overrun interrupt.
  uint8_t lovrs           : 1;

  //! Counter overflow interrupt.
  uint8_t covfs           : 1;
} interrupt;

} // timer namespace
} // avr32 namesapce
} // xpcc namespace

#endif	// __DOXYGEN__

#include "timer/timer_0.hpp"
//#include "timer/timer_1.hpp"
//#include "timer/timer_2.hpp"

#endif // XPCC_AVR32__TIMER_0_HPP
