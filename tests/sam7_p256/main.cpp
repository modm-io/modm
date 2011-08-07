
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

#include <xpcc/driver/button_group.hpp>
#include <xpcc/driver/software_spi.hpp>
#include <xpcc/driver/gpio.hpp>

#include <xpcc/driver/lcd/ea_dog.hpp>
#include <xpcc/driver/lcd/font.hpp>

// ----------------------------------------------------------------------------
xpcc::at91::Debug debugUart(115200);
xpcc::IODeviceWrapper<xpcc::at91::Debug> loggerDevice(debugUart); 

xpcc::log::Logger xpcc::log::debug(loggerDevice); 
xpcc::log::Logger xpcc::log::info(loggerDevice); 
xpcc::log::Logger xpcc::log::warning(loggerDevice); 
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led0, A, 0);
GPIO__OUTPUT(Led1, A, 1);
GPIO__OUTPUT(Led2, A, 2);
GPIO__OUTPUT(Led3, A, 7);

typedef xpcc::gpio::Nibble<Led3, Led2, Led1, Led0> Leds;

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedButtonInverted1, A, 18);	// inverted, 0=on, 1=off
GPIO__OUTPUT(LedButtonInverted2, A, 17);

typedef xpcc::gpio::Invert<LedButtonInverted1> LedGreen;
typedef xpcc::gpio::Invert<LedButtonInverted2> LedYellow;

// ----------------------------------------------------------------------------
GPIO__INPUT(ButtonInverted1, A, 19);		// inverted, 0=pressed, 1=not pressed
GPIO__INPUT(ButtonInverted2, A, 20);

typedef xpcc::gpio::Invert<ButtonInverted1> Button1;
typedef xpcc::gpio::Invert<ButtonInverted2> Button2;

#define	BUTTON1		0x1
#define	BUTTON2		0x2

static xpcc::ButtonGroup<uint32_t> button(BUTTON1 | BUTTON2);

// ----------------------------------------------------------------------------
// Graphic LCD
namespace lcd
{
	GPIO__OUTPUT(Sck, A, 14);
	GPIO__INPUT(Miso, A, 12);
	GPIO__OUTPUT(Mosi, A, 13);
	
	GPIO__OUTPUT(CS, A, 28);
	GPIO__OUTPUT(A0, A, 26);
	GPIO__OUTPUT(Reset, A, 23);
}

typedef xpcc::SoftwareSpi< lcd::Sck, lcd::Mosi, lcd::Miso, 1000000UL > SPI;
xpcc::DogS102< SPI, lcd::CS, lcd::A0, lcd::Reset, false > display;

// GPIO Expander (uses the same SPI as the LCD)
GPIO__OUTPUT(CsGpio, A, 29);
GPIO__INPUT(IntGpio, A, 27);

xpcc::Mcp23s08< SPI, CsGpio, IntGpio > gpio;

// ----------------------------------------------------------------------------
xpcc::at91::PwmChannel pwmChannel0(0);
xpcc::at91::PwmChannel pwmChannel1(1);
xpcc::at91::PwmChannel pwmChannel2(2);
xpcc::at91::PwmChannel pwmChannel3(3);

// ----------------------------------------------------------------------------
void
pitHandler(void)
{
	xpcc::Clock::increment();
	xpcc::at91::Pit::acknowledgeInterrupt();
	
	uint32_t status = 0;
	if (Button1::read()) {
		status |= BUTTON1;
	}
	if (Button2::read()) {
		status |= BUTTON2;
	}
	button.update(~status);
}

// ----------------------------------------------------------------------------
#define	LCD_BL_RED		0x01
#define	LCD_BL_GREEN	0x02

#define	BUTTON_LEFT		0x04
#define	BUTTON_DOWN		0x08
#define	BUTTON_UP		0x10
#define	BUTTON_OK		0x20
#define	BUTTON_RIGHT	0x40

#define	LED_GREEN		0x80

// ----------------------------------------------------------------------------
int
main(void)
{
	xpcc::at91::Pio::enable();
	
	/*Led0::setOutput(xpcc::gpio::LOW);
	Led1::setOutput(xpcc::gpio::LOW);
	Led2::setOutput(xpcc::gpio::LOW);
	Led3::setOutput(xpcc::gpio::LOW);*/
	Led0::enablePeripheralA();		// enable PWM for the LEDs
	Led1::enablePeripheralA();
	Led2::enablePeripheralA();
	Led3::enablePeripheralB();
	
	LedGreen::setOutput(xpcc::gpio::HIGH);
	LedYellow::setOutput(xpcc::gpio::LOW);
	
	Button1::setInput();
	Button2::setInput();
	
	display.initialize();
	
	display.setFont(xpcc::font::FixedWidth5x8);
	display.setCursor(0, 0);
	display << "SAM7-P256 Board";
	display.drawLine(0, 8, 102, 8);
	display.update();
	
	// IO-Expander have to initalized after LCD because they use the same
	// Reset line
	gpio.initialize();
	gpio.configure(0x7c, 0x7c);
	gpio.write(LED_GREEN | LCD_BL_GREEN | LCD_BL_RED);
	
	xpcc::at91::Pit::start();
	xpcc::at91::Pit::registerInterruptHandler(pitHandler);
	
	xpcc::at91::Adc::initialize(5e6);
	xpcc::at91::Adc::enableChannel(4);
	xpcc::at91::Adc::startConversion();
	
	xpcc::at91::Pwm::enable();
	
	pwmChannel0.configure(
			xpcc::at91::PwmChannel::PRESCALER_1,
			256, 
			xpcc::at91::PwmChannel::LEFT_ALIGNED |
			xpcc::at91::PwmChannel::NORMAL_MODE);
	pwmChannel0.setDutyCycle(2);
	
	pwmChannel1.configure(
			xpcc::at91::PwmChannel::PRESCALER_1,
			256, 
			xpcc::at91::PwmChannel::CENTER_ALIGNED |
			xpcc::at91::PwmChannel::NORMAL_MODE);
	pwmChannel1.setDutyCycle(1);
	
	xpcc::at91::Pwm::enableChannels(
			xpcc::at91::Pwm::CHANNEL_0 |
			xpcc::at91::Pwm::CHANNEL_1);
	
	XPCC_LOG_DEBUG << "SAM7-P256 Board (extended)" << xpcc::endl;
	
	LedGreen::reset();
	LedYellow::reset();
	
	uint32_t pattern = 0x01;
	bool dir = true;
	xpcc::PeriodicTimer<> timer(100);
	uint32_t pwm = 2;
	uint32_t pwm1 = 1;
	uint32_t adcValue = 0;
	while (1)
	{
		if (button.isPressed(BUTTON1)) {
			LedGreen::toggle();
			pwm1++;
			pwmChannel1.setDutyCycle(pwm1);
		}
		if (button.isPressed(BUTTON2)) {
			LedYellow::toggle();
			pwm1--;
			pwmChannel1.setDutyCycle(pwm1);
		}
		
		if (timer.isExpired())
		{
			if (dir) {
				pattern <<= 1;
				if (pattern == 0x08) {
					dir = false;
				}
			}
			else {
				pattern >>= 1;
				if (pattern == 0x01) {
					dir = true;
				}
			}
			//Leds::write(pattern);
			
			display.setCursor(0, 10);
			display << "sw  = " << xpcc::hex << gpio.read() << xpcc::ascii;
			
			display.setCursor(0, 20);
			display << "adc = " << adcValue << "   ";
			
			display.setCursor(0, 30);
			display << "pwm = " << pwm << "   ";
			
			display.setCursor(0, 40);
			display << "pwm1= " << pwm1 << "   ";
			display.update();
		}
		
		if (xpcc::at91::Adc::isFinished(4)) {
			adcValue = xpcc::at91::Adc::getValue(4);
			xpcc::at91::Adc::startConversion();
			
			// 0..1023 -> 0..5999
			//pwm = (adcValue * 6005) / 1024 + 1;
			pwm = adcValue / 4;
			pwmChannel0.setDutyCycle(pwm);
		}
	}
}
