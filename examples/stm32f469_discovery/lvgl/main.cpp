/*
 * Copyright (c) 2020, Raphael Lehmann
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

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

static modm::Abandonment
test_assertion_handler(const modm::AssertionInfo &info)
{
	MODM_LOG_ERROR << "AssertionInfo.name: " << info.name << "\n";
	MODM_LOG_ERROR << "AssertionInfo.context: " << info.context << "\n";
	MODM_LOG_ERROR << "AssertionInfo.behavior: " << info.behavior << "\n";
	MODM_LOG_ERROR << modm::endl;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler);

#include <lvgl/lvgl.h>

using namespace Board;
using namespace modm::glcd;
using namespace Board::ft6;

static lv_disp_buf_t disp_buf;
static lv_color_t* buf;  // Declare a buffer full screen
static constexpr size_t buf_size = LV_HOR_RES_MAX * LV_VER_RES_MAX;
/*
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    // Save the state and save the pressed coordinate
    data->state = touchpad_is_pressed() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);

    // Set the coordinates (if released use the last pressed coordinates)
    data->point.x = last_x;
    data->point.y = last_y;

    return false; //Return `false` because we are not buffering and no more data to read
}
*/

void my_disp_flush(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    /*
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //uint16_t color = color_p->full;
            display.setPixel(x, y, color_p->full);  // Put a pixel to the display.
            color_p++;
        }
    }

    lv_disp_flush_ready(disp);  // Indicate you are ready with the flushing
    */
}


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::initializeDisplay();
	Board::initializeTouchscreen();

    MODM_LOG_INFO << "modm LittlevGL example on STM32F469-Discovery board!\n\n";

	if (FaultReporter::hasReport())
	{
		MODM_LOG_ERROR << "\n\nHardFault! Copy the data into a 'coredump.txt' file, ";
		MODM_LOG_ERROR << "then execute\n\n\tscons postmortem firmware=" << modm::hex;
		for (const auto data : FaultReporter::buildId()) MODM_LOG_ERROR << data;
		MODM_LOG_ERROR << "\n\n";
		for (const auto data : FaultReporter())
			MODM_LOG_ERROR << modm::hex << data << modm::flush;
		MODM_LOG_ERROR << "\n\n\n" << modm::flush;
		FaultReporter::clear();
        while (1) ;
	}

	Touch::Data touchData;
	Touch touch{touchData};

    lv_init();

    buf = new(modm::MemoryExternal) lv_color_t[buf_size];

    setDisplayBuffer(buf);

	lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);  // Initialize the display buffer

    /*
	lv_indev_drv_init(&indev_drv);             // Descriptor of a input device driver
	indev_drv.type = LV_INDEV_TYPE_POINTER;    // Touch pad is a pointer-like device
	indev_drv.read_cb = my_touchpad_read;      // Set your driver function
	lv_indev_drv_register(&indev_drv);         // Finally register the driver
    */

    lv_obj_t* scr = lv_disp_get_scr_act(NULL);     /*Get the current screen*/

    if(scr == nullptr) {
        MODM_LOG_INFO << "Unable to get current screen. nullptr.\n";
    }

    /*Create a Label on the currently active screen*/
    lv_obj_t* label1 =  lv_label_create(scr, NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
    * NULL means align on parent (which is the screen now)
    * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);

	while (true)
	{
        MODM_LOG_INFO << "Calling lv_task_handler() inside while(1) loop.\n";
        lv_tick_inc(100);
		lv_task_handler();
		modm::delayMilliseconds(1000);
	}

	return 0;
}
