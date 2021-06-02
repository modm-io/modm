/*
 * Copyright (c) 2020, Matthew Arnold
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IABSTRACT_VIEW_HPP
#define MODM_IABSTRACT_VIEW_HPP

#include <modm/ui/display/graphic_display.hpp>

#include "menu_buttons.hpp"

namespace modm
{
	// forward declaration
	template<typename T>
	class ViewStack;

	class IAbstractView
	{
	public:
		template<typename T>
		friend class ViewStack;

		/**
		 * @param identifier can be used to determine which screen is the currently
		 *        displayed on the graphicDisplay
		 */
		IAbstractView(uint8_t identifier) :
			identifier(identifier), alive(true)
		{
		}

		virtual ~IAbstractView() = default;

		/**
		 * @brief update The update function of the top most display gets called
		 *        as often as possible. Only the update of the top view in each
		 *        ViewStack gets called.
		 */
		virtual void
		update()
		{
			// nothing to be done
		}

		/**
		 * @brief hasChanged indicates the current displayed view has changed.
		 *        This function prevents unnecessary drawing of the display
		 * @return if true the display has to be redrawn.
		 */
		virtual bool
		hasChanged() = 0;

		/**
		 * @brief draw determine the output on the Graphic Display
		 */
		virtual void
		draw() = 0;


		/**
		 * @brief shortButtonPress handle the action for the pressed button
		 */
		virtual void
		shortButtonPress(modm::MenuButtons::Button /*button*/)
		{
			// nothing to be done
		}

		/**
		 * @brief isAlive tells the ViewStack if it should remove this screen.
		 * @return
		 */
		bool
		isAlive() const
		{
			return this->alive;
		}

		/**
		 * @brief remove the view from the screen. The viewStack handles the deletion.
		 */
		void
		remove()
		{
			this->alive = false;
		}

		/**
		 * @brief getIdentifier of the view.
		 */
		inline uint8_t getIdentifier(){
			return this->identifier;
		}

	public:

		/**
		 * @brief onRemove will be called right before the view gets deleted,
		 *        can be reimplemented to reset external data.
		 */
		virtual void
		onRemove()
		{
			// nothing to be done here
		}

	private:
		const uint8_t identifier;
		bool alive;
	};
}

#endif // MODM_IABSTRACT_VIEW_HPP
