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
 * \ingroup		communication
 * \defgroup 	tipc	(TIPC) Transparent Inter-Process Communication
 * \brief 		Interface to the Linux-TIPC.
 *
 * TIPC is a network transparent, packet orientated communication protocol that
 * is part of the linux kernel. In the xpcc it is used in broadcast mode with
 * filter on incoming messages. So it has the same structure as the CAN
 * communication. Compared to TCP/IP this does not need a master in the system
 * and because of the given filter functionality, it produce less traffic and a
 * faster transport.
 *
 * \section	tipc_project	The TIPC project
 *
 * (from the TIPC web page: http://tipc.sourceforge.net/)
 *
 * The Transparent Inter-Process Communication protocol allows applications in a
 * clustered computer environment to communicate quickly and reliably with other
 * applications, regardless of their location within the cluster. TIPC
 * originated at the telecommunications manufacturer, Ericsson, and has been
 * deployed in their products for years.
 *
 * The TIPC project is an open source implementation of TIPC, and its software
 * is now being used in commercial, educational, and private products around the
 * world. TIPC applications are typically written in C (or C++) and utilize
 * sockets of the AF_TIPC address family. Support for Perl, Python, Ruby, and D
 * is also available.
 *
 * TIPC continues to evolve to meet the needs of its users. Users and developers
 * who are interested in contributing enhancements or bug fixes to TIPC can
 * obtain further information by contacting the TIPC project team on its mailing
 * list.
 *
 * \section tipc_setup	Setup your System to Use TIPC
 *
 * Since kernel 2.6.16 TIPC is part of the linux kernel. Please check if TIPC is
 * shipped with your distribution or if you have to compile your kernel.
 *
 * For local communication you need only to activate TIPC with
 * \code
 * # modprobe tipc
 * \endcode
 *
 * For communication with other PCs in a network you need to configure the TIPC
 * with the tool "tipc-config" or "tipcutils". It should be provided with the most
 * distributions or see the TIPC web page.
 *
 * Here is a example configuration:
 * \code
 * # "true" or "false" to en-/disable TIPC
 * TIPC_CONFIGURED=true
 *
 * # the local tipc address (each PC in the network need its unique ADDR
 * TIPC_ADDR=1.1.1
 *
 * # the network id used by tipc (all members of the communication need the same NETID)
 * TIPC_NETID=1234
 *
 * # interfaces to enable for TIPC
 * # the elements of this list are separated by spaces
 * TIPC_INTERFACES="eth0"
 * \endcode
 *
 * \section tipc_domain_id Software Domain ID
 * To be able to use multiple tipc connectors on one machine (e.g. simulating two robots),
 * the xpcc::tipc module has a software domain. All members of the domain receive the transmitted
 * messages. The implementation is on software level in this module. Actually every tipc-node receive
 * all messages with the same type id and than filters.
 *
 * Per default all messages are send and received on the DOMAIN_ID_UNDEFINED domain. Via the methods
 * setDomainId() it is possible to specify the domain id during runtime. Transmitter and receiver need
 * to be members of the same domain id.
 */

#include "connector.hpp"
