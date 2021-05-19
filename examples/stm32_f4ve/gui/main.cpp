/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2013, Fabian Greif
 * Copyright (c) 2012-2014, 2017, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2014, Daniel Krebs
 * Copyright (c) 2013-2018, Niklas Hauser
 * Copyright (c) 2020, Pavel Pletenev
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/driver/display/ili9341_parallel.hpp>
#include <modm/driver/bus/tft_memory_bus.hpp>
#include <modm/ui/display/image.hpp>
#include <modm/driver/touch/ads7843.hpp>
#include <modm/container.hpp>
#include <modm/utils/allocator.hpp>

#include <modm/ui/gui.hpp>


#include "touchscreen_calibrator.hpp"

#include "images/bluetooth_12x16.hpp"

using namespace modm::color;


// ----------------------------------------------------------------------------
/*
 * Setup UART Logger
 */

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// ----------------------------------------------------------------------------

/* FSMC
 *
 * * Use A18 as Command / Data pin
 * 0x60000000 is the base address for FSMC's first memory bank.
 * When accessing 0x60000000 A16 is low.
 *
 * But the TftMemoryBus16Bit uses the FSMC in 16 bit mode.
 * Then, according to Table 184 (External memory address) of
 * reference manual (p1317) address HADDR[25:1] >> 1 are issued to
 * the address pins A24:A0.
 * So when writing to offset +((1 << 18) << 1) pin A16 is high.
 */
constexpr std::uintptr_t offset = 0x60000000;
constexpr std::uintptr_t offsetCmd = offset + ((1 << 18) << 1);
modm::TftMemoryBus16Bit parallelBus(
		(volatile uint16_t *) offset,
		(volatile uint16_t *) offsetCmd);

using Display = modm::Ili9341Parallel<
	modm::TftMemoryBus16Bit, Board::display::Reset, Board::display::Backlight>;
Display tft(parallelBus);

// ----------------------------------------------------------------------------

constexpr uint8_t 	TP_TOLERANCE 		= 	30;		// in pixels
constexpr uint16_t	TOUCH_REPEAT_RATE	=	600;	// in ms, 0 to disable


modm::gui::inputQueue input_queue;
modm::gui::AsyncEventList async_events;
modm::glcd::Point last_point;

// ----------------------------------------------------------------------------
// Touchscreen

typedef GpioOutputC4 CsTouchscreen;
typedef GpioInputC5  IntTouchscreen;

modm::Ads7843<Board::display::touch::SpiMaster, Board::display::touch::Cs, Board::display::touch::Pen> ads7843;
modm::TouchscreenCalibrator touchscreen;

typedef GpioOutputD7 CS;




static void
initDisplay()
{
	MODM_LOG_DEBUG << __PRETTY_FUNCTION__ << modm::endl;
	Fsmc::initialize();
	GpioConnector<Peripheral::Fsmc,
		Board::display::fsmc::D0::D0,
		Board::display::fsmc::D1::D1,
		Board::display::fsmc::D2::D2,
		Board::display::fsmc::D3::D3,
		Board::display::fsmc::D4::D4,
		Board::display::fsmc::D5::D5,
		Board::display::fsmc::D6::D6,
		Board::display::fsmc::D7::D7,
		Board::display::fsmc::D8::D8,
		Board::display::fsmc::D9::D9,
		Board::display::fsmc::D10::D10,
		Board::display::fsmc::D11::D11,
		Board::display::fsmc::D12::D12,
		Board::display::fsmc::D13::D13,
		Board::display::fsmc::D14::D14,
		Board::display::fsmc::D15::D15,
		Board::display::fsmc::Noe::Noe,
		Board::display::fsmc::Nwe::Nwe,
		Board::display::fsmc::A18::A18>::connect();


	CS::setOutput();
	CS::reset();

	fsmc::NorSram::AsynchronousTiming timing = {
		// read
		15, 0, 15,
		// write
		15, 0, 15,
		// bus turn around
		15
	};

	fsmc::NorSram::configureAsynchronousRegion(
			fsmc::NorSram::CHIP_SELECT_1,
			fsmc::NorSram::NO_MULTIPLEX_16BIT,
			fsmc::NorSram::SRAM_ROM,
			fsmc::NorSram::MODE_A,
			timing);

	fsmc::NorSram::enableRegion(fsmc::NorSram::CHIP_SELECT_1);


	tft.initialize();
	tft.enableBacklight(true);
	tft.setOrientation(modm::glcd::Orientation::Portrait90);
}



static void
initTouchscreen()
{
	MODM_LOG_DEBUG << __PRETTY_FUNCTION__ << modm::endl;
	CsTouchscreen::setOutput();
	CsTouchscreen::set();

	IntTouchscreen::setInput(Gpio::InputType::PullUp);
	using namespace Board::display::touch;

	SpiMaster2::connect<Clk::Sck, Miso::Miso, Mosi::Mosi>();
	SpiMaster2::initialize<Board::SystemClock, 656250ul>();
	SpiMaster2::setDataMode(SpiMaster2::DataMode::Mode0);

}

// ----------------------------------------------------------------------------
/* screen calibration  */
static void
drawCross(modm::ColorGraphicDisplay& display, modm::glcd::Point center)
{
	display.setColor(html::Red);
	display.drawLine(center.x - 15, center.y, center.x - 2, center.y);
	display.drawLine(center.x + 2, center.y, center.x + 15, center.y);
	display.drawLine(center.x, center.y - 15, center.x, center.y - 2);
	display.drawLine(center.x, center.y + 2, center.x, center.y + 15);

	display.setColor(html::White);
	display.drawLine(center.x - 15, center.y + 15, center.x - 7, center.y + 15);
	display.drawLine(center.x - 15, center.y + 7, center.x - 15, center.y + 15);

	display.drawLine(center.x - 15, center.y - 15, center.x - 7, center.y - 15);
	display.drawLine(center.x - 15, center.y - 7, center.x - 15, center.y - 15);

	display.drawLine(center.x + 7, center.y + 15, center.x + 15, center.y + 15);
	display.drawLine(center.x + 15, center.y + 7, center.x + 15, center.y + 15);

	display.drawLine(center.x + 7, center.y - 15, center.x + 15, center.y - 15);
	display.drawLine(center.x + 15, center.y - 15, center.x + 15, center.y - 7);
}

static void
calibrateTouchscreen(modm::ColorGraphicDisplay& display, modm::glcd::Point *fixed_samples = nullptr)
{
	MODM_LOG_DEBUG << __PRETTY_FUNCTION__ << modm::endl;
	modm::glcd::Point calibrationPoint[3] = { { 45, 45 }, { 270, 90 }, { 100, 190 } };
	modm::glcd::Point sample[3];

	if(!fixed_samples) {
		for (uint8_t i = 0; i < 3; i++)
		{
			display.clear();

			display.setColor(modm::color::html::Yellow);
			display.setCursor(50, 5);
			display << "Touch crosshair to calibrate";

			drawCross(display, calibrationPoint[i]);
			modm::delay(500ms);

			while (!ads7843.read(&sample[i])) {
				// wait until a valid sample can be taken
			}

			MODM_LOG_DEBUG << "calibration point: ("  << sample[i].x << " | " << sample[i].y << ")" << modm::endl;
		}

		touchscreen.calibrate(calibrationPoint, sample);

	} else {
		touchscreen.calibrate(calibrationPoint, fixed_samples);
	}

	display.clear();
}

void
drawPoint(modm::GraphicDisplay& display, modm::glcd::Point point)
{
	MODM_LOG_DEBUG << __PRETTY_FUNCTION__ << modm::endl;
	if (point.x < 0 || point.y < 0) {
		return;
	}

	display.setPixel(point.x, point.y);
	display.setPixel(point.x + 1, point.y);
	display.setPixel(point.x, point.y + 1);
	display.setPixel(point.x + 1, point.y + 1);
}

// ----------------------------------------------------------------------------
/* catch touch input */
bool
touchActive()
{
	/*
	 * XPT2046:
	 *
	 * !PENIRQ when not touched:
	 * _____   _   __________   _   _______
	 *      \_/ \_/          \_/ \_/
	 *     |<----->|
	 *		~120us
	 *
	 * !PENIRQ when touched:
	 *
	 * _____|____|______|__|_|_____|___
	 *
	 * random ~100ns peaks
	 *
	 */

	bool m1, m2;

	m1 = !IntTouchscreen::read();
	modm::delay(130us);
	m2 = !IntTouchscreen::read();

	return (m1 || m2);
}

void
resetTouchLock(void* /* data */)
{
	last_point = modm::glcd::Point(-400, -400);
}

bool
debounceTouch(modm::glcd::Point *out, modm::glcd::Point *old)
{
	modm::glcd::Point raw, point;

	if(touchActive()) {

		if (ads7843.read(&raw)) {

			// translate point according to calibration
			touchscreen.translate(&raw, &point);

			if(abs(point.x - old->x) < TP_TOLERANCE &&
			   abs(point.y - old->y) < TP_TOLERANCE
			  )
			{
				// point is within area of last touch
				*old = point;
				return false;
			}

			// new touch point
			*old = point;
			*out = point;

			// schedule a reset for debounce lock, so that holding the finger fires repeated touch events
			if(TOUCH_REPEAT_RATE)
			{
				async_events.append(new modm::gui::AsyncEvent(TOUCH_REPEAT_RATE, &resetTouchLock, NULL));
			}

			return true;
		}
	} else {
		// reset old point so that when touched again you can touch the same spot
		*old = modm::glcd::Point(-400, -400);
	}
	return false;
}

void
touchUp(void* data)
{
	MODM_LOG_DEBUG << __PRETTY_FUNCTION__ << modm::endl;
	modm::gui::InputEvent* ev = static_cast<modm::gui::InputEvent*>(data);

	MODM_LOG_DEBUG << "asynchronous UP-event:" << modm::endl;
	MODM_LOG_DEBUG << "x: " << ev->coord.x << modm::endl;
	MODM_LOG_DEBUG << "y: " << ev->coord.y << modm::endl;

	// queue UP-event as new input event
	input_queue.push(ev);
}


void
gatherInput()
{
	modm::glcd::Point point;

	if (debounceTouch(&point, &last_point)) {

		auto ev_down = new modm::gui::InputEvent(point,
											 	 modm::gui::InputEvent::Type::TOUCH,
											 	 modm::gui::InputEvent::Direction::DOWN);

		auto ev_up = new modm::gui::InputEvent( point,
												modm::gui::InputEvent::Type::TOUCH,
												modm::gui::InputEvent::Direction::UP);

		// queue down event
		input_queue.push(ev_down);

		// create an asynchronous event with Direction::UP and 200ms delay
		auto async_ev = new modm::gui::AsyncEvent(500, &touchUp, (void*)(ev_up));
		async_events.append(async_ev);

		MODM_LOG_DEBUG << "touch down x: " << point.x << modm::endl;
		MODM_LOG_DEBUG << "touch down y: " << point.y << modm::endl;

	}
}

inline void
updateAsyncEvents()
{
	auto iter = async_events.begin();

	while(iter != async_events.end())
	{
		if((*iter)->is_expired()) {
			iter = async_events.erase(iter);
		} else
		{
			++iter;
		}
	}
}


// ----------------------------------------------------------------------------

void
test_callback(const modm::gui::InputEvent& ev, modm::gui::Widget* w, void* data)
{
	// avoid warnings
	(void) ev;
	(void) w;
	(void) data;

	Board::LedGreen2::toggle();
}

modm::color::Rgb565 colors[modm::gui::Color::PALETTE_SIZE] {
	html::Black,
	html::White,
	html::Gray,
	html::Red,
	html::Green,
	html::Blue,
	html::Blue,		// BORDER
	html::Red,		// TEXT
	html::Black,		// BACKGROUND
	html::Red,		// ACTIVATED
	html::Blue,		// DEACTIVATED
};
modm::gui::ColorPalette colorpalette{colors};
/*
 * empirically found calibration points
 */
// modm::glcd::Point calibration[] = {{3339, 3046},{931, 2428},{2740, 982}};
modm::glcd::Point* calibration = nullptr;
// ----------------------------------------------------------------------------

class MyView : public modm::gui::View{
public:
	MyView(modm::gui::GuiViewStack* stack, uint8_t identifier, modm::gui::Dimension dimension)
		: View(stack, identifier, dimension){

	}
	~MyView() override{}
protected:
	virtual bool hasChanged() override {
		return true;
	}
};

int
main()
{

	Board::initialize();


	MODM_LOG_DEBUG << "Hello from modm gui example!" << modm::endl;

	initDisplay();
	initTouchscreen();

	/*
	 * calibrate touchscreen with already found calibration points
	 */

	calibrateTouchscreen(tft, calibration);


	/*
	 * manipulate the color palette
	 */

	colorpalette.setColor(modm::gui::Color::TEXT, html::Yellow);


	/*
	 * Create a view and some widgets
	 */
	modm::gui::GuiViewStack stack(&tft, &input_queue);
	MyView myView(&stack, 1, modm::gui::Dimension(320, 240));

	modm::gui::ButtonWidget toggleLedButton((char*)"Toggle Green", modm::gui::Dimension(100, 50));
	modm::gui::ButtonWidget doNothingButton((char*)"Do nothing", modm::gui::Dimension(100, 50));

	modm::gui::IntegerRocker rocker1(100, 50, modm::gui::Dimension(200, 30));


	/*
	 * connect callbacks to widgets
	 */

	toggleLedButton.cb_activate = &test_callback;


	/*
	 * place widgets in view
	 */

	myView.pack(&toggleLedButton, modm::glcd::Point(110, 10));
	myView.pack(&doNothingButton, modm::glcd::Point(110, 80));
	myView.pack(&rocker1, modm::glcd::Point(60, 200));
	stack.push(&myView);

	// modm::glcd::Point pixPos(10, 200);
	// modm::glcd::Point pixDeltaPos(1, 1);
	// auto pix = modm::accessor::asFlash(bitmap::bluetooth_12x16);
	while(true) {

		gatherInput();

		// process asynchronous events
		updateAsyncEvents();

		// update view
		stack.update();


		/*
		 * display an arbitrary image
		 */
		// if(pixPos.x < 0 || (pixPos.x + pix[0]) > tft.getWidth()){
		// 	pixDeltaPos.x *= -1;
		// }
		// if(pixPos.y < 0 || (pixPos.y + pix[1]) > tft.getHeight()){
		// 	pixDeltaPos.y *= -1;
		// }
		// pixPos += pixDeltaPos;
		// tft.drawImage(pixPos, pix);
	}

	return 0;
}
