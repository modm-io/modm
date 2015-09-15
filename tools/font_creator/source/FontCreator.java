/*
 * Version 2.0 created on 05.12.2003 by Fabian Maximilian Thiele
 * 
 * Version 2.1 beta created on 19.12.2007 by sce
 * changes:
 * - added font name edit function
 * - fixed wrong font size calculation
 * - fixed blank character
 * 
 * Version 2.2 (xpcc) created on 23.9.2010 by Fabian Greif
 */

import javax.swing.JFrame;

import gui.MainWindow;

public class FontCreator extends JFrame {
	private static final long serialVersionUID = 8358706032712418201L;

	public static void main(String args[]) {
		new MainWindow();
	}
}