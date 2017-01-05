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
 * Version 2.0 created on 05.12.2003 by Fabian Maximilian Thiele
 * 
 * Version 2.1 beta created on 19.12.2007 by sce
 * changes:
 * - added font name edit function
 * - fixed wrong font size calculation
 * - fixed blank character
 * 
 * Version 2.2 (modm) created on 23.9.2010 by Fabian Greif
 */

import javax.swing.JFrame;

import gui.MainWindow;

public class FontCreator extends JFrame {
	private static final long serialVersionUID = 8358706032712418201L;

	public static void main(String args[]) {
		new MainWindow();
	}
}