/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * @ingroup		processing
 * @defgroup	resumable		Resumables
 *
 * An implementation of lightweight resumable functions which allow for nested calling.
 *
 * This base class and its macros allows you to implement and use several
 * resumable functions in one class.
 * This allows you to modularize your code by placing it into its own resumable functions
 * instead of the placing everything into one big method.
 * It also allows you to call and run resumable functions within your resumables,
 * so you can reuse their functionality.
 *
 * Note that you should call resumable functions within a protothreads.
 * It is sufficient to use the `this` pointer of the class as context
 * when calling the resumables.
 * So calling a resumable function is done using `PT_CALL(resumable(this))`
 * which will return the result of the resumable function.
 *
 * You may use the `RF_CALL_BLOCKING(resumable(ctx))` macro to execute
 * a resumable function outside of a protothread, however, this which will
 * force the CPU to busy-wait until the resumable function ended.
 *
 * Here is a (slightly over-engineered) example:
 *
 * @code
 * #include <modm/architecture/architecture.hpp>
 * #include <modm/processing/processing.hpp>
 *
 * typedef GpioOutputB0 Led;
 *
 * class BlinkingLight : public modm::pt::Protothread, private modm::NestedResumable<2>
 * {
 * public:
 *     bool
 *     run()
 *     {
 *         PT_BEGIN();
 *
 *         // set everything up
 *         Led::setOutput();
 *         Led::set();
 *
 *         while (true)
 *         {
 *             Led::set();
 *             PT_CALL(waitForTimer()))
 *
 *             Led::reset();
 *             PT_CALL(setTimer(200));
 *
 *             PT_WAIT_UNTIL(timeout.isExpired());
 *         }
 *
 *         PT_END();
 *     }
 *
 *     modm::ResumableResult<bool>
 *     waitForTimer()
 *     {
 *         RF_BEGIN();
 *
 *         // nested calling is allowed
 *         if (RF_CALL(setTimer(100)))
 *         {
 *             RF_WAIT_UNTIL(timeout.isExpired());
 *             RF_RETURN(true);
 *         }
 *
 *         RF_END_RETURN(false);
 *     }
 *
 *     modm::ResumableResult<bool>
 *     setTimer(uint16_t timeout)
 *     {
 *         RF_BEGIN();
 *
 *         timeout.restart(timeout);
 *
 *         if(timeout.isRunning())
 *             RF_RETURN(true);
 *
 *         // clean up code goes here
 *
 *         RF_END_RETURN(false);
 *     }
 *
 * private:
 *     modm::ShortTimeout timeout;
 * };
 *
 *
 * ...
 * BlinkingLight light;
 *
 * while (...) {
 *     light.run();
 * }
 * @endcode
 *
 * For other examples take a look in the `examples` folder in the MODM
 * root folder.
 */

#include "resumable/resumable.hpp"
#include "resumable/nested_resumable.hpp"
