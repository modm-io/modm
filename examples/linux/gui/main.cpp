
#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc/ui/display/image.hpp>
#include <xpcc/ui/gui.hpp>

#include "sdl_display.hpp"

#include <SDL/SDL.h>

//#include "images/bluetooth_12x16.hpp"

// ----------------------------------------------------------------------------
/*
 * Logger
 */

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

// ----------------------------------------------------------------------------

xpcc::gui::inputQueue input_queue;
xpcc::glcd::Point last_point;

xpcc::GraphicDisplay *display;

bool quit = false;

void
initSDL()
{
	SDL_Init( SDL_INIT_EVERYTHING );
    /* Enable Unicode translation */
    SDL_EnableUNICODE( 1 );

	display = new xpcc::SDLDisplay(320, 240);

	display->clear();

	display->drawPixel(10,10);

}

void
quitSDL()
{
	SDL_Quit();
	delete display;
}

void
handleKeyDownEvent(SDL_Event& event)
{
	switch( event.key.keysym.sym)
	{
		case SDLK_LEFT:
		{
			XPCC_LOG_INFO << "Key pressend: LEFT" << xpcc::endl;
			break;
		}
		case SDLK_RIGHT:
		{
			XPCC_LOG_INFO << "Key pressend: RIGHT" << xpcc::endl;
			break;
		}
		case SDLK_UP:
		{
			XPCC_LOG_INFO << "Key pressend: UP" << xpcc::endl;
			break;
		}
		case SDLK_DOWN:
		{
			XPCC_LOG_INFO << "Key pressend: DOWN" << xpcc::endl;
			break;
		}
		/*
		case SDLK_RETURN:
		{
			middleKeyWasPressed = true;
			break;
		}
		case SDLK_r:
		{
			teamRedKeyWasPressed = true;
			break;
		}
		case SDLK_b:
		{
			teamBlueKeyWasPressed = true;
			break;
		}
		case SDLK_s:
		{
			starterWasPressed = true;
			break;
		}
		case SDLK_1:
		{
			otherKey1WasPressed = true;
			break;
		}
		case SDLK_2:
		{
			otherKey2WasPressed = true;
			break;
		}
		*/
		default:
		{
			break;
		}
	}
}

void
pollSDL()
{
	SDL_Event event;
	if(SDL_PollEvent( &event ))
	{
		switch(event.type)
		{

			case SDL_MOUSEBUTTONDOWN:
			{
				xpcc::gui::InputEvent in_ev;

				in_ev.type = xpcc::gui::InputEvent::Type::TOUCH;
				//in_ev.direction = InputEvent::Direction::DOWN;

				in_ev.coord = xpcc::glcd::Point(event.button.x, event.button.y);
				//in_ev.x = point.x;
				//in_ev.y = point.y;

				input_queue.push(in_ev);
				break;
			}

			case SDL_QUIT:
			{
				quit = true;
				break;
			}
			case SDL_KEYDOWN:
			{
				handleKeyDownEvent( event );
				break;
			}
			case SDL_KEYUP:
			{
				//handleKeyUpEvent( event );
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

// ----------------------------------------------------------------------------
/* screen calibration  */
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

void
gatherInput()
{

	xpcc::glcd::Point point;
/*
	if (debounceTouch(&point, &last_point)) {
		xpcc::gui::InputEvent in_ev;

		in_ev.type = xpcc::gui::InputEvent::Type::TOUCH;
		//in_ev.direction = InputEvent::Direction::DOWN;

		in_ev.coord = point;
		//in_ev.x = point.x;
		//in_ev.y = point.y;

		input_queue.push(in_ev);



	}
	*/
}

// ----------------------------------------------------------------------------

void
test_callback(const xpcc::gui::InputEvent& ev, xpcc::gui::Widget* w)
{
	// avoid warnings
	(void) ev;
	(void) w;

	//LedGreen::toggle();

	XPCC_LOG_DEBUG << "Hello from callback" << xpcc::endl;
}


xpcc::gui::ColorPalette colorpalette[xpcc::gui::Color::PALETTE_SIZE] = {
	xpcc::glcd::Color::black(),
	xpcc::glcd::Color::white(),
	xpcc::glcd::Color::gray(),
	xpcc::glcd::Color::red(),
	xpcc::glcd::Color::green(),
	xpcc::glcd::Color::blue(),
	xpcc::glcd::Color::blue(),		// BORDER
	xpcc::glcd::Color::red(),		// TEXT
	xpcc::glcd::Color::black(),		// BACKGROUND
	xpcc::glcd::Color::red(),		// ACTIVATED
	xpcc::glcd::Color::blue(),		// DEACTIVATED

};

/*
 * empirically found calibration points
 */
xpcc::glcd::Point calibration[] = {{3339, 3046},{931, 2428},{2740, 982}};

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{

	XPCC_LOG_DEBUG << "Hello from xpcc gui example!" << xpcc::endl;


	initSDL();

	/*
	 * manipulate the color palette
	 */

	colorpalette[xpcc::gui::Color::TEXT] = xpcc::glcd::Color::yellow();


	/*
	 * Create a view and some widgets
	 */

	xpcc::gui::View myView(display, colorpalette, &input_queue);

	xpcc::gui::ButtonWidget toggleLedButton((char*)"Toggle Green", xpcc::gui::Dimension(100, 50));
	xpcc::gui::ButtonWidget doNothingButton((char*)"Do nothing", xpcc::gui::Dimension(100, 50));


	/*
	 * connect callbacks to widgets
	 */

	toggleLedButton.cb_activate = &test_callback;


	/*
	 * place widgets in view
	 */

	myView.pack(&toggleLedButton, xpcc::glcd::Point(110, 40));
	myView.pack(&doNothingButton, xpcc::glcd::Point(110, 140));


	//display->setBackgroundColor(xpcc::glcd::Color::green());
	//display->setColor(xpcc::glcd::Color::blue());

	/*
	 * main loop
	 */
	while(true) {

		pollSDL();

		if(quit)
			break;

		myView.run();

		display->update();

		/*
		 * display an arbitrary image
		 */
		//tft.drawImage(xpcc::glcd::Point(304, 3),xpcc::accessor::asFlash(bitmap::bluetooth_12x16));

	}

	quitSDL();

	return 0;
}

