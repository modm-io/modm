// coding: utf-8

#ifndef XPCC__XMEGA_TIMER_INPUT_CAPTURE_HPP
	#error	"Don't include this file directly, use 'input_capture.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename TIMER, typename PIN>
TIMER xpcc::InputCapture<TIMER, PIN>::timer;

template <typename TIMER, typename PIN>
PIN xpcc::InputCapture<TIMER, PIN>::pin;

template <typename TIMER, typename PIN>
uint16_t xpcc::InputCapture<TIMER, PIN>::capture(0);

template <typename TIMER, typename PIN>
bool xpcc::InputCapture<TIMER, PIN>::newData(false);

template <typename TIMER, typename PIN>
register16_t* xpcc::InputCapture<TIMER, PIN>::captureRegister;
// ----------------------------------------------------------------------------

template <typename TIMER, typename PIN>
bool
xpcc::InputCapture<TIMER, PIN>::initialize(register8_t& eventChannel,
										   TC_CLKSEL_t clock,
										   xpcc::input_capture::Mode action,
										   xpcc::timer::Channel timerChannel)
{
	// first check if we can actually access the timerChannel
	if (!timer.isType0() && (timerChannel > xpcc::timer::CHANNELB)) {
		return false;
	}
	
	// Configure the pin to trigger events on both edges
	pin.configure(::xpcc::gpio::PULLDOWN);
	pin.configureInputSense(::xpcc::gpio::BOTHEDGES);
	
	// Get the Event Channel Mux Input Selection from the Pins memory location
	eventChannel = pin.getEventChannelMuxInput();
	
	// Enable the Capture/Compare Channel
	timer.enableCompareCapture(1 << (timerChannel + 4));
	
	// set and enable interrupt level, use the CCA interrupt level
	timer.attachCompareCaptureInterrupt(timerChannel, TC_CCAINTLVL_MED_gc, captureData);
	
	// Get the Event Channel number from its memory location, 8 - 15
	uint8_t evChannel = static_cast<uint8_t>
		(reinterpret_cast<uint16_t>(&eventChannel) - 0x0180);
	// make sure the event source is not negative and wraps around 3 bits
	timer.setEventSource(static_cast<TC_EVSEL_t>((8 + evChannel - timerChannel)%8 + 8));
	
	// Get the Channel CC register
	captureRegister = reinterpret_cast<register16_t*>
		(reinterpret_cast<uint16_t>(&timer.getModuleBase()) + 0x0028 + 2*timerChannel);

	// tell the timer what to do: Input, Frequency or Pulse-width Capture
	timer.setEventAction(static_cast<TC_EVACT_t>(action));
	
	// Enable the timer module
	timer.setClockSource(clock);
	return true;
}

template <typename TIMER, typename PIN>
inline void
xpcc::InputCapture<TIMER, PIN>::captureData()
{
	capture = *captureRegister;
	newData = true;
}

template <typename TIMER, typename PIN>
inline bool
xpcc::InputCapture<TIMER, PIN>::isDataAvailable()
{
	return newData;
}

template <typename TIMER, typename PIN>
inline uint16_t
xpcc::InputCapture<TIMER, PIN>::getData()
{
	newData = false;
	return capture;
}

