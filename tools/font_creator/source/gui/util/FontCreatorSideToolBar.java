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
package gui.util;

import gui.CharEditor;
import gui.FontCreatorColorChooser;
import gui.MainWindow;

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.IndexColorModel;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JToolBar;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCreatorSideToolBar extends JToolBar implements ActionListener {
	private static final long serialVersionUID = -2870009527736344524L;

	private MainWindow mainWindow;
	private FontCreatorColorChooser colorChooser;

	public FontCreatorSideToolBar(MainWindow mainWindow) {
		super(JToolBar.VERTICAL);
		this.mainWindow = mainWindow;

		initToolbar();
	}

	private void initToolbar() {
		colorChooser = new FontCreatorColorChooser(mainWindow);
		this.add(colorChooser);

		Insets toolButtonInsets = new Insets(3, 3, 3, 3);

		JPanel tools = new JPanel(new GridLayout(6, 1, 5, 5));

		JButton penTool = new JButton("Pen");
		penTool.setMargin(toolButtonInsets);
		penTool.setActionCommand("pen");
		penTool.addActionListener(this);
		penTool.setPreferredSize(new Dimension(80, 25));
		tools.add(penTool);

		JButton lineTool = new JButton("Line");
		lineTool.setMargin(toolButtonInsets);
		lineTool.setActionCommand("line");
		lineTool.addActionListener(this);
		lineTool.setPreferredSize(new Dimension(80, 25));
		tools.add(lineTool);

		JButton drawRectTool = new JButton("drawRect");
		drawRectTool.setMargin(toolButtonInsets);
		drawRectTool.setActionCommand("drawRect");
		drawRectTool.addActionListener(this);
		drawRectTool.setPreferredSize(new Dimension(80, 25));
		tools.add(drawRectTool);

		JButton fillRectTool = new JButton("fillRect");
		fillRectTool.setMargin(toolButtonInsets);
		fillRectTool.setActionCommand("fillRect");
		fillRectTool.addActionListener(this);
		fillRectTool.setPreferredSize(new Dimension(80, 25));
		tools.add(fillRectTool);

		JButton drawOvalTool = new JButton("drawOval");
		drawOvalTool.setMargin(toolButtonInsets);
		drawOvalTool.setActionCommand("drawOval");
		drawOvalTool.addActionListener(this);
		drawOvalTool.setPreferredSize(new Dimension(80, 25));
		tools.add(drawOvalTool);

		JButton fillOvalTool = new JButton("fillOval");
		fillOvalTool.setMargin(toolButtonInsets);
		fillOvalTool.setActionCommand("fillOval");
		fillOvalTool.addActionListener(this);
		fillOvalTool.setPreferredSize(new Dimension(80, 25));
		tools.add(fillOvalTool);

		tools.setMaximumSize(new Dimension(100, 180));
		this.add(tools);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	public void actionPerformed(ActionEvent e) {
		String cmd = e.getActionCommand();
		if (cmd.equals("pen")) {
			mainWindow.setActiveEditorTool(CharEditor.TOOL_FREEHAND);
		} else if (cmd.equals("line")) {
			mainWindow.setActiveEditorTool(CharEditor.TOOL_STRAIGHT_LINES);
		} else if (cmd.equals("drawRect")) {
			mainWindow.setActiveEditorTool(CharEditor.TOOL_DRAW_RECT);
		} else if (cmd.equals("fillRect")) {
			mainWindow.setActiveEditorTool(CharEditor.TOOL_FILL_RECT);
		} else if (cmd.equals("drawOval")) {
			mainWindow.setActiveEditorTool(CharEditor.TOOL_DRAW_OVAL);
		} else if (cmd.equals("fillOval")) {
			mainWindow.setActiveEditorTool(CharEditor.TOOL_FILL_OVAL);
		}
	}

	public void setColor(int color, IndexColorModel cm) {
		colorChooser.setColor(color, cm);
	}
}
