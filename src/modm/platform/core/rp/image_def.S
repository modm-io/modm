/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// The RP2350 bootrom only boots an image from flash, if it finds a valid
// IMAGE_DEF block in the first 4kiB. This minimal block marks the image as a
// Secure ARM executable for the RP2350. Without a VECTOR_TABLE item the bootrom
// uses the vector table at the start of the image (0x10000000).
// See RP2350 datasheet section 5.9.5 (Minimum viable image metadata) and
// pico-sdk src/rp2_common/pico_crt0/embedded_start_block.inc.S.

	.section .image_def, "a"
	.p2align 2
	.global __image_def
__image_def:
	.word 0xffffded3	// PICOBIN_BLOCK_MARKER_START
	.byte 0x42			// PICOBIN_BLOCK_ITEM_1BS_IMAGE_TYPE
	.byte 0x01			// item size: 1 word
	.hword 0x1021		// EXE | SECURITY_S | CPU_ARM | CHIP_RP2350
	.byte 0xff			// PICOBIN_BLOCK_ITEM_2BS_LAST
	.hword 0x0001		// size of all items: 1 word
	.byte 0x00			// pad
	.word 0x00000000	// relative link to the next block: this block loops to itself
	.word 0xab123579	// PICOBIN_BLOCK_MARKER_END
