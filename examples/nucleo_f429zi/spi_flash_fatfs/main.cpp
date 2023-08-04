/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <fatfs/ff.h>
#include <fatfs/diskio.h>

#include <modm/board.hpp>
#include <modm/platform.hpp>
#include <modm/processing.hpp>

using namespace Board;
using SpiMaster = SpiMaster1;

// Spi flash chip wiring:
using Cs = GpioA4;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
// Connect WP and HOLD pins to +3V3
// and of course Vdd to +3V3 and Vss to GND

/* The example is adapted from http://elm-chan.org/fsw/ff/res/app4.c */

DWORD buff[FF_MAX_SS];              /* Working buffer (4 sector in size) */
BYTE pdrv = 0;                      /* Physical drive number to be checked (all data on the drive will be lost) */
UINT ncyc = 3;                      /* Number of test cycles */
UINT sz_buff = sizeof(buff);        /* Size of the working buffer in unit of byte */

static DWORD pn (       /* Pseudo random number generator */
    DWORD pns   /* 0:Initialize, !0:Read */
)
{
    static DWORD lfsr;
    UINT n;


    if (pns) {
        lfsr = pns;
        for (n = 0; n < 32; n++) pn(0);
    }
    if (lfsr & 1) {
        lfsr >>= 1;
        lfsr ^= 0x80200003;
    } else {
        lfsr >>= 1;
    }
    return lfsr;
}

int test_diskio()
{
    UINT n, cc, ns;
    DWORD sz_drv, lba, lba2, sz_eblk, pns = 1;
    WORD sz_sect;
    BYTE *pbuff = (BYTE*)buff;
    DSTATUS ds;
    DRESULT dr;


    MODM_LOG_INFO.printf("test_diskio(%u, %u, 0x%08X, 0x%08X)\n", pdrv, ncyc, (UINT)buff, sz_buff);

    if (sz_buff < FF_MAX_SS + 8) {
        MODM_LOG_INFO.printf("Insufficient work area to run the program.\n");
        return 1;
    }

    for (cc = 1; cc <= ncyc; cc++) {
        MODM_LOG_INFO.printf("**** Test cycle %u of %u start ****\n", cc, ncyc);

        MODM_LOG_INFO.printf(" disk_initalize(%u)", pdrv);
        ds = disk_initialize(pdrv);
        if (ds & STA_NOINIT) {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 2;
        } else {
            MODM_LOG_INFO.printf(" - ok.\n");
        }

        MODM_LOG_INFO.printf("**** Get drive size ****\n");
        MODM_LOG_INFO.printf(" disk_ioctl(%u, GET_SECTOR_COUNT, 0x%08X)", pdrv, (UINT)&sz_drv);
        sz_drv = 0;
        dr = disk_ioctl(pdrv, GET_SECTOR_COUNT, &sz_drv);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 3;
        }
        if (sz_drv < 128) {
            MODM_LOG_INFO.printf("Failed: Insufficient drive size to test.\n");
            return 4;
        }
        MODM_LOG_INFO.printf(" Number of sectors on the drive %u is %lu.\n", pdrv, sz_drv);

#if FF_MAX_SS != FF_MIN_SS
        MODM_LOG_INFO.printf("**** Get sector size ****\n");
        MODM_LOG_INFO.printf(" disk_ioctl(%u, GET_SECTOR_SIZE, 0x%X)", pdrv, (UINT)&sz_sect);
        sz_sect = 0;
        dr = disk_ioctl(pdrv, GET_SECTOR_SIZE, &sz_sect);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 5;
        }
        MODM_LOG_INFO.printf(" Size of sector is %u bytes.\n", sz_sect);
#else
        sz_sect = FF_MAX_SS;
#endif

        MODM_LOG_INFO.printf("**** Get block size ****\n");
        MODM_LOG_INFO.printf(" disk_ioctl(%u, GET_BLOCK_SIZE, 0x%X)", pdrv, (UINT)&sz_eblk);
        sz_eblk = 0;
        dr = disk_ioctl(pdrv, GET_BLOCK_SIZE, &sz_eblk);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
        }
        if (dr == RES_OK || sz_eblk >= 2) {
            MODM_LOG_INFO.printf(" Size of the erase block is %lu sectors.\n", sz_eblk);
        } else {
            MODM_LOG_INFO.printf(" Size of the erase block is unknown.\n");
        }

        /* Single sector write test */
        MODM_LOG_INFO.printf("**** Single sector write test ****\n");
        lba = 0;
        for (n = 0, pn(pns); n < sz_sect; n++) pbuff[n] = (BYTE)pn(0);
        MODM_LOG_INFO.printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
        dr = disk_write(pdrv, pbuff, lba, 1);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 6;
        }
        MODM_LOG_INFO.printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 7;
        }
        memset(pbuff, 0, sz_sect);
        MODM_LOG_INFO.printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
        dr = disk_read(pdrv, pbuff, lba, 1);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 8;
        }
        for (n = 0, pn(pns); n < sz_sect && pbuff[n] == (BYTE)pn(0); n++) ;
        if (n == sz_sect) {
            MODM_LOG_INFO.printf(" Read data matched.\n");
        } else {
            MODM_LOG_INFO.printf(" Read data differs from the data written.\n");
            return 10;
        }
        pns++;

        MODM_LOG_INFO.printf("**** Multiple sector write test ****\n");
        lba = 5; ns = sz_buff / sz_sect;
        if (ns > 4) ns = 4;
        if (ns > 1) {
            for (n = 0, pn(pns); n < (UINT)(sz_sect * ns); n++) pbuff[n] = (BYTE)pn(0);
            MODM_LOG_INFO.printf(" disk_write(%u, 0x%X, %lu, %u)", pdrv, (UINT)pbuff, lba, ns);
            dr = disk_write(pdrv, pbuff, lba, ns);
            if (dr == RES_OK) {
                MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 11;
            }
            MODM_LOG_INFO.printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
            dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
            if (dr == RES_OK) {
                MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 12;
            }
            memset(pbuff, 0, sz_sect * ns);
            MODM_LOG_INFO.printf(" disk_read(%u, 0x%X, %lu, %u)", pdrv, (UINT)pbuff, lba, ns);
            dr = disk_read(pdrv, pbuff, lba, ns);
            if (dr == RES_OK) {
                MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 13;
            }
            for (n = 0, pn(pns); n < (UINT)(sz_sect * ns) && pbuff[n] == (BYTE)pn(0); n++) ;
            if (n == (UINT)(sz_sect * ns)) {
                MODM_LOG_INFO.printf(" Read data matched.\n");
            } else {
                MODM_LOG_INFO.printf(" Read data differs from the data written.\n");
                return 14;
            }
        } else {
            MODM_LOG_INFO.printf(" Test skipped.\n");
        }
        pns++;

        MODM_LOG_INFO.printf("**** Single sector write test (unaligned buffer address) ****\n");
        lba = 5;
        for (n = 0, pn(pns); n < sz_sect; n++) pbuff[n+3] = (BYTE)pn(0);
        MODM_LOG_INFO.printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+3), lba);
        dr = disk_write(pdrv, pbuff+3, lba, 1);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 15;
        }
        MODM_LOG_INFO.printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 16;
        }
        memset(pbuff+5, 0, sz_sect);
        MODM_LOG_INFO.printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+5), lba);
        dr = disk_read(pdrv, pbuff+5, lba, 1);
        if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
        } else {
            MODM_LOG_INFO.printf(" - failed.\n");
            return 17;
        }
        for (n = 0, pn(pns); n < sz_sect && pbuff[n+5] == (BYTE)pn(0); n++) ;
        if (n == sz_sect) {
            MODM_LOG_INFO.printf(" Read data matched.\n");
        } else {
            MODM_LOG_INFO.printf(" Read data differs from the data written.\n");
            return 18;
        }
        pns++;

        MODM_LOG_INFO.printf("**** 4GB barrier test ****\n");
        if (sz_drv >= 128 + 0x80000000 / (sz_sect / 2)) {
            lba = 6; lba2 = lba + 0x80000000 / (sz_sect / 2);
            for (n = 0, pn(pns); n < (UINT)(sz_sect * 2); n++) pbuff[n] = (BYTE)pn(0);
            MODM_LOG_INFO.printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
            dr = disk_write(pdrv, pbuff, lba, 1);
            if (dr == RES_OK) {
                MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 19;
            }
            MODM_LOG_INFO.printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+sz_sect), lba2);
            dr = disk_write(pdrv, pbuff+sz_sect, lba2, 1);
            if (dr == RES_OK) {
                MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 20;
            }
            MODM_LOG_INFO.printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
            dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
            if (dr == RES_OK) {
            MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 21;
            }
            memset(pbuff, 0, sz_sect * 2);
            MODM_LOG_INFO.printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
            dr = disk_read(pdrv, pbuff, lba, 1);
            if (dr == RES_OK) {
                MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 22;
            }
            MODM_LOG_INFO.printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+sz_sect), lba2);
            dr = disk_read(pdrv, pbuff+sz_sect, lba2, 1);
            if (dr == RES_OK) {
                MODM_LOG_INFO.printf(" - ok.\n");
            } else {
                MODM_LOG_INFO.printf(" - failed.\n");
                return 23;
            }
            for (n = 0, pn(pns); pbuff[n] == (BYTE)pn(0) && n < (UINT)(sz_sect * 2); n++) ;
            if (n == (UINT)(sz_sect * 2)) {
                MODM_LOG_INFO.printf(" Read data matched.\n");
            } else {
                MODM_LOG_INFO.printf(" Read data differs from the data written.\n");
                return 24;
            }
        } else {
            MODM_LOG_INFO.printf(" Test skipped.\n");
        }
        pns++;

        MODM_LOG_INFO.printf("**** Test cycle %u of %u completed ****\n\n", cc, ncyc);
    }

    return 0;
}

modm_faststack modm::Fiber<> testFiber(test_diskio);

modm_faststack modm::Fiber<> blinkyFiber([]()
{
    Board::Leds::setOutput();
    while(true)
    {
        Board::Leds::toggle();
        modm::fiber::sleep(200ms);
    }
});

int
main()
{
    // initialize board and SPI
    Board::initialize();
    SpiMaster::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
    SpiMaster::initialize<Board::SystemClock, 11_MHz>();

    /* Check function/compatibility of the physical drive #0 */
    modm::fiber::Scheduler::run();

    while (true);

    return 0;
}
