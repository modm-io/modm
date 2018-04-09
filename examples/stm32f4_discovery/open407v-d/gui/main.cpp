/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2013, Fabian Greif
 * Copyright (c) 2012-2014, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2014, Daniel Krebs
 * Copyright (c) 2013-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/driver/display/parallel_tft.hpp>
#include <modm/driver/bus/tft_memory_bus.hpp>
#include <modm/ui/display/image.hpp>
#include <modm/driver/touch/ads7843.hpp>
#include <modm/container/container.hpp>
#include <modm/utils/allocator.hpp>

#include <modm/ui/gui.hpp>

#include "touchscreen_calibrator.hpp"

#include "images/bluetooth_12x16.hpp"

// ----------------------------------------------------------------------------
/*
 * Setup UART Logger
 */

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);


void
initLogger()
{
	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();
}

// ----------------------------------------------------------------------------

/* FSMC
 *
 * * Use A16 as Command / Data pin
 * 0x60000000 is the base address for FSMC's first memory bank.
 * When accessing 0x60000000 A16 is low.
 *
 * Why offset +0x20000 for A16?
 * (1 << 16) is 0x10000.
 *
 * But the TftMemoryBus16Bit uses the FSMC in 16 bit mode.
 * Then, according to Table 184 (External memory address) of
 * reference manual (p1317) address HADDR[25:1] >> 1 are issued to
 * the address pins A24:A0.
 * So when writing to offset +((1 << 16) << 1) pin A16 is high.
 */
modm::TftMemoryBus16Bit parallelBus(
		(volatile uint16_t *) 0x60000000,
		(volatile uint16_t *) 0x60020000);

modm::ParallelTft<modm::TftMemoryBus16Bit> tft(parallelBus);

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

modm::Ads7843<SpiMaster2, CsTouchscreen, IntTouchscreen> ads7843;
modm::TouchscreenCalibrator touchscreen;

typedef GpioOutputD7 CS;

static void
initDisplay()
{

	Fsmc::initialize();
	GpioConnector<Peripheral::Fsmc,
		GpioD14::D0,
		GpioD15::D1,
		GpioD0::D2,
		GpioD1::D3,
		GpioE7::D4,
		GpioE8::D5,
		GpioE9::D6,
		GpioE10::D7,
		GpioE11::D8,
		GpioE12::D9,
		GpioE13::D10,
		GpioE14::D11,
		GpioE15::D12,
		GpioD8::D13,
		GpioD9::D14,
		GpioD10::D15,
		GpioD4::Noe,
		GpioD5::Nwe,
		GpioD11::A16>::connect();


	CS::setOutput();
	CS::reset();

	fsmc::NorSram::AsynchronousTiming timing = {
		// read
		15,
		0,
		15,

		// write
		15,
		0,
		15,

		// bus turn around
		0
	};

	fsmc::NorSram::configureAsynchronousRegion(
			fsmc::NorSram::CHIP_SELECT_1,
			fsmc::NorSram::NO_MULTIPLEX_16BIT,
			fsmc::NorSram::SRAM_ROM,
			fsmc::NorSram::MODE_A,
			timing);

	fsmc::NorSram::enableRegion(fsmc::NorSram::CHIP_SELECT_1);

	tft.initialize();
}

static void
initTouchscreen()
{
	CsTouchscreen::setOutput();
	CsTouchscreen::set();

	IntTouchscreen::setInput(Gpio::InputType::PullUp);

	SpiMaster2::connect<GpioB13::Sck, GpioB14::Miso, GpioB15::Mosi>();
	SpiMaster2::initialize<Board::systemClock, 656250ul>();
	SpiMaster2::setDataMode(SpiMaster2::DataMode::Mode0);

}

// ----------------------------------------------------------------------------
/* screen calibration  */
static void
drawCross(modm::GraphicDisplay& display, modm::glcd::Point center)
{
	display.setColor(modm::glcd::Color::red());
	display.drawLine(center.x - 15, center.y, center.x - 2, center.y);
	display.drawLine(center.x + 2, center.y, center.x + 15, center.y);
	display.drawLine(center.x, center.y - 15, center.x, center.y - 2);
	display.drawLine(center.x, center.y + 2, center.x, center.y + 15);
	typedef void (*genericCallback)(void*);
	display.setColor(modm::glcd::Color::white());
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
calibrateTouchscreen(modm::GraphicDisplay& display, modm::glcd::Point *fixed_samples = NULL)
{
	modm::glcd::Point calibrationPoint[3] = { { 45, 45 }, { 270, 90 }, { 100, 190 } };
	modm::glcd::Point sample[3];

	if(!fixed_samples) {
		for (uint8_t i = 0; i < 3; i++)
		{
			display.clear();

			display.setColor(modm::glcd::Color::yellow());
			display.setCursor(50, 5);
			display << "Touch crosshair to calibrate";

			drawCross(display, calibrationPoint[i]);
			modm::delayMilliseconds(500);

			while (!ads7843.read(&sample[i])) {
				// wait until a valid sample can be taken
			}

			MODM_LOG_DEBUG << "calibration point: (" << sample[i].x << " | " << sample[i].y << ")" << modm::endl;
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
	if (point.x < 0 || point.y < 0) {
		return;
	}

	display.drawPixel(point.x, point.y);
	display.drawPixel(point.x + 1, point.y);
	display.drawPixel(point.x, point.y + 1);
	display.drawPixel(point.x + 1, point.y + 1);
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
	modm::delayMicroseconds(130);
	m2 = !IntTouchscreen::read();

	return (m1 || m2);
}

void
resetTouchLock(void* data)
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

	Board::LedGreen::toggle();
}


//modm::gui::ColorPalette colorpalette[modm::gui::Color::PALETTE_SIZE] = {
//	modm::glcd::Color::black(),
//	modm::glcd::Color::white(),
//	modm::glcd::Color::gray(),
//	modm::glcd::Color::red(),
//	modm::glcd::Color::green(),
//	modm::glcd::Color::blue(),
//	modm::glcd::Color::blue(),		// BORDER
//	modm::glcd::Color::red(),		// TEXT
//	modm::glcd::Color::black(),		// BACKGROUND
//	modm::glcd::Color::red(),		// ACTIVATED
//	modm::glcd::Color::blue(),		// DEACTIVATED
//
//};

/*
 * empirically found calibration points
 */
modm::glcd::Point calibration[] = {{3339, 3046},{931, 2428},{2740, 982}};

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	initLogger();

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

//	colorpalette[modm::gui::Color::TEXT] = modm::glcd::Color::yellow();


	/*
	 * Create a view and some widgets
	 */

//	modm::gui::View myView(&tft, colorpalette, &input_queue);

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

//	myView.pack(&toggleLedButton, modm::glcd::Point(110, 10));
//	myView.pack(&doNothingButton, modm::glcd::Point(110, 80));
//	myView.pack(&rocker1, modm::glcd::Point(60, 200));

	/*
	 * main loop
	 */

	while(true) {

		gatherInput();

		// process asynchronous events
		updateAsyncEvents();

		// update view
//		myView.run();

		/*
		 * display an arbitrary image
		 */
		//tft.drawImage(modm::glcd::Point(304, 3),modm::accessor::asFlash(bitmap::bluetooth_12x16));

	}

	return 0;
}
