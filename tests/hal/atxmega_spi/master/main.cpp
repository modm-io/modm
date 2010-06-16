#include <avr/interrupt.h>

#include <xpcc/architecture/gpio.hpp>
#include <xpcc/driver/software_spi.hpp>
#include <xpcc/architecture/general/time/delay.hpp>
#include <xpcc/architecture/avr/xmega/spi.hpp>

#include <xpcc/driver/lcd/dog_m16x.hpp>
#include <xpcc/driver/debounce.hpp>

#include <xpcc/io/iostream.hpp>
#include <xpcc/io/iodevice_wrapper.hpp>

#include <xpcc/utils/misc.hpp>

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
CREATE_INPUT_PIN(Miso, D, 6);
CREATE_OUTPUT_PIN(Mosi, D, 5);

CREATE_OUTPUT_PIN(LcdCs, D, 4);
CREATE_OUTPUT_PIN(LcdRs, D, 0);

CREATE_OUTPUT_PIN(SlaveCs, D, 3);

SoftwareSpi < Sclk, Mosi, Miso > spi;
DogM16x < SoftwareSpi < Sclk, Mosi, Miso >,
		  LcdCs,
		  LcdRs> display;

Debounce keys(0x07);
Debounce encoder;


struct DmaPayload{
	char string[24];
	float f;
	uint16_t ii;
	int8_t i;
	bool b;
};

volatile DmaPayload dmaPayload;
volatile DmaPayload dmaAnswer;

volatile uint8_t value = 50;

//CREATE_SPI_MODULE(mySpi, SPIC);

//void configure_spi_test(){
//	mySpi::initialize(SPI_MODE_0_gc, SPI_PRESCALER_DIV16_gc);
//	mySpi::enableInterrupt(SPI_INTLVL_MED_gc);
//}

void configureTimer(TC0_t &timer){
	timer.CTRLA |= TC_CLKSEL_DIV2_gc;
	timer.INTCTRLA |= TC_OVFINTLVL_MED_gc;
	timer.INTCTRLB |= TC_CCAINTLVL_MED_gc | TC_CCBINTLVL_MED_gc;
	
	timer.PER = 1000; // period
	timer.CCA = 500; // duty cycle
}

void sendData(){
	dmaPayload.i = value;
	uint8_t * fin = (uint8_t*)(&dmaPayload + 1);
	
	SlaveCs::low();
	for (uint8_t *p = (uint8_t*)&dmaPayload, *ap = (uint8_t*)&dmaAnswer; p < fin; p++, ap++)
		*ap = spi.put(*p);
	SlaveCs::high();
	Led5::toggle();
}

void timerUpdate(TC0_t &timer){
	static bool up = 1;
	int16_t t;
	if (up){
		t = timer.CCA + 40;
	}
	else{
		t = timer.CCA - 40;
	}
	if (t >= 1000 || t <= 0){
		up = !up;
//		timer.CCA = 0;
	}
	else{
		timer.CCA = t;
	}
}

int8_t encoderRead(){
	static int8_t table[] = {0,1,-1,0, -1,0,0,1, 1,0,0,-1, 0,-1,1,0};
	static uint8_t state_old = 0;
	static int8_t pos = 0;
	
	uint8_t state = (EncoderA::get()?1:0) | (EncoderB::get()?2:0);
	uint8_t lookup = state_old | (state<<2);
	pos += table[lookup];
	state_old = state;
	
	int8_t step = 0;
	if (pos < 0)
		step = -1;
	else if (pos >= 4)
		step = 1;
	pos &= 0x03;
	
	return step;
	
}

TC0_t &timer = TCD0;

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
	
	SlaveCs::output();
	SlaveCs::low();
	
	
	Button0::configure(gpio::PULLUP);
	Button1::configure(gpio::PULLUP);
	Button2::configure(gpio::PULLUP);
	Button3::configure(gpio::PULLUP);
	
	EncoderButton::configure(gpio::PULLUP);
	
	Led6::high();
	Led7::low();
	
	delay_ms(100);
	
	display.initialize();
	
	IOStream stream(display);
	
	stream << "Hello World ;-)\n";
	stream << "\x19" " = ";
	
	Led7::high();
	delay_ms(100);
	
	// enable medium interrupts
	PMIC.CTRL |= PMIC_MEDLVLEX_bm;
	
	configureTimer(timer);

	Led7::low();
	delay_ms(100);
	
	// enable global interrupts
	sei();
	Led7::high();
	delay_ms(100);
	Led7::low();
	
	while (1)
	{
//		delay_ms(10);
		
//		timerUpdate(timer);
		
//		keys.update(PORTB.IN);
		
		if (keys.getPress(Debounce::KEY0) || keys.getRepeat(Debounce::KEY0)) {
//			value += 2;
		}
		if (keys.getPress(Debounce::KEY1) || keys.getRepeat(Debounce::KEY1)) {
//			value -= 2;
		}
		
		if (keys.getShortPress(Debounce::KEY2)) {
//			value -= 2;
		}
		if (keys.getLongPress(Debounce::KEY2)) {
//			value += 2;
		}
		
		if (keys.getPress(Debounce::KEY3)) {
			Led0::toggle();
			sendData();
		}
		
		Led6::set(EncoderA::get());
		Led7::set(EncoderB::get());
		
		dmaPayload.b = EncoderButton::get();
		Led5::set(EncoderButton::get());
		// encoder button
//		if (encoder.getPress(Debounce::KEY2)) {
//			Led5::toggle();
//		}
		
		value += encoderRead();
		
		timer.CCB = value * 10;
		
		// check if answer is eaqual to sent payload
		bool b = true;
		uint8_t * fin = (uint8_t*)(&dmaPayload + 1);
		for (uint8_t *p = ((uint8_t*)(&dmaPayload)) + 1, *ap = ((uint8_t*)&dmaAnswer) + 1; (p < fin) && b; p++, ap++)
			b = *p == *ap;
		
		display.setPosition(1, 0);
		if (b){
			stream << (uint8_t)value << " " << (uint8_t)dmaAnswer.i << " OK   ";
		}
		else{
			stream << (uint8_t)value << " " << (uint8_t)dmaAnswer.i << " --   ";
		}
		if(Button0::get())
			sendData();
	}

}

ISR(SPIC_INT_vect){
	
}

ISR(DMA_CH0_vect){
	DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm; // clear the flag. maybe necessary if interrupt is enabled.
}

ISR(TCD0_OVF_vect){
	Led3::low();
	Led4::low();
	
	static uint8_t i = 0;
	if (i++ == 20){
		i=0;
		timerUpdate(timer);
		keys.update(PORTB.IN);
	}
}

ISR(TCD0_CCA_vect){
	Led4::high();
}

ISR(TCD0_CCB_vect){
	Led3::high();
}
