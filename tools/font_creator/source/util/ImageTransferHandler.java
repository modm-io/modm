/*
 * Copyright (c) 2010, Fabian Greif
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
 * Created on 28.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import gui.CharEditor;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.image.BufferedImage;
import java.io.IOException;

import javax.swing.JComponent;
import javax.swing.TransferHandler;

/**
 * @author Fabian Maximilian Thiele
 */
public class ImageTransferHandler extends TransferHandler {
	private static final long serialVersionUID = 9004125321158246818L;
	private DataFlavor imageFlavor = DataFlavor.imageFlavor;
	private CharEditor sourceEditor;

	public boolean importData(JComponent c, Transferable t) {
		BufferedImage image;
		if (canImport(c, t.getTransferDataFlavors())) {
			CharEditor ce = (CharEditor) c;
			// Don't drop on myself
			if (sourceEditor == ce) {
				return true;
			}
			try {
				image = (BufferedImage) t.getTransferData(imageFlavor);
				ce.pasteImage(image);
				System.out.println("data imported");
				return true;
			} catch (UnsupportedFlavorException e) {
				System.out.println("importData: unsupported data flavor");
			} catch (IOException e) {
				System.out.println("importData: I/O Exception");
			}
		}
		return false;
	}

	public int getSourceActions(JComponent c) {
		return COPY;
	}

	protected void exportDone(JComponent c, Transferable data, int action) {
		sourceEditor = null;
	}

	public boolean canImport(JComponent c, DataFlavor[] flavors) {
		for (int i = 0; i < flavors.length; i++) {
			// System.out.println(flavors[i]);
			if (imageFlavor.equals(flavors[i])) {
				System.out.println("can import data");
				return true;
			}
		}
		System.out.println("can't import data");
		return false;
	}
}
