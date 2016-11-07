/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
 * Created on 29.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import java.awt.Color;
import java.awt.image.IndexColorModel;

/**
 * @author Fabian Maximilian Thiele
 */
public class IndexColorModelUtils {
	public static IndexColorModel getColorModel(int bits)
			throws IllegalArgumentException {
		IndexColorModel cm = null;

		if (bits == 1) {
			byte[] r = { (byte) 255, (byte) 0 };
			byte[] g = { (byte) 255, (byte) 0 };
			byte[] b = { (byte) 255, (byte) 0 };
			cm = new IndexColorModel(bits, 2, r, g, b);
		}
		else if (bits == 2) {
			byte[] r = { (byte) 255, (byte) 0, (byte) 255, (byte) 0 };
			byte[] g = { (byte) 255, (byte) 0, (byte) 0, (byte) 0 };
			byte[] b = { (byte) 255, (byte) 0, (byte) 0, (byte) 255 };
			cm = new IndexColorModel(bits, 4, r, g, b);
		}
		else if (bits == 3) {
			byte[] r = { (byte) 255, (byte) 0, (byte) 255, (byte) 0, (byte) 0,
					(byte) 255, (byte) 255, (byte) 0 };
			byte[] g = { (byte) 255, (byte) 0, (byte) 0, (byte) 0, (byte) 255,
					(byte) 255, (byte) 0, (byte) 255 };
			byte[] b = { (byte) 255, (byte) 0, (byte) 0, (byte) 255, (byte) 0,
					(byte) 0, (byte) 255, (byte) 255 };
			cm = new IndexColorModel(bits, 8, r, g, b);
		}
		else if (bits == 8) {
			byte[] r = new byte[256];
			byte[] g = new byte[256];
			byte[] b = new byte[256];

			int[] red = { 0, 51, 85, 119, 153, 187, 221, 255 };
			int[] green = { 0, 51, 85, 119, 153, 187, 221, 255 };
			int[] blue = { 0, 119, 187, 255 };

			for (int i = 0; i < 256; i++) {
				r[i] = (byte) red[(i & 0xE0) >> 5];
				g[i] = (byte) green[(i & 0x1C) >> 2];
				b[i] = (byte) blue[i & 0x03];
			}
			cm = new IndexColorModel(bits, 256, r, g, b);
		}
		else {
			throw new IllegalArgumentException(
					"Only 2, 4, 8, or 256 Colors are supported");
		}

		return cm;
	}

	public static Color getColorForIndex(IndexColorModel model, int index) {
		int red = model.getRed(index);
		int green = model.getGreen(index);
		int blue = model.getBlue(index);

		return new Color(red, green, blue);
	}
}
