
#include <avr/io.h>
#include <util/delay.h>

#include <xpcc/hal/peripheral/gpio.hpp>
#include <xpcc/hal/peripheral/software_spi.hpp>

#include <xpcc/driver/lcd/dog_m16x.hpp>

#include <xpcc/io/iostream.hpp>

using namespace xpcc;

CREATE_OUTPUT_PIN(led0, A, 0);
CREATE_OUTPUT_PIN(led1, A, 1);
CREATE_OUTPUT_PIN(led2, A, 2);
CREATE_OUTPUT_PIN(led3, A, 3);
CREATE_OUTPUT_PIN(led4, A, 4);
CREATE_OUTPUT_PIN(led5, A, 5);
CREATE_OUTPUT_PIN(led6, A, 6);
CREATE_OUTPUT_PIN(led7, A, 7);

CREATE_INPUT_PIN(button0, B, 0);
CREATE_INPUT_PIN(button1, B, 1);
CREATE_INPUT_PIN(button2, B, 2);
CREATE_INPUT_PIN(button3, B, 3);

CREATE_OUTPUT_PIN(sclk, D, 7);
CREATE_OUTPUT_PIN(mosi, D, 5);

CREATE_OUTPUT_PIN(lcd_cs, D, 4);
CREATE_OUTPUT_PIN(lcd_rs, D, 0);

DogM16x < SoftwareSpi < sclk, mosi, gpio::Unused, 10000 >,
		  lcd_cs,
		  lcd_rs> display;

int
main()
{
	led0::output();
	led1::output();
	led2::output();
	led3::output();
	led4::output();
	led5::output();
	led6::output();
	led7::output();
	
	button0::configure(gpio::PULLUP, true);
	button1::configure(gpio::PULLUP, true);
	button2::configure(gpio::PULLUP, true);
	button3::configure(gpio::PULLUP, true);
	
	led6::set();
	led7::reset();
	
	display.init();
	
	IOStream stream(display);
	
	stream << "Hallo Fabian :-)\n";
	stream << "\x19" " = ";
	stream << 3.14159;
	
	while (1)
	{
		_delay_ms(200);
		
		led6::toggle();
		led7::toggle();
		
		led0::set(button0::get());
		led1::set(button1::get());
		led2::set(button2::get());
		led3::set(button3::get());
	}
}
