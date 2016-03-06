
#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc/ui/display/image.hpp>
#include <xpcc/ui/gui.hpp>
#include <xpcc/container.hpp>

#include "screens/homeview.hpp"
#include "screens/choose_color.hpp"
#include "screens/overview.hpp"


#include "sdl_display.hpp"

#include <stdlib.h>

#include <SDL/SDL.h>


// ----------------------------------------------------------------------------
/*
 * Logger
 */

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

// ----------------------------------------------------------------------------

xpcc::gui::AsyncEventList async_events;
xpcc::gui::inputQueue input_queue;

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
		}for(auto iter = async_events.begin(); iter != async_events.end(); ++iter)
		{timerfd_create()
			if((*iter)->is_expired()) {
				//async_events.erase(iter);
				// delete node
				//iter.node->previous->next = iter.node->next;
				//delete iter.node;
				//Allocator::destroy(iter->node->value);
				//this->nodeAllocator.deallocate(node);
			}
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
				auto ev_down = new xpcc::gui::InputEvent(xpcc::glcd::Point(event.button.x, event.button.y),
													 	 xpcc::gui::InputEvent::Type::TOUCH,
													 	 xpcc::gui::InputEvent::Direction::DOWN);

				// queue down event
				input_queue.push(ev_down);

				break;
			}

			case SDL_MOUSEBUTTONUP:
			{

				auto ev_up = new xpcc::gui::InputEvent(xpcc::glcd::Point(event.button.x, event.button.y),
													   xpcc::gui::InputEvent::Type::TOUCH,
													   xpcc::gui::InputEvent::Direction::UP);

				// queue up event
				input_queue.push(ev_up);

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
test_callback(const xpcc::gui::InputEvent& ev, xpcc::gui::Widget* w, void* data)
{
	// avoid warnings
	(void) ev;
	(void) w;
	(void) data;

	XPCC_LOG_DEBUG << "Hello from callback" << xpcc::endl;
}


//xpcc::gui::ColorPalette colorpalette[xpcc::gui::Color::PALETTE_SIZE] = {
//	xpcc::glcd::Color::black(),
//	xpcc::glcd::Color::white(),
//	xpcc::glcd::Color::gray(),
//	xpcc::glcd::Color::red(),
//	xpcc::glcd::Color::green(),
//	xpcc::glcd::Color::blue(),
//	xpcc::glcd::Color::yellow(),
//	xpcc::glcd::Color::blue(),		// BORDER
//	xpcc::glcd::Color::yellow(),	// TEXT
//	xpcc::glcd::Color::black(),		// BACKGROUND
//	xpcc::glcd::Color::red(),		// ACTIVATED
//	xpcc::glcd::Color::blue(),		// DEACTIVATED
//};


// ----------------------------------------------------------------------------
int
main(int, char**)
{
	XPCC_LOG_DEBUG << "Hello from xpcc gui example!" << xpcc::endl;

	initSDL();

	/*
	 * manipulate the color palette
	 */

//	colorpalette[xpcc::gui::Color::TEXT] = xpcc::glcd::Color::yellow();


	/*
	 * Create a view and some widgets
	 */

	xpcc::gui::GuiViewStack myViewStack(display, &input_queue);

	ChooseColorView colorchoose(&myViewStack, 1);

//	Overview overview(&myViewStack, 2);

//	HomeView homeview(&myViewStack, 1);


	myViewStack.push(&colorchoose);
//	myViewStack.push(&overview);
//	myViewStack.push(&homeview);

	/*
	 * main loop
	 */
	while(true) {

		// gather input events
		pollSDL();

		// process asynchronous events
		updateAsyncEvents();

		// update view
//		myView.run();

		myViewStack.update();

		// update display
		display->update();


		if(quit)
			break;
	}

	quitSDL();

	return 0;
}

