/*
 * Copyright (c) 2009-2010, 2014, Martin Rosekeit
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
 * \ingroup		backend
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

#include "tipc/connector.hpp"
