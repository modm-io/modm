
#include <xpcc/architecture.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

#include "nibo2.hpp"
#include "nds3.hpp"

MAIN_FUNCTION
{
	Nibo2::initialize();
	
	Nibo2::setStatusLed(Nibo2::LED_LEFT_FRONT, Nibo2::GREEN);
	Nibo2::setStatusLed(Nibo2::LED_RIGHT_FRONT, Nibo2::GREEN);
	
	Nibo2::setStatusLed(Nibo2::LED_LEFT_CENTER, Nibo2::YELLOW);
	Nibo2::setStatusLed(Nibo2::LED_RIGHT_CENTER, Nibo2::YELLOW);
	
	Nibo2::setStatusLed(Nibo2::LED_LEFT_SIDE, Nibo2::RED);
	Nibo2::setStatusLed(Nibo2::LED_RIGHT_SIDE, Nibo2::RED);
	
	Nibo2::setDisplayBacklight(80);
	
	xpcc::PeriodicTimer<> timer(20);
	xpcc::PeriodicTimer<> timerServo(2000);
	uint8_t intensity = 100;
	bool upCounting = false;
	while (1)
	{
		Nibo2::update();
		
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
			
			Nibo2::setStatusLedIntensity(intensity);
		}
		
		if (timerServo.isExpired()) {
			Nds3::move(30);
		}
	}
}
