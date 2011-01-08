
#include <xpcc/architecture.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

#include "nibo.hpp"

MAIN_FUNCTION
{
	Nibo::initialize();
	
	Nibo::setStatusLed(Nibo::LED_LEFT_FRONT, Nibo::GREEN);
	Nibo::setStatusLed(Nibo::LED_RIGHT_FRONT, Nibo::GREEN);
	
	Nibo::setStatusLed(Nibo::LED_LEFT_CENTER, Nibo::YELLOW);
	Nibo::setStatusLed(Nibo::LED_RIGHT_CENTER, Nibo::YELLOW);
	
	Nibo::setStatusLed(Nibo::LED_LEFT_SIDE, Nibo::RED);
	Nibo::setStatusLed(Nibo::LED_RIGHT_SIDE, Nibo::RED);
	
	Nibo::setDisplayBacklight(80);
	
	xpcc::PeriodicTimer<> timer(20);
	uint8_t intensity = 100;
	bool upCounting = false;
	while (1)
	{
		Nibo::update();
		
		if (timer.isExpired())
		{
			if (upCounting) {
				intensity++;
				if (intensity >= 70) {
					upCounting = false;
				}
			}
			else {
				intensity--;
				if (intensity == 0) {
					upCounting = true;
				}
			}
			
			Nibo::setStatusLedIntensity(intensity);
		}
	}
}
