#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing/protothread.hpp>

using namespace Board;
using namespace xpcc::glcd;
using namespace Board::ft6;

class LineDrawer : public xpcc::pt::Protothread
{
public:
	LineDrawer() :
		touch(touchData),
		display{Board::getDisplay()},
		px{-1, -1}, py{-1, -1},
		c{Color::white(), Color::white()}
		{}

	bool
	update()
	{
		PT_BEGIN();

		// Configure the touchscreen to sample with 60Hz in active and monitor mode.
		PT_CALL(touch.configure(Touch::InterruptMode::Trigger, 60, 60));

		while (1)
		{
			do {
				// Wait for either touchscreen interrupt or clear screen button
				PT_WAIT_UNTIL(Int::getExternalInterruptFlag() or Button::read());
				if (Button::read()) display.clear();
			} while (not Int::getExternalInterruptFlag());

			Int::acknowledgeExternalInterruptFlag();
			LedRed::set();

			PT_CALL(touch.readTouches());

			for (int ii=0; ii < 2; ii++)
			{
				Touch::touch_t tp;
				touch.getData().getTouch(&tp, ii);
				// mirror and rotate correctly
				uint16_t x{tp.y}, y{uint16_t(480 - tp.x)};

				if (tp.event == Touch::Event::PressDown or (px[tp.id] < 0))
				{
					// remember the start point
					px[tp.id] = x; py[tp.id] = y;
				}
				else if (tp.event == Touch::Event::Contact)
				{
					// draw the line from the previous point to this one
					display.setColor(c[tp.id]);
					display.drawLine(px[tp.id], py[tp.id], x, y);
					// remember this point for next time.
					px[tp.id] = x; py[tp.id] = y;
				}
				else if (tp.event == Touch::Event::LiftUp)
				{
					// invalidate points
					px[tp.id] = -1; py[tp.id] = -1;
					// generate new random color
					c[tp.id] = Color(uint16_t(rand()));
				}
			}
			LedRed::reset();
		}

		PT_END();
	}

private:
	Touch::Data touchData;
	Touch touch;
	xpcc::GraphicDisplay& display;
	int16_t px[2], py[2];
	Color c[2];
};

LineDrawer drawer;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::initializeDisplay();
	Board::initializeTouchscreen();

	while(1)
	{
		drawer.update();
	}

	return 0;
}
