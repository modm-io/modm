// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "protothread/nested_protothread.hpp"
#include "protothread/semaphore.hpp"
