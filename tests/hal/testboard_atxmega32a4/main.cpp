
#include <avr/io.h>
#include <util/delay.h>

#include <xpcc/hal/peripheral/gpio.hpp>
#include <xpcc/hal/peripheral/software_spi.hpp>

#include <xpcc/driver/lcd/dog_m16x.hpp>
#include <xpcc/driver/debounce.hpp>

#include <xpcc/io/iostream.hpp>
#include <xpcc/io/iodevice_wrapper.hpp>

using namespace xpcc;

CREATE_OUTPUT_PIN(Led0, A, 0);
CREATE_OUTPUT_PIN(Led1, A, 1);
CREATE_OUTPUT_PIN(Led2, A, 2);
CREATE_OUTPUT_PIN(Led3, A, 3);
CREATE_OUTPUT_PIN(Led4, A, 4);
CREATE_OUTPUT_PIN(Led5, A, 5);
CREATE_OUTPUT_PIN(Led6, A, 6);
CREATE_OUTPUT_PIN(Led7, A, 7);

CREATE_INPUT_PIN(Button0, B, 0);
CREATE_INPUT_PIN(Button1, B, 1);
CREATE_INPUT_PIN(Button2, B, 2);
CREATE_INPUT_PIN(Button3, B, 3);

CREATE_INPUT_PIN(EncoderA, C, 0);
CREATE_INPUT_PIN(EncoderB, C, 1);
CREATE_INPUT_PIN(EncoderButton, C, 2);

CREATE_OUTPUT_PIN(Sclk, D, 7);
CREATE_OUTPUT_PIN(Mosi, D, 5);

CREATE_OUTPUT_PIN(LcdCs, D, 4);
CREATE_OUTPUT_PIN(LcdRs, D, 0);

DogM16x < SoftwareSpi < Sclk, Mosi, gpio::Unused >,
		  LcdCs,
		  LcdRs> display;

Debounce keys(0x07);
Debounce encoder;

int
main()
{
	Led0::output();
	Led1::output();
	Led2::output();
	Led3::output();
	Led4::output();
	Led5::output();
	Led6::output();
	Led7::output();
	
	Button0::configure(gpio::PULLUP);
	Button1::configure(gpio::PULLUP);
	Button2::configure(gpio::PULLUP);
	Button3::configure(gpio::PULLUP);
	
	EncoderButton::configure(gpio::PULLUP);
	
	Led6::set();
	Led7::reset();
	
	_delay_ms(100);
	
	display.initialize();
	
	IOStream stream(display);
	
	stream << "Hello World ;-)\n";
	stream << "\x19" " = ";
	
	float value = 3.14159;
	
	while (1)
	{
		_delay_ms(10);
		
		keys.update(PORTB.IN);
		
		if (keys.getPress(Debounce::KEY0) || keys.getRepeat(Debounce::KEY0)) {
			value += 0.00001;
		}
		if (keys.getPress(Debounce::KEY1) || keys.getRepeat(Debounce::KEY1)) {
			value -= 0.00001;
		}
		
		if (keys.getShortPress(Debounce::KEY2)) {
			value -= 0.00001;
		}
		if (keys.getLongPress(Debounce::KEY2)) {
			value += 0.00001;
		}
		
		if (keys.getPress(Debounce::KEY3)) {
			Led0::toggle();
		}
		
		Led6::set(EncoderA::get());
		Led7::set(EncoderB::get());
		
		// encoder button
		if (encoder.getPress(Debounce::KEY2)) {
			Led5::toggle();
		}
		
		display.setPosition(1, 4);
		stream << value;
	}
}
