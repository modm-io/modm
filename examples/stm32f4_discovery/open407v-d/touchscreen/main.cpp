
#include <xpcc/architecture.hpp>

#include <xpcc/driver/display/parallel_tft.hpp>
#include <xpcc/driver/bus/tft_memory_bus.hpp>
#include <xpcc/ui/display/image.hpp>
#include <xpcc/driver/touch/ads7843.hpp>

#include "touchscreen_calibrator.hpp"

#include "../../stm32f4_discovery.hpp"


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
xpcc::TftMemoryBus16Bit parallelBus(
		(volatile uint16_t *) 0x60000000,
		(volatile uint16_t *) 0x60020000);

xpcc::ParallelTft<xpcc::TftMemoryBus16Bit> tft(parallelBus);

// ----------------------------------------------------------------------------
// Touchscreen

typedef GpioOutputC4 CsTouchscreen;
typedef GpioInputC5  IntTouchscreen;

xpcc::Ads7843<SpiMaster2, CsTouchscreen, IntTouchscreen> ads7843;
xpcc::TouchscreenCalibrator touchscreen;

typedef GpioOutputD7 CS;

static void
initDisplay()
{

	Fsmc::initialize();

	GpioD14::connect(Fsmc::D0);
	GpioD15::connect(Fsmc::D1);
	GpioD0::connect(Fsmc::D2);
	GpioD1::connect(Fsmc::D3);
	GpioE7::connect(Fsmc::D4);
	GpioE8::connect(Fsmc::D5);
	GpioE9::connect(Fsmc::D6);
	GpioE10::connect(Fsmc::D7);
	GpioE11::connect(Fsmc::D8);
	GpioE12::connect(Fsmc::D9);
	GpioE13::connect(Fsmc::D10);
	GpioE14::connect(Fsmc::D11);
	GpioE15::connect(Fsmc::D12);
	GpioD8::connect(Fsmc::D13);
	GpioD9::connect(Fsmc::D14);
	GpioD10::connect(Fsmc::D15);

	GpioD4::connect(Fsmc::Noe);
	GpioD5::connect(Fsmc::Nwe);
	GpioD11::connect(Fsmc::A16);


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

	GpioOutputB13::connect(SpiMaster2::Sck);
	GpioInputB14::connect(SpiMaster2::Miso);
	GpioOutputB15::connect(SpiMaster2::Mosi);

	SpiMaster2::initialize<defaultSystemClock, 1312500ul>();
	SpiMaster2::setDataMode(SpiMaster2::DataMode::Mode0);

}

// ----------------------------------------------------------------------------
static void
drawCross(xpcc::GraphicDisplay& display, xpcc::glcd::Point center)
{
	display.setColor(xpcc::glcd::Color::red());
	display.drawLine(center.x - 15, center.y, center.x - 2, center.y);
	display.drawLine(center.x + 2, center.y, center.x + 15, center.y);
	display.drawLine(center.x, center.y - 15, center.x, center.y - 2);
	display.drawLine(center.x, center.y + 2, center.x, center.y + 15);

	display.setColor(xpcc::glcd::Color::white());
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
calibrateTouchscreen(xpcc::GraphicDisplay& display)
{
	xpcc::glcd::Point calibrationPoint[3] = { { 45, 45 }, { 270, 90 }, { 100, 190 } };
	xpcc::glcd::Point sample[3];

	for (uint8_t i = 0; i < 3; i++)
	{
		display.clear();

		display.setColor(xpcc::glcd::Color::yellow());
		display.setCursor(50, 5);
		display << "Touch crosshair to calibrate";

		drawCross(display, calibrationPoint[i]);
		xpcc::delayMilliseconds(500);

		while (!ads7843.read(&sample[i])) {
			// wait until a valid sample can be taken
		}
	}

	touchscreen.calibrate(calibrationPoint, sample);

	display.clear();
}

void
drawPoint(xpcc::GraphicDisplay& display, xpcc::glcd::Point point)
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
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);

	Button::setInput();

	initDisplay();
	initTouchscreen();

	calibrateTouchscreen(tft);

	tft.setColor(xpcc::glcd::Color::lime());

	while (1)
	{
		xpcc::glcd::Point raw;
		if (ads7843.read(&raw)) {
			xpcc::glcd::Point point;

			touchscreen.translate(&raw, &point);
			drawPoint(tft, point);

			LedGreen::set();
		}
		else {
			LedGreen::reset();
		}

		// clear screen if the user button is pressed
		if (Button::read()) {
			tft.clear();
		}
	}

	return 0;
}

