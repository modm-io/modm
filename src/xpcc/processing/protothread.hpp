/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2011-2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * \ingroup		processing
 * \defgroup	protothread		Protothreads
 * \brief		Lightweight, stackless threads
 *
 * Protothreads are extremely lightweight stackless threads designed for
 * severely memory constrained systems, such as small embedded systems or
 * wireless sensor network nodes. Protothreads provide linear code execution
 * for event-driven systems implemented in C. Protothreads can be used with or
 * without an underlying operating system to provide blocking event-handlers.
 *
 * Protothreads provide sequential flow of control without complex state
 * machines or full multi-threading.
 *
 * As they implement some kind of cooperative multithreading
 * Protothreads are non-preemptable. Therefore, a context switch can only take
 * place on blocking operations. Therefore you don't need complex
 * synchronisation. Protothreads are also stackless, which means
 * member variables of the xpcc::Protothread subclasses are needed for
 * preserving variables across context switches.
 *
 * A protothread runs within a single function (xpcc::Protothread::run()) and
 * cannot span over other functions. A protothread may call normal functions,
 * but cannot block inside a called function. Blocking inside nested function
 * calls is instead made by spawning a separate protothread for each
 * potentially blocking function.
 *
 * The protothread concept was developed by Adam Dunkels and Oliver Schmidt.
 *
 * Originally ported to C++ for use by Hamilton Jet (www.hamiltonjet.co.nz) by
 * Ben Hoyt, but stripped down for public release. See his blog entry about
 * it for more information: http://blog.micropledge.com/2008/07/protothreads/
 *
 * \see	http://www.sics.se/~adam/pt/
 */

#include "protothread/protothread.hpp"
#include "protothread/semaphore.hpp"
