/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/debug/logger.hpp>

#include <modm/ui/display/image.hpp>
#include <modm/ui/gui.hpp>
#include <modm/container/container.hpp>

#include "screens/homeview.hpp"
#include "screens/choose_color.hpp"
#include "screens/overview.hpp"


#include <modm/driver/display/sdl_display.hpp>

#include <stdlib.h>

#include <SDL/SDL.h>


// ----------------------------------------------------------------------------
/*
 * Logger
 */

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// ----------------------------------------------------------------------------

modm::gui::AsyncEventList async_events;
modm::gui::inputQueue input_queue;

modm::GraphicDisplay *display;

bool quit = false;

void
initSDL()
{
	SDL_Init( SDL_INIT_EVERYTHING );
	/* Enable Unicode translation */
	SDL_EnableUNICODE( 1 );

	display = new modm::SDLDisplay(320, 240);

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
			MODM_LOG_INFO << "Key pressend: LEFT" << modm::endl;
			break;
		}
		case SDLK_RIGHT:
		{
			MODM_LOG_INFO << "Key pressend: RIGHT" << modm::endl;
			break;
		}
		case SDLK_UP:
		{
			MODM_LOG_INFO << "Key pressend: UP" << modm::endl;
			break;
		}
		case SDLK_DOWN:
		{
			MODM_LOG_INFO << "Key pressend: DOWN" << modm::endl;
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
				auto ev_down = new modm::gui::InputEvent(modm::glcd::Point(event.button.x, event.button.y),
													 	 modm::gui::InputEvent::Type::TOUCH,
													 	 modm::gui::InputEvent::Direction::DOWN);

				// queue down event
				input_queue.push(ev_down);

				break;
			}

			case SDL_MOUSEBUTTONUP:
			{

				auto ev_up = new modm::gui::InputEvent(modm::glcd::Point(event.button.x, event.button.y),
													   modm::gui::InputEvent::Type::TOUCH,
													   modm::gui::InputEvent::Direction::UP);

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
test_callback(const modm::gui::InputEvent& ev, modm::gui::Widget* w, void* data)
{
	// avoid warnings
	(void) ev;
	(void) w;
	(void) data;

	MODM_LOG_DEBUG << "Hello from callback" << modm::endl;
}


//modm::gui::ColorPalette colorpalette[modm::gui::Color::PALETTE_SIZE] = {
//	modm::glcd::Color::black(),
//	modm::glcd::Color::white(),
//	modm::glcd::Color::gray(),
//	modm::glcd::Color::red(),
//	modm::glcd::Color::green(),
//	modm::glcd::Color::blue(),
//	modm::glcd::Color::yellow(),
//	modm::glcd::Color::blue(),		// BORDER
//	modm::glcd::Color::yellow(),	// TEXT
//	modm::glcd::Color::black(),		// BACKGROUND
//	modm::glcd::Color::red(),		// ACTIVATED
//	modm::glcd::Color::blue(),		// DEACTIVATED
//};


// ----------------------------------------------------------------------------
int
main(int, char**)
{
	MODM_LOG_DEBUG << "Hello from modm gui example!" << modm::endl;

	initSDL();

	/*
	 * manipulate the color palette
	 */

//	colorpalette[modm::gui::Color::TEXT] = modm::glcd::Color::yellow();


	/*
	 * Create a view and some widgets
	 */

	modm::gui::GuiViewStack myViewStack(display, &input_queue);

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
