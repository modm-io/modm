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
import java.awt.Toolkit;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.IndexColorModel;

import javax.swing.JFrame;
import javax.swing.JLabel;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCreatorColorChooserFrame extends JFrame implements
		MouseListener, MouseMotionListener {
	private static final long serialVersionUID = -7023847530602240300L;
	private FontCreatorColorChooser chooser;
	private IndexColorModel cm;

	private JLabel indexL;
	private JLabel redL;
	private JLabel greenL;
	private JLabel blueL;

	private int currentColor = 0;

	public FontCreatorColorChooserFrame(FontCreatorColorChooser chooser) {
		this.chooser = chooser;
		setResizable(false);
		setTitle("Choose Color");
		setSize(420, 198);
		setAlwaysOnTop(true);
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
		setLocation(d.width / 2 - getWidth() / 2,
				d.height / 2 - getHeight() / 2);
		addMouseListener(this);
		addMouseMotionListener(this);
		
		this.setLayout(null);

		JLabel indexL1 = new JLabel("Index:");
		indexL1.setLocation(335, 70);
		indexL1.setSize(50, 16);
		this.add(indexL1);
		indexL = new JLabel("" + currentColor);
		indexL.setLocation(385, 70);
		indexL.setSize(50, 16);
		this.add(indexL);

		JLabel redL1 = new JLabel("Red:");
		redL1.setLocation(335, 90);
		redL1.setSize(50, 16);
		this.add(redL1);
		redL = new JLabel("n/a");
		redL.setLocation(385, 90);
		redL.setSize(30, 16);
		this.add(redL);

		JLabel greenL1 = new JLabel("Green:");
		greenL1.setLocation(335, 110);
		greenL1.setSize(50, 16);
		this.add(greenL1);
		greenL = new JLabel("n/a");
		greenL.setLocation(385, 110);
		greenL.setSize(30, 16);
		this.add(greenL);

		JLabel blueL1 = new JLabel("Blue:");
		blueL1.setLocation(335, 130);
		blueL1.setSize(50, 16);
		this.add(blueL1);
		blueL = new JLabel("n/a");
		blueL.setLocation(385, 130);
		blueL.setSize(30, 16);
		this.add(blueL);
	}

	public void paint(Graphics g) {
		super.paint(g);
		g.setColor(Color.GRAY);
		g.drawRect(8, 28, 321, 161);
		if (cm == null) {
			g.setColor(Color.LIGHT_GRAY);
			g.fillRect(9, 29, 320, 160);
		} else if (cm.getMapSize() == 2) {
			for (int i = 0; i < 2; i++) {
				int red = cm.getRed(i);
				int green = cm.getGreen(i);
				int blue = cm.getBlue(i);
				g.setColor(new Color(red, green, blue));
				g.fillRect(9 + i * 160, 29, 160, 160);
			}
		} else if (cm.getMapSize() == 4) {
			for (int i = 0; i < 4; i++) {
				int red = cm.getRed(i);
				int green = cm.getGreen(i);
				int blue = cm.getBlue(i);
				g.setColor(new Color(red, green, blue));
				g.fillRect(9 + i * 80, 29, 80, 160);
			}
		} else if (cm.getMapSize() == 8) {
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 4; j++) {
					int red = cm.getRed(i * 4 + j);
					int green = cm.getGreen(i * 4 + j);
					int blue = cm.getBlue(i * 4 + j);
					g.setColor(new Color(red, green, blue));
					g.fillRect(9 + j * 80, 29 + i * 80, 80, 80);
				}
			}
		} else if (cm.getMapSize() == 256) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 32; j++) {
					int red = cm.getRed(i * 32 + j);
					int green = cm.getGreen(i * 32 + j);
					int blue = cm.getBlue(i * 32 + j);
					g.setColor(new Color(red, green, blue));
					g.fillRect(9 + j * 10, 29 + i * 20, 10, 20);
				}
			}
		}

		g.setColor(Color.GRAY);
		g.drawRect(348, 28, 51, 51);
		if (cm == null) {
			g.setColor(Color.LIGHT_GRAY);
			g.fillRect(349, 29, 50, 50);
		} else {
			int red = cm.getRed(currentColor);
			int green = cm.getGreen(currentColor);
			int blue = cm.getBlue(currentColor);
			g.setColor(new Color(red, green, blue));
			g.fillRect(349, 29, 50, 50);
		}
	}

	public void setColors(IndexColorModel colors) {
		cm = colors;
	}

	public void setCurrentColor(int color) {
		currentColor = color;
		indexL.setText("" + color);
		if (cm != null) {
			redL.setText(cm.getRed(color) + "");
			greenL.setText(cm.getGreen(color) + "");
			blueL.setText(cm.getBlue(color) + "");
		}
	}

	public void mouseClicked(MouseEvent e) {
		if (cm != null && cm.getMapSize() == 2) {
			int x = (e.getX() - 9) / 160;
			int y = (e.getY() - 29) / 160;
			if (x >= 0 && x < 2 && y == 0) {
				currentColor = x;
			}
		} else if (cm != null && cm.getMapSize() == 4) {
			int x = (e.getX() - 9) / 80;
			int y = (e.getY() - 29) / 80;
			if (x >= 0 && x < 4 && y == 0) {
				currentColor = x;
			}
		} else if (cm != null && cm.getMapSize() == 8) {
			int x = (e.getX() - 9) / 80;
			int y = (e.getY() - 29) / 80;
			if (x >= 0 && x < 4 && y >= 0 && y < 2) {
				currentColor = y * 4 + x;
			}
		} else if (cm != null && cm.getMapSize() == 256) {
			int x = (e.getX() - 9) / 10;
			int y = (e.getY() - 29) / 20;
			if (x >= 0 && x < 32 && y >= 0 && y < 8) {
				currentColor = y * 32 + x;
			}
		}
		chooser.setChoosedColor(currentColor);
		repaint();
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

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.MouseMotionListener#mouseDragged(java.awt.event.MouseEvent
	 * )
	 */
	public void mouseDragged(MouseEvent arg0) {
	// unused
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.MouseMotionListener#mouseMoved(java.awt.event.MouseEvent)
	 */
	public void mouseMoved(MouseEvent e) {
		if (cm != null && cm.getMapSize() == 2) {
			int x = (e.getX() - 9) / 160;
			int y = (e.getY() - 29) / 160;
			if (x >= 0 && x < 2 && y == 0) {
				int index = x;
				indexL.setText("" + index);
				redL.setText(cm.getRed(index) + "");
				greenL.setText(cm.getGreen(index) + "");
				blueL.setText(cm.getBlue(index) + "");
			}
		} else if (cm != null && cm.getMapSize() == 4) {
			int x = (e.getX() - 9) / 80;
			int y = (e.getY() - 29) / 80;
			if (x >= 0 && x < 4 && y == 0) {
				int index = x;
				indexL.setText("" + index);
				redL.setText(cm.getRed(index) + "");
				greenL.setText(cm.getGreen(index) + "");
				blueL.setText(cm.getBlue(index) + "");
			}
		} else if (cm != null && cm.getMapSize() == 8) {
			int x = (e.getX() - 9) / 80;
			int y = (e.getY() - 29) / 80;
			if (x >= 0 && x < 4 && y >= 0 && y < 2) {
				int index = y * 4 + x;
				indexL.setText("" + index);
				redL.setText(cm.getRed(index) + "");
				greenL.setText(cm.getGreen(index) + "");
				blueL.setText(cm.getBlue(index) + "");
			}
		} else if (cm != null && cm.getMapSize() == 256) {
			int x = (e.getX() - 9) / 10;
			int y = (e.getY() - 29) / 20;
			if (x >= 0 && x < 32 && y >= 0 && y < 8) {
				int index = y * 32 + x;
				indexL.setText("" + index);
				redL.setText(cm.getRed(index) + "");
				greenL.setText(cm.getGreen(index) + "");
				blueL.setText(cm.getBlue(index) + "");
			}
		}
	}
}
