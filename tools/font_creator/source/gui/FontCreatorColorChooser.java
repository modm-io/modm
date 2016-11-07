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
 * Created on 24.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.IndexColorModel;

import javax.swing.JComponent;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCreatorColorChooser extends JComponent implements
		MouseListener {
	private static final long serialVersionUID = -1707379967960361519L;

	private MainWindow mainWindow;

	private FontCreatorColorChooserFrame chooser;
	private IndexColorModel cm;
	private int color;
	
	public FontCreatorColorChooser(MainWindow mainWindow) {
		this.mainWindow = mainWindow;
		color = -1;
		
		setPreferredSize(new Dimension(40, 30));
		setMaximumSize(new Dimension(40, 35));
		addMouseListener(this);
		chooser = new FontCreatorColorChooserFrame(this);
	}
	
	public void paint(Graphics g) {
		g.setColor(Color.GRAY);
		g.drawRect(5, 5, 30, 20);
		if (cm != null && color > -1) {
			int red = cm.getRed(color);
			int green = cm.getGreen(color);
			int blue = cm.getBlue(color);
			g.setColor(new Color(red, green, blue));
			g.fillRect(6, 6, 29, 19);
		}
		else {
			g.setColor(Color.LIGHT_GRAY);
			g.fillRect(6, 6, 29, 19);
		}
	}
	
	public void setChoosedColor(int color) {
		this.color = color;
		mainWindow.setActiveEditorColor(color);
		repaint();
	}
	
	public void setColor(int color, IndexColorModel model) {
		this.color = color;
		cm = model;
		
		chooser.setColors(model);
		chooser.setCurrentColor(color);
		
		if (chooser.isVisible()) {
			chooser.repaint();
		}
		this.repaint();
	}

	public void mouseClicked(MouseEvent e) {
		if (cm != null) {
			if (cm.getMapSize() == 2 && e.getClickCount() == 1)
			{
				// invert color
				color = (color == 1) ? 0 : 1;
				chooser.setCurrentColor(color);
				mainWindow.setActiveEditorColor(color);
				
				if (chooser.isVisible()) {
					chooser.repaint();
				}
				this.repaint();
			}
			else if (e.getClickCount() % 2 == 0) {
				chooser.setVisible(true);
			}
		}
	}
	
	public void mousePressed(MouseEvent arg0) {
	// unused
	}
	
	public void mouseReleased(MouseEvent arg0) {
	// unused
	}
	
	public void mouseEntered(MouseEvent arg0) {
	// unused
	}
	
	public void mouseExited(MouseEvent arg0) {
	// unused
	}
}
