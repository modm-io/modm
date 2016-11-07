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
 * Created on 17.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package gui.util;

import gui.MainWindow;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.JToolBar;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCreatorTopToolBar extends JToolBar implements ActionListener,
		ChangeListener {
	private static final long serialVersionUID = 7958500990972594981L;

	private MainWindow mainWnd;

	private JSlider zoomSlider;

	public FontCreatorTopToolBar(MainWindow mainWnd) {
		this.mainWnd = mainWnd;

		this.add(new JLabel("Zoom: "));
		zoomSlider = new JSlider(1, 50, 30);
		zoomSlider.setMaximumSize(new Dimension(150, 50));
		zoomSlider.setMajorTickSpacing(10);
		zoomSlider.setMinorTickSpacing(2);
		zoomSlider.setSnapToTicks(false);
		zoomSlider.setPaintTicks(true);
		zoomSlider.addChangeListener(this);
		zoomSlider.setToolTipText("Zoom: " + zoomSlider.getValue() + "x");
		this.add(zoomSlider);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	public void actionPerformed(ActionEvent arg0) {
	// leer
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * javax.swing.event.ChangeListener#stateChanged(javax.swing.event.ChangeEvent
	 * )
	 */
	public void stateChanged(ChangeEvent e) {
		int value = zoomSlider.getValue();
		mainWnd.setActiveEditorZoom(value);
		zoomSlider.setToolTipText("Zoom: " + zoomSlider.getValue() + "x");
	}

	public void setZoom(int zoom) {
		zoomSlider.setValue(zoom);
	}
}
