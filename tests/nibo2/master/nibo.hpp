
#ifndef NIBO_HPP
#define NIBO_HPP

#include <xpcc/driver/lcd/ks0108.hpp>
#include <xpcc/architecture/driver/atmega/uart.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

#include "nibo_config.hpp"

class Nibo
{
public:
	enum Led
	{
		LED_LEFT_FRONT = (1 << 4),
		LED_LEFT_CENTER = (1 << 3),
		LED_LEFT_SIDE = (1 << 2),
		LED_LEFT_BACK = (1 << 1),
		
		LED_RIGHT_FRONT = (1 << 5),
		LED_RIGHT_CENTER = (1 << 6),
		LED_RIGHT_SIDE = (1 << 7),
		LED_RIGHT_BACK = (1 << 0),
	};
	
	enum Color
	{
		OFF = 0,
		GREEN = 1,
		RED = 2,
		YELLOW = 3
	};
	
public:
	/**
	 * \brief	Initialize peripherals
	 * 
	 * Sets up PWM etc.
	 */
	static void
	initialize();
	
	/**
	 * \brief	Update internal status
	 * 
	 * Transfer data to the display etc.
	 */
	static void
	update();
	
	/**
	 * \brief	Change color of the status leds
	 */
	static void
	setStatusLed(Led led, Color color);
	
	/**
	 * \brief	Control the intensity of the status leds
	 * @param	Intensity (0..100)
	 */
	static void
	setStatusLedIntensity(uint8_t intensity);
	
	/**
	 * \brief	Enable headlights in the front	
	 * \param	Intensity (0..100)
	 */
	static void
	setHeadlights(uint8_t intensity);
	
	/**
	 * \brief	Control backlight of the graphic display	
	 * \param	Intensity (0..100)
	 */
	static void
	setDisplayBacklight(uint8_t intensity);
	
public:
	static nibo::Display display;
	static xpcc::BufferedUart1 uart;
	
private:
	/// TODO
	static uint16_t
	calculateCompareValue(uint8_t intensity);
	
	static xpcc::PeriodicTimer<> displayTimer;
};

#endif // NIBO_HPP
