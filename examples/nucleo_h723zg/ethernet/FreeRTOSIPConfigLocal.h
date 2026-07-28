/*
 * FreeRTOS+TCP V2.2.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_IP_CONFIG_H
#error "Don't include this file, use 'FreeRTOSIPConfig.h' instead!"
#endif

#define ipconfigHAS_DEBUG_PRINTF 0
#define ipconfigHAS_PRINTF       0

// Keep checksum handling in software for the first H7 ethernet backend.
#define ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM 0
#define ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM 0

#define ipconfigSOCK_DEFAULT_RECEIVE_BLOCK_TIME (5000)
#define ipconfigSOCK_DEFAULT_SEND_BLOCK_TIME    (5000)

#define ipconfigUSE_LLMNR (0)
#define ipconfigUSE_NBNS  (0)

#define ipconfigUSE_DNS_CACHE         (1)
#define ipconfigDNS_CACHE_NAME_LENGTH (16)
#define ipconfigDNS_CACHE_ENTRIES     (4)
#define ipconfigDNS_REQUEST_ATTEMPTS  (2)

extern UBaseType_t
uxRand(void);
#define ipconfigRAND32() uxRand()

#define ipconfigUSE_NETWORK_EVENT_HOOK        1
#define ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS (5000 / portTICK_PERIOD_MS)

#define ipconfigUSE_DHCP                   0
#define ipconfigMAXIMUM_DISCOVER_TX_PERIOD (120000 / portTICK_PERIOD_MS)

#define ipconfigARP_CACHE_ENTRIES       6
#define ipconfigMAX_ARP_RETRANSMISSIONS (5)
#define ipconfigMAX_ARP_AGE             150

#define ipconfigINCLUDE_FULL_INET_ADDR 1

#define ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS 60
#define ipconfigEVENT_QUEUE_LENGTH             (ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS + 5)

#define ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND 1

#define ipconfigUDP_TIME_TO_LIVE 128
#define ipconfigTCP_TIME_TO_LIVE 128

#define ipconfigUSE_TCP     (1)
#define ipconfigUSE_TCP_WIN (1)
#define ipconfigNETWORK_MTU 1500
#define ipconfigUSE_DNS     0

#define ipconfigREPLY_TO_INCOMING_PINGS             1
#define ipconfigSUPPORT_OUTGOING_PINGS              0
#define ipconfigSUPPORT_SELECT_FUNCTION             1
#define ipconfigFILTER_OUT_NON_ETHERNET_II_FRAMES   1
#define ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES 1

#define configWINDOWS_MAC_INTERRUPT_SIMULATOR_DELAY (20 / portTICK_PERIOD_MS)

#define ipconfigPACKET_FILLER_SIZE 2

#define ipconfigTCP_WIN_SEG_COUNT    240
#define ipconfigTCP_RX_BUFFER_LENGTH (2000)
#define ipconfigTCP_TX_BUFFER_LENGTH (2000)

#define ipconfigIS_VALID_PROG_ADDRESS(x) ((x) != NULL)

#define ipconfigTCP_HANG_PROTECTION      (1)
#define ipconfigTCP_HANG_PROTECTION_TIME (30)

#define ipconfigTCP_KEEP_ALIVE          (1)
#define ipconfigTCP_KEEP_ALIVE_INTERVAL (20)

#define ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES 1
#define ipconfigZERO_COPY_RX_DRIVER                 0
#define ipconfigZERO_COPY_TX_DRIVER                 0
#define ipconfigUSE_LINKED_RX_MESSAGES              1
