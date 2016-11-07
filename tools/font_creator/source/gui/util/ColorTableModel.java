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
 * Created on 18.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package gui.util;

import javax.swing.table.DefaultTableModel;

/**
 * @author Fabian Maximilian Thiele
 */
public class ColorTableModel extends DefaultTableModel {
	private static final long serialVersionUID = 4552819053605168825L;

	public ColorTableModel(int rowCnt, int columnCnt) {
		super(rowCnt, columnCnt);
	}

	public boolean isCellEditable(int row, int column) {
		return false;
	}
}
